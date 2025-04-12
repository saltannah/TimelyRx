import sqlite3 #sqlite for database management
import serial
from datetime import datetime

arduino = serial.Serial('COM5', 9600, timeout=1)  # Adjust COM port

conn = sqlite3.connect("C:\Users\hannah\Documents\GitHub\TimelyRx\python\dosage_data.db")
c = conn.cursor()

def logDose(date, time):
    c.execute("INSERT into doses (date, time) values (?, ?)")

while True:
    if ser.in_waiting:
        line = ser.readline().decode('utf-8').strip()

    try: timestamp = datetime.strptime(line, "%Y-%m-%d %H:%M:%S")

conn.close()