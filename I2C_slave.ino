#include <Wire.h>

#define SLAVE_ADDR 9       // Địa chỉ của UNO trong giao tiếp I2C
#define LED_PIN 13         // LED tích hợp trên bo UNO

bool ledState = false;

void setup() {
  Wire.begin(SLAVE_ADDR);          // Khởi tạo I2C ở chế độ SLAVE
  Wire.onReceive(receiveEvent);    // Gán hàm callback khi nhận dữ liệu
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.println("UNO (Slave) da san sang...");
}

void loop() {
  delay(100);
}

// Hàm được gọi khi nhận dữ liệu từ Master (ESP8266)
void receiveEvent(int howMany) {
  while (Wire.available()) {
    byte cmd = Wire.read(); // Đọc byte từ ESP8266
    if (cmd == 1) {
      ledState = true;
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ON (nhan tu ESP8266)");
    } else if (cmd == 0) {
      ledState = false;
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED OFF (nhan tu ESP8266)");
    }
  }
}
