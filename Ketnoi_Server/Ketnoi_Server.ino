// #include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
// #include <Wire.h>

// const char* ssid = "Tang 2";
// const char* password = "khongcomang";
// const char* server_ip = "192.168.2.3"; // IP máy chạy Flask
// const int server_port = 5000;

// #define SLAVE_ADDR 9

// void setup() {
//   Serial.begin(115200);
//   Wire.begin();
  
//   WiFi.begin(ssid,password);
//   Serial.print("Dang ket noi WiFi");
//   while(WiFi.status()!=WL_CONNECTED){
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("\nDa ket noi WiFi: "+WiFi.localIP().toString());
// }

// void loop() {
//   if(WiFi.status()==WL_CONNECTED){
//     HTTPClient http;
//     WiFiClient client;

//     // Lấy trạng thái LED từ Flask, dạng "1:ON" hoặc "2:OFF"
//     String urlGet = "http://"+String(server_ip)+":"+String(server_port)+"/led_status?key=esp8266_secret_key";
//     http.begin(client,urlGet);
//     int code = http.GET();
//     if(code==200){
//       String payload = http.getString();
//       payload.trim();
//       Serial.println("LED status từ Flask: "+payload);

//       // Gửi I2C: "<led_num:state>"
//       Wire.beginTransmission(SLAVE_ADDR);
//       Wire.write('<');
//       Wire.write(payload.c_str());
//       Wire.write('>');
//       Wire.endTransmission();
//       Serial.println("Gui I2C: "+payload);
//     } else Serial.println("Loi GET: "+http.errorToString(code));

//     http.end();
//   }
//   delay(5000); // kiểm tra 5 giây/lần
// }


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>

const char* ssid = "Tang 2";
const char* password = "khongcomang";
const char* server_ip = "192.168.2.3";  // IP máy Flask
const int server_port = 5000;

#define SLAVE_ADDR 9

void setup() {
  Serial.begin(115200);
  Wire.begin();

  WiFi.begin(ssid, password);
  Serial.print("Dang ket noi WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nDa ket noi WiFi: " + WiFi.localIP().toString());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;

    String url = "http://" + String(server_ip) + ":" + String(server_port) + "/led_status?key=esp8266_secret_key";
    http.begin(client, url);
    int code = http.GET();

    if (code == 200) {
      String payload = http.getString();
      payload.trim();
      Serial.println("Nhan JSON tu Flask:");
      Serial.println(payload);

      // Đảm bảo payload là JSON thực sự (phải có dấu { })
      if (!payload.startsWith("{")) {
        Serial.println("❌ Lỗi: payload không phải JSON hợp lệ!");
      } else {
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
          Serial.print("❌ Lỗi parse JSON: ");
          Serial.println(error.c_str());
        } else {
          // In toàn bộ key-value ra để debug
          for (JsonPair kv : doc.as<JsonObject>()) {
            Serial.printf("Key: %s - Value: %s\n", kv.key().c_str(), kv.value().as<const char*>());
          }

          // Gửi qua I2C từng LED
          for (int led = 1; led <= 3; led++) {
            String state = doc[String(led)].as<String>();
            String msg = String(led) + ":" + state;

            Wire.beginTransmission(SLAVE_ADDR);
            Wire.write('<');
            Wire.write(msg.c_str());
            Wire.write('>');
            Wire.endTransmission();

            Serial.println("Gui I2C: " + msg);
            delay(100);
          }
        }
      }
    }
    http.end();
  }

  delay(5000);
}
