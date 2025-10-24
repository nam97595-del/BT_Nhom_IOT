from flask import Flask, render_template, request

app = Flask(__name__)

led_status = "OFF"
sensor_data = "Chưa có dữ liệu"

@app.route('/')
def index():
    return render_template('index.html', led_status=led_status, sensor_data=sensor_data)

@app.route('/data', methods=['POST'])
def data():
    global sensor_data
    sensor_data = request.data.decode("utf-8")
    print("Dữ liệu nhận:", sensor_data)
    return "OK"

@app.route('/led_status')
def led_status_route():
    return led_status

@app.route('/led/<action>')
def led_control(action):
    global led_status
    if action.upper() == "ON":
        led_status = "ON"
    elif action.upper() == "OFF":
        led_status = "OFF"
    return f"LED {led_status}"

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
