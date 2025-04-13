import sqlite3 #sqlite for database management
import serial
from datetime import datetime
import time

arduino = serial.Serial('COM5', 9600, timeout=1)  # Adjust COM port

conn = sqlite3.connect("C:\\Users\\hannah\\Documents\\GitHub\\TimelyRx\\python\\dosage_data.db")
c = conn.cursor()

c.execute("CREATE TABLE IF NOT EXISTS doses(id INTEGER PRIMARY KEY AUTOINCREMENT, Date TEXT, Time TEXT)")
conn.commit()

def logDose(date, time_):
    c.execute("INSERT into doses (Date, Time) VALUES (?, ?)", (str(date), str(time_)))
    conn.commit()

try:
    while True:
        if arduino.in_waiting:
            line = arduino.readline().decode('utf-8').strip()
            try:
                timestamp = datetime.strptime(line, "%Y-%m-%d %H:%M:%S")
                date = timestamp.date()
                time_ = timestamp.time()
                logDose(date, time_)
                print("Logged:", date, time_)
            except ValueError:
                print("Invalid timestamp format:", line)

        time.sleep(0.1)  # small delay to avoid CPU overuse
except KeyboardInterrupt:
    print("Exiting program.")

conn.close()