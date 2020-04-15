//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: VFileSystem.cpp                                                            //
//////////////////////////////////////////////////////////////////////////////////////

#include "VFileSystem.hpp"
#include "GSystem.hpp"
#include "GStringUtils.hpp"
#include "GEventManager.hpp"

//namespace GBS {
//namespace STM {
//namespace API {
#if defined(GUSE_STSTREAM)
const char* VIDEOTYPESUPPORT[] = {".mp4", ".MP4", ".avi", ".AVI", ".wmv", ".WMV", ".flv", ".FLV",
                                  ".mkv", ".MKV", ".3gp", ".3GP", ".vob", ".VOB", ".iso", ".ISO", NULL, NULL};

const char* AUDIOTYPESUPPORT[] = {".mp3", ".MP3", ".aac", ".AAC", ".wav", ".WAV", NULL, NULL};
#endif

const char* BMPIMAGETYPESUPPORT[] = {".bmp", ".BMP", NULL, NULL};
const char* PNGIMAGETYPESUPPORT[] = {".png", ".PNG", NULL, NULL};
const char* JPGIMAGETYPESUPPORT[] = {".jpg", ".JPG", ".jpe", ".JPE", ".jpeg", ".JPEG", ".jfif", ".JFIF", NULL, NULL};

//////////////////////////////////////////////////////////////////////////////////////

#define USB_RDISK "/usb/rdisk0"
#define USB_HDISK "/usb/hdisk0"

#if defined(OSPLUS)

static BOOL GVFS_CallbackRegistered = FALSE;
static BOOL GVFS_Initialized = FALSE;
static BOOL GVFS_UsbMounted = FALSE;
static std::string GVFS_UsbMountedName;

time_t GVFS_GetTimeSystem(void);
void GVFS_OnMounted(void* pUSRParam, char* pMountName);
void GVFS_OnUnmounted(void* pUSRParam, char* pMountName);

//////////////////////////////////////////////////////////////////////////////////////
//Call back function: get the system time
//////////////////////////////////////////////////////////////////////////////////////
time_t GVFS_GetTimeSystem(void)
{
   static time_t timeVal = 0;
   timeVal = _S_.getCurrentTimeSecond();
   return timeVal;
}

//////////////////////////////////////////////////////////////////////////////////////
//Call back function: retrieve the Mounted event
//////////////////////////////////////////////////////////////////////////////////////
void GVFS_OnMounted(void* pUSRParam, char* pMountName)
{
   _DTraceError("GVFS_OnMounted %s", pMountName);
   GVFS_UsbMounted = TRUE;
   GVFS_UsbMountedName.assign(pMountName);
   GEventManager::getInstance().fire(GEVENT_USB_EVENT_MOUNTED);
}

//////////////////////////////////////////////////////////////////////////////////////
//Call back function: retrieve the Unmounted event
//////////////////////////////////////////////////////////////////////////////////////
void GVFS_OnUnmounted(void* pUSRParam, char* pMountName)
{
   _DTraceError("GVFS_OnUnmounted %s", pMountName);
   GVFS_UsbMounted = FALSE;
   GVFS_UsbMountedName.assign("");
   GEventManager::getInstance().fire(GEVENT_USB_EVENT_UNMOUNTED);
}

