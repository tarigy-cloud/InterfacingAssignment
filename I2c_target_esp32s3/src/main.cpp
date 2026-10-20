#include <Arduino.h>
#include <Wire.h>

#define TARGET_ADDR 0x08

// Default I2C pins for ESP32-S3
#define SDA_PIN 8
#define SCL_PIN 9

volatile bool newMsgReceived = false;
char receivedBuffer[64];

// Hardware callback executed when Controller transmits data (Master -> Slave)
void receiveEvent(int howMany) {
  int idx = 0;
  while (Wire.available() && idx < 63) {
    receivedBuffer[idx++] = (char)Wire.read();
  }
  receivedBuffer[idx] = '\0'; // Null-terminate the string
  newMsgReceived = true;
}

// Hardware callback executed when Controller requests data (Slave -> Master)
void requestEvent() {
  // Send the received buffer back to the master
  // Using strlen ensures we only send the actual valid characters
  Wire.write((uint8_t*)receivedBuffer, strlen(receivedBuffer));
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize Hardware I2C in Target mode on ESP32-S3
  Wire.begin(TARGET_ADDR, SDA_PIN, SCL_PIN, 100000); // Address, SDA, SCL, Speed
  
  // Register callbacks for Receive and Request events
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  
  Serial.println("\n--- ESP32-S3 I2C Target Initialized on Address 0x08 ---");
}

void loop() {
  // Process and display received message outside the ISR
  if (newMsgReceived) {
    Serial.print("ESP32-S3 Received: ");
    Serial.println(receivedBuffer);
    newMsgReceived = false;
  }
  
  yield(); // Keep background system tasks running
}