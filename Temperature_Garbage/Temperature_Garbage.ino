#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>

#define DHT_SENSOR_PIN 4
#define DHT_SENSOR_TYPE DHT11
#define ULTRASONIC_TRIG_PIN 14 
#define ULTRASONIC_ECHO_PIN 27  

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void tokenStatusCallback(TokenInfo tokenInfo) {
  // You can add any necessary code here if needed
}

void setup() {
  Serial.begin(115200);

  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);

  dht_sensor.begin();

  WiFi.begin("AkithN", "akith123456");
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = "AIzaSyDNCE4n8TS8TCwrl7J4hsS0zEIVYCiWFv8";
  config.database_url = "https://temperature-42a0f-default-rtdb.asia-southeast1.firebasedatabase.app/";

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = &tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // DHT sensor code
  float temperature = dht_sensor.readTemperature();
  float humidity = dht_sensor.readHumidity();

  if (!isnan(temperature) && !isnan(humidity)) {
    if (Firebase.ready() && signupOK) {
      if (Firebase.RTDB.setInt(&fbdo, "DHT_11/Temperature", temperature)) {
        Serial.print("Temperature: ");
        Serial.println(temperature);
      } else {
        Serial.println("Failed to write Temperature to Firebase");
        Serial.println("Reason: " + fbdo.errorReason());
      }

      if (Firebase.RTDB.setFloat(&fbdo, "DHT_11/Humidity", humidity)) {
        Serial.print("Humidity: ");
        Serial.print(humidity);
      } else {
        Serial.println("Failed to write Humidity to Firebase");
        Serial.println("Reason: " + fbdo.errorReason());
      }
    }
  } else {
    Serial.println("Failed to read from the DHT sensor - Invalid data");
  }

  // Ultrasonic sensor code
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  unsigned long startTime = micros();
  while (digitalRead(ULTRASONIC_ECHO_PIN) == LOW && micros() - startTime < 50000); // Timeout after 50ms

  startTime = micros();
  while (digitalRead(ULTRASONIC_ECHO_PIN) == HIGH);
  unsigned long duration = micros() - startTime;

  int distance = duration * 0.034 / 2;

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    if (Firebase.RTDB.setInt(&fbdo, "Ultrasonic/Distance", distance)) {
      Serial.print("Distance : ");
      Serial.println(distance);
    } else {
      Serial.println("Failed to write Distance to Firebase");
      Serial.println("Reason: " + fbdo.errorReason());
    }
  }

  delay(1000); // Add a delay to avoid continuous updates and reduce server load
}
