#include <Wire.h>
#include <DHT.h>
#include "RTClib.h"
#include <Adafruit_MLX90614.h>
#include <SPI.h>
#include <SD.h>

#define DHTPIN 0         // DHT11 connected to GPIO0
#define DHTTYPE DHT11    // DHT 11
#define RELAY_PIN1 4     // Relay connected 
#define RELAY_PIN2 16    // Relay connected
#define RELAY_PIN3 17    // Relay connected
#define RELAY_PIN4 5     // Relay connected
#define WATER_LEVEL_PIN 35  // Water level sensor connected to GPIO35
#define WATER_SENSOR_POWER_PIN 32  // Power pin for the water level sensor connected to GPIO32
#define SOIL_MOISTURE_PIN 34 // Soil moisture sensor connected to GPIO34
#define CS_PIN 13 // SD card pin definitions

DHT dht(DHTPIN, DHTTYPE);
RTC_DS3231 rtc;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Global Variables
File logFile;
int level25, level50, level75;
unsigned long waterLevelCheckTime;
int WATER_LEVEL_MAX, WATER_LEVEL_MIN;
float TEMP_LOW_THRESHOLD, TEMP_HIGH_THRESHOLD;

void setup() {
  Serial.begin(115200);

  // Initialize the DHT
  dht.begin();
 
  // Initialize the MLX
  mlx.begin();

  // Initialize the SD
  SD.begin(CS_PIN);

  // Create "Logs" directory if it doesn't exist
  if (!SD.exists("/Logs")) {
    SD.mkdir("/Logs");
  }

  // Create "Logs" directory if it doesn't exist
  if (!SD.exists("/Config")) {
    SD.mkdir("/Config");
  }

  // Initialize the RTC
  rtc.begin();

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // Set the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Get the current date and time
  DateTime now = rtc.now();

  // Format the date and time
  char dateTimeString[20]; // Adjust the size based on your date and time format
  sprintf(dateTimeString, "%04d/%02d/%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());

  // Create the log message
  String logMessage = String(dateTimeString) + " - Starting Up..";

  // Write to SD card and Serial
  writeToSDCard(logMessage.c_str());
  Serial.println(logMessage);

  pinMode(RELAY_PIN1, OUTPUT); // Set the relay pin as output
  pinMode(RELAY_PIN2, OUTPUT); // Set the relay pin as output
  pinMode(RELAY_PIN3, OUTPUT); // Set the relay pin as output
  pinMode(RELAY_PIN4, OUTPUT); // Set the relay pin as output
  pinMode(WATER_LEVEL_PIN, INPUT); // Set the water level pin as input
  pinMode(SOIL_MOISTURE_PIN, INPUT); // Set the soil moisture sensor pin as input
  pinMode(WATER_SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(WATER_SENSOR_POWER_PIN, LOW);  // Initially turn off the water level sensor
  //digitalWrite(WATER_SENSOR_POWER_PIN, HIGH); // Initially turn on the water level sensor

  // Turn off relays initially
  digitalWrite(RELAY_PIN1, HIGH);
  digitalWrite(RELAY_PIN2, HIGH);
  digitalWrite(RELAY_PIN3, HIGH);
  digitalWrite(RELAY_PIN4, HIGH);

  // Load configuration
  loadConfiguration("/Config/config.txt");
}

void loop() {
  delay(1000); // Wait a few seconds between measurements

  static unsigned long lastCheck = 0;
  unsigned long waterLevelCheckTime = 10000; // Delay for Logs, 60000=1 minute

  DateTime now = rtc.now(); // Get current time
  float humidity = dht.readHumidity(); // Reading humidity
  float ambientTemp = mlx.readAmbientTempC(); // Reading ambient temperature
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN); // Reading soil moisture
  int waterLevelRaw = analogRead(WATER_LEVEL_PIN);   // Reading water level
  int waterLevel;

  // Water Sensor Calibration data
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

    waterLevel = sum / 5; // Calculate average

    if (waterLevel < level25) {
      waterLevel = map(waterLevel, WATER_LEVEL_MIN, level25, 0, 25);
    } else if (waterLevelRaw < level50) {
      waterLevel = map(waterLevel, level25, level50, 25, 50);
    } else if (waterLevelRaw < level75) {
      waterLevel = map(waterLevel, level50, level75, 50, 75);
    } else {
      waterLevel = map(waterLevel, level75, WATER_LEVEL_MAX, 75, 100);
    }

    waterLevel = constrain(waterLevel, 0, 100); // Constrain to 0-100%

    if (isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
    }

    // Create data string
    String dataString = "";
    dataString += String(now.year(), DEC) + "/";
    dataString += String(now.month(), DEC) + "/";
    dataString += String(now.day(), DEC) + " ";
    dataString += String(now.hour(), DEC) + ":";
    dataString += String(now.minute(), DEC) + ":";
    dataString += String(now.second(), DEC) + " - Humidity: ";
    dataString += String(humidity) + " %, Ambient Temp: ";
    dataString += String(ambientTemp) + " C, Water Level Percent: ";
    dataString += String(waterLevel) + " %";
    dataString += ", Soil Moisture: " + String(soilMoisture);

    // Print to Serial
    Serial.println(dataString);
    
    writeToSDCard(dataString.c_str());


    lastCheck = millis(); // Update the last check time
  }

  // Control the relays based on temperature
  if (ambientTemp > TEMP_HIGH_THRESHOLD) {
    digitalWrite(RELAY_PIN3, LOW); // Turn on relay
    digitalWrite(RELAY_PIN4, LOW); // Turn on relay 
  } else if (ambientTemp <= TEMP_LOW_THRESHOLD) {
    digitalWrite(RELAY_PIN3, HIGH); // Turn off relay 
    digitalWrite(RELAY_PIN4, HIGH); // Turn off relay 
  }
}

