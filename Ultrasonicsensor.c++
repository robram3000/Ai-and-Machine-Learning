#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>

#define DHTPIN 2        
#define DHTTYPE DHT11   
#define TRIG_PIN D5     
#define ECHO_PIN D6     
DHT dht(DHTPIN, DHTTYPE);

FirebaseConfig config;
FirebaseAuth auth;

const char* ssid = "RAMIREZ_2G"; 
const char* password = "ram12345"; 

FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);
  dht.begin();  

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

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

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  float temp = dht.readTemperature();  
  float humidity = dht.readHumidity();
  float distance = measureDistance();

  if (isnan(temp) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  String uniqueKey = String(millis()); 

  String tempPath = "/sensor/temperature/" + uniqueKey; 
  String humidityPath = "/sensor/humidity/" + uniqueKey;
  String distancePath = "/sensor/distance/" + uniqueKey;

  if (Firebase.setFloat(firebaseData, tempPath, temp) &&
      Firebase.setFloat(firebaseData, humidityPath, humidity) &&
      Firebase.setFloat(firebaseData, distancePath, distance)) {
    Serial.println("Data successfully sent to Firebase.");
    Serial.print("Temperature: ");
    Serial.println(temp);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Distance: ");
    Serial.println(distance);
  } else {
    Serial.println("Failed to send data");
    Serial.println(firebaseData.errorReason());
  }

  delay(10000);  
}
