// Arduino Sim Racing Dashboard - Forza Horizon 5 (Finalized & Robust)
// Segment Pins: 2, 3, 4, 6, 7, 8, 9, 10
// Shift Light LED: Pin 13

const int segmentPins[] = {2, 3, 4, 6, 7, 8, 9}; // A, B, C, D, E, F, G, DP
const int shiftLightPin = 13;

// Verified Common Cathode digit encoding [A, B, C, D, E, F, G, DP]
const byte digitTable[10][8] = {
  {0, 0, 0, 0, 0, 0, 1, 0}, // 0
  {0, 0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1, 0}, // 2
  {0, 1, 1, 1, 1, 0, 1, 0}, // 3
  {0, 0, 1, 1, 0, 1, 1, 0}, // 4
  {0, 1, 1, 0, 1, 1, 1, 0}, // 5
  {1, 1, 1, 0, 1, 1, 1, 0}, // 6
  {0, 0, 1, 1, 1, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1, 0}, // 8
  {1, 1, 1, 1, 0, 1, 1, 0}  // 9
};

void setup() {
  Serial.begin(9600);

  // Initialize segment pins as outputs and turn them off initially (LOW for Common Cathode)
  for (int i = 0; i < 8; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], LOW);
  }

  pinMode(shiftLightPin, OUTPUT);
  digitalWrite(shiftLightPin, LOW);
}

void displayDigit(int num) {
  if (num < 0 || num > 9) {
    for (int i = 0; i < 8; i++) {
      digitalWrite(segmentPins[i], LOW);
    }
    return;
  }

  for (int i = 0; i < 8; i++) {
    digitalWrite(segmentPins[i], digitTable[num][i]);
  }
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() >= 2) {
      char cmdType = input.charAt(0);

      if (cmdType == 'G') {
        int gear = input.substring(1).toInt(); // Safe multi-digit parsing
        displayDigit(gear);
      }
      else if (cmdType == 'X') {
        int val = input.substring(1).toInt();
        digitalWrite(shiftLightPin, (val == 1) ? HIGH : LOW);
      }
    }
  }
}