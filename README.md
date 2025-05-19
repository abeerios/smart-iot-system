# 🚀 smart-iot-system
This is my college capstone project; a prototype of an IoT-based smart lighting and security system.

## How does it work and what inspired it? 
A user connects to the esp32 which acts as both a soft access point, and a captive portal. If the user is authorized, the door (or rather, the servo motor acting as a placeholder for the door) unlocks and depending on whether the user is the owner of the home or a guest, the lights will turn yellow or blue, along with an LCD message saying access has been granted. 

If the user is unathorized, the door will remain locked, or will lock if it has been in an unlocked state, and the buzzer will ring and the lights will flash brightly (just like real floodlights do) and you'll get an LCD message saying access has been denied.
To be honest, what inspired it was a thought I had about people traveling away from home. Many people forget turning off something back home - the garage door anyone? And it got me wondering, wouldn't it be cool to have a system that could simulate there being someone at home, despite there...not being? And it rolled from there :)


## 🖥️ Technology used:
ESP32 and written in C++ (used Arduino Library and PlatformIO in VS Code). HTML was used for the captive portal.

## The system includes:
- A piezo buzzer to alert neighbours of a possible intrusion.
- Addressable LEDs personalized based on type of user (owner or guest).
- Servo motor for door lock.
- Ultrasonic sensor to gauage distance from door or setup.
- LCD display to show access type.
