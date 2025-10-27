# from flask import Flask, render_template, request, redirect, session
# import mysql.connector

# app = Flask(__name__)
# app.secret_key = "supersecretkey"

# # Kết nối MySQL XAMPP
# db = mysql.connector.connect(
#     host="localhost",
#     user="root",
#     password="",    
#     database="iot"
# )

# led_statuses = {i: "OFF" for i in range(1, 9)}
# API_KEY = "esp8266_secret_key"
# sensor_data = "Chưa có dữ liệu"

# # ===== Đăng nhập =====
# @app.route('/')
# def home():
#     return redirect('/signin')

# @app.route('/signin', methods=['GET', 'POST'])
# def signin():
#     error = None
#     if request.method == 'POST':
#         username = request.form['username']
#         password = request.form['password']
#         cursor = db.cursor(dictionary=True)
#         sql = "SELECT * FROM user WHERE username=%s AND matkhau=%s"
#         cursor.execute(sql, (username, password))
#         user = cursor.fetchone()
#         cursor.close()
#         if user:
#             session['username'] = username
#             session['quyenhan'] = user['quyenhan']
#             return redirect('/index')
#         else:
#             error = "Sai tài khoản hoặc mật khẩu!"
#     return render_template('signin.html', error=error)

# # ===== Trang chính =====
# @app.route('/index')
# def index():
#     if 'username' not in session:
#         return redirect('/signin')
#     username = session['username']
#     quyenhan = session['quyenhan']
#     led_status = led_statuses.get(quyenhan, "OFF")
#     return render_template('index.html',
#                            led_status=led_status,
#                            sensor_data=sensor_data,
#                            username=username,
#                            quyenhan=quyenhan,
#                            temperature="--",
#                            humidity="--")

# # ===== Đăng xuất =====
# @app.route('/signout')
# def signout():
#     session.clear()
#     return redirect('/signin')

# # ===== Nhận dữ liệu từ ESP =====
# @app.route('/data', methods=['POST'])
# def data():
#     global sensor_data
#     sensor_data = request.data.decode("utf-8")
#     print("Dữ liệu nhận:", sensor_data)
#     return "OK"

# # ===== Điều khiển LED =====
# @app.route('/led/<action>')
# def led_control(action):
#     if 'username' not in session:
#         return "Chưa đăng nhập!"
#     quyenhan = session['quyenhan']
#     if action.upper() == "ON":
#         led_statuses[quyenhan] = "ON"
#     elif action.upper() == "OFF":
#         led_statuses[quyenhan] = "OFF"
#     return f"LED {led_statuses[quyenhan]}"

# # ===== Lấy trạng thái LED =====
# @app.route('/led_status')
# def led_status_route():
#     key = request.args.get('key')
#     # Nếu có API key đúng => cho phép truy cập
#     if key == API_KEY:
#         # Lấy trạng thái LED mặc định (ví dụ LED số 1)
#         state = led_statuses.get(1, "OFF")
#         return f"1:{state}"

#     # Còn nếu người dùng web đang đăng nhập => cho phép luôn
#     if 'username' in session:
#         quyenhan = session['quyenhan']
#         state = led_statuses.get(quyenhan, "OFF")
#         return f"{quyenhan}:{state}"

#     # Nếu không phải 2 trường hợp trên => từ chối
#     return "Chưa đăng nhập!"


# if __name__ == '__main__':
#     app.run(host='0.0.0.0', port=5000)


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

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
