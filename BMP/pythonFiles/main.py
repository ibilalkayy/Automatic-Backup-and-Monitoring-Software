# Importing the Libraries
from PyQt5.QtWidgets import QApplication, QComboBox, QFileDialog, QWidget, QMessageBox
from PyQt5 import QtWidgets, uic, QtCore 
import os, sys, Google, monitor
from ctypes import cdll
import ctypes as ct 

# Class for First Window
class MainWindow(QWidget):
	def __init__(self):
		super(MainWindow, self).__init__()

		# Calling the file
		uic.loadUi("selectDrive.ui", self)

		# Calling the functions
		self.extDriveButton.toggled.connect(self.extDriveFunction)
		self.cloudButton.toggled.connect(self.cloudFunction)

	def extDriveFunction(self):
		radioButton = self.sender()
		if radioButton.isChecked():
			self.nextButton1.clicked.connect(self.gotoPath1)

	def cloudFunction(self):
		radioButton = self.sender()
		if radioButton.isChecked():
			self.nextButton1.clicked.connect(self.gotoPath2)

	def gotoPath1(self):
		path1 = selectPath1()
		widget.addWidget(path1)
		widget.setCurrentIndex(widget.currentIndex() + 1)

	def gotoPath2(self):
		path2 = selectPath2()
		widget.addWidget(path2)
		widget.setCurrentIndex(widget.currentIndex() + 1)

# Class for Second Window
class selectPath1(QWidget):
	def __init__(self):
		super(selectPath1, self).__init__()

		# Calling the file
		uic.loadUi("selectPath1.ui", self)

		# Calling the functions
		self.nameBrowse1.clicked.connect(self.browseFile1)
		self.nameBrowse2.clicked.connect(self.browseFile2)
		self.backButton1.clicked.connect(self.backWindow)
		self.finishButton1.clicked.connect(self.operation)
		self.cancelButton1.clicked.connect(self.closeEvent)

	def browseFile1(self):
		self.fname1 = QtWidgets.QFileDialog.getExistingDirectory(
			None, 'Select folder:', './img', QtWidgets.QFileDialog.ShowDirsOnly
		)
		if self.fname1:
			self.fileName1.setText(self.fname1)

	def browseFile2(self):
		self.fname2 = QtWidgets.QFileDialog.getExistingDirectory(
			None, 'Select folder:', './img', QtWidgets.QFileDialog.ShowDirsOnly
		)
		if self.fname2:
			self.fileName2.setText(self.fname2)

	# Function of Back Button
	def backWindow(self):
		mainWindow = MainWindow()
		widget.addWidget(mainWindow)
		widget.setCurrentIndex(widget.currentIndex() + 1)

	# Function of Next Button
	def operation(self):
		if self.fileName1.text() == "" or self.fileName2.text() == "":
			self.message.setText("Error: Browse the file(s)")
			self.message.setStyleSheet("color: red")
		else:
			data = cdll.LoadLibrary("/go/to/cFiles/main.so")
			data.transfer.argtypes = ct.c_char_p, ct.c_char_p
			data.transfer.restype = None
			os.system(f"rsync -a {self.fname1}/ {self.fname2}")
			self.fname1 = bytes(self.fname1, encoding="utf-8")
			self.fname2 = bytes(self.fname2, encoding="utf-8")
			data.getNotified(self.fname1, self.fname2)

	# Function of Cancel Button
	def closeEvent(self):
		reply = QMessageBox.question(self, "Window Close", "Are you sure to close the window?",
			QMessageBox.Yes | QMessageBox.No, QMessageBox.No)

		if reply == QMessageBox.Yes:
			QtCore.QTimer.singleShot(0, widget.close)
		else:
			print("")

# Class for Third Window
class selectPath2(QWidget):
	def __init__(self):
		super(selectPath2, self).__init__()

		# Calling the file
		uic.loadUi("selectPath2.ui", self)

		# Calling the functions
		self.nameBrowse1.clicked.connect(self.browseFile1)
		self.backButton2.clicked.connect(self.backWindow)
		self.cancelButton2.clicked.connect(self.closeEvent)
		self.nextButton2.clicked.connect(self.cloudSelected)
		self.comboBox.currentTextChanged.connect(self.browseFile2)

	def browseFile1(self):
		self.fname1 = QtWidgets.QFileDialog.getExistingDirectory(
			None, 'Select folder:', './img', QtWidgets.QFileDialog.ShowDirsOnly
		)
		if self.fname1:
			self.fileName1.setText(self.fname1)

	def browseFile2(self):
		self.combo = self.findChild(QComboBox, "comboBox")

	# Function of Cloud
	def cloudSelected(self):
		if self.fileName1.text() == "" or self.comboBox.currentText() == "":
			self.message.setText("Browse the file(s)")
			self.message.setStyleSheet("color: red")
		elif self.comboBox.currentText() == "Google Drive":
			Google.main(self.fname1 + '/')
			monitor.monitorFolders(self.fname1 + '/')
		else:
			self.message.setText("")

	# Function of Back Button
	def backWindow(self):
		mainWindow = MainWindow()
		widget.addWidget(mainWindow)
		widget.setCurrentIndex(widget.currentIndex() + 1)

	# Function of Cancel Button
	def closeEvent(self):
		reply = QMessageBox.question(self, "Window Close", "Are you sure to close the window?",
			QMessageBox.Yes | QMessageBox.No, QMessageBox.No)

		if reply == QMessageBox.Yes:
			QtCore.QTimer.singleShot(0, widget.close)
		else:
			print("")

if __name__ == "__main__":
	app = QApplication(sys.argv)
	widget = QtWidgets.QStackedWidget()
	mainWindow = MainWindow()
	widget.addWidget(mainWindow)
	widget.setFixedHeight(300)
	widget.setFixedWidth(500)
	widget.show()

	sys.exit(app.exec_())


