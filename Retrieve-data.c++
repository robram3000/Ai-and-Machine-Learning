#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
const char* ssid = "RAMIREZ_2G"; 
const char* password = "ram12345"; 

FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");
  Serial.println(WiFi.localIP());

  config.host = "arduinotofirebase-d65c5-default-rtdb.firebaseio.com/"; 
  config.api_key = "AIzaSyAunMT08LY13RUJLt7eL9cxAJTsUK4BF_c"; 
  auth.user.email = "parleroalbertpalma@gmail.com";  
  auth.user.password = "Ezekiel16";  

  Serial.println("Initializing Firebase...");
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true); 
  Serial.println("Firebase initialized");

  if (Firebase.ready()) {
    Serial.println("Connected to Firebase");
  } else {
    Serial.println("Failed to connect to Firebase");
  }
}

void loop() {
  String lastTempPath = "/sensor/temperature"; 
  String lastHumidityPath = "/sensor/humidity"; 
  if (Firebase.get(firebaseData, lastTempPath)) {
    if (firebaseData.dataType() == "json") {
      FirebaseJson jsonTempData;
      jsonTempData = firebaseData.jsonObject();
      
      String jsonString;
      jsonTempData.toString(jsonString, true);
      Serial.println("Retrieved temperature data:");
      Serial.println(jsonString);  
    }
  } else {
    Serial.println("Failed to retrieve temperature data");
    Serial.println(firebaseData.errorReason());
  }

 
  if (Firebase.get(firebaseData, lastHumidityPath)) {
    if (firebaseData.dataType() == "json") {
      FirebaseJson jsonHumidityData;
      jsonHumidityData = firebaseData.jsonObject();
      
      String jsonString;
      jsonHumidityData.toString(jsonString, true);
      Serial.println("Retrieved humidity data:");
      Serial.println(jsonString); 
    }
  } else {
    Serial.println("Failed to retrieve humidity data");
    Serial.println(firebaseData.errorReason());
  }
  delay(10000); 
}
