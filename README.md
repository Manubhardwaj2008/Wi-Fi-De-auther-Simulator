# Wi-Fi Deauthentication Attack Simulator (Safe, Non-Transmitting)

## 📌 Overview
This project is an **educational cybersecurity simulator** built with the NodeMCU ESP8266 and a 0.96” OLED display.  
It **does not transmit or interfere** with Wi-Fi networks. Instead, it scans nearby networks, automatically selects the strongest SSID, and runs a **timed simulation** of a Wi-Fi deauthentication attack.  

The OLED shows a live countdown and progress bar, while the Serial Monitor logs simulated "deauth" attempts.

---

## ✨ Features
- 📡 Wi-Fi scanning with auto-selection of the strongest access point (SSID).
- 🖥️ OLED-based user interface with centered text and progress bar.
- ⏳ Simulation timer (2–5 minutes) with countdown display.
- 📝 Serial log output simulating deauthentication packet sending (without transmission).
- ✅ Safe, legal, and ethical demonstration of Wi-Fi attack concepts.

---

## 🛠️ Technical Details
- **Hardware**: NodeMCU ESP8266 (LoLin) + 0.96” SPI OLED display  
- **Libraries**: `ESP8266WiFi`, `Adafruit_GFX`, `Adafruit_SSD1306`  
- **Language**: C++ (Arduino)  
- **OLED Wiring (SPI)**:  
  - SDA → D7  
  - SCL → D5  
  - RST → D3  
  - DC  → D8  

---

## 📚 Educational Value
This project is designed to **teach wireless security concepts responsibly**.  
It allows students to **visualize how a Wi-Fi deauthentication attack works** without performing any malicious transmissions.  

The simulation demonstrates:  
- How Wi-Fi scanning works  
- How devices can be selected based on signal strength (RSSI)  
- How attack concepts can be **safely explained in classrooms and projects**  

---

## 🎓 Portfolio Relevance
This project highlights:  
- IoT and embedded systems development  
- Wireless networking concepts  
- Cybersecurity awareness and ethical design  
- User interface design on resource-limited devices  

It is suitable for inclusion in **university admission portfolios (NUS, NTU, etc.)** to demonstrate both **technical ability** and **ethical responsibility**.

---

## ⚠️ Disclaimer
This project is for **educational and simulation purposes only**.  
It does **not** transmit or interfere with Wi-Fi signals.  
Always use responsibly in compliance with local laws and regulations.

