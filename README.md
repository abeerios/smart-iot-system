# 🚀 smart-iot-system
This is a prototype of a functional IoT-based smart lighting and security system that uses an ESP32 as the microcontroller.

## What does it do?
This project shows you how to create a user-friendly security system that incorporates lighting as part of the security feature. The access to a hypothetical door is controlled through a captive portal. The code contains the following:

- A login form for the captive portal.
- An ESP32 configured to be a Soft Access Point (AP), with self-contained Web and DNS servers.
- Customizable lighting based on user type (owner, guest, and unauthorized).
- Access levels for different users based on conditional logic (i.e., if person > 1 m away, step closer)
- Code for a non-blocking active buzzer.

Download VS Code to open the file and install Platform IO. 

## How to make this project your own?

This is meant to be the minimum viable product (MVP) for my capstone project, but that doesn't mean you can't make it your own! Tinker around a bit with the original code and add a simulation of an SMS alert (which was  supposed to be added to my project but due to time constraints, it wasn't). Instead of hardcoding the credentials like I did for demonstration purposes, create a database. And if you're the hands-on type, you can even make a cardboard door/home and stick an LED strip to the interior walls.

### 🖥️ Tech Stack
- ESP32

- PlatformIO (VS Code)

- C++ (Arduino IDE library)
