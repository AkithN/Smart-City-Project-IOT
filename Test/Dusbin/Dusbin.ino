#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#define ULTRASONIC_TRIG_PIN 14 
#define ULTRASONIC_ECHO_PIN 27  

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

// Corrected function signature for tokenStatusCallback
void tokenStatusCallback(TokenInfo tokenInfo) {
  // You can add any necessary code here if needed
}

void setup() {
  Serial.begin(115200);

  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);

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

  // Assign the callback function for the long running token generation task
  config.token_status_callback = &tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Ultrasonic sensor code
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  // Wait for the pulse to start
  unsigned long startTime = micros();
  while (digitalRead(ULTRASONIC_ECHO_PIN) == LOW && micros() - startTime < 50000); // Timeout after 50ms

  // Measure the pulse duration
  startTime = micros();
  while (digitalRead(ULTRASONIC_ECHO_PIN) == HIGH);
  unsigned long duration = micros() - startTime;

  // Calculate distance
  int distance = duration * 0.034 / 2;

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    if (Firebase.RTDB.setInt(&fbdo, "Ultrasonic/Distance", distance)) {
      Serial.print("Distance : ");
      Serial.println(distance);
    } else {
      Serial.println("Failed to Read Distance from the Ultrasonic Sensor");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}
