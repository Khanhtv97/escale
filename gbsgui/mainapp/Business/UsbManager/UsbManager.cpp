//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: UsbManager.cpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#include "UsbManager.hpp"
#include "GStringUtils.hpp"
#include "GFont.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace UsbManager {

struct SortByFileNameAsc
{
   bool operator()(const FileInfos& a, const FileInfos& b)
   {
      return (a.fileName.compare(b.fileName) < 0) ? true : false;
   }
};

struct SortByFileNameDesc
{
   bool operator()(const FileInfos& a, const FileInfos& b)
   {
      return (a.fileName.compare(b.fileName) > 0) ? true : false;
   }
};

const int k_acceptRECVal = 2;

//////////////////////////////////////////////////////////////////////////////////////

UsbManager::UsbManager()
{
   m_isSortListAsc = true;
   m_pDelegateUsbMount = NULL;
   m_pDelegateUsbUnmount = NULL;
   ADD_EVENT_LISTENER(GEVENT_USB_EVENT_MOUNTED, m_pDelegateUsbMount, UsbManager, this, onEventMounted);
   ADD_EVENT_LISTENER(GEVENT_USB_EVENT_UNMOUNTED, m_pDelegateUsbUnmount, UsbManager, this, onEventUnmounted);
}

//////////////////////////////////////////////////////////////////////////////////////
UsbManager::~UsbManager()
{
   REMOVE_EVENT_LISTENER(GEVENT_USB_EVENT_MOUNTED, m_pDelegateUsbMount);
   REMOVE_EVENT_LISTENER(GEVENT_USB_EVENT_UNMOUNTED, m_pDelegateUsbUnmount);
}

