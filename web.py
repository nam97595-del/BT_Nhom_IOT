from flask import Flask, render_template, request, redirect, session
import mysql.connector

app = Flask(__name__)
app.secret_key = "supersecretkey"

# Kết nối MySQL XAMPP
db = mysql.connector.connect(
    host="localhost",
    user="root",
    password="",    
    database="iot"
)

led_statuses = {i: "OFF" for i in range(1, 9)}
sensor_data = "Chưa có dữ liệu"

# ===== Đăng nhập =====
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
            session['quyenhan'] = user['quyenhan']
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
    quyenhan = session['quyenhan']
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

# ===== Nhận dữ liệu từ ESP =====
@app.route('/data', methods=['POST'])
def data():
    global sensor_data
    sensor_data = request.data.decode("utf-8")
    print("Dữ liệu nhận:", sensor_data)
    return "OK"

# ===== Điều khiển LED =====
@app.route('/led/<action>')
def led_control(action):
    if 'username' not in session:
        return "Chưa đăng nhập!"
    quyenhan = session['quyenhan']
    if action.upper() == "ON":
        led_statuses[quyenhan] = "ON"
    elif action.upper() == "OFF":
        led_statuses[quyenhan] = "OFF"
    return f"LED {led_statuses[quyenhan]}"

# ===== Lấy trạng thái LED =====
@app.route('/led_status')
def led_status_route():
    if 'username' not in session:
        return "Chưa đăng nhập!"
    quyenhan = session['quyenhan']
    state = led_statuses.get(quyenhan, "OFF")
    return f"{quyenhan}:{state}"  # trả về dạng "1

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