//////////////////////////////////////////////////////////////////////////////////////
//Name:        GVFS_Init
//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_Init()
{
   if( GVFS_CallbackRegistered == FALSE )
   {
      // Register to any device detected on the USB
      if( device_callback_attach(GVFS_UsbDeviceCallBack,NULL,1) != 0 )
      {
         _DTraceError("ERROR: Unable to set hotplug callback");
      }
      else
      {
         GVFS_CallbackRegistered = TRUE;
      }

      if( ST_NO_ERROR != VFS_RegisterCallback( GVFS_OnMounted , NULL, TRUE, FALSE) )
      {
         _DTraceError("ERROR: Could not VFS_RegisterCallback - GVFS_OnMounted");
         GVFS_CallbackRegistered = FALSE;
      }
      else
      {
         GVFS_CallbackRegistered &= TRUE;
      }

      if( ST_NO_ERROR != VFS_RegisterCallback( GVFS_OnUnmounted , NULL, FALSE, TRUE) )
      {
         _DTraceError("ERROR: Could not VFS_RegisterCallback - GVFS_OnUnmounted");
         GVFS_CallbackRegistered = FALSE;
      }
      else
      {
         GVFS_CallbackRegistered &= TRUE;
      }
   }

   if( GVFS_Initialized == FALSE )
   {
      // Register the TimeSys
      if( registry_key_set(VFS_REGISTRY_NODE, VFS_REGISTRY_GET_TIME, (void**)GVFS_GetTimeSystem) != 0 )
      {
         _DTraceError("ERROR: Could not set registry key VFS_REGISTRY_GET_TIME");
      }
      else
      {
         _DTraceError("Registry key VFS_REGISTRY_GET_TIME successfully");
         GVFS_Initialized = TRUE;
      }

      if( ST_NO_ERROR != VFS_Init() )
      {
         _DTraceError("ERROR: Could not VFS_Init");
         GVFS_Initialized = FALSE;
      }
      else
      {
         GVFS_Initialized &= TRUE;
      }
   }
   else
   {
      _DTraceError("GVFS is already initialized !");
   }

   if( GVFS_Initialized == TRUE )
   {
      device_type_t DeviceType;
      if( device_attribute(USB_RDISK, "type", (void**)&DeviceType) == 0 )
      {
         if( ST_ERROR_ALREADY_INITIALIZED == VFS_Mount((U8*)USB_RDISK, (U8*)"/volume0", NULL, (U8*)"rw", NULL) )
         {
            GVFS_OnMounted(NULL, "/USBRDISK00"); //USBHDISK0
         }
      }
      else if( device_attribute(USB_HDISK, "type", (void**)&DeviceType) == 0 )
      {
         if( ST_ERROR_ALREADY_INITIALIZED == VFS_Mount((U8*)USB_HDISK, (U8*)"/volume0", NULL, (U8*)"rw", NULL) )
         {
            GVFS_OnMounted(NULL, "/USBHDISK00"); //USBHDISK0
         }
      }
   }

   return (GVFS_Initialized == TRUE) ? true : false;
}

