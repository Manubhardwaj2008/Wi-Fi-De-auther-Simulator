/* Safe Auto-Select + Jam Simulation for NodeMCU ESP8266
   - DOES NOT transmit or interfere with any networks (SIMULATION ONLY).
   - Auto-selects the strongest SSID and "simulates" jamming for a set time (2..5 minutes).
   - OLED wiring (SPI-style, no CS): SDA->D7, SCL->D5, RES->D3, DC->D8
   - Uses Adafruit_SSD1306 (SPI constructor), Adafruit_GFX
*/

#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED (SPI, no CS)
#define OLED_MOSI  D7  // SDA
#define OLED_CLK   D5  // SCL
#define OLED_DC    D8
#define OLED_RESET D3

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, -1);

// Simulation time (minutes) -- set 2..5
int simMinutes = 3; // default: 3 minutes
unsigned long simDurationMs = 0;
unsigned long simStartMs = 0;
bool simRunning = false;

// UI refresh interval
const unsigned long UI_REFRESH_MS = 250;

void showCenteredLine(int y, const String &txt, uint8_t size=1) {
  display.setTextSize(size);
  int16_t x1,y1;
  uint16_t w,h;
  display.getTextBounds(txt, 0, y, &x1, &y1, &w, &h);
  int x = max(0, (SCREEN_WIDTH - (int)w)/2);
  display.setCursor(x, y);
  display.print(txt);
}

void scanAndAutoSelect(String &bestSSID, int &bestRSSI, int &bestIndex) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  showCenteredLine(0, "Scanning Wi-Fi...", 1);
  display.display();

  // Do synchronous scan
  int n = WiFi.scanNetworks(false, true); // async=false, show_hidden=true
  delay(100);

  bestRSSI = -999;
  bestSSID = "";
  bestIndex = -1;

  if (n <= 0) {
    // nothing found
    display.clearDisplay();
    showCenteredLine(10, "No networks found", 1);
    display.display();
    return;
  }

  // Find strongest
  for (int i = 0; i < n; ++i) {
    int rssi = WiFi.RSSI(i);
    String ssid = WiFi.SSID(i);
    if (rssi > bestRSSI) {
      bestRSSI = rssi;
      bestSSID = ssid;
      bestIndex = i;
    }
  }

  // Show top 5 on OLED briefly (optional)
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.printf("Found %d networks\n", n > 0 ? n : 0);
  int toShow = min(5, n);
  for (int i = 0; i < toShow; ++i) {
    String ssid = WiFi.SSID(i);
    if (ssid.length() > 20) ssid = ssid.substring(0, 17) + "...";
    display.setCursor(0, 12 + i*10);
    display.printf("%d:%s %ddB", i+1, ssid.c_str(), WiFi.RSSI(i));
  }
  display.display();
  delay(1500); // allow viewer to see list
}

void showSelectionScreen(const String &ssid, int rssi) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  showCenteredLine(0, "Auto-selected target", 1);
  // SSID may be long: wrap/truncate
  String s = ssid;
  if (s.length() > 24) s = s.substring(0, 21) + "...";
  showCenteredLine(14, s, 1);
  display.setCursor(0, 34);
  display.printf("RSSI: %d dBm\n", rssi);
  display.setCursor(0, 46);
  display.printf("Sim time: %d min", simMinutes);
  display.display();
}

void startSimulation() {
  simRunning = true;
  simStartMs = millis();
  simDurationMs = (unsigned long)simMinutes * 60UL * 1000UL;
  Serial.printf("SIMULATION START: %lu ms duration (%d minutes)\n", simDurationMs, simMinutes);
}

void showSimulationProgress(const String &ssid) {
  unsigned long now = millis();
  unsigned long elapsed = now - simStartMs;
  if (elapsed >= simDurationMs) {
    // finished
    simRunning = false;
    display.clearDisplay();
    showCenteredLine(20, "Simulation finished", 1);
    showCenteredLine(36, "No packets transmitted", 1);
    display.display();
    Serial.println("SIMULATION COMPLETE (no transmissions performed)");
    return;
  }

  unsigned long remaining = simDurationMs - elapsed;
  int secsLeft = (int)(remaining / 1000UL);
  int mins = secsLeft / 60;
  int secs = secsLeft % 60;

  // Update screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  showCenteredLine(0, "SIMULATION (SIM ONLY)", 1);
  // show target
  String s = ssid;
  if (s.length() > 24) s = s.substring(0,21) + "...";
  display.setCursor(0, 16);
  display.printf("Target: %s", s.c_str());
  display.setCursor(0, 28);
  display.printf("Time left: %02d:%02d", mins, secs);

  // progress bar
  float frac = (float)elapsed / (float)simDurationMs;
  if (frac < 0) frac = 0; if (frac > 1) frac = 1;
  int barW = (int)(SCREEN_WIDTH * 0.9);
  int barX = (SCREEN_WIDTH - barW) / 2;
  int barY = 44;
  int fill = (int)(barW * frac);
  display.drawRect(barX, barY, barW, 10, SSD1306_WHITE);
  if (fill > 2) display.fillRect(barX+1, barY+1, fill-2, 8, SSD1306_WHITE);

  display.display();

  // simulated packet log to serial
  static unsigned long lastSimPacket = 0;
  if (millis() - lastSimPacket > 300) {
    lastSimPacket = millis();
    Serial.printf("[SIM] deauth -> %s  (simulated)\n", ssid.c_str());
  }
}

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println();
  Serial.println("Safe Auto-Select Jam Simulation (NO TRANSMIT)");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("OLED init failed - check wiring");
    while (true) delay(1000);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  showCenteredLine(20, "Deauth Simulator", 1);
  showCenteredLine(36, "Auto-select strongest", 1);
  display.display();
  delay(1000);
}

void loop() {
  static String targetSSID = "";
  static int targetRSSI = -999;
  static int targetIndex = -1;

  if (!simRunning) {
    // 1) Scan and auto-select strongest
    scanAndAutoSelect(targetSSID, targetRSSI, targetIndex);

    if (targetIndex < 0) {
      // nothing found - show message and retry after delay
      display.clearDisplay();
      showCenteredLine(16, "No networks found", 1);
      showCenteredLine(34, "Retrying in 10s...", 1);
      display.display();
      delay(10000);
      return;
    }

    // show selection
    Serial.printf("Auto-selected: %s (%d dBm)\n", targetSSID.c_str(), targetRSSI);
    showSelectionScreen(targetSSID, targetRSSI);

    // small pause so user can cancel by disconnecting power if needed
    delay(2000);

    // Start simulation immediately
    startSimulation();
  }

  // simulation running: update UI + simulated log
  if (simRunning) {
    showSimulationProgress(targetSSID);
  } else {
    // finished; wait a short time then rescan automatically
    delay(2000);
    Serial.println("Simulation ended. Rescanning...");
  }

  // avoid tight loop
  delay(UI_REFRESH_MS);
}
