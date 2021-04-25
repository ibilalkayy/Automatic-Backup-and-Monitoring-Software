# Importing all the Libraries
import os
import time
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler 
from Google import MyDrive
from pathlib import Path
import Google

class MyHandler(FileSystemEventHandler):
	# Monitor the creation of a file
	def on_created(self, event):
		print(f"event type: {event.event_type} path: {event.src_path}")
		folder_id = "write_a_folder_id"
		originalFile = event.src_path.split('/')[-1]
		originalPath = event.src_path.rsplit('/', 1)[0] + '/'
		pathName = event.src_path.split('/')[-2]
		my_drive = MyDrive()

		if Path(event.src_path).parent.samefile(fullPath):
			if os.path.isdir(event.src_path):
				my_drive.uploadFolders(originalFile, [folder_id])
			else:
				my_drive.uploadFiles(originalFile, fullPath, folder_id)
		else:
			my_drive.findId(pathName)
			choosenId = Google.dirId2[0]

			if os.path.isdir(event.src_path):
				my_drive.uploadFolders(originalFile, [choosenId])
			else:
				my_drive.uploadFiles(originalFile, originalPath, choosenId)

	# Monitor the modification of a file
	def on_modified(self, event):
		folder_id = "write_a_folder_id"
		originalFile = event.src_path.split('/')[-1]
		originalPath = event.src_path.rsplit('/', 1)[0] + '/'
		pathName = event.src_path.split('/')[-2]
		my_drive = MyDrive()

		if os.path.isfile(event.src_path):
			print(f"event type: {event.event_type} path: {event.src_path}")

			if Path(event.src_path).parent.samefile(fullPath):
				my_drive.findId(originalFile)
				choosenId = Google.dirId2[0]
				my_drive.deleteFile(choosenId)
				my_drive.uploadFiles(originalFile, fullPath, folder_id)	
			else:
				my_drive.findId(originalFile)
				choosenId = Google.dirId2[0]
				my_drive.deleteFile(choosenId)

				my_drive.findId(pathName)
				pathId = Google.dirId2[1]
				my_drive.uploadFiles(originalFile, originalPath, pathId)
		else:
			print("")

	# Monitor the deletion of a file
	def on_deleted(self, event):
		print(f"event type: {event.event_type} path: {event.src_path}")

def monitorFolders(path):
	event_handler = MyHandler()
	observer = Observer()
	observer.schedule(event_handler, path, recursive=True)
	observer.start()

	try:
		while True:
			time.sleep(1)
	except KeyboardInterrupt:
		observer.stop()
	observer.join()

fullPath = "go/to/path1/"

if __name__ == "__main__":
	m1 = monitorFolders(fullPath)
	m1.start()
	m1.join()
