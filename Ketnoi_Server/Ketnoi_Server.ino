#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>

const char* ssid = "Duc Anh";
const char* password = "31122004";
const char* server_ip = "192.168.2.3"; // IP máy chạy Flask
const int server_port = 5000;

#define SLAVE_ADDR 9

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  WiFi.begin(ssid,password);
  Serial.print("Dang ket noi WiFi");
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nDa ket noi WiFi: "+WiFi.localIP().toString());
}

void loop() {
  if(WiFi.status()==WL_CONNECTED){
    HTTPClient http;
    WiFiClient client;

    // Lấy trạng thái LED từ Flask, dạng "1:ON" hoặc "2:OFF"
    String urlGet = "http://"+String(server_ip)+":"+String(server_port)+"/led_status";
    http.begin(client,urlGet);
    int code = http.GET();
    if(code==200){
      String payload = http.getString();
      payload.trim();
      Serial.println("LED status từ Flask: "+payload);

      // Gửi I2C: "<led_num:state>"
      Wire.beginTransmission(SLAVE_ADDR);
      Wire.write('<');
      Wire.write(payload.c_str());
      Wire.write('>');
      Wire.endTransmission();
      Serial.println("Gui I2C: "+payload);
    } else Serial.println("Loi GET: "+http.errorToString(code));

    http.end();
  }
  delay(5000); // kiểm tra 5 giây/lần
}
