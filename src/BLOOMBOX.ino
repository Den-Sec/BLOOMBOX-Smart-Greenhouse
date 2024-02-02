// Include
  #include <Arduino.h>
  #include <Wire.h>
  #include <DHT.h>
  #include "RTClib.h"
  #include <Adafruit_MLX90614.h>
  #include <SPI.h>
  #include <WiFi.h>
  #include <HTTPUpdate.h>
  #include <WiFiUdp.h>
  #include "FS.h"
  #include "SD_MMC.h"

// Include

//Define
  #define DHTPIN 0         // DHT11 connected to GPIO0
  #define DHTTYPE DHT11    // DHT 11
  #define RELAY_PIN1 32     // Relay connected to Dev Board
  #define RELAY_PIN2 33    // Relay connected to Dev Board
  #define WATER_LEVEL_PIN 35  // Water level sensor connected to GPIO35
  #define WATER_SENSOR_POWER_PIN 32  // Power pin for the water level sensor connected to GPIO32
  #define SOIL_MOISTURE_PIN 34 // Soil moisture sensor connected to GPIO34
  //PIN SD MMC
  #define SD_CLK 14 // SD connected to Dev Board
  #define SD_CMD 15 // SD connected to Dev Board
  #define SD_D0 2 // SD connected to Dev Board
//Define

// DEBUG
  #define DEBUG // Uncomment this line to enable debug output.

  #ifdef DEBUG
    #define DEBUG_PRINTLN(x)          Serial.println(x)
    #define DEBUG_PRINT(x)            Serial.print(x)
    #define DEBUG_PRINTF(x,y)         Serial.printf(x,y)
    #define DEBUG_PRINTFF(x,y,z)      Serial.printf(x,y,z)
  #else
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTF(x,y)
    #define DEBUG_PRINTFF(x,y,z)
  #endif
// DEBUG

// Firmware
  String FIRMWARE_VERSION = "v1.2"; // Your current firmware version

  const char* versionFileUrl = "https://raw.githubusercontent.com/Den-Sec/BLOOMBOX-Smart-Greenhouse/main/src/fota/version.txt";
  const char* firmwareBaseURL = "https://raw.githubusercontent.com/Den-Sec/BLOOMBOX-Smart-Greenhouse/main/src/fota/BLOOMBOX-";
// Firmware

//Sensors
  DHT dht(DHTPIN, DHTTYPE);
  RTC_DS3231 rtc;
  Adafruit_MLX90614 mlx = Adafruit_MLX90614();
//Sensors

// Global Variables
  DateTime dateTime; // Global variable to keep track of the last log time
  char dateTimeString[20]; // Adjust the size based on your date and time format
  static unsigned long lastLogCheck = 0;
// Global Variables

// Task handles for managing tasks
  TaskHandle_t Task0;
  TaskHandle_t Task1;
// Task handles for managing tasks

//Wi-Fi Variables
  WiFiClient wifiClient;
  static unsigned long lastWiFiCheck = 0;
  const unsigned long checkWiFi = 3600000; // 1 hour in millisecond
//Wi-Fi Variables

//Variables for Controls
  File logFile;
  float tempLowThreshold, tempHighThreshold;
  float humidityLowThreshold, humidityHighThreshold;
  int lightHours;
  int soilMoistureLowThreshold, soilMoistureHighThreshold;
  int currentStageDuration;
  bool relayFanState = false; // Assuming LOW means ON for fans
  bool relayLightState = false; // Assuming LOW means ON for lights
//Variables for Controls

// Variables to Read Sensor Values
  float humidity; // Variable to read humidity
  float ambientTemp; // Variable to read ambient temperature
  int soilMoisture; // Variable to read soil moisture
  int waterLevelRaw; // Variable to read water level
  int waterLevel; // Variable to convert waterLevelRaw to percentage value
  int lastRecordedDay;
  DateTime now ; // Variable to get current time
// Variables to Read Different Values

// Start Variables from config.txt
  String WIFI_SSID, WIFI_PASSWORD, IP_CONFIG, SUBNET_CONFIG, GATEWAY_CONFIG; // WiFi Settings

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
// End Variables from config.txt

