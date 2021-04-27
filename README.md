# Automatic-Backup-and-Monitoring-Software
This repository contains the code of a backup and monitoring software that will automatically backup all your data from a selected directory in your PC to External Drive or Google Drive.

This software is written in two Languages. The first one is C and another is Python. 
C code monitors and takes a backup of data from your computer system to External Drive(if it is attached). Python language monitors and takes a backup of your data from your computer system to Google Drive.

---

### Table of Contents
The headers will be used to reference the location of destination.

- [The First Part](#the-first-part)
- [The Second Part](#the-second-part)
- [Author Info](#author-info)

---


# The First Part
The first part of this software is written in C language. Here I used inotify instance. It is a Linux Kernel subsystem which monitors changes to the filesystem and report those changes to applications.

I used one main function and that is getNotified(). All the other functions whether these are ```transfer()```, ```removeFile()```, ```closeInotify()```, ```listDir()``` are supporting the ```getNotified()``` function. So what these functions are doing?

I will be starting from the above code. 

1. ```transfer()``` will copy all the data of one directory and transfer them into another directory. 
2. ```removeFile()``` will remove a particular file whose location is given.T
3. ```closeInotify()``` will close the inotify instance after monitoring the filesystem.
4. ```listDir()``` will give a list of all the directories so that we can easily count them and use them in getNotified() function.
5. ```getNotified()``` will monitor all the files using all the above functions. This function has mainly three parts. 
   - Initialize the inotify instance using ```inotify_init()```
   - Add all the paths and sub paths to watchlist ```inotify_add_watch()```
   - Read all the paths and then use **IN_CREATE** to monitor the creation of files, **IN_MODIFY** to monitor the modification of files, **IN_DELETE** to monitor the deletion of files, **IN_MOVED_FROM** to monitor the files moved from a path, and at the end **IN_MOVED_TO** to monitor the files moved to a path. 
6. **main()** will call the ```getNotified()``` function with an actual file path.

[Back To The Top](#Automatic-Backup-and-Monitoring-Software)

# The second part
The first part has been covered. Now come to the second part that is to monitor and take backup from computer system to Google Drive. Here I have used three python files. First one is *Google.py*, second is **monitor.py**, and third one is **main.py**.

I also used the PyQt5 for the purpose of making a user interface. PyQt5 has also three files that are **selectDrive.ui**, **selectPath1.ui**, and **selectPath2.ui**. Further three JSON files are also created. 

So let's explain what these files are doing?

### 1. Google.py
   - First of all I took the code from Google Drive for Developers documentation and then modified and used them in a class called ```MyDrive()```. After that, I made some functions. 
   - The first one is ```uploadFiles()``` that will upload all the present files from the computer system to Google Drive.
   - The second function is ```uploadFolders()``` that will upload all the folders from the computer system to Google Drive.
   - The third function is ```findId()``` that will find the id of a specific file.
   - The forth function is ```deleteFile()``` that will delete a file according to the given file Id.
   - The fifth function is ```listFolders()``` that will show all the folders present in a given path.
   - The sixth function is ```listFiles()``` that will show all the files present in a given path.
   - The seventh function is ```main()``` that will call the ```MyDrive()``` class.

### 2. monitor.py
   - The ```monitor.py``` has one class ```MyHandler()``` that contains all the events and these events will give message when something is changed in a path.
   - ```MyHandler()``` contains three functions. ```on_created()```, ```on_modified()```, ```on_deleted()```.
   - ```on_created()``` will monitor the creation of files.
   - ```on_modified()``` will monitor the modification of files.
   - ```on_deleted()``` will monitor the deletion of files.
   - After that, another function is created that ```monitorFolders()```. It will call the the class and use ```Oberserver()``` to monitor the path.
   - At the end, I will call the ```monitorFolders()``` function to execute the code.

### 3. main.py
   Now come to the last file. It has three classes because the software has three user windows.
   1. The first class is ```MainWindow()``` that has four functions.
      - ```extDriveFunction()``` will enable the first radio button.
      - ```cloudFunction()``` will enable the second radio button.
      - ```gotoPath1()``` will enable the next button to go to the external drive window
      - ```gotoPath2()``` will enable the next button to go cloud drive window
   2. The second class is ```selectPath1()``` that has five functions.
      - ```browseFile1()``` will browse the source path.
      - ```browseFile2()``` will browse the destination path.
      - ```backWindow()``` will enable the back button.
      - ```operation()``` will make us notified and it will perform operations to monitor both the paths.
      - ```closeEvent()``` will enable the cancel button.
   3. The third class is ```selectPath2()``` that has five functions.
      - ```browseFile1()``` will browse the source path.
      - ```browseFile2()``` and ```cloudSelected()``` will will perform operations to monitor both the paths.
      - ```backWindow()``` will enable the back button.
      - ```closeEvent()``` will enable the cancel button.

[Back To The Top](#Automatic-Backup-and-Monitoring-Software)

## Author Info

- YouTube - [ibilalkayy](https://www.youtube.com/channel/UCBLTfRg0Rgm4FtXkvql7DRQ)
- LinkedIn - [@ibilalkayy](https://www.linkedin.com/in/ibilalkayy/)

[Back To The Top](#Automatic-Backup-and-Monitoring-Software)