//////////////////////////////////////////////////////////////////////////////////////
static void GVFS_UsbDeviceCallBack( device_event_t event, device_notify_t* notifications[], int total, void* cookie )
{
   _DTraceError("GVFS_UsbDeviceCallBack...");

   if( event == device_created )
   {
      _DTraceError("Device created !");
   }
   else
   {
      _DTraceError("Device deleted !");
      GEventManager::getInstance().fire(GEVENT_USB_EVENT_CONNECTION_LOST);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GVFS_DeInit()
{
   GVFS_Initialized = FALSE;

   if( ST_NO_ERROR != VFS_Term() )
   {
      _DTraceError("GVFS_DeInit could not terminate VFS !");
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_IsUsbPlugged()
{
   bool retVal = true;

   device_type_t DeviceType;

   if( device_attribute(USB_RDISK, "type", (void**)&DeviceType) != 0 )
   {
      if( device_attribute(USB_HDISK, "type", (void**)&DeviceType) != 0 )
      {
         retVal = false;
      }
   }

   if( (DeviceType != device_type(OSPLUS_DEVICE_TYPE_FIXED_STORAGE)) &&
       (DeviceType != device_type(OSPLUS_DEVICE_TYPE_REMOVABLE_STORAGE)) )
   {
      retVal = false;
   }

   return retVal;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_IsUsbMounted()
{
   return (GVFS_UsbMounted == TRUE) ? true : false;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_GetMountedName(std::string& mountedName)
{
   mountedName.assign(GVFS_UsbMountedName);
   return GVFS_IsUsbMounted();
}

//////////////////////////////////////////////////////////////////////////////////////
//Name:        GVFS_Mount
//Description: Mount a volume name
//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_Mount(std::string diskName, std::string volumeName, std::string modeName, std::string& mountName)
{
   char* sMountName;
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;

   if( (ErrCode = VFS_Mount((U8*)diskName.c_str(), (U8*)volumeName.c_str(), NULL, (U8*)modeName.c_str(), (U8**)&sMountName)) != ST_NO_ERROR )
   {
      if( ErrCode == ST_ERROR_ALREADY_INITIALIZED )
      {
         _DTraceError("This device is already mounted !\n");
      }
      else
      {
         _DTraceError("Unable to mount this device : errno=%s !\n", strerror(errno));
      }

      return false;
   }

   mountName.assign(sMountName);

   _DTraceError("This device is mounted successfully !\n");
   return true;
}


//////////////////////////////////////////////////////////////////////////////////////
//Name:        GVFS_UMount
//Description: UMount a mount name
//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_UMount(std::string mountName)
{
   if( VFS_UMount((U8*)mountName.c_str()) != ST_NO_ERROR )
   {
      _DTraceError("Unable to unmount driver: %s !", mountName.c_str());
      return false;
   }

   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// Name:        GVFS_DetectFileType
// Description: Check is file playable
//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_DetectFileType(FileInfos& fileInf)
{
   std::string tmpFilename;
   size_t pos;
   std::list<FileInfos> tmpListFiles;
   std::string tmpFilePath2;
   std::string tmpFilePath1;
   std::string templateFileNameInUse[DRVR_MAX_FILE];
   int i, tmpCount = 0;
   U32 tmpFilesize;
   time_t tmpdtModified;
   bool getRECFileInfo;
   DvrFileExt fileExt;

   if( !fileInf.isDir )
   {
      bool isFileTypeDetected = false;
      i = 0;
      while( BMPIMAGETYPESUPPORT[i] != NULL && !isFileTypeDetected  )
      {
         //toupper or tolower ???
         if( fileInf.fileName.find(BMPIMAGETYPESUPPORT[i]) != std::string::npos )
         {
            isFileTypeDetected = true;
            fileInf.fileType = GFILE_TYPE_IMG_BMP;
            break;
         }
         i++;
      }

      i = 0;
      while( PNGIMAGETYPESUPPORT[i] != NULL && !isFileTypeDetected  )
      {
         //toupper or tolower ???
         if( fileInf.fileName.find(PNGIMAGETYPESUPPORT[i]) != std::string::npos )
         {
            isFileTypeDetected = true;
            fileInf.fileType = GFILE_TYPE_IMG_PNG;
            break;
         }
         i++;
      }

      i = 0;
      while( JPGIMAGETYPESUPPORT[i] != NULL && !isFileTypeDetected  )
      {
         //toupper or tolower ???
         if( fileInf.fileName.find(JPGIMAGETYPESUPPORT[i]) != std::string::npos )
         {
            isFileTypeDetected = true;
            fileInf.fileType = GFILE_TYPE_IMG_JPEG;
            break;
         }
         i++;
      }

#if defined(GUSE_STSTREAM)
      i = 0;
      while( VIDEOTYPESUPPORT[i] != NULL && !isFileTypeDetected )
      {
         //toupper or tolower ???
         if( fileInf.fileName.find(VIDEOTYPESUPPORT[i]) != std::string::npos )
         {
            isFileTypeDetected = true;
            fileInf.fileType = GFILE_TYPE_VIDEO;
            break;
         }
         i++;
      }

      i = 0;
      while( AUDIOTYPESUPPORT[i] != NULL && !isFileTypeDetected )
      {
         //toupper or tolower ???
         if( fileInf.fileName.find(AUDIOTYPESUPPORT[i]) != std::string::npos )
         {
            isFileTypeDetected = true;
            fileInf.fileType = GFILE_TYPE_AUDIO;
            break;
         }
         i++;
      }
#endif
   }
   else
   {
      pos = fileInf.getFullPath().find_last_of("/");
      tmpFilename = fileInf.getFullPath().substr(pos+1);

      if( tmpFilename != "..")
      {
         GVFS_Dir( fileInf.getFullPath(), tmpListFiles);

         // Make template list file of Playable file
         pos = fileInf.getFullPath().find_last_of("/");
         tmpFilePath2 = fileInf.getFullPath().substr(pos+1);
         tmpFilePath1 = fileInf.getFullPath() + "/" + tmpFilePath2;

         templateFileNameInUse[0] = tmpFilePath1 + fileExt.exIdx;
         templateFileNameInUse[1] = tmpFilePath1 + fileExt.exIfo;
         templateFileNameInUse[2] = tmpFilePath1 + fileExt.exTrp;
         templateFileNameInUse[3] = tmpFilePath1 + fileExt.exRap;
      }

      // Check is file playable
      std::list<FileInfos>::iterator p = tmpListFiles.begin();
      tmpCount = 0;
      tmpFilesize = 0;
      tmpdtModified = 0;
      getRECFileInfo = false;
      for( i=0; i<DRVR_MAX_FILE; i++)
      {
         p = tmpListFiles.begin();
         while( p != tmpListFiles.end() )
         {
            if( strcasecmp((*p).getFullPath().c_str(), templateFileNameInUse[i].c_str()) == 0 )
            {
               tmpCount++;
            }
            if( !getRECFileInfo )
            {
               tmpFilesize += (*p).fileSize;
               tmpdtModified = (*p).dtModified;
            }
            p++;
         }
         getRECFileInfo = true;
      }

      if( tmpCount == DRVR_MAX_FILE )
      {
         fileInf.fileType = GFILE_TYPE_MEDIA_TRP;
         fileInf.dtModified = tmpdtModified;
         fileInf.fileSize = tmpFilesize;
      }
   }

   return fileInf.isPlayable();
}
//////////////////////////////////////////////////////////////////////////////////////
// Name:        GVFS_Dir
// Description: List a directory
//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_Dir(const std::string fullPath, std::list<FileInfos>& listFiles)
{
   // Check Path Name
   std::string sFullPath = "/";
   std::string sDir1 = "";
   std::string sDir2 = "";
   size_t pos;

   char* pFullPath = new char[fullPath.length() + 1];
   strcpy(pFullPath, fullPath.c_str());
   char* pChar = strtok(pFullPath, "/");

   while( pChar != 0 )
   {
      sDir1.assign( pChar );
      pChar = strtok(NULL, "/");
      if( pChar != 0 )
      {
         sDir2.assign(pChar);
         if( sDir2 != ".." )
         {
            sFullPath.append(sDir1);
            sFullPath.append("/");
         }
      }
   }

   if( sDir1 != "." && sDir1 != ".." )
   {
       sFullPath.append(sDir1);
       sFullPath.append("/");
   }

   delete[] pFullPath;
   pFullPath = NULL;


   // List directory
   vfs_dir_t* pVfsDir;
   vfs_dirent_t vfsDirent;
   vfs_stat64_t vfsStats;

   listFiles.clear();

   // Open the directory
   pVfsDir = vfs_opendir( sFullPath.c_str() );

   if( pVfsDir == NULL )
   {
      _DTraceError("--> The system cannot find the directory specified : errno=%s !\n", strerror(errno));
      return false;
   }

   // Explores the directory
   int id = 0;
   while(1)
   {
      if( vfs_readdir( pVfsDir, &vfsDirent ) != 0 )
      {
         break;
      }

      FileInfos fileInfo;
      fileInfo.filePath = sFullPath;
      fileInfo.fileName.assign( vfsDirent.d_name );

      if( vfs_stat64(fileInfo.getFullPath().c_str(), &vfsStats) != 0 )
      {
         _DTraceError("Unexpected Error !!! \n");
         break;
      }

      if( vfsDirent.mode & VFS_MODE_DIR )
      {
         fileInfo.isDir = true;
         if( fileInfo.fileName == "." )
         {
            continue;
         }
      }
      else
      {
          fileInfo.fileSize = vfsStats.st_size;
      }

      fileInfo.dtModified = vfsStats.st_mtime;

      GVFS_DetectFileType(fileInfo);

      fileInfo.fileId = id++;
      listFiles.push_back(fileInfo);
   }

   // Close the directory
   vfs_closedir( pVfsDir );

   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// Name:        GVFS_Get_FS_Info
// Description: List a directory
//////////////////////////////////////////////////////////////////////////////////////

bool GVFS_Get_FS_Info(MountPartition& mountPartitionInfo)
{
 // If no mount
 if( VFS_MountNumber == 0 || GVFS_UsbMounted == FALSE )
 {
   _DTraceError("There are no mount points...\n");
   return false;
 }

 // Display info of the mount points
  vfs_info_t VFS_Info;
  _DTraceError("\nMount point info :\n");

   if( VFS_MountList[0].MountName[0] != 0 )
   {
      if( vfs_fscntl((char*) VFS_MountList[0].MountName, VFS_FSCNTL_INFO_GET, (void*)&VFS_Info) != 0 )
      {
         _DTraceError("--> Unable to get FS info\n");
         return false;
      }
      mountPartitionInfo.totalSize_MB  = VFS_Info.total_size/2048;
      mountPartitionInfo.freeSize_MB   = VFS_Info.free_blocks/2048;
   }

 // Return no errors
 return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// Name:        GVFS_DelRECFile
// Description: List a directory
//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_DelRECFile(const std::string fullPath)
{
   bool ret = true;

   std::list<FileInfos> listFiles;

   // Delete if this is a single file
   if( SYS_FRemove(fullPath.c_str()) == 0 )
   {
      return ret;
   }
   else
   {
      _DTraceError("--> Unable to delete a file !\n");
      ret = false;
   }

   GVFS_Dir(fullPath, listFiles);

   std::list<FileInfos>::iterator p = listFiles.begin();

   // Delete file/folder
   while( p != listFiles.end() )
   {
      std::string fileNameToDelete;
      fileNameToDelete = fullPath + "/" + (*p).fileName;
      const std::string tmpFileNameToDelete = fileNameToDelete;

      if( !(*p).isDir )
      {
         if( SYS_FRemove(fileNameToDelete.c_str())!=0 )
         {
            _DTraceError("--> Unable to delete file: %s\n", fileNameToDelete.c_str());
            ret = false;
         }
      }
      else
      {
         if( (*p).fileName != "..")
         {
            GVFS_DelRECFile(tmpFileNameToDelete);
         }
      }
      p++;
   }

   // Do the rmdir
   // ============
   if( (*p).isDir )
   {
      char* tmpFolderPath = new char[fullPath.size()+1];
      strcpy(tmpFolderPath, fullPath.c_str());

      if (SYS_FRmDir(tmpFolderPath)!=0)
      {
         _DTraceError("--> Unable to delete directory: %s\n", tmpFolderPath);
      }
      else
      {
         ret = true;
      }

      delete[] tmpFolderPath;
      tmpFolderPath = NULL;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
// Name:        GVFS_CopyFile
// Description: List a directory
//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_CopyFile(const std::string fullPathSrc, const std::string fullPathDes)
{
   char    src_name[MAX_LINE_LEN], dst_name[MAX_LINE_LEN];
   void*   src_handle;
   void*   dst_handle;
   S64     src_filesize,current_size,read_size;
   U32     tmp;
   U8*     buffer_file;

   strcpy(src_name, fullPathSrc.c_str());
   strcpy(dst_name, fullPathDes.c_str());

   // Get size of the source file
   // ===========================
   src_handle=SYS_FOpen(src_name,"rb");
   if (src_handle==NULL)
   {
      _DTraceError("--> Unable to open the source file \"%s\" !\n",src_name);
      return false;
   }
   SYS_FSeek(src_handle,0,SEEK_END);
   src_filesize=SYS_FTell(src_handle);
   SYS_FClose(src_handle);

   // Open the source file
   // ====================
   src_handle=SYS_FOpen(src_name,"rb");
   if (src_handle==NULL)
   {
      _DTraceError("--> Unable to open the source file \"%s\" !\n",src_name);
      return false;
   }

   // Open the destination file
   // =========================
   dst_handle=SYS_FOpen(dst_name,"wb");
   if (dst_handle==NULL)
   {
      _DTraceError("--> Unable to open the destination file \"%s\" !\n",dst_name);
      return false;
   }

   // Allocate a temp buffer to receive datas
   // =======================================
   buffer_file=(U8 *)SYS_Memory_Allocate(SYS_MEMORY_CACHED,(16*VFS_MAX_BLOCK/*256Kbytes*/),256);
   if (buffer_file==NULL)
   {
      SYS_FClose(dst_handle);
      SYS_FClose(src_handle);
      return false;
   }

   // Process the write
   // =================
   _DTraceError("Copy %llu bytes from \"%s\" to \"%s\"...\n",src_filesize,src_name,dst_name);
   current_size = 0;
   while(current_size<src_filesize)
   {
      if ((src_filesize-current_size)>=(16*VFS_MAX_BLOCK)/*256Kbytes*/)
      {
         read_size=16*VFS_MAX_BLOCK/*256Kbytes*/;
      }
      else
      {
         read_size=(src_filesize-current_size);
      }

      tmp=SYS_FRead(buffer_file,1,read_size,src_handle);
      if (tmp!=read_size)
      {
         _DTraceError("--> Read access problem on the source file !\n");
         SYS_FClose(dst_handle);
         SYS_FClose(src_handle);
         SYS_Memory_Deallocate(SYS_MEMORY_CACHED,(U32)buffer_file);
         return false;
      }
      tmp=SYS_FWrite(buffer_file,1,read_size,dst_handle);
      if (tmp!=read_size)
      {
         _DTraceError("--> Write access problem on the destination file !\n");
         SYS_FClose(dst_handle);
         SYS_FClose(src_handle);
         SYS_Memory_Deallocate(SYS_MEMORY_CACHED,(U32)buffer_file);
         return false;
      }
      current_size=current_size+read_size;
   }

   // Close the files
   // ===============
   SYS_Memory_Deallocate(SYS_MEMORY_CACHED,(U32)buffer_file);
   SYS_FClose(dst_handle);
   SYS_FClose(src_handle);

   // Return no errors
   // ================
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// Name:        GVFS_CopyFile
// Description: List a directory
//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_RenameFile(const std::string fullPathOldName, const std::string fullPathNewName)
{
   char oldName[MAX_LINE_LEN], newName[MAX_LINE_LEN];
   strcpy(oldName, fullPathOldName.c_str());
   strcpy(newName, fullPathNewName.c_str());

   _DTraceError("Rename file: %s to %s\n", fullPathOldName.c_str(), fullPathNewName.c_str());

   // Do the rename
   // =============
   if (SYS_FRename(oldName,newName)!=0)
   {
      _DTraceError("--> Unable to rename a file or directory !\n");
      return false;
   }

   // Return no errors
   // ================
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        GVFS_MkDir
//   Description: Create a directory
//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_MkDir(const std::string fullPath)
{
   char PathName[MAX_LINE_LEN];

   //Get Path Name
   strcpy(PathName, fullPath.c_str());

   //Do the mkdir
   if( SYS_FMkDir(PathName) != 0 )
   {
      _DTraceError("--> Unable to create a directory !\n");
      return false;
   }

   //Return no errors
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        GVFS_RmDir
//   Description: Remove a directory
//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_RmDir(const std::string fullPath)
{
   char PathName[MAX_LINE_LEN];

   //Get Path Name
   strcpy(PathName, fullPath.c_str());

   //Do the rmdir
   if( SYS_FRmDir(PathName) != 0 )
   {
      _DTraceError("--> Unable to delete a directory !\n");
      return false;
   }
   //Return no errors
   return true;
}
#else //defined(OSPLUS)||defined(ST_OSLINUX)

//////////////////////////////////////////////////////////////////////////////////////
// Mount a volume name with the mount name
bool GVFS_Mount(const std::string diskName, const std::string volumeName, const std::string modeName, std::string& mountName)
{
   diskName;
   volumeName;
   modeName;
   mountName;
   //Do not support at the moment
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
// Umount a mount name disk from
bool GVFS_UMount(const std::string mountName)
{
   mountName;
   //Do not support at the moment
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
// List a directory
bool GVFS_Dir(const std::string fullPath, std::list<FileInfos>& listFiles)
{
   fullPath;
   listFiles.clear();
   //Do not support at the moment
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
// get a struct of FS mounted
bool get_FS_Info(MountPartition& mountPartitionInfo)
{
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
// Name:        GVFS_DelRECFile
bool GVFS_DelRECFile(const std::string fullPath)
{
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
// Name:        GVFS_DetectFileType
bool GVFS_DetectFileType(FileInfos& fileInf)
{
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
// Name:        GVFS_CopyFile
bool GVFS_CopyFile(const std::string fullPathSrc, const std::string fullPathDes)
{
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
// Name:        GVFS_CopyFile
bool GVFS_RenameFile(const std::string fullPathOldName, const std::string fullPathNewName)
{
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
// GVFS_MkDir
//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_MkDir(const std::string fullPath)
{
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
//   Name:        GVFS_RmDir
//   Description: Remove a directory
//////////////////////////////////////////////////////////////////////////////////////
bool GVFS_RmDir(const std::string fullPath)
{
   return false;
}

#endif //defined(OSPLUS)||defined(ST_OSLINUX)

//////////////////////////////////////////////////////////////////////////////////////
//} //API
//} //STM
//} //GBS
