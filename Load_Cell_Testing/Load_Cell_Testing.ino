#include "HX711.h"

#define DOUT  2
#define CLK  3

HX711 scale;

void setup() {
  Serial.begin(9600);
  scale.begin(DOUT, CLK);
}

void loop() {
  long reading = scale.read();  // Get the raw reading (no scaling)
  Serial.print("Raw Reading: ");
  Serial.println(reading);
  delay(100);  // Delay for readability
}
