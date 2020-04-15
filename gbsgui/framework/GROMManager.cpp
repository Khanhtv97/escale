//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GROMManager.cpp                                                            //
//////////////////////////////////////////////////////////////////////////////////////

#include "GROMManager.hpp"
#include "GFileSystem.hpp"
#include "GStringUtils.hpp"
#include "I2C.hpp"
#include <stddef.h>

#define BOOTROM_SIGNATURE                  "______STAPI_SDK_ROMIMAGE______" // Ensure this a multiple of 4 bytes in length
#define BOOTROM_SIGNATURE_LENGTH           32                               // Ensure this a multiple of 4 bytes in length
#define MAINAPP_SIGNATURE                  "_GBS_ST_IRD_DVBT2_APPLICATION_" // Ensure this a multiple of 4 bytes in length
#define MAINAPP_SIGNATURE_LENGTH           32                               // Ensure this a multiple of 4 bytes in length
#define BOOTROM_BASE_ADDR                  0x00000000
#define BOOTROM_SIZE                       ROM_BASE_ADDRESS_SLOT_0          //0x0001FFE4

#define ROM_LOAD_ADDRESS_OFFSET            0x00
#define ROM_SIZE_ADDRESS_OFFSET            0x04
#define ROM_COMPRESSED_ADDRESS_OFFSET      0x08
#define ROM_COMPRESSED_SIZE_ADDRESS_OFFSET 0x0C
#define ROM_START_ADDRESS_OFFSET           0x10
#define ROM_STACK_ADDRESS_OFFSET           0x14
#define ROM_DBG_CONNECTED_ADDRESS_OFFSET   0x18

//namespace GBS {
//namespace STM {
//namespace Framework {

const char app_state_manufacture[]  = "_GBS_MANUFACTURE______________\0";
const char app_state_declaration[]  = "_GBS_DECLARATION______________\0";
const char app_state_registration[] = "_GBS_REGISTRATION_____________\0";

