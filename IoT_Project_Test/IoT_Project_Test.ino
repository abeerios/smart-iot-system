// == Libraries ==
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESP32Servo.h>
#include <Adafruit_NeoPixel.h>

// == Ultrasonic Sensor == 
const int trigPin = 16;
const int echoPin = 18;
int delay_sensor = 10;
float duration, distance;

// == WiFi Config ==
const char* ssid = "ESP32-Access-Point";
WebServer server(80);
bool portalStarted = false;

// == Authorized Users == 
String ownerUser = "abeer_salman";
String ownerPass = "h0me_1s!";

String guestUser = "my_guest";
String guestPass = "hoM3_2s!";

// == Servo Motor == 
Servo myServo;
static const int servoPin = 19;
int pos = 0;

// == Piezo Buzzer == 
const int buzzerPin = 14;
unsigned long previousMillis = 0;  // Stores last time the buzzer was toggled
const long interval = 5000;         // Interval at which to sound the buzzer (5s)
bool buzzerState = false;           // Keeps track of whether the buzzer is on or off

// == LED Strip == 
#define ws2812bPin 17
#define numPixels 60
#define DELAYVAL 500

Adafruit_NeoPixel ws2812b(numPixels, ws2812bPin, NEO_GRB + NEO_KHZ800);

// == Timing variables for light effects
unsigned long lastRandLightsMillis = 0;
unsigned long lastAmbChangeMillis = 0;
const long randLightsInterval = 100;  // Interval for random light sweep
const long ambChangeInterval = 5000;  // Interval for changing ambient light colors

// == Functions ==

// function to lock door
void unlockDoor() {
  myServo.write(90); // unlocks position by moving servo
  Serial.println("Door unlocked!");
}

void lockDoor() {
  myServo.write(0); // locks position by moving servo
  Serial.println("Door locked!");
}

// server login page 
void handleRoot() {
  String html = "<html><body><h2>Login</h2>"
                "<form action='/login' method='POST'>"
                "Username:<br><input type='text' name='username'><br>"
                "Password:<br><input type='password' name='password'><br><br>"
                "<input type='submit' value='Login'></form></body></html>";
  server.send(200, "text/html", html);
}

void handleLogin() {
  if (server.hasArg("username") && server.hasArg("password")) {
    String username = server.arg("username");
    String password = server.arg("password");

    if (username == ownerUser && password == ownerPass) {
      Serial.println("Owner logged in!");
      unlockDoor();
      ambYellow();
      server.send(200, "text/html", "<h1>Owner Access Granted</h1>");
    } 
    else if (username == guestUser && password == guestPass) {
      Serial.println("Guest logged in!");
      unlockDoor();
      ambBlue();
      server.send(200, "text/html", "<h1>Guest Access Granted</h1>");
    } 
    else {
      Serial.println("Login failed");
      server.send(200, "text/html", "<h1>Access Denied</h1>");
    }
  } else {
    server.send(400, "text/html", "Bad Request");
  }
}

// lights full brightness (white)
void fullBrightness(){
  for (int i = 0; i < numPixels; i++) {
    ws2812b.setPixelColor(i, ws2812b.Color(255, 255, 255));
  }
  ws2812b.show();
}

// light sweep effect
void randLights(){
  if (millis() - lastRandLightsMillis >= randLightsInterval) {
    lastRandLightsMillis = millis();
    int pixel = random(numPixels);
    ws2812b.setPixelColor(pixel, ws2812b.Color(255, 255, 255));
    ws2812b.show();
  }
}

// lights ambient mode (owner)
void ambYellow(){
  if (millis() - lastAmbChangeMillis >= ambChangeInterval) {
    lastAmbChangeMillis = millis();
    for (int i = 0; i < numPixels; i++) {
      ws2812b.setPixelColor(i, ws2812b.Color(250, 250, 210));  // Soft yellow
    }
    ws2812b.show();
  }
}

// lights ambient mode (guest)
void ambBlue(){
  if (millis() - lastAmbChangeMillis >= ambChangeInterval) {
    lastAmbChangeMillis = millis();
    for (int i = 0; i < numPixels; i++) {
      ws2812b.setPixelColor(i, ws2812b.Color(135, 206, 235));  // Soft blue
    }
    ws2812b.show();
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  myServo.attach(servoPin);

  pinMode(buzzerPin, OUTPUT);
  Serial.println("Buzzer test started");

  ws2812b.begin();
  randomSeed(digitalRead(17));
}

void loop() {
  unsigned long currentMillis = millis();

  // ultrasonic distance check
  digitalWrite(trigPin, LOW);
  delay(delay_sensor);  
  digitalWrite(trigPin, HIGH);
  delay(delay_sensor);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH); // in microseconds
  distance = (duration * 0.0343) / 2; // in centimeters 

  Serial.print("Distance: ");
  Serial.println(distance);

  // Access point logic
  if (distance <= 100 && !portalStarted) {
    WiFi.softAP(ssid);
    portalStarted = true;
    // Lock door immediately
    lockDoor();
  }

  // Handle buzzer toggle
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Save the last time
    buzzerState = !buzzerState;
    digitalWrite(buzzerPin, buzzerState ? HIGH : LOW);  // Toggle buzzer state
  }

  // Lights randomized
  randLights();

  // Handle the different user access scenarios
  if (portalStarted) {
    // open door
    unlockDoor();
    // Simulate owner or guest lighting
    if (currentMillis % 10000 < 5000) {
      ambYellow();  // Owner's lighting
    } else {
      ambBlue();  // Guest's lighting
    }

  }

  // When no one is around, shut down access point
  if (distance > 100 && portalStarted) {
    WiFi.softAPdisconnect(true);  // Disconnect AP
    portalStarted = false;
    Serial.println("AP disconnected. No one detected.");
  }
}


