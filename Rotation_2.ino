#include <Servo.h>
#include <SoftwareSerial.h>

Servo servoTilt;   // Controls vertical tilt
Servo servoRotate; // Controls horizontal rotation

const int tiltPin = 10;   // Vertical tilt (up/down)
const int rotatePin = 9;  // Horizontal rotation

// Receiver
SoftwareSerial dataSerial(2, 3); // RX, TX

// Define 4 directions with tilt + rotate combinations
const int directionCount = 4;
const int tiltAngles[directionCount]   = {165, 30, 30, 30};
const int rotateAngles[directionCount] = {0,   0,  90, 180};

float signalValues[directionCount][6]; // Store 6 values per direction

void setup() {
  servoTilt.attach(tiltPin);
  servoRotate.attach(rotatePin);
  Serial.begin(9600);

  // Receiver
  dataSerial.begin(9600);
  delay(500);
}

void loop() {
  Serial.println("Starting full directional scan...");
  delay(500);

  // Sweep through 4 directions
  for (int dir = 0; dir < directionCount; dir++) {
    servoRotate.write(rotateAngles[dir]);
    servoTilt.write(tiltAngles[dir]);

    Serial.print("Moving to direction ");
    Serial.print(dir + 1);
    Serial.print(": Rotate = ");
    Serial.print(rotateAngles[dir]);
    Serial.print("°, Tilt = ");
    Serial.print(tiltAngles[dir]);
    Serial.println("°");

  // Reciving data
    for (int j = 0; j < 6; j++) {
      delay(5000); // wait 5 seconds between readings

      if (dataSerial.available()) {
        String valueStr = dataSerial.readStringUntil('\n');
        float value = valueStr.toFloat();
        signalValues[dir][j] = value;

        Serial.print("Reading ");
        Serial.print(j);
        Serial.print(" from direction ");
        Serial.print(dir + 1);
        Serial.print(": ");
        Serial.println(value);
      }
    }
  }

  // Calculate averages (excluding min and max)
  float averages[directionCount];
  for (int i = 0; i < directionCount; i++) {
    float sum = 0;
    float minVal = signalValues[i][0];
    float maxVal = signalValues[i][0];

    // First pass: find sum, min, and max
    for (int j = 0; j < 6; j++) {
      float val = signalValues[i][j];
      sum += val;
      if (val < minVal) minVal = val;
      if (val > maxVal) maxVal = val;
    }

    float adjustedSum = sum - minVal - maxVal;
    averages[i] = adjustedSum / 4.0;

    Serial.print("Average for direction ");
    Serial.print(i + 1);
    Serial.print(" (Rotate=");
    Serial.print(rotateAngles[i]);
    Serial.print("°, Tilt=");
    Serial.print(tiltAngles[i]);
    Serial.print("°): ");
    Serial.println(averages[i]);
  }

  // Find best direction
  int bestDirIndex = 0;
  float maxAvg = averages[0];
  for (int i = 1; i < directionCount; i++) {
    if (averages[i] > maxAvg) {
      maxAvg = averages[i];
      bestDirIndex = i;
    }
  }

  // Move to best direction
  servoRotate.write(rotateAngles[bestDirIndex]);
  servoTilt.write(tiltAngles[bestDirIndex]);

  Serial.print("Rotating to best direction (");
  Serial.print(bestDirIndex + 1);
  Serial.print("): Rotate = ");
  Serial.print(rotateAngles[bestDirIndex]);
  Serial.print("°, Tilt = ");
  Serial.print(tiltAngles[bestDirIndex]);
  Serial.println("°");

  Serial.println("Cycle complete. Repeating...\n");
  Serial.println("\n");

// Reading data from the best position
  for (int j = 0; j < 6; j++) {
      delay(5000); // wait 5 seconds between readings

      if (dataSerial.available()) {
        String valueStr = dataSerial.readStringUntil('\n');
        float value = valueStr.toFloat();

        Serial.print("Reading ");
        Serial.print(j);
        Serial.print(": ");
        Serial.println(value);
      }
    }
}
