#include <Wire.h>
#include <DHT.h>

#define DHTPIN 12        // Chân DHT11
#define DHTTYPE DHT11
#define SLAVE_ADDR 9     // Địa chỉ I2C của UNO
#define LED_PIN 13       // LED test

DHT dht(DHTPIN, DHTTYPE);

String dataToSend = "";
unsigned long lastReadTime = 0;
const unsigned long readInterval = 2000; // 2s đọc cảm biến 1 lần

void setup() {
  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(sendData);
  Wire.onReceive(receiveData);

  dht.begin();
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.begin(9600);
  Serial.println("UNO (Slave I2C) da san sang!");
}

void loop() {
  unsigned long currentMillis = millis();

  // Cập nhật dữ liệu DHT11 mỗi 2 giây
  if (currentMillis - lastReadTime >= readInterval) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      dataToSend = "Loi doc DHT11";
    } else {
      dataToSend = String(t, 1) + " " + String(h, 1);  // Gửi dạng "27.8 56.2"
    }

    Serial.println("Cap nhat DHT11: " + dataToSend);
    lastReadTime = currentMillis;
  }

  delay(5); // tránh nghẽn I2C
}

// Gửi dữ liệu khi ESP8266 yêu cầu
void sendData() {
  Wire.write(dataToSend.c_str());
}

// Nhận lệnh LED từ ESP8266
void receiveData(int howMany) {
  while (Wire.available()) {
    byte cmd = Wire.read();
    if (cmd == 1) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED: BAT");
    } else if (cmd == 0) {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED: TAT");
    }
  }
}
