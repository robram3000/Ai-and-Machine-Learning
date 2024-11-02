
#ifndef FIREBASE_CONNECT_H
#define FIREBASE_CONNECT_H

#include <FirebaseESP8266.h>
FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseData;

void setupFirebase() {
  config.host = "arduinotofirebase-d65c5-default-rtdb.firebaseio.com/";
  config.api_key = "AIzaSyAunMT08LY13RUJLt7eL9cxAJTsUK4BF_c";
  auth.user.email = "parleroalbertpalma@gmail.com";
  auth.user.password = "Ezekiel16";

  Serial.println("Initializing Firebase...");
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (Firebase.ready()) {
    Serial.println("Connected to Firebase");
  } else {
    Serial.println("Failed to connect to Firebase");
  }
}

void uploadSensorData(float temperature, float humidity) {
  if (Firebase.ready()) {
    String path = "/sensorData";

    if (Firebase.setFloat(firebaseData, path + "/temperature", temperature)) {
      Serial.println("Temperature uploaded successfully");
    } else {
      Serial.print("Error uploading temperature: ");
      Serial.println(firebaseData.errorReason());
    }

    if (Firebase.setFloat(firebaseData, path + "/humidity", humidity)) {
      Serial.println("Humidity uploaded successfully");
    } else {
      Serial.print("Error uploading humidity: ");
      Serial.println(firebaseData.errorReason());
    }
  }
}

#endif
