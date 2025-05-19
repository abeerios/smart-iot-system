#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include <DNSServer.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
#include <WebServer.h>
#include <WiFi.h>
#include <Wire.h>

// == Credentials ==

// network credentials
const char *ssid = "ESP32-AP";
const char *password = "h0meWiFi";
int max_connection = 4;

// authorized user credentials
const String ownerUser = "owner";
const String ownerPass = "ownerpass1";
const String guestUser = "guest";
const String guestPass = "guestpass2";


DNSServer dnsServer;
WebServer server(80); // setting web server port number

const int ledPin = 2;  // onboard LED pin (GPIO2 for ESP32)


// setting time duration for buzzer
bool reminderActive = false;
unsigned long reminderStartTime = 0;

// == Ultrasonic Sensor ==

const int trigPin = 13;
const int echoPin = 14;

float duration, distance;

// == Servo Motor ==
Servo myServo;
const int servoPin = 18;
int pos = 0;

// == Buzzer Pin ==
const int buzzerPin = 25;

// == NeoPixel LED ==
#define ws2812bPin 23
#define numPixels 9
Adafruit_NeoPixel ws2812b(numPixels, ws2812bPin, NEO_GRB + NEO_KHZ800);

// == LCD Display ==
LiquidCrystal_I2C lcd(0x27, 16, 2); // configuring LCD 

// == States ==
bool loginAllowed = false;
bool showDeniedMsg = false;
String accessLevel = "";
bool loginAttempted = false;
bool ledState = false; // track LED state (useful for confirming if the ESP32 is
// connected to WiFi)

// == Functions ==

// == Buzzer Pin ==
void buzzAlarm(int durationMillis) { 
  for (int i = 0; i < durationMillis / 200; i++) {  // simulating a realistic sound 
                                                    // for the buzzer when it rings
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
    delay(100);
  }
}

// == Servo Motor ==
void unlockDoor() {
  if (distance <=100) { // if person is less than or equal to 1m away,
                        // unlock doors
      myServo.write(90); // open position
      delay(1000);
  }
}

void lockDoor() {
  myServo.write(0); // locked position
  delay(1000);
}

// == Lights ==

// lights at full brightness (white)
void fullBrightness() {
  for (int i = 0; i < numPixels; i++) {
    ws2812b.setPixelColor(i, ws2812b.Color(255, 255, 255));
  }
  ws2812b.show();
  delay(500);
}

// light sweep effect
void randLights() {
  // turn on even-numbered pixels first
  for (int i = 0; i < numPixels; i += 2) {
    ws2812b.setPixelColor(i, ws2812b.Color(255, 255, 255));
    ws2812b.show();
    delay(100);
  }
  // then turn on the odd-numbered pixels
  for (int i = 1; i < numPixels; i += 2) {
    ws2812b.setPixelColor(i, ws2812b.Color(255, 255, 255));
    ws2812b.show();
    delay(100);
  }
}
// lights ambient mode (owner)
void ambYellow() {
  for (int i = 0; i < numPixels; i++) {
    ws2812b.setPixelColor(i, ws2812b.Color(255, 180, 20)); // soft yellow
  }
  ws2812b.show();
}

// lights ambient mode (guest)
void ambBlue() {
  for (int i = 0; i < numPixels; i++) {
    ws2812b.setPixelColor(i, ws2812b.Color(135, 206, 235)); // soft blue
  }
  ws2812b.show();
}

// == Security Mode ==
void triggerAlarm() {
  if (!reminderActive) {
    lockDoor();
    fullBrightness();
    randLights();
    buzzAlarm(5000);
    reminderStartTime = millis();
    reminderActive = true;
  }
}

// == Distance Check ==
void checkDist() {

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * .0343) / 2;
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(1000);
  
}

void distPrompt(){
  if (distance > 100) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Step closer to");
    lcd.setCursor(0, 1);
    lcd.print("unlock door");
  } else {
    lcd.clear(); // clears message when within range
  }
}

