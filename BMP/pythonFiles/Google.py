# Importing the Libraries
import os.path
from googleapiclient.discovery import build
from google_auth_oauthlib.flow import InstalledAppFlow
from google.auth.transport.requests import Request
from google.oauth2.credentials import Credentials
from apiclient.http import MediaFileUpload
 
from oauth2client import client, tools, file
from apiclient import discovery
from httplib2 import Http

# Global Variables
dirId1 = []
dirId2 = []
pathStored = []
totalFiles = 0
totalFolders = 0
store = file.Storage('storage.json')

class MyDrive():
    def __init__(self):
        # If modifying these scopes, delete the file token.json.
        SCOPES = ['https://www.googleapis.com/auth/drive']

        """Shows basic usage of the Drive v3 API.
        Prints the names and ids of the first 10 files the user has access to.
        """
        creds = None
        # The file token.json stores the user's access and refresh tokens, and is
        # created automatically when the authorization flow completes for the first
        # time.
        if os.path.exists('token.json'):
            creds = Credentials.from_authorized_user_file('token.json', SCOPES)
        # If there are no (valid) credentials available, let the user log in.
        if not creds or not creds.valid:
            if creds and creds.expired and creds.refresh_token:
                creds.refresh(Request())
            else:
                flow = InstalledAppFlow.from_client_secrets_file(
                    'credentials.json', SCOPES)
                creds = flow.run_local_server(port=0)
            # Save the credentials for the next run
            with open('token.json', 'w') as token:
                token.write(creds.to_json())

        self.service = build('drive', 'v3', credentials=creds)

    # Upload the files to Google Drive
    def uploadFiles(self, filename, path, folder_id):
        media = MediaFileUpload(f"{path}{filename}")

        response = self.service.files().list(
                                        q=f"name='{filename}' and parents='{folder_id}'",
                                        spaces='drive',
                                        fields='nextPageToken, files(id, name)',
                                        pageToken=None).execute()
        if len(response['files']) == 0:
            file_metadata = {
                'name': filename,
                'parents': [folder_id]
            }
            files = self.service.files().create(body=file_metadata, media_body=media, fields='id').execute()
            print(f"A new file is created: {files.get('id')}")
        else:
            for file in response.get('files', []):
                # Process change

                update_file = self.service.files().update(
                    fileId=file.get('id'),
                    media_body=media,
                ).execute()
                print(f'Updated File')

    # Upload the folders to Google Drive
    def uploadFolders(self, filename, folder_id):
        file_metadata = {
            'name': filename,
            'mimeType': 'application/vnd.google-apps.folder',
            'parents': folder_id
        }
        directory = self.service.files().create(body=file_metadata, fields='id').execute()
        print(f"A new directory is created: {directory.get('id')}")
        global dirId1
        dirId1.append(directory.get('id'))

    # Find the ID of a filename
    def findId(self, filename):
        page_token = None
        global dirId2
        while True:
            response = self.service.files().list(q="name = '" + filename + "'",
                                                  spaces='drive',
                                                  fields='nextPageToken, files(id, name)',
                                                  pageToken=page_token).execute()
            for file in response.get('files', []):
                # Process change
                print('Found file: %s (%s)' % (file.get('name'), file.get('id')))
                dirId2.append(file.get('id'))
            page_token = response.get('nextPageToken', None)
            if page_token is None:
                break

    # Delete a particular file from Google Drive
    def deleteFile(self, fileDelete):
        SCOPES = ['https://www.googleapis.com/auth/drive']
        creds = store.get()
        if not creds or creds.invalid:
            flow = client.flow_from_clientsecrets('credentials.json', SCOPES)
            creds = tools.run_flow(flow, store)
        self.DRIVE = discovery.build('drive', 'v3', http=creds.authorize(Http()))
        file = self.DRIVE.files().delete(fileId=fileDelete).execute()

# List all the folders
def listFolders(path, folder_id):
    my_drive = MyDrive()
    global totalFolders
    global pathStored

    for files in os.listdir(path):
        d = os.path.join(path, files) 
        if os.path.isdir(d):
            my_drive.uploadFolders(files, folder_id)
            pathStored.append(d)
            totalFolders += 1

# List all the files
def listFiles(path, folder_id):
    my_drive = MyDrive()

    data = next(os.walk(path))[2]
    for files in data:
        my_drive.uploadFiles(files, path, folder_id)
        global totalFiles
        totalFiles += 1

def main(path):
    my_drive = MyDrive()
    folderId = "write_a_folder_id"
    files = os.listdir(path)
    global pathStored
    global dirId1
    global dirId2
    global totalFolders
    global totalFiles

    listFolders(path, [folderId])
    for i in range(totalFolders):
        listFolders(pathStored[i], [dirId1[i]])

    listFiles(path, folderId)
    for i in range(totalFiles):
        listFiles(pathStored[i] + '/', dirId1[i])

if __name__ == '__main__':
    main("give/the/pathname")