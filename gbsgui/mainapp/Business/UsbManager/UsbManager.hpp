//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: UsbManager.hpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef USBMANAGER_HPP_
#define USBMANAGER_HPP_

#include "UsbMassStorage.hpp"
#include "GList.hpp"
#include <string>

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace UsbManager {
//////////////////////////////////////////////////////////////////////////////////////

class UsbManager : public virtual GObject
{
public:
   static UsbManager& getInstance();

   bool init();
   bool deInit();
   bool deleteFile(std::string itemPath);
   bool copyFile(std::string fullPathSrc, std::string fullPathDes);
   bool renameFile(std::string fullPathOldName, std::string fullPathNewName);
   bool getListFiles(std::list<FileInfos>& listFiles);
   bool getListFileItems(GList*& pListItems, GScrollBar*& pScrollBar, bool bShowScrollBar = true, bool bSortList = true, bool sFilter = false);
   bool getListFileImages(GList*& pListItems, GScrollBar*& pScrollBar, bool bShowScrollBar = true);

   bool isReady();
   bool isPlugged();
   bool isMounted();

   bool getFileToRecord(std::string& fileName);

   bool getFileInfoAt(int index, FileInfos& fInf);
   FileSelectedAction selectFileAt(int index);

   bool validateFileName(std::string str);

   void setWorkingDir(std::string path);
   std::string getWorkingDir();
   std::string getWorkingDirToShow();
   bool isUSBStorageFull();

private:
   UsbManager();
   ~UsbManager();

   void onEventMounted(EVENT_HANDLER_PARAM);
   void onEventUnmounted(EVENT_HANDLER_PARAM);

   UsbMassStorage m_usbMassStorage;
   bool m_checkVideoInfo;
   bool m_isSortListAsc;

   IExecutable* m_pDelegateUsbMount;
   IExecutable* m_pDelegateUsbUnmount;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UsbManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif // USBMANAGER_HPP_
