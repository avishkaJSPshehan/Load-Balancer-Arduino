#include "HX711.h"

// Pin definitions for HX711 modules
#define DT_PIN1 6   // Data pin for Load Cell 1
#define SCK_PIN1 7  // Clock pin for Load Cell 1
#define DT_PIN2 8   // Data pin for Load Cell 2
#define SCK_PIN2 11 // Clock pin for Load Cell 2

// Motor driver pins
#define IN1 2   // Motor A direction pin 1
#define IN2 3   // Motor A direction pin 2
#define ENA 9   // Motor A speed (PWM)
#define IN3 4   // Motor B direction pin 1
#define IN4 5   // Motor B direction pin 2
#define ENB 10  // Motor B speed (PWM)

// HX711 instances
HX711 scale1; // Load Cell 1
HX711 scale2; // Load Cell 2

// Calibration factors for each load cell (default values)
float calibration_factor1 = 1.0;
float calibration_factor2 = 1.0;

// Function to calibrate a single load cell
float calibrateLoadCell(HX711& scale, const char* loadCellName) {
    Serial.print("Calibrating ");
    Serial.print(loadCellName);
    Serial.println(". Please place a known weight on the load cell and press 'c'...");

    // Wait for user input to start calibration
    while (Serial.read() != 'c') {
        delay(100);
    }

    if (scale.is_ready()) {
        long raw_reading = scale.get_units(10); // Get raw reading
        Serial.print("Raw reading: ");
        Serial.println(raw_reading);

        Serial.print("Enter the actual weight in grams: ");
        while (!Serial.available()) {} // Wait for user input
        float known_weight = Serial.parseFloat();
        Serial.println(known_weight);

        float calibration_factor = raw_reading / known_weight; // Calculate calibration factor
        Serial.print("Calibration factor for ");
        Serial.print(loadCellName);
        Serial.print(" is: ");
        Serial.println(calibration_factor);

        delay(2000);
        return calibration_factor;
    } else {
        Serial.print(loadCellName);
        Serial.println(" not ready during calibration. Check connections.");
        return 1.0; // Default calibration factor
    }
}

// Function to control Motor A
void controlMotorA(int speed, bool direction) {
    digitalWrite(IN1, direction);       // Set direction
    digitalWrite(IN2, !direction);
    analogWrite(ENA, speed);           // Set speed
}

// Function to control Motor B
void controlMotorB(int speed, bool direction) {
    digitalWrite(IN3, direction);       // Set direction
    digitalWrite(IN4, !direction);
    analogWrite(ENB, speed);           // Set speed
}

void setup() {
    Serial.begin(9600);

    // Initialize motor driver pins
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENB, OUTPUT);

    // Initialize HX711 modules
    scale1.begin(DT_PIN1, SCK_PIN1);
    scale2.begin(DT_PIN2, SCK_PIN2);

    // Wait for HX711 modules to stabilize
    Serial.println("Initializing HX711 modules...");
    delay(2000);

    if (scale1.is_ready() && scale2.is_ready()) {
        Serial.println("HX711 modules are ready.");
    } else {
        Serial.println("HX711 not ready. Check connections.");
        while (1); // Stop execution if HX711 is not ready
    }

    // Tare the scales
    Serial.println("Taring Load Cell 1...");
    scale1.tare();
    Serial.println("Load Cell 1 tared.");

    Serial.println("Taring Load Cell 2...");
    scale2.tare();
    Serial.println("Load Cell 2 tared.");

    // Calibrate Load Cell 1
    calibration_factor1 = calibrateLoadCell(scale1, "Load Cell 1");

    // Calibrate Load Cell 2
    calibration_factor2 = calibrateLoadCell(scale2, "Load Cell 2");

    Serial.println("Calibration complete.");
    Serial.println("Calibration factors saved for both load cells.");
}

void loop() {
    // Read weights from both load cells
    float weight1 = scale1.get_units(10) / calibration_factor1;
    float weight2 = scale2.get_units(10) / calibration_factor2;

    // Print the weights
    Serial.print("Weight 1: ");
    Serial.print(weight1);
    Serial.println(" grams");

    Serial.print("Weight 2: ");
    Serial.print(weight2);
    Serial.println(" grams");

    // Control motors based on weights
    if (weight1 > 100) {
        Serial.println("Motor A activated!");
        controlMotorA(200, true); // Spin Motor A forward at speed 200
    } else {
        controlMotorA(0, true); // Stop Motor A
    }

    if (weight2 > 100) {
        Serial.println("Motor B activated!");
        controlMotorB(200, true); // Spin Motor B forward at speed 200
    } else {
        controlMotorB(0, true); // Stop Motor B
    }

    delay(500); // Update every 500ms
}
