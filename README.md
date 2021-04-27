# Automatic-Backup-and-Monitoring-Software
This repository contains the code of a backup and monitoring software that will automatically backup all your data from a selected directory in your PC to External Drive or Google Drive.

This software is written in two Languages. The first one is C and another is Python. 
C code monitors and takes a backup of data from your computer system to External Drive(if it is attached). Python language monitors and takes a backup of your data from your computer system to Google Drive.

# Let's look at the first part
The first part of this software is written in C language. Here I used inotify instance. It is a Linux Kernel subsystem which monitors changes to the filesystem and report those changes to applications.

I used one main function and that is getNotified(). All the other functions whether these are transfer(), removeFile(), closeInotify(), listDir() are supporting the getNotified() function. So what these functions are doing?

I will be starting from the above code. 

1. transfer() that will copy all the data of one directory and transfer them into another directory. 
2. removeFile() that will remove a particular file whose location is given.T
3. closeInotify() that will close the inotify instance after monitoring the filesystem.
4. listDir() that will give a list of all the directories so that we can easily count them and use them in getNotified() function.
5. getNotified() that will monitor all the files using all the above functions. This function has mainly three parts. 
6. Initializing the inotify instance using inotify_init()
7. Adding all the paths and sub paths to watchlist inotify_add_watch
8. Reading all the paths and then use IN_CREATE to monitor the creation of files, IN_MODIFY to monitor the modification of files, IN_DELETE to monitor the deletion of files, IN_MOVED_FROM to monitor the files moved from a path, and at the end IN_MOVED_TO to monitor the files moved to a path. 
9. main() that will call the getNotified() function with an actual file path.


