#include "HX711.h"

// Pin definitions for HX711 module
#define DT_PIN 6   // Data pin for Load Cell
#define SCK_PIN 7  // Clock pin for Load Cell

// HX711 instance
HX711 scale;

// Calibration factor for the load cell
float calibration_factor = 1.0; // Default calibration factor

// Function to calibrate the load cell
float calibrateLoadCell(HX711 &scale) {
    Serial.println("Calibrating the load cell...");
    Serial.println("Place a known weight on the load cell and press 'c'...");

    // Wait for user to press 'c'
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

        // Calculate the calibration factor
        float calibration_factor = raw_reading / known_weight;
        Serial.print("Calibration factor is: ");
        Serial.println(calibration_factor);

        delay(2000); // Allow user to note down the calibration factor
        return calibration_factor;
    } else {
        Serial.println("Load cell is not ready. Check connections.");
        return 1.0; // Default calibration factor
    }
}

void setup() {
    Serial.begin(9600);

    // Initialize the HX711 module
    scale.begin(DT_PIN, SCK_PIN);

    // Wait for HX711 to stabilize
    Serial.println("Initializing HX711 module...");
    delay(2000);

    // Check if the scale is ready
    if (scale.is_ready()) {
        Serial.println("HX711 module is ready.");
    } else {
        Serial.println("HX711 module is not ready. Check connections.");
        while (1); // Halt execution if HX711 is not ready
    }

    // Tare the load cell
    Serial.println("Taring the load cell...");
    scale.tare();
    Serial.println("Load cell tared.");

    // Calibrate the load cell
    calibration_factor = calibrateLoadCell(scale);

    Serial.println("Calibration complete. Use the printed calibration factor for future use.");
}

void loop() {
    // Optionally read the weight to verify calibration
    float weight = scale.get_units(10) / calibration_factor;

    // Print the weight
    Serial.print("Weight: ");
    Serial.print(weight);
    Serial.println(" grams");

    delay(1000); // Read values every second
}

