from flask import Flask, request

app = Flask(__name__)

@app.route('/')
def home():
    return "Hello from Flask Webserver!"

@app.route('/data', methods=['POST'])
def receive_data():
    data = request.json
    print("Du lieu tu ESP8266: ", data)
    return "OK"

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)