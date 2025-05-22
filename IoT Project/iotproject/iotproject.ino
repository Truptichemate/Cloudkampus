#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Ultrasonic Sensor Pins
#define TRIG_PIN 5
#define ECHO_PIN 18

// LED Pins
#define GREEN_LED 26  // Parking Available
#define RED_LED 27    // Parking Full

// WiFi & ThingSpeak Credentials
const char* ssid = "Wokwi-GUEST";  
const char* password = "";  
const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "TG0T2GLWRO2UZY9I"; 

// LCD Display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Parking Configuration
const int maxParkingSlots = 20;
int carCount = 0;
bool carPresent = false;  
int prevDistance = 100;   

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi...");

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  // Wait for WiFi Connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  
  Serial.println("Connected to WiFi!");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Parking");
}

int measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.0344 / 2;  // Convert to cm
  return distance;
}

void updateLCDandLEDs(String status) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cars: ");
  lcd.print(carCount);
  
  lcd.setCursor(0, 1);
  lcd.print("Status: ");
  lcd.print(status);

  if (status == "FULL") {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
  } else {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
  }
}

void sendToThingSpeak(int cars, String status) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey + "&field1=" + String(cars) + "&field2=" + status;
    
    http.begin(url);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      Serial.print("ThingSpeak Response: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.println("Error Sending Data");
    }
    
    http.end();
  } else {
    Serial.println("WiFi Disconnected!");
  }
}

void loop() {
  int distance = measureDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Detect a car entering and leaving
  if (distance < 20) {  // Car is within detection range
    if (!carPresent) {  
      if (carCount < maxParkingSlots) {  
        carCount++;  // Increment count
        Serial.println("Car Entered! Count Updated.");
      } else {
        Serial.println("Parking FULL! Next Car Cannot Enter.");
      }
      carPresent = true;  
    }
  } else if (distance > 25 && carPresent) { 
    Serial.println("Car Exited. Ready to count next car.");
    carPresent = false;  
  }

  // Set Parking Status
  String parkingStatus = (carCount >= maxParkingSlots) ? "FULL" : "AVAILABLE";

  Serial.print("Parking Status: ");
  Serial.println(parkingStatus);
  Serial.print("Total Cars Parked: ");
  Serial.println(carCount);

  // Update LCD, LEDs, and ThingSpeak
  updateLCDandLEDs(parkingStatus);
  sendToThingSpeak(carCount, parkingStatus);

  prevDistance = distance;  

  delay(15000); 
}
