# Folder-Synchronization
... is an application allowing users to synchronize data between two folders. 

## How does it work?
First of all, it checks if there are any lacking files in directory which is being compared, if so the files are copied. The application then checks if compared files differ (in contents or size when ***Fast mode*** is selected), if it happens so that the files in fact differ, the compared file is overwritten with the contents of the original file.

## Features
- [x] *Logging*
- [x] *File mapping*
- [x] *Switchable fast comparison mode*
- [ ] *Image comparison with OpenCV*
- [ ] *Better GUI*
