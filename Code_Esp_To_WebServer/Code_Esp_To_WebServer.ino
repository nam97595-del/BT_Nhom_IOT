#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// --- THAY ĐỔI CÁC THÔNG TIN DƯỚI ĐÂY ---
const char* ssid = "Tang 1";
const char* password = "khongcomang";
const char* server_ip = "192.168.110.110"; // << IP CỦA LAPTOP BẠN
const int server_port = 5000;
// ---------------------------------------------

// Định nghĩa chân nối với đèn LED
const int ledPin = 5; // D1 trên NodeMCU là GPIO5

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT); // Cấu hình chân LED là OUTPUT
  digitalWrite(ledPin, LOW); // Ban đầu cho đèn tắt

  // Kết nối Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Dang ket noi Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nDa ket noi!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    
    // ------ GỬI DỮ LIỆU NHIỆT ĐỘ (giống như cũ) ------
    String serverUrlPost = "http://" + String(server_ip) + ":" + String(server_port) + "/data";
    http.begin(client, serverUrlPost);
    http.addHeader("Content-Type", "text/plain");
    String dataToSend = "Nhiet do: " + String(random(20, 30));
    int httpResponseCode = http.POST(dataToSend);
    if (httpResponseCode > 0) {
      Serial.printf("Gui du lieu OK, ma phan hoi: %d\n", httpResponseCode);
    } else {
      Serial.printf("Loi khi gui du lieu: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end(); // Quan trọng: phải đóng kết nối cũ

    delay(100); // Chờ một chút trước khi gửi yêu cầu mới

    // ------ NHẬN LỆNH ĐIỀU KHIỂN ĐÈN (phần mới) ------
    String serverUrlGet = "http://" + String(server_ip) + ":" + String(server_port) + "/led_status";
    http.begin(client, serverUrlGet);
    httpResponseCode = http.GET();

    if (httpResponseCode == 200) { // Mã 200 là thành công
      String payload = http.getString();
      Serial.println("Nhan lenh tu server: " + payload);

      if (payload == "ON") {
        digitalWrite(ledPin, HIGH); // Bật đèn
      } else if (payload == "OFF") {
        digitalWrite(ledPin, LOW); // Tắt đèn
      }
    } else {
      Serial.printf("Loi khi lay trang thai den: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
  }

  // Chờ 5 giây rồi lặp lại
  delay(5000); 
}