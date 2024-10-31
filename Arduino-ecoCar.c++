#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Pin definitions
#define DHTPIN 14           // GPIO14 corresponds to D5 on NodeMCU
#define DHTTYPE DHT22       // Define sensor type as DHT22
#define IN1 5               // Motor control pin 1 (D1 on NodeMCU)
#define IN2 4               // Motor control pin 2 (D2 on NodeMCU)
#define IN3 0               // Motor control pin 3 (D3 on NodeMCU)
#define IN4 2               // Motor control pin 4 (D4 on NodeMCU)

// Firebase and Wi-Fi configuration
FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseData;
DHT dht(DHTPIN, DHTTYPE);

// Wi-Fi credentials
const char* ssid = "RAMIREZ_2G";
const char* password = "ram12345";

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Initialize motor control pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Firebase configuration
  config.host = "arduinotofirebase-d65c5-default-rtdb.firebaseio.com/";
  config.api_key = "AIzaSyAunMT08LY13RUJLt7eL9cxAJTsUK4BF_c";
  auth.user.email = "parleroalbertpalma@gmail.com";
  auth.user.password = "Ezekiel16";

  // Connecting to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize Firebase
  Serial.println("Initializing Firebase...");
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  if (Firebase.ready()) {
    Serial.println("Connected to Firebase");
  } else {
    Serial.println("Failed to connect to Firebase");
  }
}

void loop() {
  delay(2000); // Wait 2 seconds between readings

  // Reading temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print the readings to Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // Upload data to Firebase
  if (Firebase.ready()) {
    String path = "/sensorData";
    
    // Upload temperature
    if (Firebase.setFloat(firebaseData, path + "/temperature", temperature)) {
      Serial.println("Temperature uploaded successfully");
    } else {
      Serial.print("Error uploading temperature: ");
      Serial.println(firebaseData.errorReason());
    }

    // Upload humidity
    if (Firebase.setFloat(firebaseData, path + "/humidity", humidity)) {
      Serial.println("Humidity uploaded successfully");
    } else {
      Serial.print("Error uploading humidity: ");
      Serial.println(firebaseData.errorReason());
    }

    // Retrieve motor direction from Firebase
    String direction;
    if (Firebase.getString(firebaseData, "/Direction/Direction")) {
      direction = firebaseData.stringData();
      Serial.print("Direction from Firebase: ");
      Serial.println(direction);

      // Control motors based on Firebase direction
      if (direction == "1") {
        Serial.println("Moving Forward (Up)");
        moveForward();
      } else if (direction == "2") {
        Serial.println("Moving Backward (Down)");
        moveBackward();
      } else if (direction == "3") {
        Serial.println("Turning Left");
        turnLeft();
      } else if (direction == "4") {
        Serial.println("Turning Right");
        turnRight();
      } else {
        Serial.println("Stopping Motors");
        stopMotors();
      }
    } else {
      Serial.print("Error reading direction: ");
      Serial.println(firebaseData.errorReason());
    }

    // Wi-Fi reconnection if disconnected
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Lost Wi-Fi connection! Reconnecting...");
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("\nReconnected to Wi-Fi!");
    }
  }
}

// Motor control functions
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
