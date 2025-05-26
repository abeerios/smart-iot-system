# 🚀 smart-iot-system
This is a prototype of a functional IoT-based smart lighting and security system that uses an ESP32 as the microcontroller.

## What does it do?
This project shows you how to create a user-friendly security system that incorporates lighting as part of the security feature. The access to the hypothetical door is controlled through a captive portal. The code contains the following:

- A login form for the captive portal.
- An ESP32 configured to be a Soft Access Point (AP), with self-contained Web and DNS servers.
- Customizable lighting based on user type (owner, guest, and unauthorized).
- Access levels for different users based on conditional logic (i.e., if person > 1 m away, step closer)
- Code for a non-blocking active buzzer.

### Watch the system in action <a href="https://youtu.be/Oaq3DMGut0Q" target="_blank">here</a>!

Download VS Code to open the file and install Platform IO as well. 

## How to make this project your own?

This is meant to be the minimum viable product (MVP) for my capstone project, but that doesn't mean you can't make it your own! Tinker around a bit with the original code and add a simulation of an SMS alert (which, by the way, was supposed to be added to my project but due to time constraints, I couldn't). Instead of hardcoding the credentials like I did for demo purposes, create a database. And if you're the hands-on type, you can even make a cardboard door/home and stick an LED strip if you have one, to the interior walls. The possibilities are endless.

### 🖥️ Tech Stack
- ESP32

- PlatformIO (VS Code)

- HTML for the captive portal

- C++ (Arduino IDE library)
