//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: InstallManager.cpp                                                         //
//////////////////////////////////////////////////////////////////////////////////////

#include "InstallManager.hpp"
#include "GSystem.hpp"
#include "GFileSystem.hpp"
#include "GROMManager.hpp"
#include "GStringUtils.hpp"
#include "UsbManager.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace InstallManager {
//////////////////////////////////////////////////////////////////////////////////////

InstallManager& InstallManager::getInstance()
{
   static InstallManager instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
InstallManager::InstallManager()
{
   m_installFile = "";
   m_isInstalling = false;
   m_imageType = GIMAGE_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////////////
InstallManager::~InstallManager()
{
   deinit();
}

//////////////////////////////////////////////////////////////////////////////////////
bool InstallManager::init()
{
   return true;
}
//////////////////////////////////////////////////////////////////////////////////////
void InstallManager::deinit()
{
}

//////////////////////////////////////////////////////////////////////////////////////
bool InstallManager::startInstall()
{
   bool ret = false;

   if( m_isInstalling )
   {
      return ret;
   }

   Semaphore_Init_Fifo(m_installSemaphore, 0);

   m_isInstalling = true;
   m_imageType = GIMAGE_UNKNOWN;

   _DTraceError("StartInstall...");

   std::string installFile = InstallManager::getInstance().getInstallFile();

   GROMError result = GROMB_ERR_MEMORY;
   GROMSlots currentSlot = GROM_SLOT_UNKNOWN;
   GROMSlots activatingSlot = GROM_SLOT_UNKNOWN;

   if( GROMManager::getInstance().readROMInfo(true) == TRUE )
   {
      int pos;
      char* pSign = NULL;

      m_imageType = GROM_DetectImageType(installFile, pos, pSign);

      if( m_imageType == GIMAGE_APPLICATION_INVALID )
      {
         _DTraceError("File is not valid...");
         result = GROMB_ERR_FILE_FORMAT;
      }
      else if( (m_imageType == GIMAGE_UNKNOWN) || (pSign == NULL) || (pos == -1) )
      {
         _DTraceError("File format incorrect...");
         result = GROMB_ERR_FILE_FORMAT;
      }
      else if( (m_imageType == GIMAGE_LOGO_SPLASH) || (m_imageType == GIMAGE_LOGO_INFO) || (m_imageType == GIMAGE_LOGO_MENU) )
      {
         pos += ROMB_SIGNATURE_LEN;
         U32 address;

         switch( m_imageType )
         {
         case GIMAGE_LOGO_SPLASH:
            address = GFileSystem::getInstance().toLogoAddress(GLOGO_SPLASH);
            break;

         case GIMAGE_LOGO_INFO:
            address = GFileSystem::getInstance().toLogoAddress(GLOGO_INFO);
            break;

         case GIMAGE_LOGO_MENU:
            address = GFileSystem::getInstance().toLogoAddress(GLOGO_MENU);
            break;
         }

         result = GROM_BurnFlash(DEFAULT_FLASH_ID, address, 0, installFile, m_imageType, pos, pSign);

         if( result == GROMB_ERR_NONE )
         {
            _S_.setReboot();
         }
      }
      else if( m_imageType == GIMAGE_USER_DATA )
      {
         pos += ROMB_SIGNATURE_LEN;
         U32 address = FS_BASED_ADDRESS;

         result = GROM_BurnFlash(DEFAULT_FLASH_ID, address, 0, installFile, m_imageType, pos, pSign);
         if( result == GROMB_ERR_NONE )
         {
            _DTraceError("Restore user data successfully... should restart !");
            _S_.setReboot();
         }
         else
         {
            _DTraceError("Restore user data failed !");
         }
      }
      else
      {
         pos += ROMB_SIGNATURE_LEN;

         _DTraceError("File: Type = %d, signature = %s, pos = %d", m_imageType, pSign, pos);

         if( m_imageType == GIMAGE_APPLICATION_STSTREAM )
         {
            // The Application supports STSTREAM feature, the size of app is bigger than 8MB
            // so it will always be installed into slot 0 due to flash's size is limited
            // the STB WILL NEVER RUN again if the power is turned off during the installing
            currentSlot = GROM_SLOT_1;
         }
         else
         {
            currentSlot = GROMManager::getInstance().getActiveSlot();
         }

         if( currentSlot == GROM_SLOT_0 )
         {
            activatingSlot = GROM_SLOT_1;
            result = GROM_BurnFlash(DEFAULT_FLASH_ID, ROM_BASE_ADDRESS_SLOT_1, 0, installFile, m_imageType, pos, pSign);
         }
         else if( currentSlot == GROM_SLOT_1 )
         {
            activatingSlot = GROM_SLOT_0;
            result = GROM_BurnFlash(DEFAULT_FLASH_ID, ROM_BASE_ADDRESS_SLOT_0, 0, installFile, m_imageType, pos, pSign);
         }
         else
         {
            _DTraceError("Unknow active slot");
         }

         if( result == GROMB_ERR_NONE || result == GROMB_ERR_NONE_BLD || result == GROMB_ERR_NONE_BLD_LINUX )
         {
            Task_Delay(ST_GetClocksPerSecond()/2);
            _DTraceError("restarting...");

            if( result == GROMB_ERR_NONE_BLD_LINUX )
            {
               //Treats error to GROMB_ERR_NONE
               result = GROMB_ERR_NONE;
            }
            else if( result == GROMB_ERR_NONE_BLD )
            {
               //Bootloader installed, keeping the current slot
               GROMManager::getInstance().activateSlot(currentSlot);
               //Treats error to GROMB_ERR_NONE
               result = GROMB_ERR_NONE;
            }
            else
            {
               //Main app installed, switch the opposite slot
               GROMManager::getInstance().activateSlot(activatingSlot);
            }

            _S_.setReboot();
         }
         else
         {
            _DTraceError("Could not start an installation...");
         }
      }
   }

   Task_Delay(ST_GetClocksPerSecond()/2);
   this->finishInstall(result);

   m_isInstalling = false;

   Semaphore_Signal(m_installSemaphore);
   Semaphore_Wait(m_installSemaphore);
   Semaphore_Delete(m_installSemaphore);

   ret = (result == GROMB_ERR_NONE);
   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool InstallManager::backupUserData()
{
   bool ret = false;
   std::string fullFileName = "";
   std::string fileName = "";
   DayTime_dtv_t dt;
   _S_.getCurrentTime(dt);
   GStringUtils::format(fileName, "uda%02d%02d", dt.ymdw.day, dt.ymdw.month);
   fileName.append(".sndf");

   if( UsbManager::getInstance().isReady() )
   {
      GVFS_GetMountedName(fullFileName);
      fullFileName.append("/");
      fullFileName.append(fileName);
      U32 addrBegin = FS_BASED_ADDRESS;
      U32 addrEnd = FS_LOGO_ADDRESS;

      if( GROMB_ERR_NONE == GROM_DumpFlash(DEFAULT_FLASH_ID, addrBegin, addrEnd-addrBegin, fullFileName) )
      {
         _DTraceError("Backup user data SUCESSFULLY, %s", fullFileName.c_str());
         if( GROMB_ERR_NONE == GROM_SignOnFile(fullFileName, GIMAGE_USER_DATA) )
         {
            ret = true;
         }
      }
      else
      {
         _DTraceError("Backup user data FAILED, %s", fullFileName.c_str());
      }
   }

   //fire event finish backup here
   this->dispatchEvent(GEVENT_BACKUP_USERDATA_DONE, NULL);

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool InstallManager::restoreUserData(const std::string fileName)
{
   setInstallFile(fileName);
   return startInstall();
}

//////////////////////////////////////////////////////////////////////////////////////
#ifdef DEMO_VERSION

bool InstallManager::startUninstall()
{
   bool ret = false;
   GROMError result;

   if( m_isInstalling )
   {
      return ret;
   }

   Semaphore_Init_Fifo(m_installSemaphore, 0);

   m_isInstalling = true;

   _DTraceError("Start Uninstall...");

   // Delete boot loader
   result = GROM_EraseFlash(DEFAULT_FLASH_ID, FS_SYSTEM_ADDRESS,        128*1024, FALSE);
   Task_Delay(ST_GetClocksPerSecond()/20);
   // Delete slot 1
   result = GROM_EraseFlash(DEFAULT_FLASH_ID, ROM_BASE_ADDRESS_SLOT_0, 1024*1024, FALSE);
   Task_Delay(ST_GetClocksPerSecond()/20);
   // Delete slot 2
   result = GROM_EraseFlash(DEFAULT_FLASH_ID, ROM_BASE_ADDRESS_SLOT_1, 1024*1024, FALSE);
   Task_Delay(ST_GetClocksPerSecond()/20);
   // Delete partitions
   result = GROM_EraseFlash(DEFAULT_FLASH_ID, FS_BASED_ADDRESS,         256*1024, FALSE);
   Task_Delay(ST_GetClocksPerSecond()/20);

   if( result == GROMB_ERR_NONE )
   {
      Task_Delay(ST_GetClocksPerSecond()/2);
      _DTraceError("restarting...");
      _S_.setReboot();
   }

   Task_Delay(ST_GetClocksPerSecond()/2);
   this->finishInstall(result);

   m_isInstalling = false;

   Semaphore_Signal(m_installSemaphore);
   Semaphore_Wait(m_installSemaphore);
   Semaphore_Delete(m_installSemaphore);

   ret = (result == GROMB_ERR_NONE);
   return ret;
}

#else

bool InstallManager::startUninstall()
{
   _DTraceError("Start Uninstall... Feature is not supported");
   return false;
}

#endif

//////////////////////////////////////////////////////////////////////////////////////
void InstallManager::finishInstall(GROMError result)
{
   this->setLastError(result);
   this->dispatchEvent(GEVENT_GUI_UPDATE_FIRMWARE_FINISH, &result);
}

//////////////////////////////////////////////////////////////////////////////////////
bool InstallManager::isInstalling()
{
   return m_isInstalling;
}

//////////////////////////////////////////////////////////////////////////////////////
void InstallManager::setInstallFile(const std::string fileName)
{
   m_installFile = fileName;
}

//////////////////////////////////////////////////////////////////////////////////////
std::string InstallManager::getInstallFile()
{
   return m_installFile;
}

//////////////////////////////////////////////////////////////////////////////////////
void InstallManager::setLastError(GROMError error)
{
   m_lastError = error;
}

//////////////////////////////////////////////////////////////////////////////////////
GROMError InstallManager::getLastError()
{
   return m_lastError;
}

//////////////////////////////////////////////////////////////////////////////////////
GIMAGEType InstallManager::getImageType()
{
   return m_imageType;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //InstallManager
//} //Business
//} //Application
//} //STM
//} //GBS
