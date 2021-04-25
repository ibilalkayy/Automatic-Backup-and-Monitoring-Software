/* Including all the libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#define SIZE 100
#define ELEMENTS 100
#define EVENT_SIZE (sizeof (struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

/* Declaring the variables */
typedef struct {
	int length, fd, wd1, wd2;
	char buffer[4096];
} notification;

notification inotify;

char *transfer(char *f1, char *f2){
	char cmd[500];
	snprintf(cmd, 500, "rsync -a %s %s", f1, f2);
	system(cmd);
	return 0;
}

char *removeFile(char *file){
	char cmd[500];
	snprintf(cmd, 500, "rm -rf %s", file);
	system(cmd);
	return 0;
}

void closeInotify(int signal){
	printf("\nYour program is closed\n");
	/*removing the “pathname” directory from the watch list.*/
	inotify_rm_watch(inotify.fd, inotify.wd1);
	inotify_rm_watch(inotify.fd, inotify.wd2);

	/*closing the INOTIFY instance*/
	close(inotify.fd);
	exit(0);
}

int listDir(char *basePath, char files[][SIZE], int *total){
	char path[SIZE];
	struct dirent *dp;
	DIR *dir;

	if (NULL != (dir = opendir(basePath))){ /* try to open dir */
		while (NULL != (dp = readdir(dir))){ /* iterate through contents */
			if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
				snprintf(path, SIZE, "%s/%s", basePath, dp->d_name);
				if (dp->d_type == DT_DIR){
					if (*total < ELEMENTS){
						snprintf(files[*total], SIZE, "%s", path);
						files[*total][SIZE - 1] = 0;
						++(*total);
					}
				}
				if (dp->d_type == DT_REG){
					listDir(path, files, total); /* recursive call for directory */
				}
			}
		}
		closedir(dir); /* opendir failed */
	}
	else{
		perror(basePath);
	}

	return 0;
}

