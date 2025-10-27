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

const char* ssid = "Lab nghien cuu";
const char* password = "fit@12345";
const char* server_ip = "192.168.0.200";  // <-- NHỚ THAY BẰNG IP ĐÚNG!
const int server_port = 5000;

#define SLAVE_ADDR 9

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  Serial.println("\nKhoi dong...");

  WiFi.begin(ssid, password);
  Serial.print("Dang ket noi WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nDa ket noi WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;

    String url = "http://" + String(server_ip) + ":" + String(server_port) + "/led_status?key=esp8266_secret_key";
    Serial.println("===================================");
    Serial.print("Dang thuc hien GET request toi: ");
    Serial.println(url);

    if (http.begin(client, url)) { // Kiểm tra http.begin thành công
      int httpCode = http.GET();

      if (httpCode > 0) { // HTTP code > 0 nghĩa là đã nhận được phản hồi
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK) { // Chỉ xử lý nếu code là 200
          String payload = http.getString();
          payload.trim();
          Serial.println("Nhan payload tu Flask:");
          Serial.println(payload);
          
          StaticJsonDocument<200> doc; // Giảm kích thước nếu JSON nhỏ
          DeserializationError error = deserializeJson(doc, payload);

          if (error) {
            Serial.print("Loi parse JSON: ");
            Serial.println(error.c_str());
          } else {
            Serial.println("Parse JSON thanh cong!");
            for (int led = 1; led <= 3; led++) {
              const char* state = doc[String(led)];
              String msg = String(led) + ":" + String(state);
              
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
      } else {
        // Lỗi ở đây có nghĩa là không thể kết nối tới server
        Serial.printf("[HTTP] GET... that bai, loi: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      Serial.printf("[HTTP] Khong the ket noi\n");
    }
  } else {
    Serial.println("Mat ket noi WiFi...");
  }

  delay(5000);
}