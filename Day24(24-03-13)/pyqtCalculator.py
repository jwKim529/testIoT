import sys
from PyQt5.QtWidgets import (QApplication,
                             QWidget,
                             QPushButton,
                             QMainWindow,
                             QDesktopWidget,
                             QLabel,
                             QHBoxLayout,
                             QVBoxLayout,
                             QLineEdit)

class MyApp(QMainWindow):

    def __init__(self):
        super().__init__()
        self.vboxes = []
        self.vboxes.append(self.setCalculatorVBox())
        self.initUI()

    def setCalculatorVBox(self):
        hbox_structure_text = [
            "[] + [] = [] {계산}",
            # "[] - [] = [] {계산}",
            # "[] * [] = [] {계산}",
            # "[] / [] = [] {계산}",
        ]
        vbox = QVBoxLayout()
        vbox.addStretch(1)
        for txt in hbox_structure_text:
            new_hbox = self.createHBox(txt)
            vbox.addLayout(new_hbox)
        vbox.addStretch(1)

        return vbox

    def initUI(self):
        for vbox in self.vboxes:
            wid1 = QWidget(self)
            self.setCentralWidget(wid1)
            wid1.setLayout(vbox)

        self.setWindowTitle('MyWindow')
        self.setGeometry(100,100,800,400)
        self.center()
        self.show()

    def center(self):
        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())

    def createHBox(self, text):
        tList = text.split()
        hbox = QHBoxLayout()
        hbox.addStretch(1)
        structs = {}
        for data in tList :
            if data == "[]":
                new = QLineEdit()
                if 'lineedit' not in structs.keys() :
                    structs['lineedit'] = [new]
                else :
                    structs['lineedit'].append(new)
                hbox.addWidget(new)
            elif data in ['+','-','*','/','='] :
                new = QLabel(data)
                if 'qlabel' not in structs.keys():
                    structs['qlabel'] = [data]
                else:
                    structs['qlabel'].append(data)
                hbox.addWidget(new)
            else :
                new = QPushButton(data[1:-1])
                structs['qpushbutton'] = new
                new.clicked.connect(lambda: self.calculate(structs))
                hbox.addWidget(new)
        hbox.addStretch(1)
        return hbox

    def calculate(self, structs_dict):
        answer = 0
        operator = '+'
        operator_cursor = 0
        result_label = structs_dict['lineedit'][-1]
        for target_label in structs_dict['lineedit'][:-1] :
            txt = target_label.text()
            if txt.isnumeric() and operator in ['+','-','*','/'] :
                if operator == '+' :
                    answer += int(txt)
                elif operator == '-':
                    answer -= int(txt)
                elif operator == '*':
                    answer *= int(txt)
                elif operator == '/' :
                    answer /= int(txt)
                operator = structs_dict['qlabel'][operator_cursor]
                operator_cursor+=1
            else :
                result_label.setText("잘못된 입력입니다. 다시 확인해주세요.")
                return 0
        result_label.setText(str(answer))

if __name__ == '__main__':
   app = QApplication(sys.argv)
   ex = MyApp()
   sys.exit(app.exec_())
