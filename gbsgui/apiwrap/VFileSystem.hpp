//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: VFileSystem.hpp                                                            //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef FILESYSTEM_API_HPP_
#define FILESYSTEM_API_HPP_

#include "stapp_main.h"
#include "GDebug.hpp"
#include <list>

#define DRVR_MAX_FILE 4

typedef enum
{
   GFILE_TYPE_UNKNOWN = 0,
   GFILE_TYPE_MEDIA_TRP,
   GFILE_TYPE_VIDEO,
   GFILE_TYPE_AUDIO,
   GFILE_TYPE_IMG_PNG,
   GFILE_TYPE_IMG_JPEG,
   GFILE_TYPE_IMG_BMP
}
GFSType;

struct FileInfos
{
   int fileId;
   U32 fileSize; //in MB
   bool isDir;
   GFSType fileType;
   std::string fileName;
   std::string filePath;
   time_t dtModified;

   FileInfos()
   {
      fileId = -1;
      fileSize = 0;
      isDir = false;
      fileType = GFILE_TYPE_UNKNOWN;
      fileName = "";
      filePath = "";
      dtModified = 0;
   };

   std::string getFullPath()
   {
      std::string fileFullPath;
      fileFullPath.assign(filePath);
      fileFullPath.append(fileName);
      return fileFullPath;
   };

   bool isPlayable()
   {
      bool ret = (fileType == GFILE_TYPE_MEDIA_TRP ||
                  fileType == GFILE_TYPE_VIDEO ||
                  fileType == GFILE_TYPE_AUDIO );
      return ret;
   }

   bool isPicture()
   {
      bool ret = (fileType == GFILE_TYPE_IMG_PNG ||
                  fileType == GFILE_TYPE_IMG_BMP ||
                  fileType == GFILE_TYPE_IMG_JPEG );
      return ret;
   }
};

struct MountPartition
{
   U32 totalSize_MB;
   U32 freeSize_MB;

   MountPartition()
   {
      totalSize_MB = 0;
      freeSize_MB = 0;
   };
};


struct DvrFileExt
{
   std::string exIdx;
   std::string exIfo;
   std::string exTrp;
   std::string exRap;

   DvrFileExt()
   {
      exIdx = ".idx";
      exIfo = ".ifo";
      exTrp = ".trp";
      exRap = ".rap";
   };
};

//namespace GBS {
//namespace STM {
//namespace API {

//////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
   extern "C"
   {
#endif

   static void GVFS_UsbDeviceCallBack(device_event_t event, device_notify_t* notifications[], int total, void* cookie);

   bool GVFS_Init();
   void GVFS_DeInit();

   bool GVFS_IsUsbPlugged();
   bool GVFS_IsUsbMounted();
   bool GVFS_GetMountedName(std::string& mountedName);

   // Mount a volume name with the mount name
   bool GVFS_Mount(const std::string diskName, const std::string volumeName, const std::string modeName, std::string& mountName);

   // Umount a mount name disk from
   bool GVFS_UMount(const std::string mountName);

   // Check is file playable
   bool GVFS_DetectFileType(FileInfos& fileInf);

   // List a directory
   bool GVFS_Dir(const std::string fullPath, std::list<FileInfos>& listFiles);

   // get a struct of FS mounted
   bool GVFS_Get_FS_Info(MountPartition& mountPartitionInfo);

   // delete a file
   bool GVFS_DelRECFile(const std::string fullPath);

   // Copy paste a file
   bool GVFS_CopyFile(const std::string fullPathSrc, const std::string fullPathDes);

   // Rename a file
   bool GVFS_RenameFile(const std::string fullPathOldName, const std::string fullPathNewName);

   //Make a directory
   bool GVFS_MkDir(const std::string fullPath);

   //Remove a directory
   bool GVFS_RmDir(const std::string fullPath);

#ifdef __cplusplus
   }
#endif

//////////////////////////////////////////////////////////////////////////////////////
//} //API
//} //STM
//} //GBS
#endif //FILESYSTEM_API_HPP_
