import subprocess
import datetime
import time
import atexit
import os
from pymongo import MongoClient
from flask import Flask, render_template, redirect
from apscheduler.schedulers.background import BackgroundScheduler
from apscheduler.triggers.interval import IntervalTrigger
app = Flask(__name__)
uri = "mongodb://0416043:patty716@140.113.195.24/DB_0416043"
client = MongoClient(uri)

ip = 'http://192.168.0.105:5000'

db = client['DB_0416043']
collection = db['Table1']

DBaccess = False


RelTep=0

def toRecord():
    runClient = "./coap-client -m get -o temp.txt coap://192.168.0.103/light"
    p = subprocess.Popen(runClient, shell=True, stderr=subprocess.PIPE)
    tmp = open("temp.txt", "r")
    Tep = tmp.readline()
    tmp.close()
    global RelTep
    RelTep = Tep[:2]+'.'+Tep[2:]
    result={
              "Temperature": RelTep,
              "Time": datetime.datetime.utcnow()
           }
    post_id = collection.insert_one(result).inserted_id


@app.route('/',methods=['GET'])
def index():
  global DBaccess
  if(DBaccess==False):
        DBaccess=True
        scheduler = BackgroundScheduler()
        scheduler.start()
        scheduler.add_job(
            func=toRecord,  
            trigger=IntervalTrigger(seconds=10),
            id='db_job',
            name='Print date and time every five seconds',
            replace_existing=True)
        # Shut down the scheduler when exiting the app
        atexit.register(lambda: scheduler.shutdown())
  return render_template('index.html',response=RelTep)

@app.route('/my-link-close/')
def my_link():
  runClient = "./coap-client -m put -o temp.txt -e 0 coap://192.168.0.103/light"
  print("close")
  p = subprocess.Popen(runClient, shell=True, stderr=subprocess.PIPE)
  return redirect(ip,code=302);

@app.route('/my-link-open/')
def my_link_open():
  runClient = "./coap-client -m put -o temp.txt -e 1 coap://192.168.0.103/light"
  print("open")
  p = subprocess.Popen(runClient, shell=True, stderr=subprocess.PIPE)
  return redirect(ip,code=302)

if __name__ == '__main__':
  app.run(debug=True, host='0.0.0.0')
