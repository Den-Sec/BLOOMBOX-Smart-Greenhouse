#include <Wire.h>
#include <DHT.h>
#include "RTClib.h"
#include <Adafruit_MLX90614.h>


#define DHTPIN 0         // DHT11 connected to GPIO0
#define DHTTYPE DHT11    // DHT 11
//#define RELAY_PIN1 4     // Relay connected 
//#define RELAY_PIN2 16    // Relay connected
#define RELAY_PIN3 17    // Relay connected
#define RELAY_PIN4 5     // Relay connected
#define TEMP_LOW_THRESHOLD 14.0 // Temperature threshold
#define TEMP_HIGH_THRESHOLD 14.5 // Temperature threshold
#define WATER_LEVEL_PIN 35  // Water level sensor connected to GPIO35
#define WATER_SENSOR_POWER_PIN 32  // Power pin for the water level sensor connected to GPIO32
#define SOIL_MOISTURE_PIN 34 // Soil moisture sensor connected to GPIO34


#define WATER_LEVEL_MAX 305  // Maximum water level reading
#define WATER_LEVEL_MIN 120     // Minimum water level reading


// Initialize water level percentage variable
int waterLevelPercent;


int waterLevelFinal;
unsigned long waterLevelCheckTime = 60000; // 1 minute, for example
unsigned long lastCheck = 0;


DHT dht(DHTPIN, DHTTYPE);
RTC_DS3231 rtc;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();


void setup() {
  Serial.begin(115200);
  
  dht.begin();


  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }


  if (!mlx.begin()) {
    Serial.println("Failed to start MLX90614");
    while (1);
  }


  //pinMode(RELAY_PIN1, OUTPUT); // Set the relay pin as output
  //pinMode(RELAY_PIN2, OUTPUT); // Set the relay pin as output
  pinMode(RELAY_PIN3, OUTPUT); // Set the relay pin as output
  pinMode(RELAY_PIN4, OUTPUT); // Set the relay pin as output
  pinMode(WATER_LEVEL_PIN, INPUT); // Set the water level pin as input
  pinMode(SOIL_MOISTURE_PIN, INPUT); // Set the soil moisture sensor pin as input
  pinMode(WATER_SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(WATER_SENSOR_POWER_PIN, LOW);  // Initially turn off the sensor
  //digitalWrite(WATER_SENSOR_POWER_PIN, HIGH);



  // Turn off relays initially
  //digitalWrite(RELAY_PIN1, HIGH);
  //digitalWrite(RELAY_PIN2, HIGH);
  digitalWrite(RELAY_PIN3, HIGH);
  digitalWrite(RELAY_PIN4, HIGH);


  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}


void loop() {
  delay(1000); // Wait a few seconds between measurements


  float humidity = dht.readHumidity(); // Reading humidity
  float ambientTemp = mlx.readAmbientTempC(); // Reading ambient temperature
  int waterLevel = analogRead(WATER_LEVEL_PIN); // Reading water level
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN); // Reading soil moisture


  // Reading water level
  int waterLevelRaw = analogRead(WATER_LEVEL_PIN); 


  // Calibration data
  const int level25 = 205; 
  const int level50 = 255; 
  const int level75 = 280; 


// Check if it's time to calculate the final water level
  if (millis() - lastCheck >= waterLevelCheckTime) {


    digitalWrite(WATER_SENSOR_POWER_PIN, HIGH);  // Turn on the sensor
    delay(30);  // Short delay for power stabilization


    int sum = 0;
    for (int i = 0; i < 5; i++) {
      sum += analogRead(WATER_LEVEL_PIN); // Read water level
      delay(3000); // Wait for 3 seconds
    }


    digitalWrite(WATER_SENSOR_POWER_PIN, LOW);  // Turn off the sensor


    waterLevelFinal = sum / 5; // Calculate average


    if (waterLevelFinal < level25) {
      waterLevelFinal = map(waterLevelFinal, WATER_LEVEL_MIN, level25, 0, 25);
    } else if (waterLevelRaw < level50) {
      waterLevelFinal = map(waterLevelFinal, level25, level50, 25, 50);
    } else if (waterLevelRaw < level75) {
      waterLevelFinal = map(waterLevelFinal, level50, level75, 50, 75);
    } else {
      waterLevelFinal = map(waterLevelFinal, level75, WATER_LEVEL_MAX, 75, 100);
    }


    waterLevelFinal = constrain(waterLevelFinal, 0, 100); // Constrain to 0-100%



    // Print the final water level
    Serial.print("Final Water Level: ");
    Serial.println(waterLevelFinal);


    lastCheck = millis(); // Update the last check time
  }


// Convert raw readings to percentage based on calibration data
  if (waterLevelRaw < level25) {
    waterLevelPercent = map(waterLevelRaw, WATER_LEVEL_MIN, level25, 0, 25);
  } else if (waterLevelRaw < level50) {
    waterLevelPercent = map(waterLevelRaw, level25, level50, 25, 50);
  } else if (waterLevelRaw < level75) {
    waterLevelPercent = map(waterLevelRaw, level50, level75, 50, 75);
  } else {
    waterLevelPercent = map(waterLevelRaw, level75, WATER_LEVEL_MAX, 75, 100);
  }


  waterLevelPercent = constrain(waterLevelPercent, 0, 100); // Constrain to 0-100%



  if (isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }


  DateTime now = rtc.now(); // Get current time


  // Print the humidity, ambient temperature and time
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.print(" - Humidity: ");
  Serial.print(humidity);
  Serial.print(" %, Ambient Temp: ");
  Serial.print(ambientTemp);
  Serial.print(" C");
  Serial.print(", Water Level: ");
  Serial.print(waterLevel);
  Serial.print(", Water Level Percent: ");
  Serial.print(waterLevelPercent);
  Serial.print(" %");
  Serial.print(", Soil Moisture: ");
  Serial.println(soilMoisture);


  // Control the relays based on temperature
  if (ambientTemp > TEMP_HIGH_THRESHOLD) {
    digitalWrite(RELAY_PIN3, LOW); // Turn on relay
    digitalWrite(RELAY_PIN4, LOW); // Turn on relay 
  } else if (ambientTemp <= TEMP_LOW_THRESHOLD) {
    digitalWrite(RELAY_PIN3, HIGH); // Turn off relay 
    digitalWrite(RELAY_PIN4, HIGH); // Turn off relay 
  }
}

