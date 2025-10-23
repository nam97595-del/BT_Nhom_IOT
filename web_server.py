from flask import Flask, request

app = Flask(__name__)

# BIẾN TOÀN CỤC: Dùng để lưu trạng thái hiện tại của đèn LED
led_status = "OFF"

# --- ROUTE CŨ ĐỂ NHẬN DỮ LIỆU TỪ ESP ---
@app.route('/data', methods=['POST'])
def receive_data():
    data = request.data.decode('utf-8')
    print(f"Da nhan duoc du lieu tu ESP8266: {data}")
    return "OK", 200

# --- ROUTE MỚI: ĐỂ ESP HỎI TRẠNG THÁI ĐÈN ---
@app.route('/led_status', methods=['GET'])
def get_led_status():
    # Chỉ cần trả về trạng thái hiện tại
    return led_status

# --- ROUTE MỚI: ĐỂ BẠN DÙNG TRÌNH DUYỆT BẬT ĐÈN ---
@app.route('/led/on', methods=['GET'])
def turn_led_on():
    global led_status # Cần 'global' để thay đổi biến bên ngoài hàm
    led_status = "ON"
    print("=> Nguoi dung da BAT den")
    return "Da bat den!"

# --- ROUTE MỚI: ĐỂ BẠN DÙNG TRÌNH DUYỆT TẮT ĐÈN ---
@app.route('/led/off', methods=['GET'])
def turn_led_off():
    global led_status
    led_status = "OFF"
    print("=> Nguoi dung da TAT den")
    return "Da tat den!"

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)