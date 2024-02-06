#include <Arduino.h>
#include <WiFi.h>               
#include <Firebase_ESP_Client.h>
#include <DHT.h>

#define DHT_SENSOR_PIN 4
#define DHT_SENSOR_TYPE DHT11

//To provide the ESP32 / ESP8266 with the connection and the sensor type
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "AkithN"
#define WIFI_PASSWORD "akith123456"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDNCE4n8TS8TCwrl7J4hsS0zEIVYCiWFv8"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://temperature-42a0f-default-rtdb.asia-southeast1.firebasedatabase.app/"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;                     //since we are doing an anonymous sign in

void setup() {

  dht_sensor.begin();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Temperature and humidity measured should be stored in variables
  // so the user can use it later in the database
  float temperature = dht_sensor.readTemperature();
  float humidity = dht_sensor.readHumidity();

  if (Firebase.ready() && signupOK) {
    // Update Temperature in the DHT_11 Table
    if (Firebase.RTDB.setInt(&fbdo, "DHT_11/Temperature", temperature)) {
      Serial.print("Temperature: ");
      Serial.println(temperature);
    } else {
      Serial.println("Failed to read from the sensor");
      Serial.println("Reason: " + fbdo.errorReason());
    }

    // Update Humidity in the DHT_11 Table
    if (Firebase.RTDB.setFloat(&fbdo, "DHT_11/Humidity", humidity)) {
      Serial.print("Humidity: ");
      Serial.print(humidity);
    } else {
      Serial.println("Failed to read from the sensor");
      Serial.println("Reason: " + fbdo.errorReason());
    }
  }

  // Add a delay to avoid continuous updates and reduce server load
  delay(1000);
}
