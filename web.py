from flask import Flask, render_template, request, redirect, url_for

app = Flask(__name__)

led_status = "OFF"
temperature = "Chưa có dữ liệu"

@app.route("/")
def index():
    return render_template("index.html", led=led_status, temp=temperature)

@app.route("/set_led/<state>")
def set_led(state):
    global led_status
    if state.upper() in ["ON", "OFF"]:
        led_status = state.upper()
    return redirect(url_for("index"))

@app.route("/data", methods=["POST"])
def data():
    global temperature
    temperature = request.data.decode("utf-8")
    print("ESP8266 gửi:", temperature)
    return "OK"

@app.route("/led_status")
def get_led_status():
    return led_status

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
