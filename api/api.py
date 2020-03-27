from flask import Flask

app = Flask(__name__)


@app.route("/msg")
def msg():

    file = open('txt/msg.txt', 'r')

    msg = file.readline()
    file.close

    return msg


if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True)
