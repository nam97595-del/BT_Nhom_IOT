

from flask import Flask, render_template, request, redirect, session, jsonify
import mysql.connector

app = Flask(__name__)
app.secret_key = "supersecretkey"

# ===== Kết nối MySQL XAMPP =====
db = mysql.connector.connect(
    host="localhost",
    user="root",
    password="",
    database="iot"
)

# ===== Biến toàn cục =====
led_statuses = {1: "OFF", 2: "OFF", 3: "OFF"}   # 3 LED tương ứng 3 user
API_KEY = "esp8266_secret_key"
sensor_data = "Chưa có dữ liệu"

# ===== Đăng nhập =====
@app.route('/')
def home():
    return redirect('/signin')

@app.route('/signin', methods=['GET', 'POST'])
def signin():
    error = None
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        cursor = db.cursor(dictionary=True)
        sql = "SELECT * FROM user WHERE username=%s AND matkhau=%s"
        cursor.execute(sql, (username, password))
        user = cursor.fetchone()
        cursor.close()
        if user:
            session['username'] = username
            session['quyenhan'] = user['quyenhan']  # ví dụ 1, 2, 3
            return redirect('/index')
        else:
            error = "Sai tài khoản hoặc mật khẩu!"
    return render_template('signin.html', error=error)

# ===== Trang chính =====
@app.route('/index')
def index():
    if 'username' not in session:
        return redirect('/signin')
    username = session['username']
    quyenhan = session['quyenhan']   # 1, 2, 3
    led_status = led_statuses.get(quyenhan, "OFF")
    return render_template('index.html',
                           led_status=led_status,
                           sensor_data=sensor_data,
                           username=username,
                           quyenhan=quyenhan,
                           temperature="--",
                           humidity="--")

# ===== Đăng xuất =====
@app.route('/signout')
def signout():
    session.clear()
    return redirect('/signin')

# ===== Nhận dữ liệu cảm biến từ ESP =====
@app.route('/data', methods=['POST'])
def data():
    global sensor_data
    sensor_data = request.data.decode("utf-8")
    print("Dữ liệu nhận:", sensor_data)
    return "OK"

# ===== Điều khiển LED (người dùng web nhấn nút) =====
@app.route('/led/<action>')
def led_control(action):
    if 'username' not in session:
        return "Chưa đăng nhập!"
    quyenhan = session['quyenhan']   # số LED tương ứng (1, 2, 3)
    if action.upper() == "ON":
        led_statuses[quyenhan] = "ON"
    elif action.upper() == "OFF":
        led_statuses[quyenhan] = "OFF"
    return f"LED{quyenhan} -> {led_statuses[quyenhan]}"

# ===== ESP8266 hỏi trạng thái LED =====
@app.route('/led_status')
def led_status_route():
    key = request.args.get('key')
    print(f"[DEBUG] Request to /led_status with key: {key}")

    # Ưu tiên kiểm tra key của ESP
    if key == API_KEY:
        # Chuyển key int -> string để đảm bảo JSON đúng chuẩn
        led_json = {str(k): v for k, v in led_statuses.items()}
        print(f"[GỬI ESP] LED status: {led_json}")
        return jsonify(led_json)
    
    # Nếu không phải ESP, kiểm tra xem có phải người dùng web không
    elif 'username' in session:
        quyenhan = session['quyenhan']
        state = led_statuses.get(quyenhan, "OFF")
        # Trả về JSON để đồng nhất, dễ xử lý hơn ở client
        return jsonify({"led": quyenhan, "status": state})

    # Nếu không phải cả hai, từ chối truy cập
    else:
        # Trả về lỗi 401 Unauthorized thay vì chuỗi text
        return "Unauthorized", 401
# Thêm route này vào cuối file web.py

@app.route('/get_updated_data')
def get_updated_data():
    if 'username' not in session:
        return jsonify({"error": "Unauthorized"}), 401

    quyenhan = session['quyenhan']
    led_status = led_statuses.get(quyenhan, "OFF")

    temp_val = "--"
    humid_val = "--"

    # Đoạn code xử lý chuỗi y hệt như trong hàm index()
    if "Nhiet do" in sensor_data:
        try:
            parts = sensor_data.split(',')
            temp_val = parts[0].split(':')[1].strip()
            humid_val = parts[1].split(':')[1].strip()
        except Exception:
            pass
            
    # Trả về dữ liệu dưới dạng JSON
    return jsonify({
        "temperature": temp_val,
        "humidity": humid_val,
        "led_status": led_status
    })


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
