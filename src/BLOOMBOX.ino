#include <Wire.h>
#include <DHT.h>
#include "RTClib.h"
#include <Adafruit_MLX90614.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>

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
char dateTimeString[20]; // Adjust the size based on your date and time format
static unsigned long lastCheck = 0;

float tempLowThreshold, tempHighThreshold;
float humidityLowThreshold, humidityHighThreshold;
int lightHours;
int soilMoistureLowThreshold, soilMoistureHighThreshold;
int currentStageDuration;

float humidity; // Variable to read humidity
float ambientTemp; // Variable to read ambient temperature
int soilMoisture; // Variable to read soil moisture
int waterLevelRaw; // Variable to read water level
int waterLevel; // Variable to convert waterLevelRaw to percentage value
int lastRecordedDay;
DateTime now ; // Variable to get current time


// Variables from config.txt

String WIFI_SSID, WIFI_PASSWORD; // WiFi credentials

unsigned long LOG_INTERVAL;

int LIGHT_START_HOUR;

//General Plant Info
String PLANT_SPECIES; // Plant species for display
String CURRENT_GROWTH_STAGE; // Current growth stage
int DAYS_IN_CURRENT_STAGE; // Days in the current stage

// Temperature settings for different stages
float SEEDLING_TEMP_LOW_THRESHOLD, SEEDLING_TEMP_HIGH_THRESHOLD;
float VEGETATIVE_TEMP_LOW_THRESHOLD, VEGETATIVE_TEMP_HIGH_THRESHOLD;
float FLOWERING_TEMP_LOW_THRESHOLD, FLOWERING_TEMP_HIGH_THRESHOLD;
float FRUITING_TEMP_LOW_THRESHOLD, FRUITING_TEMP_HIGH_THRESHOLD;

// Humidity settings for different stages
float SEEDLING_HUMIDITY_LOW_THRESHOLD, SEEDLING_HUMIDITY_HIGH_THRESHOLD;
float VEGETATIVE_HUMIDITY_LOW_THRESHOLD, VEGETATIVE_HUMIDITY_HIGH_THRESHOLD;
float FLOWERING_HUMIDITY_LOW_THRESHOLD, FLOWERING_HUMIDITY_HIGH_THRESHOLD;
float FRUITING_HUMIDITY_LOW_THRESHOLD, FRUITING_HUMIDITY_HIGH_THRESHOLD;

// Light settings for different stages
int SEEDLING_LIGHT_ON_DURATION_HOURS;
int VEGETATIVE_LIGHT_ON_DURATION_HOURS;
int FLOWERING_LIGHT_ON_DURATION_HOURS;
int FRUITING_LIGHT_ON_DURATION_HOURS;

// Soil humidity settings for different stages
float SEEDLING_SOIL_MOISTURE_LOW_THRESHOLD, SEEDLING_SOIL_MOISTURE_HIGH_THRESHOLD;
float VEGETATIVE_SOIL_MOISTURE_LOW_THRESHOLD, VEGETATIVE_SOIL_MOISTURE_HIGH_THRESHOLD;
float FLOWERING_SOIL_MOISTURE_LOW_THRESHOLD, FLOWERING_SOIL_MOISTURE_HIGH_THRESHOLD;
float FRUITING_SOIL_MOISTURE_LOW_THRESHOLD, FRUITING_SOIL_MOISTURE_HIGH_THRESHOLD;

// Duration in days for different stages
int SEEDLING_STAGE_DURATION_DAYS;
int VEGETATIVE_STAGE_DURATION_DAYS;
int FLOWERING_STAGE_DURATION_DAYS;
int FRUITING_STAGE_DURATION_DAYS;

// Water Tank Settings
int WATER_LEVEL_MAX, WATER_LEVEL_MIN; //Settings for minimum and maximum Analog Value for Water Level
int level25, level50, level75; //Settings for percantages values for Water Level