void setup() {
  #ifdef DEBUG
  Serial.begin(115200); // Initialize serial communication at 115200 baud rate
  DEBUG_PRINTLN("");
  DEBUG_PRINTLN(F("BLOOMBOX"));

  DEBUG_PRINTLN(F("Firmware Version: "));
  DEBUG_PRINTLN(FIRMWARE_VERSION);
  #endif

  // Call the function to initialize sensors and modules
  initializeSensorsAndModules();

  // Synchronize RTC time on startup
  synchronizeRTCTime();

  // Call the function to create directories if not present in the SD card
  createDirectories();

  // Load configuration from SD Card
  loadConfiguration();

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

  // Call growthStageManager at the beginning of each loop iteration
  growthStageManager();

  // Call the function to connect to WiFi  
  connectToWiFi();

  // Create tasks pinned to specific cores
  xTaskCreatePinnedToCore(Task0_Core0, "HighFreqTasks", 10000, NULL, 1, &Task0, 0);
  xTaskCreatePinnedToCore(Task1_Core1, "LowFreqTasks", 10000, NULL, 1, &Task1, 1);
}

void loop() {
  // Empty loop since tasks are managed by FreeRTOS
}

void Task0_Core0(void *pvParameters) {
  for (;;) {
    readSensors();
    logToSDCard();
    ambientControl();
    delay(1000); // 1-second delay delay to prevent WDT reset (Can be set as small as 10)
  }
}

void Task1_Core1(void *pvParameters) {
  for (;;) {
    checkAndUpdateGrowthStage();

    // WiFi check every hour
    if (millis() - lastWiFiCheck >= checkWiFi) {
      checkAndConnectToWiFi();
      lastWiFiCheck = millis();
    }

    // Firmware update check at 3 AM
    updateCurrentTime(); // Ensure the current time is updated before checking
    if (now.hour() == 3 && now.minute() == 0) {
      static int lastUpdateDay = -1;
      if (lastUpdateDay != now.day()) {
        checkForFirmwareUpdates();
        lastUpdateDay = now.day();
      }
    }

    delay(1000); // 1-second delay to prevent constant looping and allow time checks
  }
}

void checkForFirmwareUpdates() {
  if (!checkAndConnectToWiFi()) {
    logUpdateMessage("Not connected to WiFi, skipping firmware update check.");
    return;
  }

  String latestVersion = getLatestFirmwareVersion();
  if (latestVersion.isEmpty()) {
    logUpdateMessage("Failed to check for firmware update.");
    return;
  }

  if (latestVersion == FIRMWARE_VERSION) {
    logUpdateMessage("Already on the latest firmware version.");
    return;
  }

  performFirmwareUpdate(latestVersion);
}

bool checkAndConnectToWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    String wifiDisconnectedMessage = String(dateTimeString) + " - Disconnected from:  " + WIFI_SSID + ". Attempting to reconnect..";
    // Write to SD card and Serial
    writeToSDCard(wifiDisconnectedMessage.c_str());
    DEBUG_PRINTLN(wifiDisconnectedMessage);
    // Function to connect to WiFi
    connectToWiFi();
  } else {
    return true;
  }
}

String getLatestFirmwareVersion() {
  HTTPClient httpClient;
  httpClient.begin(wifiClient, versionFileUrl); // versionFileUrl should be defined globally

  int httpCode = httpClient.GET();
  if (httpCode != HTTP_CODE_OK) {
    httpClient.end();
    return ""; // Return an empty string to indicate failure
  }

  String latestVersion = httpClient.getString();
  httpClient.end();
  latestVersion.trim();
  return latestVersion;
}

void logUpdateMessage(const String& message) {
  String logMessage = String(dateTimeString) + " - " + message; // dateTimeString should be updated with the current date-time
  DEBUG_PRINTLN(logMessage);
  writeToSDCard(logMessage.c_str()); // Assuming writeToSDCard is implemented to handle SD card writing
}

