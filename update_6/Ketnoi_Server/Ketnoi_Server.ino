
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


// Dán code này để THAY THẾ toàn bộ hàm loop() cũ của ESP8266

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    
    // ===== BẮT ĐẦU REQUEST 1: LẤY DATA TỪ ARDUINO VÀ POST LÊN SERVER =====
    { // Tạo một scope riêng cho request 1
      HTTPClient http_post;
      WiFiClient client_post;
      
      String dataToSendToServer = "Nhiet do: --, Do am: --"; // Giá trị mặc định

      // --- PHẦN SỬA ĐỔI BẮT ĐẦU ---
      
      // 1. Yêu cầu Arduino gửi dữ liệu (Master ĐỌC)
      Serial.println("===================================");
      Serial.print("Dang yeu cau I2C tu Arduino (Slave)... ");
      
      // Yêu cầu 16 byte dữ liệu từ Slave địa chỉ 9
      // Arduino sẽ kích hoạt hàm sendData()
      Wire.requestFrom(SLAVE_ADDR, 16); 
      
      String i2cData = "";
      while (Wire.available()) {
        char c = Wire.read();
        i2cData += c;
      }
      i2cData.trim(); // Xóa ký tự rỗng
      Serial.println("Nhan duoc: " + i2cData);

      // 2. Xử lý chuỗi nhận được từ Arduino (ví dụ: "25.1 60.0")
      if (i2cData.length() > 0 && i2cData != "Loi DHT11") {
        int spaceIndex = i2cData.indexOf(' '); // Tìm dấu cách
        if (spaceIndex != -1) {
          String temp = i2cData.substring(0, spaceIndex);
          String humid = i2cData.substring(spaceIndex + 1);
          
          // 3. Định dạng lại chuỗi để POST lên server
          // (Phải khớp với logic xử lý chuỗi bên web.py)
          dataToSendToServer = "Nhiet do: " + temp + ", Do am: " + humid;
        } else {
          dataToSendToServer = "Loi dinh dang I2C";
        }
      } else {
        dataToSendToServer = "Nhiet do: --, Do am: --"; // Gửi giá trị lỗi
      }
      // --- PHẦN SỬA ĐỔI KẾT THÚC ---


      String urlPost = "http://" + String(server_ip) + ":" + String(server_port) + "/data";
      Serial.print("Dang thuc hien POST request toi: ");
      Serial.println(urlPost);
      Serial.println("Data: " + dataToSendToServer);

      if (http_post.begin(client_post, urlPost)) {
        http_post.addHeader("Content-Type", "text/plain");
        
        // 4. Gửi dữ liệu ĐÃ XỬ LÝ lên, thay vì random()
        int code = http_post.POST(dataToSendToServer); 
        
        if (code > 0)
          Serial.printf("[HTTP] POST... Da gui du lieu, code: %d\n", code);
        else
          Serial.printf("[HTTP] POST... that bai, loi: %s\n", http_post.errorToString(code).c_str());
          
        http_post.end();
      } else {
        Serial.printf("[HTTP] POST... Khong the ket noi\n");
      }
    } // Hủy http_post và client_post
    
    // Đợi một chút để giải phóng tài nguyên
    delay(200); 

    // ===== BẮT ĐẦU REQUEST 2: GET LED STATUS (Giữ nguyên) =====
    { // Tạo một scope riêng cho request 2
      HTTPClient http_get;
      WiFiClient client_get;

      String urlGet = "http://" + String(server_ip) + ":" + String(server_port) + "/led_status?key=esp8266_secret_key";
      Serial.print("Dang thuc hien GET request toi: ");
      Serial.println(urlGet);

      if (http_get.begin(client_get, urlGet)) { 
        int httpCode = http_get.GET();

        if (httpCode > 0) {
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);

          if (httpCode == HTTP_CODE_OK) {
            String payload = http_get.getString();
            payload.trim();
            Serial.println("Nhan payload tu Flask:");
            Serial.println(payload);
            
            StaticJsonDocument<200> doc;
            DeserializationError error = deserializeJson(doc, payload);

            if (error) {
              Serial.print("Loi parse JSON: ");
              Serial.println(error.c_str());
            } else {
              Serial.println("✅ Parse JSON thanh cong!");
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
          Serial.printf("[HTTP] GET... that bai, loi: %s\n", http_get.errorToString(httpCode).c_str());
        }
        http_get.end();
      } else {
        Serial.printf("[HTTP] GET... Khong the ket noi\n");
      }
    } // Hủy http_get và client_get

  } else {
    Serial.println("Mat ket noi WiFi...");
  }

  Serial.println("===================================");
  delay(3000); // Giữ delay 3 giây là hợp lý
}