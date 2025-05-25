# 🚀 smart-iot-system
This is a prototype of a functional IoT-based smart lighting and security system that uses an ESP32 as the microcontroller.

## What Does It Do?
This project shows you how to create a user-friendly security system that incorporates lighting as part of the security feature. The access to the hypothetical door is controlled through a captive portal. The code contains the following:

- A login form for the captive portal.
- An ESP32 configured to be a Soft Access Point (AP), with self-contained Web and DNS servers.
- Customizable lighting based on user type (owner, guest, and unauthorized).
- Access levels for different users based on conditional logic (i.e. if person > 1 m away, do this)
- Code for a non-blocking active buzzer.