//////////////////////////////////////////////////////////////////////////////////////
GROMManager& GROMManager::getInstance()
{
   static GROMManager instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
GROMManager::~GROMManager()
{
   //_D_.Debug("GROMManager destructor...\n");
}

//////////////////////////////////////////////////////////////////////////////////////
GROMManager::GROMManager()
{
   //_D_.Debug("GROMManager constructor...\n");
   m_activeSlot = GROM_SLOT_UNKNOWN;
   m_appState = GROM_APP_STATE_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GROMManager::readROMInfo(bool detail)
{
   U32 size = 1024;
   U32 realSize;
   U8* buffer = (U8*) malloc(sizeof(U8) * size);

   if( buffer == NULL )
   {
      _DTraceError("Can not allocate memory");
      return FALSE;
   }

   if( !GFileSystem::getInstance().fastRawRead(BOOTROM_BASE_ADDR, size, buffer, &realSize, false) )
   {
      _DTraceError("Can not read bootrom info");
      free(buffer);
      return FALSE;
   }

   int bootRomSignaturePos = GStringUtils::detectSignature(buffer, realSize, BOOTROM_SIGNATURE);

   if( bootRomSignaturePos < 0 )
   {
      _DTraceError("Bootrom signature not found");
      free(buffer);
      return FALSE;
   }

   m_loadAddressAddress = bootRomSignaturePos + BOOTROM_SIGNATURE_LENGTH;
   m_loadAddressAddress += 4;  // by pass rom_main_marker_size
   m_loadAddressAddress += 16; // by pass p0_mask, poke_list_start_address, poke_list_stop_address, poke_loop_address

   _DTraceError("ROM info identified: ");

   m_loadAddress = GStringUtils::getU32FromBuffer(buffer, m_loadAddressAddress);
   _DTraceError("rom m_loadAddress: 0x%08x", m_loadAddress);

   m_imageSizeAddress = GStringUtils::getU32FromBuffer(buffer, m_loadAddressAddress + 4);
   _DTraceError("rom m_imageSizeAddress: 0x%08x", m_imageSizeAddress);

   m_imageCompressedAddress = GStringUtils::getU32FromBuffer(buffer, m_loadAddressAddress + 8);
   _DTraceError("rom m_imageCompressedAddress: 0x%08x", m_imageCompressedAddress);

   m_imageCompressedSizeAddress = GStringUtils::getU32FromBuffer(buffer, m_loadAddressAddress + 12);
   _DTraceError("rom m_imageCompressedSizeAddress: 0x%08x", m_imageCompressedSizeAddress);

   m_startAddress = GStringUtils::getU32FromBuffer(buffer, m_loadAddressAddress + 16);
   _DTraceError("rom m_startAddress: 0x%08x", m_startAddress);

   m_stackAddress = GStringUtils::getU32FromBuffer(buffer, m_loadAddressAddress + 20);
   _DTraceError("rom m_stackAddress: 0x%08x", m_stackAddress);

   m_dbgConnectedAddress = GStringUtils::getU32FromBuffer(buffer, m_loadAddressAddress + 24);

   _DTraceError("rom m_dbgConnectedAddress: 0x%08x", m_dbgConnectedAddress);

   if( m_loadAddress == ROM_BASE_ADDRESS_SLOT_0 )
   {
      m_activeSlot = GROM_SLOT_0;
   }
   else if( m_loadAddress == ROM_BASE_ADDRESS_SLOT_1)
   {
      m_activeSlot = GROM_SLOT_1;
   }
   else
   {
      _DTraceError("FAILED: wrong m_loadAddress 0x%08x!", m_loadAddress);
      free(buffer);
      return FALSE;
   }

   // Read application state of the current Slot
   if( !GFileSystem::getInstance().fastRawRead(m_loadAddress, size, buffer, &realSize, false) )
   {
       _DTraceError("Can not read m_loadAddress value");
       free(buffer);
       return FALSE;
   }
   else
   {
       m_appState = detectAppState(buffer, realSize);
   }

   _DTraceError("Current ROM location: %d!", m_activeSlot);

   if( detail )
   {
      // doc gia tri tai cac o dia chi tren
      if( !GFileSystem::getInstance().fastRawRead(m_loadAddress, 4, buffer, &realSize, false) )
      {
         _DTraceError("Can not read m_loadAddress value");
         free(buffer);
         return FALSE;
      }
      else
      {
         _DTraceError("!!!=> Load address value at m_loadAddress: 0x%08x, value: 0x%08x\n", m_loadAddress, GStringUtils::getU32FromBuffer(buffer,0));
      }

      if( !GFileSystem::getInstance().fastRawRead(m_imageSizeAddress, 4, buffer, &realSize, false) )
      {
         _DTraceError("Can not read image size value");
         free(buffer);
         return FALSE;
      }
      else
      {
         _DTraceError("!!!=> Load image size at m_imageSizeAddress: 0x%08x, value: %d", m_imageSizeAddress, GStringUtils::getU32FromBuffer(buffer,0));
      }

      if( !GFileSystem::getInstance().fastRawRead(m_imageCompressedAddress, 4, buffer, &realSize, false) )
      {
         _DTraceError("Can not read compressed image address value");
         free(buffer);
         return FALSE;
      }
      else
      {
         _DTraceError("!!!=> Load compressed image address at m_imageCompressedAddress: 0x%08x, value: 0x%08x", m_imageCompressedAddress, GStringUtils::getU32FromBuffer(buffer,0));
      }

      if( !GFileSystem::getInstance().fastRawRead(m_imageCompressedSizeAddress, 4, buffer, &realSize, false) )
      {
         _DTraceError("Can not read image compressed size value");
         free(buffer);
         return FALSE;
      }
      else
      {
         _DTraceError("!!!=> Load image compressed size at m_imageCompressedSizeAddress: 0x%08x, value: %d", m_imageCompressedSizeAddress, GStringUtils::getU32FromBuffer(buffer,0));
      }

      if( !GFileSystem::getInstance().fastRawRead(m_startAddress, 4, buffer, &realSize, false) )
      {
         _DTraceError("Can not read start address value");
         free(buffer);
         return FALSE;
      }
      else
      {
         _DTraceError("!!!=> Load start address at m_startAddress: 0x%08x, value: 0x%08x", m_startAddress, GStringUtils::getU32FromBuffer(buffer,0));
      }

      if( !GFileSystem::getInstance().fastRawRead(m_stackAddress, 4, buffer, &realSize, false))
      {
         _DTraceError("Can not read stack address value");
         free(buffer);
         return FALSE;
      }
      else
      {
         _DTraceError("!!!=> Load stack address value at m_stackAddress: 0x%08x, value: 0x%08x", m_stackAddress, GStringUtils::getU32FromBuffer(buffer,0));
      }

      if( !GFileSystem::getInstance().fastRawRead(m_dbgConnectedAddress, 4, buffer, &realSize, false) )
      {
         _DTraceError("Can not read dbg connected address value");
         free(buffer);
         return FALSE;
      }
      else
      {
         _DTraceError("!!!=> Load dbg connected address at m_dbgConnectedAddress: 0x%08x, value: 0x%08x", m_dbgConnectedAddress, GStringUtils::getU32FromBuffer(buffer,0));
      }
   }

   free(buffer);

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GROMManager::switchSlot()
{
   BOOL ret = FALSE;

   if( readROMInfo() == FALSE )
   {
      _DTraceError("FAILED: cannot read ROM Info");
      return ret;
   }

   if( m_activeSlot == GROM_SLOT_0 )
   {
      _DTraceError("----------- SWITCH TO SLOT 1 -----------");
      ret = activateSlot(GROM_SLOT_1);
   }
   else if( m_activeSlot == GROM_SLOT_1 )
   {
      _DTraceError("----------- SWITCH SLOT 0 -----------");
      ret = activateSlot(GROM_SLOT_0);
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GROMManager::activateSlot(const GROMSlots slotActivated)
{
   U8 buffer[32];
   U32 new_load_address;


   if( m_activeSlot != slotActivated )
   {
      _DTraceError("SWITCH SLOT %d TO %d", m_activeSlot, slotActivated);
      m_activeSlot = slotActivated;
   }

   if( slotActivated == GROM_SLOT_0 )
   {
      _DTraceError("----------- ACTIVATE SLOT 0 -----------");
      new_load_address = ROM_BASE_ADDRESS_SLOT_0;
   }
   else if( slotActivated == GROM_SLOT_1 )
   {
      _DTraceError("----------- ACTIVATE SLOT 1 -----------");
      new_load_address = ROM_BASE_ADDRESS_SLOT_1;
   }
   else
   {
      _DTraceError("FAILED: Unknow the slot");
      return FALSE;
   }

   GStringUtils::putU32ToBuffer(new_load_address, buffer, 0);
   GStringUtils::putU32ToBuffer(new_load_address + ROM_SIZE_ADDRESS_OFFSET, buffer, 4);
   GStringUtils::putU32ToBuffer(new_load_address + ROM_COMPRESSED_ADDRESS_OFFSET, buffer, 8);
   GStringUtils::putU32ToBuffer(new_load_address + ROM_COMPRESSED_SIZE_ADDRESS_OFFSET, buffer, 12);
   GStringUtils::putU32ToBuffer(new_load_address + ROM_START_ADDRESS_OFFSET, buffer, 16);
   GStringUtils::putU32ToBuffer(new_load_address + ROM_STACK_ADDRESS_OFFSET, buffer, 20);
   GStringUtils::putU32ToBuffer(new_load_address + ROM_DBG_CONNECTED_ADDRESS_OFFSET, buffer, 24);

   //!!! WARNING! - WARNING! - WARNING! - WARNING! - WARNING!
   //!!! FORSURE, STB WILL NEVER RUN AGAIN IF POWER-OFF HERE !!!
   if( !GFileSystem::getInstance().fastRawWrite(m_loadAddressAddress, 28, buffer, false) )
   {
      _DTraceError("FAILED: cannot write rom m_loadAddress");
      return FALSE;
   }
   else
   {
      _DTraceError("write rom m_loadAddress successfully from 0x%08x", m_loadAddressAddress);
   }
   //!!! THANKS TO THE GOD - WE COULD WRITE FLASH WITHOUT POWER-OFF !!!

   // read again to verify
   if( readROMInfo() == TRUE )
   {
      if( m_loadAddress == new_load_address )
      {
         _DTraceError("Verified OK, current rom id: %d, start at 0x%08x", m_activeSlot, m_loadAddress);

         // update compressed image start address = loadaddress + 28 (7 data fields above) + 128 (32 start image header0-3)
         U32 compressedImageStartAt = m_loadAddress + 7*4 + MAINAPP_SIGNATURE_LENGTH*4;

         //!!! WARNING! - WARNING! - WARNING! - WARNING! - WARNING!
         //!!! FORSURE, STB WILL NEVER RUN AGAIN IF POWER-OFF HERE !!!
         GStringUtils::putU32ToBuffer(compressedImageStartAt, buffer, 0);
         if( !GFileSystem::getInstance().fastRawWrite(m_imageCompressedAddress, 4, buffer, false) )
         {
            _DTraceError("FAILED: cannot update compressed image address");
            return FALSE;
         }
         else
         {
            _DTraceError("write compressedImageStartAt = 0x%08x at m_imageCompressedAddress = 0x%08x", compressedImageStartAt, m_imageCompressedAddress);
         }
         //!!! THANKS TO THE GOD - WE COULD WRITE FLASH WITHOUT POWER-OFF !!!
      }
      else
      {
         _DTraceError("verify FAILED: written m_loadAddress not match");
         return FALSE;
      }
   }
   else
   {
      _DTraceError("cannot read ROM info again to verify");
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
GROMSlots GROMManager::getActiveSlot()
{
   return m_activeSlot;
}

//////////////////////////////////////////////////////////////////////////////////////
GROMAppStates GROMManager::getAppState()
{
   return m_appState;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GROMManager::loadSetting(U32 address, U8* Buffer, U8 NumOfByte)
{
   _DTraceError("Requested read at ROM addr: 0x%08X, size: %08d!\n", address, NumOfByte);
#if defined(EEPROM_VERSION)
   return (BOOL)GI2C_Read( (U16)address, Buffer, NumOfByte);
#else
   return (BOOL)GFileSystem::getInstance().readSettings(address, Buffer, NumOfByte);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GROMManager::saveSetting(U32 address, U8* Buffer, U8 NumOfByte)
{
   _DTraceError("Requested write at ROM addr: 0x%08X, size: %08d!\n", address, NumOfByte);
#if defined(EEPROM_VERSION)
   return (BOOL)GI2C_Write( (U16)address, Buffer, NumOfByte);
#else
   return (BOOL)GFileSystem::getInstance().writeSettings(address, Buffer, NumOfByte);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GROMManager::dumpSettings()
{
#if defined(EEPROM_VERSION)
   return GI2C_Dump();
#else
   return FALSE;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
GROMAppStates GROMManager::detectAppState(U8* buffer, U32 bufferSize)
{
   if( buffer == NULL )
   {
      _DTraceError("Buffer is null!");
      return GROM_APP_STATE_UNKNOWN;
   }

   // Test if the app state is REGISTRATION
   int appStateFound = GStringUtils::detectSignature(buffer, bufferSize, app_state_registration);
   if( appStateFound != -1 )
   {
      _DTraceError("GROM_APP_STATE_REGISTRATION");
      return GROM_APP_STATE_REGISTRATION;
   }

   // Test if the app state is DECLARATION
   appStateFound = GStringUtils::detectSignature(buffer, bufferSize, app_state_declaration);
   if( appStateFound != -1 )
   {
      _DTraceError("GROM_APP_STATE_DECLARATION");
      return GROM_APP_STATE_DECLARATION;
   }

   // Test if the app state is MANUFACTURE
   appStateFound = GStringUtils::detectSignature(buffer, bufferSize, app_state_manufacture);
   if( appStateFound != -1 )
   {
      _DTraceError("GROM_APP_STATE_MANUFACTURE");
      return GROM_APP_STATE_MANUFACTURE;
   }

   _DTraceError("GROM_APP_STATE_UNKNOWN");
   return GROM_APP_STATE_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GROMManager::isAppValid(U8* buffer, U32 bufferSize)
{
   BOOL ret = FALSE;

   GROMAppStates appFileState = detectAppState(buffer, bufferSize);

   if( appFileState != GROM_APP_STATE_UNKNOWN )
   {
      if( (m_appState == GROM_APP_STATE_MANUFACTURE) && (appFileState == GROM_APP_STATE_MANUFACTURE ||
                                                       appFileState == GROM_APP_STATE_DECLARATION ) )
      {
         ret = TRUE;
      }
      else if( (m_appState == GROM_APP_STATE_DECLARATION) && (appFileState == GROM_APP_STATE_DECLARATION ||
                                                            appFileState == GROM_APP_STATE_REGISTRATION ) )
      {
         ret = TRUE;
      }
      else if( (m_appState == GROM_APP_STATE_REGISTRATION) && (appFileState == GROM_APP_STATE_REGISTRATION) )
      {
         ret = TRUE;
      }
      else if( (m_appState == GROM_APP_STATE_UNKNOWN) && (appFileState == GROM_APP_STATE_MANUFACTURE) )
      {
         ret = TRUE;
      }
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
