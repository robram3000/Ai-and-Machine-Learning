#ifndef ACTION_H
#define ACTION_H

#include <FirebaseESP8266.h>
#include <Servo.h>  // Include the Servo library

#define IN1 5
#define IN2 4
#define IN3 0
#define IN4 2
#define SERVO_PIN 2  // Define the pin for the servo motor
#define TRIG_PIN 5   // Define the pin for the ultrasonic trigger (D5)
#define ECHO_PIN 4   // Define the pin for the ultrasonic echo (D6)
#define RED_LED_PIN 13 // Define the pin for the red LED
#define BUZZER_PIN 3   // Define the pin for the buzzer

extern FirebaseData firebaseData;

Servo myServo;  // Create a servo object

void setupPins() {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT); // Initialize motor pins
    myServo.attach(SERVO_PIN);  // Attach the servo on the defined pin
    pinMode(TRIG_PIN, OUTPUT); // Initialize ultrasonic sensor trigger pin
    pinMode(ECHO_PIN, INPUT);   // Initialize ultrasonic sensor echo pin
    pinMode(RED_LED_PIN, OUTPUT); // Initialize red LED pin
    pinMode(BUZZER_PIN, OUTPUT);  // Initialize buzzer pin
}

void moveForward() {
    digitalWrite(IN1, HIGH);  // Left motor forward
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);  // Right motor forward
    digitalWrite(IN4, LOW);
}

void moveBackward() {
    digitalWrite(IN1, LOW);   // Left motor backward
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);   // Right motor backward
    digitalWrite(IN4, HIGH);
}

void turnLeft() {
    digitalWrite(IN1, LOW);   // Stop left motor
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);  // Right motor forward
    digitalWrite(IN4, LOW);
}

void turnRight() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);   // Stop right motor
    digitalWrite(IN4, LOW);
}

void stopMotors() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void setServoPosition(int position) {
    myServo.write(position);  // Set the servo position (0 to 180 degrees)
}

long readUltrasonicDistance() {
    // Clear the trigger
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    
    // Set the trigger high for 10 microseconds
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    // Read the echo pin, returning the sound wave travel time in microseconds
    long duration = pulseIn(ECHO_PIN, HIGH);
    
    // Calculate the distance (duration / 2) * speed of sound (in cm)
    long distance = duration * 0.034 / 2; // Convert to cm
    return distance;
}

void updateStatus(String action, String servo, String obstacle, String movingDirection) {
    // Update the Firebase database with the status
    Firebase.setString(firebaseData, "/Status/Action", action);
    Firebase.setString(firebaseData, "/Status/servo", servo);
    Firebase.setString(firebaseData, "/Status/Obstacle", obstacle);
    Firebase.setString(firebaseData, "/Status/MovingDirection", movingDirection);
}

void checkLeftControllerAction() {
    String leftControllerAction;
    if (Firebase.getString(firebaseData, "/Direction/Leftcontroller")) {
        leftControllerAction = firebaseData.stringData();
        Serial.print("Left Controller Action from Firebase: ");
        Serial.println(leftControllerAction);

        if (leftControllerAction == "1") {
            Serial.println("Left Controller: Moving Forward");
            moveForward();
            updateStatus("Moving", "N/A", "No", "Forward");
        } else if (leftControllerAction == "0") {
            Serial.println("Left Controller: Moving Backward");
            moveBackward();
            updateStatus("Moving", "N/A", "No", "Backward");
        } else {
            Serial.println("Stopping Motors");
            stopMotors();
            updateStatus("Stopped", "N/A", "No", "N/A");
        }
    } else {
        Serial.print("Error reading left controller action: ");
        Serial.println(firebaseData.errorReason());
    }
}

void checkDirectionAction() {
    String powerStatus;
    if (Firebase.getString(firebaseData, "/Direction/Power")) {
        powerStatus = firebaseData.stringData(); 
        Serial.print("Power Status from Firebase: ");
        Serial.println(powerStatus);

        if (powerStatus == "1") {
            String direction;
            if (Firebase.getString(firebaseData, "/Direction/Direction")) {
                direction = firebaseData.stringData();
                Serial.print("Direction from Firebase: ");
                Serial.println(direction);

                if (direction == "3") {
                    Serial.println("Turning Left");
                    turnLeft();
                    updateStatus("Turning", "N/A", "No", "Left");
                } else if (direction == "4") {
                    Serial.println("Turning Right");
                    turnRight();
                    updateStatus("Turning", "N/A", "No", "Right");
                } else if (direction == "1") {
                    Serial.println("Moving Forward");
                    moveForward();
                    updateStatus("Moving", "N/A", "No", "Forward");
                } else {
                    Serial.println("Stopping Motors");
                    stopMotors();
                    updateStatus("Stopped", "N/A", "No", "N/A");
                }
            } else {
                Serial.print("Error reading direction: ");
                Serial.println(firebaseData.errorReason());
            }
        } else {
            Serial.println("Power is off. Stopping motors and resetting servo.");
            stopMotors();
            setServoPosition(0); // Reset servo to 0 degrees when power is off
            updateStatus("Stopped", "N/A", "No", "N/A");
        }
    } else {
        Serial.print("Error reading power status: ");
        Serial.println(firebaseData.errorReason());
    }
}

void checkServoAction() {
    String servoPosition;
    if (Firebase.getString(firebaseData, "/Direction/Direction")) {
        servoPosition = firebaseData.stringData();
        Serial.print("Servo Position from Firebase: ");
        Serial.println(servoPosition);
        
        // Forward
        if (servoPosition == "1") {
            Serial.println("Setting servo to position for Forward");
            setServoPosition(90); // Example: set servo to 90 degrees for forward
            updateStatus("Moving", "Forward", "No", "N/A");
        }
        // Backward
        else if (servoPosition == "2") {
            Serial.println("Setting servo to position for Backward");
            setServoPosition(180); // Example: set servo to 180 degrees for backward
            updateStatus("Moving", "Backward", "No", "N/A");
        }
        // Left forward
        else if (servoPosition == "3") {
            Serial.println("Setting servo to position for Left Forward");
            setServoPosition(45); // Example: set servo to 45 degrees for left forward
            updateStatus("Moving", "Left Forward", "No", "N/A");
        }
        else if (servoPosition == "4") {
            Serial.println("Setting servo to position for Right Forward");
            setServoPosition(135); 
            updateStatus("Moving", "Right Forward", "No", "N/A");
        }
        else {
            Serial.println("No valid servo command received. Stopping servo.");
            setServoPosition(0);
            updateStatus("Stopped", "N/A", "No", "N/A");
        }
    } else {
        Serial.print("Error reading servo position: ");
        Serial.println(firebaseData.errorReason());
    }
}

void checkAction() {
    checkLeftControllerAction();
    checkDirectionAction();
    checkServoAction();
    long distance = readUltrasonicDistance();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance < 20) { 
        Serial.println("Obstacle detected! Stopping motors and sounding buzzer.");
        stopMotors();
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(BUZZER_PIN, HIGH);  // Turn on the buzzer
        updateStatus("Stopped", "N/A", "Yes", "N/A");
    } else {
        digitalWrite(RED_LED_PIN, LOW); 
        digitalWrite(BUZZER_PIN, LOW);  // Turn off the buzzer
    }
}

#endif  // ACTION_H