void setup() {
  Serial.begin(115200);

  // Load configuration from SD Card
  loadConfiguration("/Config/config.txt");

  // Call the function to initialize sensors and modules
  initializeSensorsAndModules();

  // Call the function to create directories if not present in the SD card
  createDirectories();

  // Call the function to configure Relays  
  configureRelayPins();

  // Set Pin Mode for Water and Soil Moisture Sensor
  setPinMode();

  // Date and Time Settings
  updateCurrentTime(); // Update current time
  
  // Initialize with the current day  
  lastRecordedDay = now.day(); 

  // Log startup message
  logStartupMessage();

  Serial.println("tempLowThreshold");
  Serial.println(tempLowThreshold);

  Serial.println("tempLowThreshold");
  Serial.println(WIFI_SSID);


  // Call growthStageManager at the beginning of each loop iteration
  growthStageManager();

  // Call the function to connect to WiFi  
  connectToWiFi();
}

void loop() {
  // Call readSensors at the beginning of each loop iteration to read from the sensors
  readSensors();

  Serial.println("tempLowThreshold");
  Serial.println(tempLowThreshold);

  // Call growthStageManager at the beginning of each loop iteration
  growthStageManager();

  // Check if a day has passed
  checkAndUpdateGrowthStage();

  // Log to SD Card the readings for the different sensor and modules
  logToSDCard();

  // Control the relays for ambient changes such as Fan, Light and Water Pump
  controlRelays();
}

