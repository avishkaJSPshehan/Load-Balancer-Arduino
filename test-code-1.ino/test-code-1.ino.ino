#include <Wire.h>
#include <MPU6050.h>
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

MPU6050 mpu;

// Pins for HX711
const int HX711_dout1 = 2;
const int HX711_sck1 = 3;
const int HX711_dout2 = 4;
const int HX711_sck2 = 5;

// Motor A connections
const int ENA = 10;
const int IN1 = 6;
const int IN2 = 7;

// Motor B connections
const int ENB = 11;
const int IN3 = 8;
const int IN4 = 9;

// HX711 instances
HX711_ADC LoadCell1(HX711_dout1, HX711_sck1);
HX711_ADC LoadCell2(HX711_dout2, HX711_sck2);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Initialize MPU6050
  mpu.initialize();
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connected successfully!");
  } else {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }

  // Initialize Load Cells
  LoadCell1.begin();
  LoadCell2.begin();

  float calibrationValue = 696.0;
  EEPROM.get(calVal_eepromAdress, calibrationValue);

  LoadCell1.start(2000, true);
  LoadCell2.start(2000, true);

  LoadCell1.setCalFactor(calibrationValue);
  LoadCell2.setCalFactor(calibrationValue);

  // Motor setup
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.println("Startup is complete");
}

void loop() {
  // MPU6050 readings
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Only values for Serial Plotter
  

  // Load Cell readings
  if (LoadCell1.update() && LoadCell2.update()) {
    float value1 = LoadCell1.getData();
    float value2 = LoadCell2.getData();

    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");

    // Serial.print("Load_cell1 output: ");
    Serial.print(value1);
    Serial.print("\t	");
    // Serial.print("Load_cell2 output: ");
    Serial.println(value2);

    // Motor control
    controlMotor(value1, value2);
  }

  if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') {
      LoadCell1.tareNoDelay();
      LoadCell2.tareNoDelay();
    }
  }

  if (LoadCell1.getTareStatus() && LoadCell2.getTareStatus()) {
    Serial.println("Tare complete");
  }

  delay(50);
}

void controlMotor(float value1, float value2) {
  if (value1 > 40) { 
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      analogWrite(ENA, 180);
      analogWrite(ENB, 180);
      delay(100);

      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      analogWrite(ENA, 0);
      analogWrite(ENB, 0);
    } else {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      analogWrite(ENA, 0);
      analogWrite(ENB, 0);
    }

    if (value2 > 25) { 

      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      analogWrite(ENA, 180);
      analogWrite(ENB, 180);
      delay(100);

      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      analogWrite(ENA, 0);
      analogWrite(ENB, 0);
    } else {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      analogWrite(ENA, 0);
      analogWrite(ENB, 0);
    }
}