void performFirmwareUpdate(const String& version) {
  String firmwareUrl = firmwareBaseURL + version + ".bin";
  logUpdateMessage("New firmware version found: " + version + ". Starting firmware update from: " + firmwareUrl);

  t_httpUpdate_return ret = httpUpdate.update(wifiClient, firmwareUrl);
  switch (ret) {
    case HTTP_UPDATE_FAILED:
      logUpdateMessage("HTTP_UPDATE_FAILED Error: " + String(httpUpdate.getLastError()) + " " + httpUpdate.getLastErrorString());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      logUpdateMessage("HTTP_UPDATE_NO_UPDATES");
      break;
    case HTTP_UPDATE_OK:
      logUpdateMessage("HTTP_UPDATE_OK - Device will restart.");
      break;
  }
}

void synchronizeRTCTime() {
  // Configure NTP and wait for a response
  configTime(0, 0, "pool.ntp.org", "time.nist.gov"); // Configure NTP
  delay(1000); // Wait a bit for NTP response

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    // If NTP time couldn't be obtained, log the failure and set the RTC to compile time
    DEBUG_PRINTLN("Failed to obtain time from NTP, setting RTC to compile time");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set RTC to compile time as fallback
  } else {
    // If NTP time was successfully obtained, set the RTC to the NTP time
    rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
    DEBUG_PRINTLN("RTC set to NTP time successfully");
  }
}

void initializeSensorsAndModules() {
  // Initialize the RTC
  rtc.begin();
  if (rtc.lostPower()) {
    String lostRTCBatteryMessage = "RTC lost power, attempting to set time using NTP...";
    // Write to SD card and Serial
    writeToSDCard(lostRTCBatteryMessage.c_str());
    DEBUG_PRINTLN(lostRTCBatteryMessage);

    synchronizeRTCTime();
  }

  // Initialize the DHT sensor
  dht.begin();

  // Initialize the MLX sensor
  mlx.begin();

  // Initialize the SD card
    if(! SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0)) {
      Serial.println("Pin change failed!");
      return;
  }
  const char * mountpoint="/sdcard";
  bool mode1bit=true;
  bool format_if_mount_failed=false;
  int sdmmc_frequency=BOARD_MAX_SDMMC_FREQ;
  uint8_t maxOpenFiles = 5;

  if(!SD_MMC.begin(mountpoint, mode1bit, format_if_mount_failed, sdmmc_frequency, maxOpenFiles)){
      Serial.println("Card Mount Failed");
      return;
  }
    uint8_t cardType = SD_MMC.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD_MMC card attached");
        return;
    }

    Serial.print("SD_MMC Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);


}

void createDirectories() {
  // Create "Logs" directory if it doesn't exist
  if (!SD_MMC.exists("/Logs")) {
    SD_MMC.mkdir("/Logs");
  }
  // Create "Config" directory if it doesn't exist
  if (!SD_MMC.exists("/Config")) {
    SD_MMC.mkdir("/Config");
    DEBUG_PRINTLN("config was made!!!!!!!");
  }
}

void connectToWiFi() {
  // Create the message
  String wifiConnectMessage = String(dateTimeString) + " - Connecting to:  " + WIFI_SSID + "..";

  // Record the start time of the connection attempt
  unsigned long startAttemptTime = millis(); // Record the start time

  // Write to SD card and Serial
  writeToSDCard(wifiConnectMessage.c_str());
  DEBUG_PRINTLN(wifiConnectMessage);

  // Check if IP_CONFIG is not set to "dhcp"
  if (IP_CONFIG != "dhcp" || IP_CONFIG != "DHCP") {
    IPAddress staticIP, subnet, gateway;
    staticIP.fromString(IP_CONFIG); // Convert IP_CONFIG String to IPAddress
    gateway.fromString(GATEWAY_CONFIG); // Example gateway
    subnet.fromString(SUBNET_CONFIG); // Example subnet mask

    // Apply the static IP configuration
    WiFi.config(staticIP, gateway, subnet);
  }

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 3) {
    // Begin WiFi connection
    WiFi.begin(WIFI_SSID.c_str(), WIFI_PASSWORD.c_str());
    attempts++;
    delay(5000); // Wait 5 seconds before retrying
  }

  // If connected, create and log the success message
  if (WiFi.status() == WL_CONNECTED) {
    char ipString[16]; // Buffer for the IP address string
    sprintf(ipString, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
    String wifiConnectedMessage = String(dateTimeString) + " - WiFi connected, IP: " + ipString;
    writeToSDCard(wifiConnectedMessage.c_str());
    DEBUG_PRINTLN(wifiConnectedMessage);
  } else {
    // Create and log the failure message
    String wifiFailMessage = String(dateTimeString) + " - Failed to connect to " + WIFI_SSID + " after 3 attempts. ";
    writeToSDCard(wifiFailMessage.c_str());
    DEBUG_PRINTLN(wifiFailMessage);
  }
}

