#include <DHT.h>                         // Include the DHT library
#include <LiquidCrystal_I2C.h>
#include <ThingSpeak.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#define DHTPIN D3                        // Define the pin for DHT sensor
#define DHTTYPE DHT11                    // Define the type of DHT sensor

#define SOIL_MOISTURE_PIN A0              // Define the pin for soil moisture sensor
#define WATER_SENSOR_PIN D6               // Define the pin for water sensor
#define RELAY_PIN D5

#define WIFI_SSID "Divya"         // Replace with your WiFi SSID
#define WIFI_PASSWORD "Divya@123" // Replace with your WiFi password
#define THINGSPEAK_API_KEY "LAL4MO3NM1B9TBH2"  // Replace with your ThingSpeak API key
#define THINGSPEAK_CHANNEL_ID 2504244
WiFiClient client;

LiquidCrystal_I2C lcd(0x3F,16,2);        // Set the LCD address to 0x27 for a 16 chars and 2 line display
DHT dht(DHTPIN, DHTTYPE);                 // Initialize the DHT sensor

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Turn on the relay
  delay(1000); // Wait for 1 second
  digitalWrite(RELAY_PIN, LOW); // Turn off the relay
  delay(1000);

  lcd.init();                             // Initialize the LCD 
  lcd.backlight();                        // Turn on the backlight
  dht.begin();                            // Initialize the DHT sensor
  
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  lcd.setCursor(0,0);                     // Set cursor to the first line
  lcd.print("System");              // Print label for temperature
  lcd.setCursor(0,1);                     // Set cursor to the second line
  lcd.print("initializing...");  
  delay(2000);               // Print label for humidity
}

void loop() {
  delay(2000);                            // Delay to stabilize sensor readings

  float temperature = dht.readTemperature();   // Read temperature from DHT sensor
  float humidity = dht.readHumidity();         // Read humidity from DHT sensor
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN); // Read soil moisture value from sensor
  int soilMoisturePercentage = map(soilMoistureValue, 500, 1023, 100, 0); // Convert to percentage
if(soilMoisturePercentage > 100)
{soilMoisturePercentage = 100;}
  int waterSensorState = digitalRead(WATER_SENSOR_PIN); // Read water sensor state

  lcd.clear();
  lcd.setCursor(0,0);                     // Set cursor to the first line
  lcd.print("Temp:");
  lcd.setCursor(5,0);                           // Reset cursor to the temperature value position
  lcd.print(temperature);                        // Print temperature value
  lcd.print("C");

  lcd.setCursor(0,1);                     // Set cursor to the second line
  lcd.print("Hum:");                            // Clear previous humidity value
  lcd.setCursor(5,1);                           // Reset cursor to the humidity value position
  lcd.print(humidity);                           // Print humidity value
  lcd.print("%");

  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);                          // Set cursor to display soil moisture value
  lcd.print("Moisture: ");                       // Print label for soil moisture
  lcd.print(soilMoisturePercentage);            // Print soil moisture percentage value
  lcd.print("%");                               // Print percentage symbol

  lcd.setCursor(0, 1);                           // Set cursor to display water sensor state              
  lcd.print(waterSensorState == HIGH ? "Water Present" : "No Water"); // Print water sensor state
  delay(2000);

  if(soilMoisturePercentage < 25 && waterSensorState == HIGH )
 {
  digitalWrite(RELAY_PIN, HIGH);
  }
 if(soilMoisturePercentage > 70 || waterSensorState == LOW)
 digitalWrite(RELAY_PIN, LOW);  

  // Send data to ThingSpeak
  ThingSpeak.begin(client);
  ThingSpeak.setField(1, temperature); // Field 1: Temperature
  ThingSpeak.setField(2, humidity);    // Field 2: Humidity
  ThingSpeak.setField(3, soilMoisturePercentage); // Field 3: Soil Moisture
  ThingSpeak.setField(4, waterSensorState); // Field 4: Water Sensor State
  int response = ThingSpeak.writeFields(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_KEY);
  if (response == 200) {
    Serial.println("Data sent to ThingSpeak successfully!");
  } else {
    Serial.println("Failed to send data to ThingSpeak!");
  }
}