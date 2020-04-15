//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: UsbMassStorage.hpp                                                         //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef USBMASSSTORAGE_H_
#define USBMASSSTORAGE_H_

#include "UsbDevice.hpp"
#include "VFileSystem.hpp"
#include "GListItemEx.hpp"

enum FileSelectedAction
{
   FSA_NONE,
   FSA_OPEN_DIR,
   FSA_OPEN_FILE,
   FSA_COPY,
   FSA_RENAME,
   FSA_DELETE
};

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace UsbManager {
//////////////////////////////////////////////////////////////////////////////////////

class FileListItem : public GListItemEx
{
public:
   FileListItem() {m_fileId = -1;};
   void setId(int id) {m_fileId = id;};
   int getId() {return m_fileId;};

private:
   int m_fileId;
};

//////////////////////////////////////////////////////////////////////////////////////
class UsbMassStorage : public UsbDevice
{
public:
   UsbMassStorage();
   virtual ~UsbMassStorage();

   virtual bool init();
   virtual bool deInit();
   virtual bool mount();
   virtual bool unmount();
   virtual bool isPlugged();
   virtual bool isMounted();
   virtual bool isReady();

   void setWorkingDirectory(std::string path);
   std::string getWorkingDirectory();

   bool getListFiles(std::list<FileInfos>& listFiles);
   bool getFileInfoAt(int index, FileInfos& fInf);

   bool getFileToRecord(std::string& fileName);

   void refreshListFiles();
   bool delRECFile(std::string itemPath);
   bool copyRECFile(std::string fullPathSrc, std::string fullPathDes);
   bool renameRECFile(std::string fullPathOldName, std::string fullPathNewName);

   FileSelectedAction selectFileAt(int index);

   bool getUSBStorageInfo(MountPartition& mountPartitionInfo);

   bool validateFileName(std::string str);

private:
   std::string m_mountedName;
   std::string m_workingDirectory;
   std::list<FileInfos> m_Files;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UsbManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //USBMASSSTORAGE_H_
