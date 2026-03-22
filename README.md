# ESP32 Blood Oxygen & Heart Rate Monitor

A portable IoT-based health monitoring system that measures 
blood oxygen saturation (SpO₂) and heart rate in real time 
using MAX30100 pulse oximeter sensor and ESP32 microcontroller 
with ThingSpeak cloud integration.

---

## 🔧 Components Used
- ESP32 microcontroller (WiFi + Bluetooth)
- MAX30100 pulse oximeter sensor
- 16x2 LCD Display
- HC-05 Bluetooth module
- LM35 Temperature sensor
- ThingSpeak IoT cloud platform
- Arduino IDE

---

## ⚙️ Features
- Real-time SpO₂ monitoring (normal range: 95–99%)
- Heart rate monitoring (normal range: 70–95 BPM)
- Automatic alert when SpO₂ drops below 90%
- WiFi-based wireless data upload to ThingSpeak cloud
- Remote monitoring via web dashboard and mobile
- Low-cost and portable design

---

## 📡 Communication Protocols
- I2C — MAX30100 sensor to ESP32
- WiFi — ESP32 to ThingSpeak cloud
- Bluetooth — HC-05 for local monitoring

---

## 📊 Results
| Parameter | Normal Range | Alert Level |
|-----------|-------------|-------------|
| SpO₂ | 95% – 99% | Below 90% |
| Heart Rate | 70 – 95 BPM | Above 100 BPM |

---

## 🏥 Applications
- Remote patient monitoring
- Elderly health care at home
- COVID-19 patient monitoring
- Sports and fitness tracking
- Rural healthcare systems

---

## 👨‍💻 Project By
**M. Nithin Kumar** — ECE, VBIT Hyderabad (2022–2026)

Under guidance of Dr. V. Sharmila, Professor, Dept. of ECE