void writeToSDCard(const char * dataString) {
    Serial.println("Appending to file: /Logs/log.txt");

    File logFile = SD.open("/Logs/log.txt", FILE_APPEND);
    if(!logFile){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(logFile.println(dataString)){
        Serial.println("Data appended to file");
    } else {
        Serial.println("Append failed");
    }
    logFile.close();
}

void loadConfiguration(const char* filename) {
  Serial.print("Loading configuration from: ");
  Serial.println(filename);

  File configFile = SD.open(filename);
  if (!configFile) {
    Serial.println("Failed to open configuration file");
    return;
  }

  while (configFile.available()) {
    String line = configFile.readStringUntil('\n');
    line.trim(); // Trim whitespace from the line

    if (line.length() == 0 || line.startsWith("#")) {
      // Skip empty lines and comments
      continue;
    }

    int index = line.indexOf('='); // Search for the position of the first '=' character in the line
    // Check if the '=' character is found in the line
    if (index == -1) {
      // If '=' is not found, print a message to the Serial Monitor
      Serial.println("No '=' found in line, skipping");
      
      // Skip the rest of the loop iteration because the line does not contain
      // a valid key-value pair (i.e., it's not in the expected "key=value" format)
      continue;
    }

    String key = line.substring(0, index);
    String value = line.substring(index + 1);

    Serial.print("Key: ");
    Serial.print(key);
    Serial.print(", Value: ");
    Serial.println(value);

    if (key == "level25") {
      level25 = value.toInt();
    } else if (key == "level50") {
      level50 = value.toInt();
    } else if (key == "level75") {
      level75 = value.toInt();
    } else if (key == "waterLevelCheckTime") {
      waterLevelCheckTime = value.toInt();
    } else if (key == "WATER_LEVEL_MAX") {
      WATER_LEVEL_MAX = value.toInt();
    } else if (key == "WATER_LEVEL_MIN") {
      WATER_LEVEL_MIN = value.toInt();
    } else if (key == "TEMP_LOW_THRESHOLD") {
      TEMP_LOW_THRESHOLD = value.toFloat();
    } else if (key == "TEMP_HIGH_THRESHOLD") {
      TEMP_HIGH_THRESHOLD = value.toFloat();
    }
  }
  configFile.close();
}