void configureRelayPins() {
  pinMode(RELAY_PIN1, OUTPUT); // Set the relay pin as output
  pinMode(RELAY_PIN2, OUTPUT); // Set the relay pin as output

  // Turn off relays initially
  digitalWrite(RELAY_PIN1, HIGH);
  digitalWrite(RELAY_PIN2, HIGH);
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
  DEBUG_PRINTLN(logMessage);
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
    if (CURRENT_GROWTH_STAGE.equals("SEEDLING")) {
      CURRENT_GROWTH_STAGE = "VEGETATIVE";
    } else if (CURRENT_GROWTH_STAGE.equals("VEGETATIVE")) {
      CURRENT_GROWTH_STAGE = "FLOWERING";
    } else if (CURRENT_GROWTH_STAGE.equals("FLOWERING")) {
      CURRENT_GROWTH_STAGE = "FRUITING";
    } else if (CURRENT_GROWTH_STAGE.equals("FRUITING")) {
      CURRENT_GROWTH_STAGE = "FLOWERING";
    } 

    DAYS_IN_CURRENT_STAGE = 1; // Reset days in current stage
    saveIncrementalUpdate("CURRENT_GROWTH_STAGE", CURRENT_GROWTH_STAGE);
    saveIncrementalUpdate("DAYS_IN_CURRENT_STAGE", String(DAYS_IN_CURRENT_STAGE));

    growthStageManager(); // Update settings for new stage
  }
}