//////////////////////////////////////////////////////////////////////////////////////
UsbManager& UsbManager::getInstance()
{
   static UsbManager instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::init()
{
   // just init an instance
   return m_usbMassStorage.init();
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::deInit()
{
   return m_usbMassStorage.deInit();
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::getFileInfoAt(int index, FileInfos& fInf)
{
   return m_usbMassStorage.getFileInfoAt(index, fInf);
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::validateFileName(std::string str)
{
   return m_usbMassStorage.validateFileName(str);
}

//////////////////////////////////////////////////////////////////////////////////////
FileSelectedAction UsbManager::selectFileAt(int index)
{
   return m_usbMassStorage.selectFileAt(index);
}

//////////////////////////////////////////////////////////////////////////////////////
void UsbManager::setWorkingDir(std::string path)
{
   return m_usbMassStorage.setWorkingDirectory(path);
}

//////////////////////////////////////////////////////////////////////////////////////
std::string UsbManager::getWorkingDir()
{
   return m_usbMassStorage.getWorkingDirectory();
}

//////////////////////////////////////////////////////////////////////////////////////
std::string UsbManager::getWorkingDirToShow()
{
   size_t pos;
   std::string tmpDir, tmpDir1;

   std::string workingDir = getWorkingDir();

   pos = workingDir.find("/../");
   if( pos != std::string::npos)
   {
      tmpDir = workingDir.substr(0, pos);
      pos = tmpDir.find_last_of("/");
      workingDir = tmpDir.substr(0, pos);
   }

   if( (workingDir).length() > 40 )
   {
      pos = workingDir.find_last_of("/");
      tmpDir = workingDir.substr(0, pos);
      pos = tmpDir.find_last_of("/");
      workingDir = tmpDir.substr(pos);
      workingDir = ".." + workingDir;
   }

   return workingDir;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::isUSBStorageFull()
{
   MountPartition mountPartitionInfo;

   if( !m_usbMassStorage.getUSBStorageInfo(mountPartitionInfo) )
   {
      return true;
   }

   if( mountPartitionInfo.freeSize_MB <= k_acceptRECVal )
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
void UsbManager::onEventMounted(EVENT_HANDLER_PARAM)
{
   m_usbMassStorage.mount();
   this->dispatchEvent(GEVENT_USB_EVENT_PLUGGED, NULL);
}

//////////////////////////////////////////////////////////////////////////////////////
void UsbManager::onEventUnmounted(EVENT_HANDLER_PARAM)
{
   m_usbMassStorage.unmount();
   this->dispatchEvent(GEVENT_USB_EVENT_UNPLUGGED, NULL);
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::isReady()
{
   bool isUsbReady = m_usbMassStorage.isReady();
   if( !isUsbReady )
   {
      if( m_usbMassStorage.isPlugged() )
      {
         _DTraceError("USB Mass Storage is plugged, try to mount againt...");
         //unexpected case -> try to reinit vfs
         m_usbMassStorage.deInit();
         Task_Delay(ST_GetClocksPerSecond());
         m_usbMassStorage.init();
         Task_Delay(3*ST_GetClocksPerSecond());
         isUsbReady = m_usbMassStorage.isReady();
      }
      else
      {
         _DTraceError("USB Mass Storage is unplugged...");
      }
   }

   return isUsbReady;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::isPlugged()
{
   return m_usbMassStorage.isPlugged();
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::isMounted()
{
   return m_usbMassStorage.isMounted();
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::getFileToRecord(std::string& fileName)
{
   return m_usbMassStorage.getFileToRecord(fileName);
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::deleteFile(std::string itemPath)
{
   bool ret = m_usbMassStorage.delRECFile(itemPath);
   dispatchEvent(GEVENT_DVR_FILE_OPERATION_COMPLETE, &ret);
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::copyFile(std::string fullPathSrc, std::string fullPathDes)
{
   bool ret = m_usbMassStorage.copyRECFile(fullPathSrc, fullPathDes);
   dispatchEvent(GEVENT_DVR_FILE_OPERATION_COMPLETE, &ret);
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::renameFile(std::string fullPathOldName, std::string fullPathNewName)
{
   bool ret = false;
   fullPathOldName = GStringUtils::trimAll( fullPathOldName );
   fullPathNewName = GStringUtils::trimAll( fullPathNewName );

   if( fullPathOldName.compare(fullPathNewName) != 0 )
   {
      size_t pos = fullPathNewName.find_last_of("/");
      std::string fileName = fullPathNewName.substr(pos+1);

      if( !validateFileName(fileName) )
      {
         ret = false;
      }
      else
      {
         ret = m_usbMassStorage.renameRECFile( fullPathOldName, fullPathNewName );
      }
   }
   else
   {
      ret = true;
   }

   dispatchEvent(GEVENT_DVR_FILE_OPERATION_COMPLETE, &ret);
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::getListFiles(std::list<FileInfos>& listFiles)
{
   listFiles.clear();
   m_usbMassStorage.getListFiles(listFiles);

   if( listFiles.empty() )
   {
      return false;
   }

   if( m_isSortListAsc )
   {
     _DTraceError("Sort->Ascending");
     listFiles.sort(SortByFileNameAsc());
   }
   else
   {
     _DTraceError("Sort->Descending");
     listFiles.sort(SortByFileNameDesc());
   }

   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::getListFileItems(GList*& pListItems, GScrollBar*& pScrollBar, bool bShowScrollBar, bool bSortList, bool sFilter)
{
   bool ret = false;
   std::list<FileInfos> listOfFiles;
   listOfFiles.clear();

   if( m_usbMassStorage.isReady() )
   {
      m_usbMassStorage.refreshListFiles();
      ret = m_usbMassStorage.getListFiles( listOfFiles );
   }

   if( pListItems != NULL )
   {
      pListItems->destroyAllItem();
      pListItems->destroyViews();
      delete pListItems;
      pListItems = NULL;
   }

   pListItems = new GList( listOfFiles.size() );
   pListItems->setCircleNavigation(true);

   // init parameters for chanelList
   pListItems->setCols(1);
   pListItems->setRows(10);
   pListItems->setItemSpace(0, 1);
   pListItems->setPos(0, 78);
   pListItems->m_fullWidth = 500;
   pListItems->m_fullHeight = 315;

   // add pScrollBar for pListItems
   if( bShowScrollBar )
   {
      if( pScrollBar == NULL )
      {
         //pScrollBar = new GScrollBar(NULL, new GBitmap(LoadPreDefinedGam(&GList_ScrollBar_ThumbBG_gam), 5, 22));
         pScrollBar = new GScrollBar(NULL, NULL);
         pScrollBar->setPos(515, 0);
         pScrollBar->setTrackLength(308);
      }

      pListItems->setScrollBar(pScrollBar);
   }

   m_isSortListAsc = bSortList;

   if( m_isSortListAsc )
   {
     _DTraceError("Sort->Ascending");
     listOfFiles.sort(SortByFileNameAsc());
   }
   else
   {
     _DTraceError("Sort->Descending");
     listOfFiles.sort(SortByFileNameDesc());
   }

   std::list<FileInfos>::iterator p = listOfFiles.begin();

   while( p != listOfFiles.end() )
   {
      if( ((*p).isDir) && ((*p).fileName.compare("..") == 0) )
      {
         listOfFiles.splice( listOfFiles.begin(), listOfFiles, p );
         break;
      }
      else
      {
         p++;
      }
   }

   p = listOfFiles.begin();

   while( p != listOfFiles.end() )
   {
      if( !(*p).isPlayable() && !(*p).isDir && sFilter )
      {
         p++;
         continue;
      }

      // Get size of file
      char tmpFileSize[9];
      if( (*p).isDir  && ((*p).fileType != GFILE_TYPE_MEDIA_TRP) )
      {
         sprintf(tmpFileSize, "%s", "");
      }
      else
      {
         U32 size = (*p).fileSize / 1024;
         if( (size < 1) && ((*p).fileSize > 0))
         {
            size = 1;
         }

         if(size >= 1024)
         {
            size = (*p).fileSize / (1024*1024);
            sprintf(tmpFileSize, "%u MB", size);
         }
         else if(size < 1024)
         {
            sprintf(tmpFileSize, "%u KB", size);
         }
      }

      //Get date time modified of file
      char tmpFileDtModified[30];

      if( ((*p).isDir) && ((*p).fileName.compare("..") == 0) )
      {
         sprintf( tmpFileDtModified, "%s", "" );
      }
      else
      {
         struct tm* pTimeInf;
         pTimeInf = localtime( &((*p).dtModified) );
         strftime( tmpFileDtModified, 30, "%d/%m/%y", pTimeInf );
      }

      // Fix displayed file name to first 18 chars
      std::string tempFileName = (*p).fileName;
      if( ((*p).fileName).length() > 18 )
      {
         tempFileName = (*p).fileName.substr(0, 15);
         tempFileName.append("...");
         (*p).fileName.assign(tempFileName);
      }

      GText* fNameNormal = new GText( 30, -2, tempFileName, VERA_SANS_14_N, NORMAL_TEXTFONT);
      GText* fDateNormal = new GText( 300, -2, tmpFileDtModified, VERA_SANS_14_N, NORMAL_TEXTFONT);
      GText* fSizeNormal = new GText( 400, -2, tmpFileSize, VERA_SANS_14_N, NORMAL_TEXTFONT);
      GText* fNameFocus = new GText( 30, -2, tempFileName, VERA_SANS_14_N, FOCUSED_TEXTFONT);
      GText* fDateFocus = new GText( 300, -2, tmpFileDtModified, VERA_SANS_14_N, FOCUSED_TEXTFONT);
      GText* fSizeFocus = new GText( 400, -2, tmpFileSize, VERA_SANS_14_N, FOCUSED_TEXTFONT);

      GText* fNameSelected = new GText( 30, -2, tempFileName, VERA_SANS_14_N, SELECTED_TEXTFONT);
      GText* fDateSelected = new GText( 300, -2, tmpFileDtModified, VERA_SANS_14_N, SELECTED_TEXTFONT);
      GText* fSizeSelected = new GText( 400, -2, tmpFileSize, VERA_SANS_14_N, SELECTED_TEXTFONT);
      GText* fNameFocusSelected = new GText( 30, -2, tempFileName, VERA_SANS_14_N, SELECTED_TEXTFONT);
      GText* fDateFocusSelected = new GText( 300, -2, tmpFileDtModified, VERA_SANS_14_N, SELECTED_TEXTFONT);
      GText* fSizeFocusSelected = new GText( 400, -2, tmpFileSize, VERA_SANS_14_N, SELECTED_TEXTFONT);

      fNameNormal->textField_W = 160;
      fNameFocus->textField_W = 160;
      fNameSelected->textField_W = 160;
      fNameFocusSelected->textField_W = 160;

      fDateNormal->textField_W = 80;
      fDateFocus->textField_W = 80;
      fDateSelected->textField_W = 80;
      fDateFocusSelected->textField_W = 80;

      fSizeNormal->textField_W = 105;
      fSizeFocus->textField_W = 105;
      fSizeSelected->textField_W = 105;
      fSizeFocusSelected->textField_W = 105;

      fSizeNormal->alignType = ALIGN_RIGHT;
      fSizeFocus->alignType = ALIGN_RIGHT;
      fSizeSelected->alignType = ALIGN_RIGHT;
      fSizeFocusSelected->alignType = ALIGN_RIGHT;

      //GView *viewNormal, *viewFocus, *viewSelected, *viewFocusSelected;
      // init views for every channelListItem *** begin
      GView* viewNormal = new GView();
      viewNormal->addView(fNameNormal).addView(fDateNormal).addView(fSizeNormal);

      GView* viewFocus = new GView();
      viewFocus->addView(NULL, 5, 0).addView(fNameFocus).addView(fDateFocus).addView(fSizeFocus);

      GView* viewSelected = new GView();
      viewSelected->addView(fNameSelected).addView(fDateSelected).addView(fSizeSelected);

      GView* viewFocusSelected = new GView();
      viewFocusSelected->addView(NULL, 5 , 0)
                               .addView(fNameFocusSelected).addView(fDateFocusSelected).addView(fSizeFocusSelected);

      if( (*p).isDir && ((*p).fileType != GFILE_TYPE_MEDIA_TRP) )
      {
          viewNormal->addView(NULL, 10, 5);
          viewFocus->addView(NULL, 10, 5);
          viewSelected->addView(NULL, 10, 5);
          viewFocusSelected->addView(NULL, 10, 5);
      }
      else if( ((*p).fileType == GFILE_TYPE_MEDIA_TRP) || ((*p).fileType == GFILE_TYPE_VIDEO) )
      {
         viewNormal->addView(NULL, 10, 5);
         viewFocus->addView(NULL, 10, 5);
         viewSelected->addView(NULL, 10, 5);
         viewFocusSelected->addView(NULL, 10, 5);
      }
      else if( (*p).fileType == GFILE_TYPE_AUDIO )
      {
         viewNormal->addView(NULL, 10, 5);
         viewFocus->addView(NULL, 10, 5);
         viewSelected->addView(NULL, 10, 5);
         viewFocusSelected->addView(NULL, 10, 5);
      }
      else if( (*p).isPicture() )
      {
         viewNormal->addView(NULL, 10, 5);
         viewFocus->addView(NULL, 10, 5);
         viewSelected->addView(NULL, 10, 5);
         viewFocusSelected->addView(NULL, 10, 5);
      }

      FileListItem* pFileItem = new FileListItem();
      pFileItem->setViews(viewNormal, viewFocus, viewSelected, viewFocusSelected);
      pFileItem->setId((*p).fileId);

      pListItems->addItem(pFileItem);

      p++;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool UsbManager::getListFileImages(GList*& pListItems, GScrollBar*& pScrollBar, bool bShowScrollBar)
{
   bool ret = false;
   std::list<FileInfos> listOfFiles;
   listOfFiles.clear();

   if( m_usbMassStorage.isReady() )
   {
      m_usbMassStorage.refreshListFiles();
      ret = m_usbMassStorage.getListFiles( listOfFiles );
   }

   if( pListItems != NULL )
   {
      pListItems->destroyAllItem();
      pListItems->destroyViews();
      delete pListItems;
      pListItems = NULL;
   }

   std::list<FileInfos>:: iterator p = listOfFiles.begin();

   while( p != listOfFiles.end() )
   {
      if( ((*p).isDir) && ((*p).fileName.compare("..") == 0) )
      {
         listOfFiles.splice( listOfFiles.begin(), listOfFiles, p );
         break;
      }
      else
      {
         p++;
      }
   }

   if( listOfFiles.size() == 0 )
   {
      return ret;
   }

   pListItems = new GList( listOfFiles.size() );
   pListItems->setCircleNavigation(true);

   pListItems->setCols(1);
   pListItems->setRows(13);
   pListItems->setItemSpace(0, 1);
   pListItems->setPos(67, 106);
   pListItems->m_fullWidth = 120;
   pListItems->m_fullHeight = 410;

   // add pScrollBar for pListItems
   if( bShowScrollBar )
   {
      if( pScrollBar == NULL )
      {
         pScrollBar = new GScrollBar(NULL, NULL);
         pScrollBar->setPos(135, 0);
         pScrollBar->setTrackLength(400);
      }

      pListItems->setScrollBar(pScrollBar);
   }

   p = listOfFiles.begin();

   while( p != listOfFiles.end() )
   {
      if( (!(*p).isPicture() && !(*p).isDir) || (*p).isPlayable() )
      {
         p++;
         continue;
      }

      // Fix displayed file name to first 18 chars
      std::string tempFileName = (*p).fileName;
      if( ((*p).fileName).length() > 18 )
      {
         tempFileName = (*p).fileName.substr(0, 15);
         tempFileName.append("...");
         (*p).fileName.assign(tempFileName);
      }

      GText* fNameNormal = new GText( 25, -2, tempFileName, VERA_SANS_14_N, NORMAL_TEXTFONT);
      GText* fNameFocus = new GText( 25, -2, tempFileName, VERA_SANS_14_N, FOCUSED_TEXTFONT);

      GText* fNameSelected = new GText( 25, -2, tempFileName, VERA_SANS_14_N, SELECTED_TEXTFONT);
      GText* fNameFocusSelected = new GText( 25, -2, tempFileName, VERA_SANS_14_N, SELECTED_TEXTFONT);

      fNameNormal->textField_W = 85;
      fNameFocus->textField_W = 85;
      fNameSelected->textField_W = 85;
      fNameFocusSelected->textField_W = 85;

      GView* viewNormal = new GView();
      viewNormal->addView(fNameNormal);

      GView* viewFocus = new GView();
      viewFocus->addView(NULL, 0, 0).addView(fNameFocus);

      GView* viewSelected = new GView();
      viewSelected->addView(fNameSelected);

      GView* viewFocusSelected = new GView();
      viewFocusSelected->addView(NULL, 0 , 0).addView(fNameFocusSelected);

      if( (*p).isDir )
      {
         viewNormal->addView(NULL, 0, 5);
         viewFocus->addView(NULL, 0, 5);
         viewSelected->addView(NULL, 0, 5);
         viewFocusSelected->addView(NULL, 0, 5);
      }
      else if( (*p).isPicture() )
      {
         viewNormal->addView(NULL, 0, 5);
         viewFocus->addView(NULL, 0, 5);
         viewSelected->addView(NULL, 0, 5);
         viewFocusSelected->addView(NULL, 0, 5);
      }

      FileListItem* pFileItem = new FileListItem();
      pFileItem->setViews(viewNormal, viewFocus, viewSelected, viewFocusSelected);
      pFileItem->setId((*p).fileId);

      pListItems->addItem(pFileItem);

      p++;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UsbManager
//} //Business
//} //Application
//} //STM
//} //GBS