void initializeSensorsAndModules() {
  // Initialize the RTC
  rtc.begin();
  if (rtc.lostPower()) {
    Serial.println("RTC lost power");
    // Set the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Initialize the DHT sensor
  dht.begin();

  // Initialize the MLX sensor
  mlx.begin();

  // Initialize the SD card
  SD.begin(CS_PIN);
}

void createDirectories() {
  // Create "Logs" directory if it doesn't exist
  if (!SD.exists("/Logs")) {
    SD.mkdir("/Logs");
  }
  // Create "Config" directory if it doesn't exist
  if (!SD.exists("/Config")) {
    SD.mkdir("/Config");
  }
}

void connectToWiFi() {
  // Create the message
  String wifiConnectMessage = String(dateTimeString) + " - Connecting to:  " + WIFI_SSID + "..";

  unsigned long startAttemptTime = millis(); // Record the start time

  // Write to SD card and Serial
  writeToSDCard(wifiConnectMessage.c_str());
  Serial.println(wifiConnectMessage);
  WiFi.begin(WIFI_SSID.c_str(), WIFI_PASSWORD.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startAttemptTime > 30000) { // 30 seconds timeout
      // Create the failure message
      String wifiFailMessage = String(dateTimeString) + " - Failed to connect to WiFi: " + WIFI_SSID;
    
      // Write to SD card and Serial
      writeToSDCard(wifiFailMessage.c_str());
      Serial.println(wifiFailMessage);
    
      break; // Exit the while loop
    }
    delay(500);
    Serial.print(".");
  }

  // Create the message for successful WiFi connection
  char ipString[16]; // Adjust the size based on IP address format
  sprintf(ipString, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  String wifiConnectedMessage = String(dateTimeString) + " - WiFi connected, IP: " + ipString;

  // Write to SD card and Serial
  writeToSDCard(wifiConnectedMessage.c_str());
  Serial.println(wifiConnectedMessage);
}

void configureRelayPins() {
  pinMode(RELAY_PIN1, OUTPUT); // Set the relay pin as output
  pinMode(RELAY_PIN2, OUTPUT); // Set the relay pin as output
  pinMode(RELAY_PIN3, OUTPUT); // Set the relay pin as output
  pinMode(RELAY_PIN4, OUTPUT); // Set the relay pin as output

  // Turn off relays initially
  digitalWrite(RELAY_PIN1, HIGH);
  digitalWrite(RELAY_PIN2, HIGH);
  digitalWrite(RELAY_PIN3, HIGH);
  digitalWrite(RELAY_PIN4, HIGH);
}

void updateCurrentTime() {
  // Update current time
  now = rtc.now();
}

void logStartupMessage() {
  // Format the date and time
  sprintf(dateTimeString, "%04d/%02d/%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());

  // Create the log message
  String logMessage = String(dateTimeString) + " - Starting Up..";

  // Write to SD card and Serial
  writeToSDCard(logMessage.c_str());
  Serial.println(logMessage);
}

void setPinMode(){
  pinMode(WATER_LEVEL_PIN, INPUT); // Set the water level pin as input
  pinMode(SOIL_MOISTURE_PIN, INPUT); // Set the soil moisture sensor pin as input
  pinMode(WATER_SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(WATER_SENSOR_POWER_PIN, LOW);  // Initially turn off the water level sensor
  //digitalWrite(WATER_SENSOR_POWER_PIN, HIGH); // Initially turn on the water level sensor
}

void readSensors() {
  delay(1000); // Wait a 1 seconds between measurements 

  // Update current time
  updateCurrentTime();

  // Perform sensor readings
  humidity = dht.readHumidity(); // Reading humidity
  ambientTemp = mlx.readAmbientTempC(); // Reading ambient temperature
  soilMoisture = analogRead(SOIL_MOISTURE_PIN); // Reading soil moisture
  waterLevelRaw = analogRead(WATER_LEVEL_PIN);   // Reading water level
}

void readAndProcessWaterLevel() {
    digitalWrite(WATER_SENSOR_POWER_PIN, HIGH);  // When time to log, turn on water level sensor
    delay(30);  // Short delay for power stabilization

    int sum = 0;
    for (int i = 0; i < 5; i++) {
      sum += analogRead(WATER_LEVEL_PIN); // Read water level
      delay(3000); // Wait for 3 seconds
    }

    digitalWrite(WATER_SENSOR_POWER_PIN, LOW);  // Turn off the sensor after the readings

    waterLevel = sum / 5; // Calculate average of multiple mesurements for more accuracy

    // This section maps the raw water level reading to a percentage value.
    if (waterLevel < level25) {
      waterLevel = map(waterLevel, WATER_LEVEL_MIN, level25, 0, 25);
    } else if (waterLevelRaw < level50) {
      waterLevel = map(waterLevel, level25, level50, 25, 50);
    } else if (waterLevelRaw < level75) {
      waterLevel = map(waterLevel, level50, level75, 50, 75);
    } else {
      waterLevel = map(waterLevel, level75, WATER_LEVEL_MAX, 75, 100);
    }
    // After mapping, 'constrain' is used to ensure the water level percentage stays within 0-100%.
    waterLevel = constrain(waterLevel, 0, 100); // Constrain to 0-100%
}


void updateGrowthStage() {
  if (DAYS_IN_CURRENT_STAGE > currentStageDuration) {
    // Logic for stage transition
    if (CURRENT_GROWTH_STAGE == "SEEDLING") {
      CURRENT_GROWTH_STAGE = "VEGETATIVE";
    } else if (CURRENT_GROWTH_STAGE == "VEGETATIVE") {
      CURRENT_GROWTH_STAGE = "FLOWERING";
    } else if (CURRENT_GROWTH_STAGE == "FLOWERING") {
      CURRENT_GROWTH_STAGE = "FRUITING";
    } else if (CURRENT_GROWTH_STAGE == "FRUITING") {
      CURRENT_GROWTH_STAGE = "FLOWERING";
    } 

    DAYS_IN_CURRENT_STAGE = 1; // Reset days in current stage

    growthStageManager(); // Update settings for new stage
  }
}

void growthStageManager() {
  // Determine the current growth stage and set appropriate values
  if (CURRENT_GROWTH_STAGE == "SEEDLING") {
    tempLowThreshold = SEEDLING_TEMP_LOW_THRESHOLD;
    tempHighThreshold = SEEDLING_TEMP_HIGH_THRESHOLD;
    humidityLowThreshold = SEEDLING_HUMIDITY_LOW_THRESHOLD;
    humidityHighThreshold = SEEDLING_HUMIDITY_HIGH_THRESHOLD;
    lightHours = SEEDLING_LIGHT_ON_DURATION_HOURS;
    soilMoistureLowThreshold = SEEDLING_SOIL_MOISTURE_LOW_THRESHOLD;
    soilMoistureHighThreshold = SEEDLING_SOIL_MOISTURE_HIGH_THRESHOLD;
    currentStageDuration = SEEDLING_STAGE_DURATION_DAYS;
  } else if (CURRENT_GROWTH_STAGE == "VEGETATIVE") {
    tempLowThreshold = VEGETATIVE_TEMP_LOW_THRESHOLD;
    tempHighThreshold = VEGETATIVE_TEMP_HIGH_THRESHOLD;
    humidityLowThreshold = VEGETATIVE_HUMIDITY_LOW_THRESHOLD;
    humidityHighThreshold = VEGETATIVE_HUMIDITY_HIGH_THRESHOLD;
    lightHours = VEGETATIVE_LIGHT_ON_DURATION_HOURS;
    soilMoistureLowThreshold = VEGETATIVE_SOIL_MOISTURE_LOW_THRESHOLD;
    soilMoistureHighThreshold = VEGETATIVE_SOIL_MOISTURE_HIGH_THRESHOLD;
    currentStageDuration = VEGETATIVE_STAGE_DURATION_DAYS;
  } else if (CURRENT_GROWTH_STAGE == "FLOWERING") {
    tempLowThreshold = FLOWERING_TEMP_LOW_THRESHOLD;
    tempHighThreshold = FLOWERING_TEMP_HIGH_THRESHOLD;
    humidityLowThreshold = FLOWERING_HUMIDITY_LOW_THRESHOLD;
    humidityHighThreshold = FLOWERING_HUMIDITY_HIGH_THRESHOLD;
    lightHours = FLOWERING_LIGHT_ON_DURATION_HOURS;
    soilMoistureLowThreshold = FLOWERING_SOIL_MOISTURE_LOW_THRESHOLD;
    soilMoistureHighThreshold = FLOWERING_SOIL_MOISTURE_HIGH_THRESHOLD;
    currentStageDuration = FLOWERING_STAGE_DURATION_DAYS;
  } else if (CURRENT_GROWTH_STAGE == "FRUITING") {
    tempLowThreshold = FRUITING_TEMP_LOW_THRESHOLD;
    tempHighThreshold = FRUITING_TEMP_HIGH_THRESHOLD;
    humidityLowThreshold = FRUITING_HUMIDITY_LOW_THRESHOLD;
    humidityHighThreshold = FRUITING_HUMIDITY_HIGH_THRESHOLD;
    lightHours = FRUITING_LIGHT_ON_DURATION_HOURS;
    soilMoistureLowThreshold = FRUITING_SOIL_MOISTURE_LOW_THRESHOLD;
    soilMoistureHighThreshold = FRUITING_SOIL_MOISTURE_HIGH_THRESHOLD;
    currentStageDuration = FRUITING_STAGE_DURATION_DAYS;
  }
}

void checkAndUpdateGrowthStage() {
  // Check if a day has passed
  if (now.day() != lastRecordedDay) {
    DAYS_IN_CURRENT_STAGE++;
    lastRecordedDay = now.day();

    updateGrowthStage(); // Call function to update growth stage if necessary
  }
}

void logToSDCard() {
  // Check if it is time to log based on LOG_INTERVAL
  if (millis() - lastCheck >= LOG_INTERVAL) {

    readAndProcessWaterLevel();

    // Create data string for logs
    String logData = createDataStringForLogs();

    // Print to Serial
    Serial.println(logData);
    
    // Write to SD Card the data string for the logs
    writeToSDCard(logData.c_str());


    lastCheck = millis(); // Update the last check time
  }
}

String createDataStringForLogs() {
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

  return dataString;
}

void controlRelays() {
    if (ambientTemp > tempHighThreshold) {
        digitalWrite(RELAY_PIN3, LOW); // Turn on Fan relay
        digitalWrite(RELAY_PIN4, LOW); // Turn on Fan relay 
    } else if (ambientTemp <= tempLowThreshold) {
        digitalWrite(RELAY_PIN3, HIGH); // Turn off Fan relay 
        digitalWrite(RELAY_PIN4, HIGH); // Turn off Fan relay 
    }

    // Control light based on lightHours
    updateCurrentTime(); // Update current time
    int currentHour = now.hour(); // Get the current hour

    // Assuming lights should be on from (e.g., 6 AM to 6 AM + lightHours)
    int lightEndHour = LIGHT_START_HOUR + lightHours; // Calculate ending hour

    if (currentHour >= LIGHT_START_HOUR && currentHour < lightEndHour) {
        digitalWrite(RELAY_PIN2, LOW); // Turn on Light relay
    } else {
        digitalWrite(RELAY_PIN2, HIGH); // Turn off Light relay
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

    // Get all variables values

    // WiFi Settings
    if (key == "WIFI_SSID") {
      WIFI_SSID = value;
    } else if (key == "WIFI_PASSWORD") {
      WIFI_PASSWORD = value;
    }

    // Log time frequencies
    else if (key == "LOG_INTERVAL") {
      LOG_INTERVAL = value.toInt();
    }

    // Light Settings
    if (key == "LIGHT_START_HOUR") {
      LIGHT_START_HOUR = value.toInt();
    }

    // General Plant Info
    else if (key == "PLANT_SPECIES") {
      PLANT_SPECIES = value;
    } else if (key == "CURRENT_GROWTH_STAGE") {
      CURRENT_GROWTH_STAGE = value;
    } else if (key == "DAYS_IN_CURRENT_STAGE") {
      DAYS_IN_CURRENT_STAGE = value.toInt();
    }

    // Temperature Settings for Different Stages
    else if (key == "SEEDLING_TEMP_LOW_THRESHOLD") {
      SEEDLING_TEMP_LOW_THRESHOLD = value.toFloat();
    } else if (key == "SEEDLING_TEMP_HIGH_THRESHOLD") {
      SEEDLING_TEMP_HIGH_THRESHOLD = value.toFloat();
    } else if (key == "VEGETATIVE_TEMP_LOW_THRESHOLD") {
      VEGETATIVE_TEMP_LOW_THRESHOLD = value.toFloat();
    } else if (key == "VEGETATIVE_TEMP_HIGH_THRESHOLD") {
      VEGETATIVE_TEMP_HIGH_THRESHOLD = value.toFloat();
    } else if (key == "FLOWERING_TEMP_LOW_THRESHOLD") {
      FLOWERING_TEMP_LOW_THRESHOLD = value.toFloat();
    } else if (key == "FLOWERING_TEMP_HIGH_THRESHOLD") {
      FLOWERING_TEMP_HIGH_THRESHOLD = value.toFloat();
    } else if (key == "FRUITING_TEMP_LOW_THRESHOLD") {
      FRUITING_TEMP_LOW_THRESHOLD = value.toFloat();
    } else if (key == "FRUITING_TEMP_HIGH_THRESHOLD") {
      FRUITING_TEMP_HIGH_THRESHOLD = value.toFloat();
    }

    // Humidity Settings for Different Stages
    else if (key == "SEEDLING_HUMIDITY_LOW_THRESHOLD") {
      SEEDLING_HUMIDITY_LOW_THRESHOLD = value.toFloat();
    } else if (key == "SEEDLING_HUMIDITY_HIGH_THRESHOLD") {
      SEEDLING_HUMIDITY_HIGH_THRESHOLD = value.toFloat();
    } else if (key == "VEGETATIVE_HUMIDITY_LOW_THRESHOLD") {
      VEGETATIVE_HUMIDITY_LOW_THRESHOLD = value.toFloat();
    } else if (key == "VEGETATIVE_HUMIDITY_HIGH_THRESHOLD") {
      VEGETATIVE_HUMIDITY_HIGH_THRESHOLD = value.toFloat();
    } else if (key == "FLOWERING_HUMIDITY_LOW_THRESHOLD") {
      FLOWERING_HUMIDITY_LOW_THRESHOLD = value.toFloat();
    } else if (key == "FLOWERING_HUMIDITY_HIGH_THRESHOLD") {
      FLOWERING_HUMIDITY_HIGH_THRESHOLD = value.toFloat();
    } else if (key == "FRUITING_HUMIDITY_LOW_THRESHOLD") {
      FRUITING_HUMIDITY_LOW_THRESHOLD = value.toFloat();
    } else if (key == "FRUITING_HUMIDITY_HIGH_THRESHOLD") {
      FRUITING_HUMIDITY_HIGH_THRESHOLD = value.toFloat();
    }

    // Light Settings for Different Stages
    else if (key == "SEEDLING_LIGHT_ON_DURATION_HOURS") {
      SEEDLING_LIGHT_ON_DURATION_HOURS = value.toInt();
    } else if (key == "VEGETATIVE_LIGHT_ON_DURATION_HOURS") {
      VEGETATIVE_LIGHT_ON_DURATION_HOURS = value.toInt();
    } else if (key == "FLOWERING_LIGHT_ON_DURATION_HOURS") {
      FLOWERING_LIGHT_ON_DURATION_HOURS = value.toInt();
    } else if (key == "FRUITING_LIGHT_ON_DURATION_HOURS") {
      FRUITING_LIGHT_ON_DURATION_HOURS = value.toInt();
    }

    // Soil Humidity Settings for Different Stages
    else if (key == "SEEDLING_SOIL_MOISTURE_LOW_THRESHOLD") {
      SEEDLING_SOIL_MOISTURE_LOW_THRESHOLD = value.toFloat();
    } else if (key == "SEEDLING_SOIL_MOISTURE_HIGH_THRESHOLD") {
      SEEDLING_SOIL_MOISTURE_HIGH_THRESHOLD = value.toFloat();
    } else if (key == "VEGETATIVE_SOIL_MOISTURE_LOW_THRESHOLD") {
      VEGETATIVE_SOIL_MOISTURE_LOW_THRESHOLD = value.toFloat();
    } else if (key == "VEGETATIVE_SOIL_MOISTURE_HIGH_THRESHOLD") {
      VEGETATIVE_SOIL_MOISTURE_HIGH_THRESHOLD = value.toFloat();
    } else if (key == "FLOWERING_SOIL_MOISTURE_LOW_THRESHOLD") {
      FLOWERING_SOIL_MOISTURE_LOW_THRESHOLD = value.toFloat();
    } else if (key == "FLOWERING_SOIL_MOISTURE_HIGH_THRESHOLD") {
      FLOWERING_SOIL_MOISTURE_HIGH_THRESHOLD = value.toFloat();
    } else if (key == "FRUITING_SOIL_MOISTURE_LOW_THRESHOLD") {
      FRUITING_SOIL_MOISTURE_LOW_THRESHOLD = value.toFloat();
    } else if (key == "FRUITING_SOIL_MOISTURE_HIGH_THRESHOLD") {
      FRUITING_SOIL_MOISTURE_HIGH_THRESHOLD = value.toFloat();
    }

    // Water Tank Settings
    else if (key == "WATER_LEVEL_MAX") {
      WATER_LEVEL_MAX = value.toInt();
    } else if (key == "WATER_LEVEL_MIN") {
      WATER_LEVEL_MIN = value.toInt();
    } else if (key == "level25") {
      level25 = value.toInt();
    } else if (key == "level50") {
      level50 = value.toInt();
    } else if (key == "level75") {
      level75 = value.toInt();
    }
  }
  configFile.close();
}