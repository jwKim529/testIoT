import time
import datetime
import psutil
import sqlite3
from matplotlib import pyplot as plt


class SystemUsageData :
    def getNow(self):
        return datetime.datetime.now()
    def getCPULog(self):
        return psutil.cpu_percent(interval=1)
    def getMemoryLog(self):
        return psutil.virtual_memory().percent

    def __str__(self):
        return "{} CPU Usage: {}% Memory Usage: {}%\n".format(self.getNow(),self.getCPULog(), self.getMemoryLog())

def executeDB(cur, text, options=None):
    if options == None :
        cur.execute(text)
    else :
        cur.execute(text, options)
    return cur

class DBControl :
    def __init__(self, db, tableName, createDB):
        self.__con = sqlite3.connect(db)
        self.__cur = self.__con.cursor()
        executeDB(self.__cur, "DROP TABLE IF EXISTS {};".format(tableName))

        createText = "CREATE TABLE IF NOT EXISTS {} (id INTEGER PRIMARY KEY AUTOINCREMENT,".format(tableName)
        for i,(k,v) in enumerate(createDB.items()) :
            if i != len(createDB.items())-1:
                createText+="{} {},".format(k,v)
            else :
                createText+="{} {});".format(k,v)
        executeDB(self.__cur, createText)

    def writeData(self, tableName, columnName, data):
        insertText = "INSERT INTO {} ({}) VALUES ({})".format(
            tableName,
            ", ".join(columnName),
            ",".join(("? "*len(data)).strip().split())
        )
        executeDB(self.__cur,insertText,data)

    def readData(self, tableName, columnName):
        return executeDB(self.__cur, "SELECT {} FROM {}".format(
            ", ".join(columnName),
            tableName
        ))

    def closeDB(self):
        self.__cur.close()
        self.__con.close()
        return "COMPLETE"


if __name__ == "__main__" :
    logDBName = "system_info.db" #DB위치/이름 지정
    dbdata = {
        'tablename' : 'system_info',
        'columns' : {
            'timestamp':'DATETIME NOT NULL',
            'cpu_usage':'REAL NOT NULL',
            'memory_usage':'REAL NOT NULL'
        }
    }
    pcUse = SystemUsageData()
    dbC = DBControl(logDBName, dbdata['tablename'], dbdata['columns'])

    for i in range(3 * 12):
        dbC.writeData(dbdata['tablename'],dbdata['columns'].keys(),(pcUse.getNow(),pcUse.getCPULog(), pcUse.getMemoryLog()))
        time.sleep(5)

    ctData = []
    cpuData = []
    memData = []
    for cTime, uCpu, uMemory in pcUse.readData(dbdata['tablename'],dbdata['columns'].keys()) :
        ctData.append(cTime)
        cpuData.append(uCpu)
        memData.append(uMemory)

    plt.plot(range(len(ctData)), cpuData, label='CPU')
    plt.plot(range(len(ctData)), memData, label='Memory')
    plt.xticks(range(len(ctData)))
    plt.xlabel("TIME(Start:{})".format(ctData[0]))
    plt.ylabel("Usage(%)")
    plt.title("PC Usage Chart")
    plt.legend(loc='upper left')
    plt.savefig(f'<{ctData[0]}~{ctData[-1]}> UsageChart.png')

    # DB 연결종료
    pcUse.closeDB()
