// == Libraries ==
#include <ESP32Servo.h>
#include <Adafruit_NeoPixel.h>

// == Ultrasonic Sensor == 
const int trigPin = 16;
const int echoPin = 18;
int delay_sensor = 10;
float duration, distance;

// == Piezo Buzzer == 
const int buzzerPin = 14;

// == LED Strip == 
#define ws2812bPin 17
#define numPixels 60
#define DELAYVAL 500

Adafruit_NeoPixel ws2812b(numPixels, ws2812bPin, NEO_GRB + NEO_KHZ800);

// == Functions ==

// lights full brightness (white)
void fullBrightness(){
  for (int i = 0; i < numPixels; i++) {
    ws2812b.setPixelColor(i, ws2812b.Color(255, 255, 255));
  }
}

// light sweep effect 
void randLights(){
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

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  
  Serial.begin(115200);
  ws2812b.begin();
  randomSeed(digitalRead(17));
}

void loop() {
  // ultrasonic distance check
  digitalWrite(trigPin, LOW);
  delay(delay_sensor);
  digitalWrite(trigPin, HIGH);
  delay(delay_sensor);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2;

  Serial.print("Distance: ");
  Serial.println(distance);
  delay(100);

  
  if (distance <= 100) { 
    // lights on full white for 5s
    ws2812b.clear();
    fullBrightness();
    ws2812b.show();
    delay(5000);

    // lights off for 0.5s
    ws2812b.clear();
    ws2812b.show();
    delay(DELAYVAL);

    // simulate presence inside the home
    randLights();

    // TODO: servo or SMS logic
  } else {
    // Do nothing
  }
}