int getNotified(char *basePath1, char *basePath2)
{	
	int countPath1 = 0, countPath2 = 0, i;
	char newPath1[ELEMENTS], newPath2[ELEMENTS], pathName1[ELEMENTS][SIZE], pathName2[ELEMENTS][SIZE];
	
	listDir(basePath1, pathName1, &countPath1);
	printf("Total directories in '%s' are %d\n", basePath1, countPath1);

	listDir(basePath2, pathName2, &countPath2);
	printf("Total directories in '%s' are %d\n", basePath2, countPath2);

	char subPath1[countPath1][SIZE], subPath2[countPath2][SIZE],
	newSubPath1[countPath1][SIZE], newSubPath2[countPath2][SIZE];

	for (i = 0; i<countPath1; i++){
		strcpy(subPath1[i], pathName1[i]);
	}
	for (i = 0; i<countPath2; i++){
		strcpy(subPath2[i], pathName2[i]);
	}

	/* Closing inotify */
	signal(SIGINT, closeInotify);

	/* Initialize the inotify instance */
	inotify.fd = inotify_init();

	/*checking for error*/
	if(inotify.fd < 0){
		perror("inotify_init");
		exit(1);
	}

	/* Adding 'basePath1' to watchlist */
	inotify.wd1 = inotify_add_watch(inotify.fd, basePath1, ENOENT|IN_CREATE|IN_MODIFY|IN_DELETE|IN_MOVED_FROM|IN_MOVED_TO);
	if(inotify.wd1 < 0){
		perror("Error");
		exit(2);
	}
	/* adding the subdirectories 'subPath1' directory into watch list. */
	int subWd1[countPath1];
	for (i=0; i<countPath1; i++){
		subWd1[i] = inotify_add_watch(inotify.fd, subPath1[i], ENOENT|IN_CREATE|IN_MODIFY|IN_DELETE|IN_MOVED_FROM|IN_MOVED_TO);
	}
	for (i=0; i<countPath1; i++){
		if (subWd1[i] < 0){
			perror("Error");
			exit(3);
		}
	}

	/* Adding 'basePath2' to watchlist */
	inotify.wd2 = inotify_add_watch(inotify.fd, basePath2, ENOENT|IN_CREATE|IN_MODIFY|IN_DELETE|IN_MOVED_FROM|IN_MOVED_TO);
	if(inotify.wd2 < 0){
		perror("Error");
		exit(4);
	}
	/* adding the subdirectories 'subPath2' directory into watch list. */
	int subWd2[countPath2];
	for (i=0; i<countPath2; i++){
		subWd2[i] = inotify_add_watch(inotify.fd, subPath2[i], ENOENT|IN_CREATE|IN_MODIFY|IN_DELETE|IN_MOVED_FROM|IN_MOVED_TO);
	}
	for (i=0; i<countPath2; i++){
		if (subWd2[i] < 0){
			perror("Error");
			exit(5);
		}
	}

	printf("Both locations are being monitored\n");

	while(1)
	{
		/* read to determine the event change happens on “/pathname” directory. */
		inotify.length = read(inotify.fd, inotify.buffer, EVENT_BUF_LEN);
		if(inotify.length < 0){
			perror("read");
			exit(4);
		}

		int i = 0;
		/* Here, read the change event one by one and process it accordingly. */
		while(i < inotify.length){
			struct inotify_event *event = (struct inotify_event*)&inotify.buffer[i];

			if(event->len)
			{
				/* Watch the creation of filesystem */
				if(event->mask & IN_CREATE)
				{
					if (event->mask & IN_ISDIR){
						if(event->wd == inotify.wd1){
							printf("New directory named '%s' is created in %s\n", event->name, basePath1);
							snprintf(newPath1, SIZE, "%s/%s", basePath1, event->name);
							transfer(newPath1, basePath2);
							break;
						}
						for (i=0; i<countPath1; i++){
							if (event->wd == subWd1[i]){
								printf("New directory named '%s' is created in %s\n", event->name, subPath1[i]);
								snprintf(newSubPath1[i], SIZE, "%s/%s", subPath1[i], event->name);
								transfer(newSubPath1[i], subPath2[i]);
								break;
							}
						}
						if(event->wd == inotify.wd2){
							printf("New directory named '%s' is created in %s\n", event->name, basePath2);
							break;
						}
						for (i=0; i<countPath2; i++){
							if (event->wd == subWd2[i]){
								printf("New directory named '%s' is created in %s\n", event->name, subPath2[i]);
								break;
							}
						}	
					}
					else{
						if(event->wd == inotify.wd1){
							printf("New file named '%s' is created in %s\n", event->name, basePath1);
							snprintf(newPath1, SIZE, "%s/%s", basePath1, event->name);
							transfer(newPath1, basePath2);
							break;
						}
						for (i=0; i<countPath1; i++){
							if (event->wd == subWd1[i]){
								printf("New file named '%s' is created in %s\n", event->name, subPath1[i]);
								snprintf(newSubPath1[i], SIZE, "%s/%s", subPath1[i], event->name);
								transfer(newSubPath1[i], subPath2[i]);
								break;
							}
						}
						if(event->wd == inotify.wd2){
							printf("New file named '%s' is created in %s\n", event->name, basePath2);
							break;
						}
						for (i=0; i<countPath2; i++){
							if (event->wd == subWd2[i]){
								printf("New file named '%s' is created in %s\n", event->name, subPath2[i]);
								break;
							}
						}
					}
				}

				/* Watch the modification of filesystem */
				if(event->mask & IN_MODIFY)
				{
					if (event->mask & IN_ISDIR){
						if(event->wd == inotify.wd1){
							printf("Directory named '%s' is modified in %s\n", event->name, basePath1);
							break;
						}
						if(event->wd == inotify.wd2){
							printf("Directory named '%s' is modified in %s\n", event->name, basePath2);
							break;
						}	
					}
					else{
						if(event->wd == inotify.wd1){
							printf("File named '%s' is modified in %s\n", event->name, basePath1);
							snprintf(newPath1, SIZE, "%s/%s", basePath1, event->name);
							snprintf(newPath2, SIZE, "%s/%s", basePath2, event->name);
							removeFile(newPath2);
							transfer(newPath1, basePath2);
							break;
						}
						for (i=0; i<countPath1; i++){
							if (event->wd == subWd1[i]){
								printf("File named '%s' is modified in %s\n", event->name, subPath1[i]);
								snprintf(newSubPath1[i], SIZE, "%s/%s", subPath1[i], event->name);
								snprintf(newSubPath2[i], SIZE, "%s/%s", subPath2[i], event->name);
								removeFile(newSubPath2[i]);
								transfer(newSubPath1[i], subPath2[i]);
								break;
							}
						}
						if(event->wd == inotify.wd2){
							printf("File named '%s' is modified in %s\n", event->name, basePath2);
							break;
						}
						for (i=0; i<countPath2; i++){
							if (event->wd == subWd2[i]){
								printf("File named '%s' is modified in %s\n", event->name, subPath2[i]);
								break;
							}
						}	
					}
				}

				/* Watch the deletion of filesystem */
				if(event->mask & IN_DELETE)
				{
					if (event->mask & IN_ISDIR){
						if(event->wd == inotify.wd1){
							printf("Directory named '%s' is deleted in %s\n", event->name, basePath1);
							break;
						}
						for (i=0; i<countPath1; i++){
							if (event->wd == subWd1[i]){
								printf("Directory named '%s' is deleted in %s\n", event->name, subPath1[i]);
								break;
							}
						}
						if(event->wd == inotify.wd2){
							printf("Directory named '%s' is deleted in %s\n", event->name, basePath2);
							break;
						}	
						for (i=0; i<countPath2; i++){
							if (event->wd == subWd2[i]){
								printf("Directory named '%s' is deleted in %s\n", event->name, subPath2[i]);
								break;
							}
						}
					}
					else{
						if(event->wd == inotify.wd1){
							printf("File named '%s' is deleted in %s\n", event->name, basePath1);
							break;
						}
						for (i=0; i<countPath1; i++){
							if (event->wd == subWd1[i]){
								printf("File named '%s' is deleted in %s\n", event->name, subPath1[i]);
								break;
							}
						}
						if(event->wd == inotify.wd2){
							printf("File named '%s' is deleted in %s\n", event->name, basePath2);
							break;
						}	
						for (i=0; i<countPath2; i++){
							if (event->wd == subWd2[i]){
								printf("File named '%s' is deleted in %s\n", event->name, subPath2[i]);
								break;
							}
						}
					}
				}


				/* Watch the filesystem moved from */
 				if(event->mask & IN_MOVED_FROM)
				{
					if (event->mask & IN_ISDIR){
						if(event->wd == inotify.wd1){
							printf("Directory named '%s' is moved from %s\n", event->name, basePath1);
							break;
						}
						for (i=0; i<countPath1; i++){
							if (event->wd == subWd1[i]){
								printf("Directory named '%s' is moved from %s\n", event->name, subPath1[i]);
							}
						}
						if(event->wd == inotify.wd2){
							printf("Directory named '%s' is moved from %s\n", event->name, basePath2);
							break;
						}	
						for (i=0; i<countPath2; i++){
							if (event->wd == subWd2[i]){
								printf("Directory named '%s' is moved from %s\n", event->name, subPath2[i]);
								break;
							}
						}
					}
					else{
						if(event->wd == inotify.wd1){
							printf("File named '%s' is moved from %s\n", event->name, basePath1);
							break;
						}
						for (i=0; i<countPath1; i++){
							if (event->wd == subWd1[i]){
								printf("File named '%s' is moved from %s\n", event->name, subPath1[i]);
								break;
							}
						}
						if(event->wd == inotify.wd2){
							printf("File named '%s' is moved from %s\n", event->name, basePath2);
							break;
						}	
						for (i=0; i<countPath2; i++){
							if (event->wd == subWd2[i]){
								printf("File named '%s' is moved from %s\n", event->name, subPath2[i]);
								break;
							}
						}
					}
				}

				/* Watch the filesystem moved to */
				if(event->mask & IN_MOVED_TO){
					if (event->mask & IN_ISDIR){
						if(event->wd == inotify.wd1){
							printf("Directory named '%s' is moved to %s\n", event->name, basePath1);
							snprintf(newPath1, SIZE, "%s/%s", basePath1, event->name);
							transfer(newPath1, basePath2);
							break;
						}
						for (i=0; i<countPath1; i++){
							if (event->wd == subWd1[i]){
								printf("Directory named '%s' is moved to %s\n", event->name, subPath1[i]);
								snprintf(newSubPath1[i], SIZE, "%s/%s", subPath1[i], event->name);
								transfer(newSubPath1[i], subPath2[i]);
								break;
							}
						}
						if(event->wd == inotify.wd2){
							printf("Directory named '%s' is moved to %s\n", event->name, basePath2);
							break;
						}
						for (i=0; i<countPath2; i++){
							if (event->wd == subWd2[i]){
								printf("Directory named '%s' is moved to %s\n", event->name, subPath2[i]);
								break;
							}
						}	
					}
					else{
						if(event->wd == inotify.wd1){
							printf("File named '%s' is moved to %s\n", event->name, basePath1);
							snprintf(newPath1, SIZE, "%s/%s", basePath1, event->name);
							transfer(newPath1, basePath2);
							break;
						}
						for (i=0; i<countPath1; i++){
							if (event->wd == subWd1[i]){
								printf("File named '%s' is moved to %s\n", event->name, subPath1[i]);
								snprintf(newSubPath1[i], SIZE, "%s/%s", subPath1[i], event->name);
								transfer(newSubPath1[i], subPath2[i]);
								break;
							}
						}
						if(event->wd == inotify.wd2){
							printf("File named '%s' is moved to %s\n", event->name, basePath2);
							break;
						}
						for (i=0; i<countPath2; i++){
							if (event->wd == subWd2[i]){
								printf("File named '%s' is moved to %s\n", event->name, subPath2[i]);
								break;
							}
						}	
					}
				}
			}
			i += EVENT_SIZE + event->len;
		}
	}

	return 0;
}

int main(void){
	getNotified("/go/to/path1", "/go/to/path2");
	return 0;
}
