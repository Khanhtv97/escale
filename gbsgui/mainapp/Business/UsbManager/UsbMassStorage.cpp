//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: UsbMassStorage.cpp                                                         //
//////////////////////////////////////////////////////////////////////////////////////

#include "UsbMassStorage.hpp"
#include "GStringUtils.hpp"
#include "GEventManager.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace UsbManager {
//////////////////////////////////////////////////////////////////////////////////////

UsbMassStorage::UsbMassStorage() : UsbDevice(USB_DEVICE_STORAGE)
{
   m_Files.clear();
   m_workingDirectory = "";
   m_mountedName = "";
}

//////////////////////////////////////////////////////////////////////////////////////
UsbMassStorage::~UsbMassStorage()
{
   m_Files.clear();
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::init()
{
   m_Files.clear();
   m_workingDirectory = "";
   m_mountedName = "";
   return GVFS_Init();
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::deInit()
{
   m_Files.clear();
   GVFS_DeInit();
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::mount()
{
   GVFS_GetMountedName(m_mountedName);
   m_workingDirectory = m_mountedName + "/";
   _DTraceDebug("\n m_mountedName: %s\n m_workingDirectory: %s\n", m_mountedName.c_str(), m_workingDirectory.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::unmount()
{
   m_mountedName = "";
   m_workingDirectory = "";
   _DTraceError("\n m_mountedName: %s\n m_workingDirectory: %s\n", m_mountedName.c_str(), m_workingDirectory.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::isPlugged()
{
   return GVFS_IsUsbPlugged();
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::isMounted()
{
   return GVFS_IsUsbMounted();
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::isReady()
{
   return (GVFS_IsUsbMounted() && GVFS_IsUsbPlugged());
}

//////////////////////////////////////////////////////////////////////////////////////
void UsbMassStorage::setWorkingDirectory(std::string path)
{
   m_workingDirectory = path;
}

//////////////////////////////////////////////////////////////////////////////////////
std::string UsbMassStorage::getWorkingDirectory()
{
   return m_workingDirectory;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::getListFiles(std::list<FileInfos>& listFiles)
{
   listFiles = m_Files;
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
void UsbMassStorage::refreshListFiles()
{
   m_Files.clear();

   Task_Delay(ST_GetClocksPerSecond()/10);

   if( !GVFS_Dir(m_workingDirectory, m_Files) )
   {
     _DTraceError("Could not open directory...");
   }
   else
   {
      _DTraceError("Refresh list DONE !\n");
      GEventManager::getInstance().fire(GEVENT_LIST_REFRESH_DONE);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::delRECFile(std::string itemPath)
{
   return GVFS_DelRECFile( itemPath );
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::validateFileName(std::string str)
{
   bool ret = false;

   // Valid values: 0-9, a-z, A-Z, "-", "_"
   for( int i = 0; i < str.length(); i++ )
   {
      int x = str.at(i);
      if( (x == '-') || (x == '_') || ((x >= '0') && (x <= '9')) || ((x >= 'A') && (x <= 'Z')) || ((x >= 'a') && (x <= 'z')) )
      {
         ret = true;
      }
      else
      {
         ret = false;
         break;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::copyRECFile(std::string fullPathSrc, std::string fullPathDes)
{
   std::string tmpfullPathDes;
   std::string tmpfullPathDes1;
   std::string tmpfullPathSrc;
   std::string tmpfileName;
   std::string tmpfilePath;
   std::list<FileInfos> recFileList;
   size_t pos;
   bool isSrcPlayAble = false;
   bool isSrcDir = false;
   bool ret = true;

   _DTraceError("fullPathSrc: %s fullPathDes: %s\n", fullPathSrc.c_str(), fullPathDes.c_str());

   pos = fullPathDes.find_last_of("/");
   tmpfullPathDes = fullPathDes.substr(0,pos);
   pos = fullPathSrc.find_last_of("/");
   tmpfullPathSrc = fullPathSrc.substr(0,pos);
   tmpfileName = fullPathSrc.substr(pos+1);

   tmpfullPathDes1 = tmpfullPathDes + "/" + tmpfileName;

   if( GVFS_Dir(tmpfullPathDes1.c_str(), recFileList) )
   {
      _DTraceError("File already exist !\n");
      return false;
   }

   if( SYS_FMkDir(tmpfullPathDes1.c_str())!=0 )
   {
      _DTraceError("--> Unable to create a directory !\n");
      return false;
   }

   recFileList.clear();

   if( !GVFS_Dir(fullPathSrc.c_str(), recFileList) )
   {
     _DTraceError("Could not open directory...");
   }

   std::list<FileInfos>::iterator p = recFileList.begin();
   while( p != recFileList.end() )
   {
      tmpfullPathDes = tmpfullPathDes + "/" + (*p).fileName;
      tmpfullPathSrc = tmpfullPathSrc + "/" + (*p).fileName;

      if( !(*p).isDir )
      {
         _DTraceError("Copy file: %s to %s\n", tmpfullPathSrc.c_str(), tmpfullPathDes.c_str());
         ret =  GVFS_CopyFile( tmpfullPathSrc, tmpfullPathDes );
      }

      tmpfullPathDes = tmpfullPathDes1;
      tmpfullPathSrc = fullPathSrc;

      p++;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::renameRECFile(std::string fullPathOldName, std::string fullPathNewName)
{
   std::string tmpfullPathNewName;
   std::string tmpfullPathNewName1;
   std::string tmpfullPathOldName;
   std::string tmpfileName;
   std::string tmpfileName1;
   std::string tmpOldfileName;
   std::string tmpfilePath;
   std::list<FileInfos> recFileList;
   std::string templateFileNameInUse[DRVR_MAX_FILE];
   DvrFileExt fileExt;
   size_t pos;
   bool isSrcPlayAble = false;
   bool isSrcDir = false;
   bool ret = true;
   int i;

   _DTraceError("fullPathOldName: %s fullPathNewName: %s\n", fullPathOldName.c_str(), fullPathNewName.c_str());

   templateFileNameInUse[0] = fileExt.exIdx;
   templateFileNameInUse[1] = fileExt.exIfo;
   templateFileNameInUse[2] = fileExt.exTrp;
   templateFileNameInUse[3] = fileExt.exRap;

   pos = fullPathOldName.find_last_of("/");
   tmpfullPathOldName = fullPathOldName.substr(0,pos);
   tmpfileName1 = fullPathOldName.substr(pos+1);

   pos = fullPathNewName.find_last_of("/");
   tmpfileName = fullPathNewName.substr(pos+1);

   tmpfullPathNewName1 = tmpfullPathOldName + "/" + tmpfileName;

   if( GVFS_Dir(tmpfullPathNewName1.c_str(), recFileList) )
   {
     _DTraceError("File already exist !");
     return false;
   }

   recFileList.clear();

   std::list<FileInfos>::iterator p = recFileList.begin();
   if( GVFS_Dir(fullPathOldName.c_str(), recFileList) )
   {
      p = recFileList.begin();
      while( p != recFileList.end() )
      {
         for( i=0; i<DRVR_MAX_FILE; i++)
         {
            if( !(*p).isDir && (strcasecmp((*p).fileName.c_str(), (tmpfileName1 + templateFileNameInUse[i]).c_str()) == 0) )
            {
               ret =  GVFS_RenameFile( fullPathOldName + "/" + (*p).fileName, fullPathOldName + "/" + tmpfileName + templateFileNameInUse[i] );
            }
         }
         p++;
      }
      ret =  GVFS_RenameFile( fullPathOldName, fullPathNewName );
   }
   else
   {
      _DTraceError("Could not open directory...");
      ret =  GVFS_RenameFile( fullPathOldName, fullPathNewName );
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::getFileInfoAt(int index, FileInfos& fInf)
{
   bool found = false;

   if( index >= m_Files.size() || index < 0 )
   {
      _DTraceError("Invalid index...");
      return found;
   }

   std::list<FileInfos>::iterator p = m_Files.begin();

   while( p != m_Files.end() )
   {
      if( (*p).fileId == index )
      {
         fInf.fileId = (*p).fileId;
         fInf.fileName = (*p).fileName;
         fInf.filePath = (*p).filePath;
         fInf.dtModified  = (*p).dtModified;
         fInf.fileSize = (*p).fileSize;
         fInf.isDir = (*p).isDir;
         fInf.fileType = (*p).fileType;

        _DTraceError("Found...");
         found = true;
         break;
      }

      p++;
   }

   return found;
}

//////////////////////////////////////////////////////////////////////////////////////
FileSelectedAction UsbMassStorage::selectFileAt(int index)
{
   FileSelectedAction ret = FSA_NONE;

   if( index >= m_Files.size() || index < 0 )
   {
     _DTraceError("invalid index...");
      return ret;
   }

   std::list<FileInfos>::iterator p = m_Files.begin();
   while( p != m_Files.end() )
   {
      if( (*p).fileId == index )
      {
        _DTraceError("Found %s", (*p).getFullPath().c_str());

         if( ((*p).isDir == true) && ((*p).fileType != GFILE_TYPE_MEDIA_TRP) )
         {
            m_workingDirectory = (*p).getFullPath() + "/";
            ret = FSA_OPEN_DIR;
         }
         else
         {
            ret = FSA_OPEN_FILE;
         }

         break;
      }
      p++;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::getFileToRecord(std::string& fileName)
{
   bool ret = false;
   std::list<FileInfos> lsTemp;
   fileName = "";

   std::string dirForRecording = m_mountedName + "/";

   if( !GVFS_Dir(dirForRecording, lsTemp) )
   {
     _DTraceError("Could not open directory...");
      return ret;
   }

   int maxFileNum = 10000;
   for( int i = 0; i < maxFileNum; i++ )
   {
      bool isFileNameFound = false;
      std::string testFile;
      GStringUtils::format(testFile, "mv%d%d%d%d", (i%10000)/1000, (i%1000)/100, (i%100)/10, i%10);

      std::list<FileInfos>::iterator p = lsTemp.begin();
      while( p != lsTemp.end() )
      {
         if( ((*p).isDir) || ((*p).fileType == GFILE_TYPE_MEDIA_TRP) )
         {
            if( strcasecmp(testFile.c_str(), (*p).fileName.c_str()) == 0 )
            {
               isFileNameFound = true;
               break;
            }
         }
         p++;
      }

      //not found, so we can get the filename
      if( !isFileNameFound )
      {
         fileName.assign(dirForRecording);
         fileName.append(testFile);
         fileName.append(".trp");

         _DTraceError("%s", fileName.c_str());

         ret = true;
         break;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbMassStorage::getUSBStorageInfo(MountPartition& mountPartitionInfo)
{
   if( !GVFS_Get_FS_Info(mountPartitionInfo) )
   {
      return false;
   }

   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UsbManager
//} //Business
//} //Application
//} //STM
//} //GBS
