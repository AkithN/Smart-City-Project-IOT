#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#define NSR 5
#define NSY 18
#define NSG 19

#define EWR 2
#define EWY 15
#define EWG 4

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;

bool defMode = false;
bool freeMode = false;

void setup() {
  Serial.begin(9600);
  pinMode(NSR, OUTPUT);
  pinMode(NSY, OUTPUT);
  pinMode(NSG, OUTPUT);
  pinMode(EWR, OUTPUT);
  pinMode(EWY, OUTPUT);
  pinMode(EWG, OUTPUT);

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
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  while (Firebase.ready() && signupOK) {
    if (Firebase.RTDB.getBool(&fbdo, "color_lights/default")) {
      if (fbdo.dataType() == "boolean") {
        onDefModeChange(fbdo);
        if (!Firebase.RTDB.getBool(&fbdo, "color_lights/default")) {
          break;  // Exit the loop if the value becomes false
        }
      }
    } else {
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.RTDB.getBool(&fbdo, "color_lights/free_mode")) {
      if (fbdo.dataType() == "boolean") {
        onFreeModeChange();
        if (!Firebase.RTDB.getBool(&fbdo, "color_lights/free_mode")) {
          break;  // Exit the loop if the value becomes false
        }
      }
    } else {
      Serial.println(fbdo.errorReason());
    }
  }
}

void onDefModeChange(FirebaseData &fbdo) {
  defMode = fbdo.boolData();
      if (defMode) {
      freeMode = false;
      digitalWrite(NSR, HIGH);
      digitalWrite(NSY, LOW);
      digitalWrite(NSG, LOW);
      digitalWrite(EWR, LOW);
      digitalWrite(EWY, LOW);
      digitalWrite(EWG, HIGH);
      delay(3000);

      digitalWrite(NSR, LOW);
      digitalWrite(NSY, HIGH);
      digitalWrite(NSG, LOW);
      digitalWrite(EWR, LOW);
      digitalWrite(EWY, HIGH);
      digitalWrite(EWG, LOW);
      delay(1000);

      digitalWrite(NSR, LOW);
      digitalWrite(NSY, LOW);
      digitalWrite(NSG, HIGH);
      digitalWrite(EWR, HIGH);
      digitalWrite(EWY, LOW);
      digitalWrite(EWG, LOW);
      delay(3000);

      digitalWrite(NSR, LOW);
      digitalWrite(NSY, HIGH);
      digitalWrite(NSG, LOW);
      digitalWrite(EWR, LOW);
      digitalWrite(EWY, HIGH);
      digitalWrite(EWG, LOW);
      delay(1000);
    }
  else
  {
  digitalWrite(NSR, LOW);
  digitalWrite(NSY, LOW);
  digitalWrite(NSG, LOW);
  digitalWrite(EWR, LOW);
  digitalWrite(EWY, LOW);
  digitalWrite(EWG, LOW);
  }
}

void onFreeModeChange() {
  freeMode = fbdo.boolData(); 
  if (freeMode) {
      defMode = false;
      digitalWrite(NSY, HIGH);
      digitalWrite(EWY, HIGH);
      delay(500);

      digitalWrite(NSY, LOW);
      digitalWrite(EWY, LOW);
      delay(500);
    }
  else
  {
  digitalWrite(NSR, LOW);
  digitalWrite(NSY, LOW);
  digitalWrite(NSG, LOW);
  digitalWrite(EWR, LOW);
  digitalWrite(EWY, LOW);
  digitalWrite(EWG, LOW);
  }
}