void growthStageManager() {
  // Determine the current growth stage and set appropriate values
  if (CURRENT_GROWTH_STAGE.equals("SEEDLING")) {
    tempLowThreshold = SEEDLING_TEMP_LOW_THRESHOLD;
    tempHighThreshold = SEEDLING_TEMP_HIGH_THRESHOLD;
    humidityLowThreshold = SEEDLING_HUMIDITY_LOW_THRESHOLD;
    humidityHighThreshold = SEEDLING_HUMIDITY_HIGH_THRESHOLD;
    lightHours = SEEDLING_LIGHT_ON_DURATION_HOURS;
    soilMoistureLowThreshold = SEEDLING_SOIL_MOISTURE_LOW_THRESHOLD;
    soilMoistureHighThreshold = SEEDLING_SOIL_MOISTURE_HIGH_THRESHOLD;
    currentStageDuration = SEEDLING_STAGE_DURATION_DAYS;
  } else if (CURRENT_GROWTH_STAGE.equals("VEGETATIVE")) {
    tempLowThreshold = VEGETATIVE_TEMP_LOW_THRESHOLD;
    tempHighThreshold = VEGETATIVE_TEMP_HIGH_THRESHOLD;
    humidityLowThreshold = VEGETATIVE_HUMIDITY_LOW_THRESHOLD;
    humidityHighThreshold = VEGETATIVE_HUMIDITY_HIGH_THRESHOLD;
    lightHours = VEGETATIVE_LIGHT_ON_DURATION_HOURS;
    soilMoistureLowThreshold = VEGETATIVE_SOIL_MOISTURE_LOW_THRESHOLD;
    soilMoistureHighThreshold = VEGETATIVE_SOIL_MOISTURE_HIGH_THRESHOLD;
    currentStageDuration = VEGETATIVE_STAGE_DURATION_DAYS;
  } else if (CURRENT_GROWTH_STAGE.equals("FLOWERING")) {
    tempLowThreshold = FLOWERING_TEMP_LOW_THRESHOLD;
    tempHighThreshold = FLOWERING_TEMP_HIGH_THRESHOLD;
    humidityLowThreshold = FLOWERING_HUMIDITY_LOW_THRESHOLD;
    humidityHighThreshold = FLOWERING_HUMIDITY_HIGH_THRESHOLD;
    lightHours = FLOWERING_LIGHT_ON_DURATION_HOURS;
    soilMoistureLowThreshold = FLOWERING_SOIL_MOISTURE_LOW_THRESHOLD;
    soilMoistureHighThreshold = FLOWERING_SOIL_MOISTURE_HIGH_THRESHOLD;
    currentStageDuration = FLOWERING_STAGE_DURATION_DAYS;
  } else if (CURRENT_GROWTH_STAGE.equals("FRUITING")) {
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
  updateCurrentTime();
  // Check if a day has passed
  if (now.day() != lastRecordedDay) {
    DAYS_IN_CURRENT_STAGE++;
    lastRecordedDay = now.day();
    
    saveIncrementalUpdate("DAYS_IN_CURRENT_STAGE", String(DAYS_IN_CURRENT_STAGE));
  }
  updateGrowthStage(); // Call function to update growth stage if necessary
}

void logToSDCard() {
  DateTime now = rtc.now(); // Get the current time from the RTC

  if (!now.isValid()) {
    DEBUG_PRINTLN("RTC time is not valid.");
    return;
  }

  long nowSeconds = now.unixtime();
  // Ensure 'dateTime' is initialized at program start and updated here
  long lastCheckSeconds = dateTime.unixtime();

  // Check if the log interval has passed
  if ((nowSeconds - lastCheckSeconds) >= LOG_INTERVAL) { // Assuming LOG_INTERVAL is defined in seconds
    readAndProcessWaterLevel();
    String logData = "[" + now.timestamp(DateTime::TIMESTAMP_FULL) + "] " + createDataStringForLogs();
    DEBUG_PRINTLN(logData);
    writeToSDCard(logData.c_str());
    dateTime = now; // Update the last log time
  }
}

/*
void logToSDCard() {
  // Check if it is time to log based on LOG_INTERVAL
  if (millis() - lastLogCheck >= LOG_INTERVAL) {

    readAndProcessWaterLevel();

    // Create data string for logs
    String logData = createDataStringForLogs();

    // Print to Serial
    DEBUG_PRINTLN(logData);
    
    // Write to SD Card the data string for the logs
    writeToSDCard(logData.c_str());


    lastLogCheck = millis(); // Update the last check time
  }
}
*/

String createDataStringForLogs() {
    String dataString = "Plant: " + PLANT_SPECIES;
    dataString += ", Stage: " + CURRENT_GROWTH_STAGE;
    dataString += ", Days in Stage: " + String(DAYS_IN_CURRENT_STAGE);
    dataString += ", Light turn on at: " + String(LIGHT_START_HOUR);
    dataString += ", The log is set at: " + String(LOG_INTERVAL);
    dataString += " - ";

    // Append the rest of your log data
    dataString += " - Humidity: " + String(humidity) + " %, Ambient Temp: " + String(ambientTemp) + " C, Water Level Percent: " + String(waterLevel) + " %" + ", Soil Moisture: " + String(soilMoisture);

    return dataString;
}

void ambientControl() {
  String logMessage;
  updateCurrentTime(); // Update current time
  String currentTime = String(now.year(), DEC) + "/" + String(now.month(), DEC) + "/" + String(now.day(), DEC) + " " + String(now.hour(), DEC) + ":" + String(now.minute(), DEC) + ":" + String(now.second(), DEC);
  int currentHour = now.hour(); // Get the current hour

  bool shouldFanBeOn = (ambientTemp > tempHighThreshold) || (ambientTemp <= tempLowThreshold);
  bool shouldLightBeOn = (currentHour >= LIGHT_START_HOUR) && (currentHour < LIGHT_START_HOUR + lightHours);

  // Fan Relay Control
  if (shouldFanBeOn != relayFanState) {
    digitalWrite(RELAY_PIN1, shouldFanBeOn ? LOW : HIGH); // Update Fan relay
    relayFanState = shouldFanBeOn;
    logMessage = currentTime + " - Fan relays turned " + (shouldFanBeOn ? "ON" : "OFF") + ".";
    writeToSDCard(logMessage.c_str());
  }

  // Light Relay Control
  if (shouldLightBeOn != relayLightState) {
    digitalWrite(RELAY_PIN2, shouldLightBeOn ? LOW : HIGH); // Update Light relay
    relayLightState = shouldLightBeOn;
    logMessage = currentTime + " - Light relay turned " + (shouldLightBeOn ? "ON" : "OFF") + ".";
    writeToSDCard(logMessage.c_str());
  }
}

void writeToSDCard(const char * dataString) {
    DEBUG_PRINTLN("Appending to file: /Logs/log.txt");

    File logFile = SD_MMC.open("/Logs/log.txt", FILE_APPEND);
    if(!logFile){
        DEBUG_PRINTLN("Failed to open file for appending");
        return;
    }
    if(logFile.println(dataString)){
        DEBUG_PRINTLN("Data appended to file");
    } else {
        DEBUG_PRINTLN("Append failed");
    }
    logFile.close();
}

void saveIncrementalUpdate(String key, String value) {
  File updateFile = SD_MMC.open("/Config/updateConfig.txt", FILE_APPEND); // Open for appending
  if (updateFile) {
    updateFile.println(key + "=" + value); // Append the new key-value pair
    updateFile.close();
    DEBUG_PRINTLN("Updated " + key + " in incremental update file");
  } else {
    DEBUG_PRINTLN("Failed to open updateConfig.txt for writing");
  }
}

void loadConfiguration() {

  DEBUG_PRINTLN("Loading configuration from: /Config/config.txt");

  // Load main configuration
  File configFile = SD_MMC.open("/Config/config.txt");
  if (!configFile) {
    DEBUG_PRINTLN("Failed to open configuration file");
    return;
  }

  // Read from the file until there's nothing else in it
  while (configFile.available()) {
    String line = configFile.readStringUntil('\n');
    parseConfigurationLine(line);
  }
  configFile.close();
  // Apply incremental updates
  File updateFile = SD_MMC.open("/Config/updateConfig.txt");
  if (updateFile) {
    while (updateFile.available()) {
      String line = updateFile.readStringUntil('\n');
      parseConfigurationLine(line);
    }
    updateFile.close();

    // Clear the contents of updateConfig.txt after updates are applied
    clearUpdateFile();
  }
}

void clearUpdateFile() {
  // Open the file in write mode to truncate it (clear its contents)
  File updateFile = SD_MMC.open("/Config/updateConfig.txt", FILE_WRITE);
  if (updateFile) {
    updateFile.close(); // Closing the file immediately to clear its contents
    DEBUG_PRINTLN("updateConfig.txt cleared after loading updates.");
  } else {
    DEBUG_PRINTLN("Failed to open updateConfig.txt for clearing.");
  }
}

void parseConfigurationLine(String line) {
  line.trim(); // Trim whitespace from the line

  if (line.length() == 0 || line.startsWith("#")) return;//skip empty lines and comments

  int index = line.indexOf('='); // Search for the position of the first '=' character in the line
  // Check if the '=' character is found in the line
  if (index == -1) {
    // If '=' is not found, print a message to the Serial Monitor
    DEBUG_PRINTLN("No '=' found in line, skipping");
      
    // Skip the rest of the loop iteration because the line does not contain
    // a valid key-value pair (i.e., it's not in the expected "key=value" format)
    return;
  }

  String key = line.substring(0, index);
  String value = line.substring(index + 1);

  key.replace("\r", "");  // Remove carriage return character
  key.trim();             // Trim whitespace from the key
  value.replace("\r", "");  // Remove carriage return character
  value.trim();             // Trim whitespace from the value

  DEBUG_PRINT("Key: ");
  DEBUG_PRINT(key);
  DEBUG_PRINT(", Value: ");
  DEBUG_PRINTLN(value);

  // Get all variables values

  // WiFi Settings
  if (key.equals("WIFI_SSID")) {
    DEBUG_PRINT("WIFI OKKK ");
    WIFI_SSID = value;
  } else if (key.equals("WIFI_PASSWORD")) {
    WIFI_PASSWORD = value;
    DEBUG_PRINT("WIFI pass OKKK ");
  } else if (key.equals("IP_CONFIG")) {
    IP_CONFIG = value;
    DEBUG_PRINT("WIFI ip OKKK ");
  } else if (key.equals("SUBNET_CONFIG")) {
    SUBNET_CONFIG = value;
    DEBUG_PRINT("WIFI subnet OKKK ");
  } else if (key.equals("GATEWAY_CONFIG")) {
    GATEWAY_CONFIG = value;
    DEBUG_PRINT("WIFI gateway OKKK ");
  }

  // Log time frequencies
  else if (key.equals("LOG_INTERVAL")) {
    LOG_INTERVAL = value.toInt();
  }

  // Light Settings
  else if (key.equals("LIGHT_START_HOUR")) {
    LIGHT_START_HOUR = value.toInt();
  }

  // General Plant Info
  else if (key.equals("PLANT_SPECIES")) {
    PLANT_SPECIES = value;
  } else if (key.equals("CURRENT_GROWTH_STAGE")) {
    CURRENT_GROWTH_STAGE = value;
  } else if (key.equals("DAYS_IN_CURRENT_STAGE")) {
    DAYS_IN_CURRENT_STAGE = value.toInt();
  }

  // Temperature Settings for Different Stages
  else if (key.equals("SEEDLING_TEMP_LOW_THRESHOLD")) {
    SEEDLING_TEMP_LOW_THRESHOLD = value.toFloat();
  } else if (key.equals("SEEDLING_TEMP_HIGH_THRESHOLD")) {
    SEEDLING_TEMP_HIGH_THRESHOLD = value.toFloat();
  } else if (key.equals("VEGETATIVE_TEMP_LOW_THRESHOLD")) {
    VEGETATIVE_TEMP_LOW_THRESHOLD = value.toFloat();
  } else if (key.equals("VEGETATIVE_TEMP_HIGH_THRESHOLD")) {
    VEGETATIVE_TEMP_HIGH_THRESHOLD = value.toFloat();
  } else if (key.equals("FLOWERING_TEMP_LOW_THRESHOLD")) {
    FLOWERING_TEMP_LOW_THRESHOLD = value.toFloat();
  } else if (key.equals("FLOWERING_TEMP_HIGH_THRESHOLD")) {
    FLOWERING_TEMP_HIGH_THRESHOLD = value.toFloat();
  } else if (key.equals("FRUITING_TEMP_LOW_THRESHOLD")) {
    FRUITING_TEMP_LOW_THRESHOLD = value.toFloat();
  } else if (key.equals("FRUITING_TEMP_HIGH_THRESHOLD")) {
    FRUITING_TEMP_HIGH_THRESHOLD = value.toFloat();
  }

  // Humidity Settings for Different Stages
  else if (key.equals("SEEDLING_HUMIDITY_LOW_THRESHOLD")) {
    SEEDLING_HUMIDITY_LOW_THRESHOLD = value.toFloat();
  } else if (key.equals("SEEDLING_HUMIDITY_HIGH_THRESHOLD")) {
    SEEDLING_HUMIDITY_HIGH_THRESHOLD = value.toFloat();
  } else if (key.equals("VEGETATIVE_HUMIDITY_LOW_THRESHOLD")) {
    VEGETATIVE_HUMIDITY_LOW_THRESHOLD = value.toFloat();
  } else if (key.equals("VEGETATIVE_HUMIDITY_HIGH_THRESHOLD")) {
    VEGETATIVE_HUMIDITY_HIGH_THRESHOLD = value.toFloat();
  } else if (key.equals("FLOWERING_HUMIDITY_LOW_THRESHOLD")) {
    FLOWERING_HUMIDITY_LOW_THRESHOLD = value.toFloat();
  } else if (key.equals("FLOWERING_HUMIDITY_HIGH_THRESHOLD")) {
    FLOWERING_HUMIDITY_HIGH_THRESHOLD = value.toFloat();
  } else if (key.equals("FRUITING_HUMIDITY_LOW_THRESHOLD")) {
    FRUITING_HUMIDITY_LOW_THRESHOLD = value.toFloat();
  } else if (key.equals("FRUITING_HUMIDITY_HIGH_THRESHOLD")) {
    FRUITING_HUMIDITY_HIGH_THRESHOLD = value.toFloat();
  }

  // Light Settings for Different Stages
  else if (key.equals("SEEDLING_LIGHT_ON_DURATION_HOURS")) {
    SEEDLING_LIGHT_ON_DURATION_HOURS = value.toInt();
  } else if (key.equals("VEGETATIVE_LIGHT_ON_DURATION_HOURS")) {
    VEGETATIVE_LIGHT_ON_DURATION_HOURS = value.toInt();
  } else if (key.equals("FLOWERING_LIGHT_ON_DURATION_HOURS")) {
    FLOWERING_LIGHT_ON_DURATION_HOURS = value.toInt();
  } else if (key.equals("FRUITING_LIGHT_ON_DURATION_HOURS")) {
    FRUITING_LIGHT_ON_DURATION_HOURS = value.toInt();
  }

  // Soil Humidity Settings for Different Stages
  else if (key.equals("SEEDLING_SOIL_MOISTURE_LOW_THRESHOLD")) {
    SEEDLING_SOIL_MOISTURE_LOW_THRESHOLD = value.toFloat();
  } else if (key.equals("SEEDLING_SOIL_MOISTURE_HIGH_THRESHOLD")) {
    SEEDLING_SOIL_MOISTURE_HIGH_THRESHOLD = value.toFloat();
  } else if (key.equals("VEGETATIVE_SOIL_MOISTURE_LOW_THRESHOLD")) {
    VEGETATIVE_SOIL_MOISTURE_LOW_THRESHOLD = value.toFloat();
  } else if (key.equals("VEGETATIVE_SOIL_MOISTURE_HIGH_THRESHOLD")) {
    VEGETATIVE_SOIL_MOISTURE_HIGH_THRESHOLD = value.toFloat();
  } else if (key.equals("FLOWERING_SOIL_MOISTURE_LOW_THRESHOLD")) {
    FLOWERING_SOIL_MOISTURE_LOW_THRESHOLD = value.toFloat();
  } else if (key.equals("FLOWERING_SOIL_MOISTURE_HIGH_THRESHOLD")) {
    FLOWERING_SOIL_MOISTURE_HIGH_THRESHOLD = value.toFloat();
  } else if (key.equals("FRUITING_SOIL_MOISTURE_LOW_THRESHOLD")) {
    FRUITING_SOIL_MOISTURE_LOW_THRESHOLD = value.toFloat();
  } else if (key.equals("FRUITING_SOIL_MOISTURE_HIGH_THRESHOLD")) {
    FRUITING_SOIL_MOISTURE_HIGH_THRESHOLD = value.toFloat();
  }

  // Settings for Different Stages Duration in Days
  else if (key.equals("SEEDLING_STAGE_DURATION_DAYS")) {
    SEEDLING_STAGE_DURATION_DAYS = value.toInt();
  } else if (key.equals("VEGETATIVE_STAGE_DURATION_DAYS")) {
    VEGETATIVE_STAGE_DURATION_DAYS = value.toInt();
  } else if (key.equals("FLOWERING_STAGE_DURATION_DAYS")) {
    FLOWERING_STAGE_DURATION_DAYS = value.toInt();
  } else if (key.equals("FRUITING_STAGE_DURATION_DAYS")) {
    FRUITING_STAGE_DURATION_DAYS = value.toInt();
  }

  // Water Tank Settings
  else if (key.equals("WATER_LEVEL_MAX")) {
    WATER_LEVEL_MAX = value.toInt();
  } else if (key.equals("WATER_LEVEL_MIN")) {
    WATER_LEVEL_MIN = value.toInt();
  } else if (key.equals("level25")) {
    level25 = value.toInt();
  } else if (key.equals("level50")) {
    level50 = value.toInt();
  } else if (key.equals("level75")) {
    level75 = value.toInt();
  } else { DEBUG_PRINTLN("Not Assigned: " + value);}
}
