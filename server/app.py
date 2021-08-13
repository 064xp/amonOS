from flask import Flask, render_template, url_for, session, redirect, jsonify, request
from struct import *
import time
import threading
import sqlite3
from ipc import leerOutput, escribirInput, packPacket, unpackPacket
from db import insertResponse, getResponse, deleteResponse, initDatabase, getAllResponses

app = Flask(__name__)
conn = sqlite3.connect('responses.db', check_same_thread = False)

def lectura():
    while True:
        res = leerOutput()
        print("recieved res")
        #convertir en diccionario
        usuarioBytes, cwdBytes, bufferBytes, secCode = unpackPacket(res)

        user = usuarioBytes.decode('utf8').strip('\x00')
        cwd = cwdBytes.decode('utf8').strip('\x00')
        buffer = bufferBytes.decode('utf8').strip('\x00')
        insertResponse(conn, user, cwd, buffer, secCode)

@app.route('/request', methods = ['POST'])
def inicio():
    command = request.json.get('command')
    user = request.json.get('user')
    cmdBytes = bytes(command, 'utf-8')
    userBytes = bytes(user, 'utf-8')
    # secCode = session["secCode"]
    secCode = 123
    count = 0

    estructura = packPacket(userBytes, secCode, cmdBytes)
    escribirInput(estructura)
    while True:
        if count > 100:
            break;
        # print(requestBuffer)
        response = getResponse(conn, user)
        if response is not None:
            session["user"] = response[0]
            json =  jsonify({
                'user': response[0],
                'cwd': response[1],
                'buffer': response[2],
                'secCode': response[3],
            })
            deleteResponse(conn, user)
            return json
        count += 1
        time.sleep(.2)

    return "Timeout"

if __name__ == '__main__':
    thread = threading.Thread(target = lectura)
    thread.start()
    initDatabase(conn)
    app.secret_key = 'super secret key'
    app.run(debug = True, port = 3000)
