import os
import flask
from replit import db
from datetime import datetime

boot = True
app = flask.Flask(__name__)
server_key = os.getenv("SERVER_KEY")

@app.route("/")
def default():
    return "Battleship Server."

@app.route('/' + server_key + "/connect")
def connect():
    global boot
    if boot:
        db["boots"].append(str(datetime.now()))
        db["games"] = {}
        boot = False
    return '1'

@app.route('/' + server_key + "/log")
def log():
    db["connection_log"].append((flask.request.args["replUser"], flask.request.args["alias"], str(datetime.now())))
    return '1'

@app.route('/' + server_key + "/like")
def like():
    db["likes"] += 1
    db["like_log"].append((flask.request.args["replUser"], str(datetime.now())))
    return "Thank you!"

@app.route('/' + server_key + "/join")
def join():
    code = flask.request.args["code"]
    if code not in db["games"]:
        return '0'
    db["games"][code][1] = flask.request.args["user"]
    db["games"][code][2] = True
    return '1' + db["games"][code][0]

@app.route('/' + server_key + "/new")
def new():
    code = flask.request.args["code"]
    if code in db["games"]:
        return '0'
    # code {host, guest, two players, host ready, guest ready, turn, x, y, message1, message2}
    db["games"][code] = [flask.request.args["user"], "", False, False, False, '0', 1, 1, "", ""]
    return '1'

@app.route('/' + server_key + "/sping")
def sping():
    code = flask.request.args["code"]
    if code in db["games"] and db["games"][code][2]:
        return '1' + db["games"][code][1]
    return '0'

@app.route('/' + server_key + "/scancel")
def scancel():
    code = flask.request.args["code"]
    if code in db["games"]:
        db["games"].pop(code)
    return '1'

@app.route('/' + server_key + "/ready")
def ready():
    code = flask.request.args["code"]
    if code in db["games"]:
        db["games"][code][3 + int(flask.request.args["host"])] = True
        return '1'
    return '0'

@app.route('/' + server_key + "/gping")
def gping():
    code = flask.request.args["code"]
    if code in db["games"] and db["games"][code][3] and db["games"][code][4]:
        return '1'
    return '0'

@app.route('/' + server_key + "/leave")
def leave():
    code = flask.request.args["code"]
    if code in db["games"]:
        db["games"].pop(code)
    return '1'

@app.route('/' + server_key + "/fire")
def fire():
    code = flask.request.args["code"]
    if code in db["games"] and flask.request.args["host"] == db["games"][code][5]: # code {host, guest, two players, host ready, guest ready, turn, x, y, message1, message2}
        db["games"][code][6] = flask.request.args["x"]
        db["games"][code][7] = flask.request.args["y"]
        db["games"][code][8] = flask.request.args["message"]
        db["games"][code][5] = ['1', '0'][int(db["games"][code][5])]
        return '1'
    return '0'

@app.route('/' + server_key + "/fping")
def fping():
    code = flask.request.args["code"]
    if code in db["games"] and db["games"][code][9] != "":
        m = db["games"][code][9]
        db["games"][code][9] = ""
        return '1' + m
    return '0'

@app.route('/' + server_key + "/iping")
def iping():
    code = flask.request.args["code"]
    if code in db["games"] and db["games"][code][5] == flask.request.args["host"]:
        return '1' + str(db["games"][code][6]) + str(db["games"][code][7]) + db["games"][code][8]
    return '0'

@app.route('/' + server_key + "/acknowledge")
def acknowledge():
    code = flask.request.args["code"]
    if code in db["games"]:
        db["games"][code][9] = flask.request.args["sunk"]
        if flask.request.args["sunk"][0] == '3':
                db["game_log"].append((db["games"][code][0], db["games"][code][1], db["games"][code][5], str(datetime.now())))
        return '1'
    return '0'

app.run("0.0.0.0")