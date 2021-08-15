from flask import Flask, render_template, url_for, session, redirect, jsonify, request
from struct import *
import time
import threading
import sqlite3
from ipc import leerOutput, escribirInput, packPacket, unpackPacket
from db import insertResponse, insertRequest, getResponse, requestCleanup, initDatabase, getAllResponses

app = Flask(__name__)

def lectura():
    conn = sqlite3.connect('responses.db')
    while True:
        res = leerOutput()
        usuarioBytes, cwdBytes, bufferBytes, secCode = unpackPacket(res)

        user = usuarioBytes.decode('utf8').strip('\x00')
        cwd = cwdBytes.decode('utf8').strip('\x00')
        buffer = bufferBytes.decode('utf8').strip('\x00')
        insertResponse(conn, user, cwd, buffer, secCode)

@app.route('/')
def index():
    session["user"] = ""
    return render_template("index.html")

@app.route('/clean')
def clean():
    if 'user' in session:
        session.pop("user")
    return 'Cleaned!'

@app.route('/request', methods = ['POST'])
def requestHandler():
    conn = sqlite3.connect('responses.db')
    command = request.json.get('command')
    try:
        user = session['user']
    except:
        user = ''

    cmdBytes = bytes(command, 'utf-8')
    userBytes = bytes(user, 'utf-8')
    secCode = insertRequest(conn, user)
    estructura = packPacket(userBytes, secCode, cmdBytes)
    escribirInput(estructura)

    count = 0
    while True:
        if count > 100:
            break;

        response = getResponse(conn, secCode)
        if response is not None:
            session["user"] = response[0]
            json =  jsonify({
                'user': response[0],
                'cwd': response[1],
                'buffer': response[2],
                'secCode': response[3],
            })
            requestCleanup(conn, secCode)
            return json
        count += 1
        time.sleep(.01)

    requestCleanup(conn, secCode)
    return jsonify({"error": "Request to server timed out"}), 408

if __name__ == '__main__':
    conn = sqlite3.connect('responses.db')
    thread = threading.Thread(target = lectura)
    thread.start()
    initDatabase(conn)
    app.secret_key = 'super secret key'
    app.run(debug = True, port = 3000)
