#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>

// --- Cấu hình Wi-Fi và server ---
const char* ssid = "Tang 1";
const char* password = "khongcomang";
const char* server_ip = "192.168.110.201"; // IP của laptop (chạy web.py)
const int server_port = 5000;

// --- Cấu hình I2C ---
#define SLAVE_ADDR 9 // Trùng với địa chỉ trong Uno

void setup() {
  Serial.begin(115200);
  Wire.begin(); // ESP8266 làm Master
  Serial.println("ESP8266 Master I2C ready");

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  Serial.print("Dang ket noi Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nDa ket noi Wi-Fi!");
  Serial.print("Dia chi IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;

    // Gửi dữ liệu mẫu lên server
    String urlPost = "http://" + String(server_ip) + ":" + String(server_port) + "/data";
    http.begin(client, urlPost);
    http.addHeader("Content-Type", "text/plain");
    String dataToSend = "Nhiet do: " + String(random(20, 30));
    int code = http.POST(dataToSend);
    if (code > 0)
      Serial.printf("Da gui du lieu (%d)\n", code);
    else
      Serial.printf("Loi POST: %s\n", http.errorToString(code).c_str());
    http.end();

    delay(100);

    // Nhận trạng thái LED từ server
    String urlGet = "http://" + String(server_ip) + ":" + String(server_port) + "/led_status";
    http.begin(client, urlGet);
    code = http.GET();

    if (code == 200) {
      String payload = http.getString();
      payload.trim();
      Serial.println("Trang thai LED tu server: " + payload);

      Wire.beginTransmission(SLAVE_ADDR);
      if (payload == "ON") {
        Wire.write(1);
        Serial.println("Gui I2C: LED ON");
      } else if (payload == "OFF") {
        Wire.write(0);
        Serial.println("Gui I2C: LED OFF");
      }
      Wire.endTransmission();
    } else {
      Serial.printf("Loi GET: %s\n", http.errorToString(code).c_str());
    }

    http.end();
  }

  delay(5000); // Lặp lại mỗi 5 giây
}