// HTML Content for the captive portal
String captive_portal() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><title>Smart IoT Systems</title></head>";
  html += "<h1>Welcome Home!</h1>";
  html += "<form action='/login' method='POST'>";
  html += "Username: <input type='text' name='username'><br>";
  html += "Password: <input type='password' name='password'><br>";
  html += "<input type='submit' value='Login'>";
  html += "</form>";
  html += "</body></html>";
  return html;
}

// index page handler
void handleRoot() { 
  server.send(200, "text/html", captive_portal());
 }

// this will redirect unknown http req's to THE captive portal page
// based on this redirect various systems could detect that WiFi AP has a
// captive portal page
void handleNotFound() {
    // Redirect EVERYTHING to captive portal
    server.sendHeader("Location", "/portal");
    server.send(302, "text/plain", "Redirecting to captive portal");
  }

void handleLogin() {
    loginAttempted = true; // user tried to log in

    String username = server.arg("username");
    String password = server.arg("password");

    if (username == ownerUser && password == ownerPass) {
      accessLevel = "owner";
      loginAllowed = true;
      server.send(200, "text/html", "<h2>Owner Access Granted</h2>");
    } else if (username == guestUser && password == guestPass) {
      accessLevel = "guest";
      loginAllowed = true;
      server.send(200, "text/html", "<h2>Guest Access Granted</h2>");
    } else {
      loginAllowed = false;
      accessLevel = "";
      showDeniedMsg = true; // flag to show access denied message
      server.send(200, "text/html", "<h2>Access Denied</h2>");
      triggerAlarm(); // if user is unathorized, trigger the alarm
    }
  }

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password, 1, 0, max_connection);
  delay(500);
  Serial.println("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  dnsServer.start(53, "*", WiFi.softAPIP());


  // serve portal page
  server.on("/portal", [](){ server.send(200, "text/html", captive_portal()); });
  // redirecting users to the captive portal
  server.on("/", handleRoot);
  server.on("/generate_204", handleRoot);        // for Android users
  server.on("/hotspot-detect.html", handleRoot); // for Apple users
                    

  // handle login POST
  server.on("/login", HTTP_POST, handleLogin);

  // all unknown pages are redirected to the captive portal
  server.onNotFound(handleNotFound);
  server.begin();

  lcd.init();      // initialize the LCD
  lcd.backlight(); // turn on the backlight
  lcd.setCursor(0, 0);
  lcd.print("System Ready...");

  // initialize onboard LED pin as output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // start with LED off

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  myServo.attach(servoPin);
  lockDoor();

  pinMode(buzzerPin, OUTPUT);
  

  ws2812b.begin();
  ws2812b.show();
  randomSeed(digitalRead(23));
}

void loop() {
  checkDist();
  dnsServer.processNextRequest();
  int numClients = WiFi.softAPgetStationNum();
  Serial.print("Num of Connected Clients : ");
  Serial.println(numClients);
  

  if (numClients > 0) {
    digitalWrite(ledPin, HIGH); // turn LED on
  } else {
    digitalWrite(ledPin, LOW); // turn LED off when no clients are connected
  }
  
    ws2812b.clear();
    server.handleClient(); // let person login to the captive portal
      if (loginAllowed) {  // if the user is authorized, trigger the system
        distPrompt();      // if user is > 1 m away, step closer
        if (accessLevel == "owner")
        {
          unlockDoor();   
          ambYellow();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Access Granted"); // show message on LCD
          delay(1000);
          lcd.clear();
        }
        else if (accessLevel == "guest")
        {
          unlockDoor();
          ambBlue();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Access Granted");
          delay(1000);
          lcd.clear();
        }
        showDeniedMsg = false;
      } else if (showDeniedMsg) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Access Denied"); // show message on LCD 
        delay(1000);
        lcd.clear();
        showDeniedMsg = false; 
        loginAttempted = false;
        loginAllowed = false;
        accessLevel = "";
        lockDoor();
        ws2812b.clear();
      }

  // == code for non-blocking buzzer ==
  if (reminderActive && millis() - reminderStartTime >= 5000) {
    randLights();                 // show light effect
    delay(500);                   // brief delay
    ws2812b.clear();              // turn off lights
    reminderActive = false;       // reset alarm state
  }
}