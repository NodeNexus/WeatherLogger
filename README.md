# 🌦️ ESP32 Local Weather Logger

## 📌 Overview
This project logs **temperature, humidity, and pressure** using ESP32 + BME280.  
It serves a **local web dashboard** and stores readings to CSV for later download.

---

## 🛠️ Hardware Required
- ESP32 Dev Board  
- BME280 Temperature/Pressure/Humidity Sensor (I2C)  
- Jumper wires, breadboard  

---

## 🔌 Wiring
- BME280 → ESP32  
  - VCC → 3.3V  
  - GND → GND  
  - SDA → GPIO21  
  - SCL → GPIO22  

---

## ▶️ Usage
1. Upload the sketch to ESP32.  
2. Connect to Wi-Fi AP `ESP32-AP` (password: `12345678`).  
3. Open browser → go to `192.168.4.1`.  
4. Dashboard shows live readings and updates every second.  
5. Click **Download CSV** to save all readings.

---

## 📂 Repo Structure
