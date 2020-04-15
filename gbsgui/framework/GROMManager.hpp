//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GROMManager.hpp                                                            //
//////////////////////////////////////////////////////////////////////////////////////

// There can be 2 ROM images on flash memory at the same time:
// - one image locate at first half of flash, starting at address 0x00 and WITH bootrom image at
//   the beginning. Size of bootrom part is defined in BOOTROM_SIZE macro, typically 0x0001FFE4 bytes
// - another image locate at second half of flash (8MB), starting at address 0x00400000 and WITHOUT
//   bootrom image preceeding. This allow some more free space at the end of flash to store persistent
//   data (150KB, used in GFileSystem)
//
// In this structure, rom image (excluding bootrom) size cannot exceed 3.85MB, or 4.037.017 bytes

#include "stapp_main.h"
#include "GDebug.hpp"

#ifndef GROMMANAGER_HPP_
#define GROMMANAGER_HPP_

//////////////////////////////////////////////////////////////////////////////////////
//BEGIN: ROM address define for user settings location
//////////////////////////////////////////////////////////////////////////////////////
#define EEPROM_ADDR_BASE                 0x00000004             // First 4 bytes are not used, aligned for 4
////// Begin Block #1 - Only for Flash saving 64kb //////
#define EEPROM_ADDR_CHANNEL              EEPROM_ADDR_BASE       //2 byte
////// End Block #1 - Only for Flash saving 64kb   //////

////// Begin Block #2 - Only for Flash saving 64kb //////
#define EEPROM_ADDR_VOLUME               EEPROM_ADDR_BASE + 2   //1 byte
////// End Block #2 - Only for Flash saving 64kb   //////

////// Begin Block #3 - Only for Flash saving 64kb //////
#define EEPROM_ADDR_STANDBY              EEPROM_ADDR_BASE + 3   //1 byte
////// End Block #3 - Only for Flash saving 64kb   //////

////// Begin Block #4 - Only for Flash saving 64kb //////
#define EEPROM_ADDR_ANTENPOWER           EEPROM_ADDR_BASE + 4   //1 byte
////// End Block #4 - Only for Flash saving 64kb   //////

////// Begin Block #5 - Only for Flash saving 64kb //////
#define EEPROM_ADDR_FAV                  EEPROM_ADDR_BASE + 5   //1 byte
#define EEPROM_ADDR_MUTE                 EEPROM_ADDR_BASE + 6   //1 byte
//For TV Settings
#define EEPROM_ADDR_LCN                  EEPROM_ADDR_BASE + 10  //1 byte
#define EEPROM_ADDR_FREQMODE             EEPROM_ADDR_BASE + 11  //1 byte
#define EEPROM_ADDR_CONTROLVOLUME        EEPROM_ADDR_BASE + 12  //1 byte
#define EEPROM_ADDR_DISPLAYCHANNELLIST   EEPROM_ADDR_BASE + 13  //1 byte
#define EEPROM_ADDR_AUTOSCANMODE         EEPROM_ADDR_BASE + 14  //1 byte

//For Video Settings
#define EEPROM_ADDR_VIDEOSETTING         EEPROM_ADDR_BASE + 20  //3 byte
#define EEPROM_ADDR_ASPECTRATIO          EEPROM_ADDR_BASE + 23  //1 byte
#define EEPROM_ADDR_RATIO                EEPROM_ADDR_BASE + 24  //1 byte
#define EEPROM_ADDR_STANDARD             EEPROM_ADDR_BASE + 25  //1 byte
#define EEPROM_ADDR_VTGMODE              EEPROM_ADDR_BASE + 26  //1 byte
#define EEPROM_ADDR_ERRRECOVERY          EEPROM_ADDR_BASE + 27  //1 byte
#define EEPROM_ADDR_DENCMOD              EEPROM_ADDR_BASE + 28  //1 byte

//For Display Settings
#define EEPROM_ADDR_PERDISPLAY           EEPROM_ADDR_BASE + 30  //1 byte
#define EEPROM_ADDR_TIDISPLAY            EEPROM_ADDR_BASE + 31  //1 byte
#define EEPROM_ADDR_TIMEZONE             EEPROM_ADDR_BASE + 32  //1 byte
#define EEPROM_ADDR_LNBSETTING           EEPROM_ADDR_BASE + 33  //1 byte
//#define EEPROM_ADDR_LNB22KHZ           EEPROM_ADDR_BASE + 34  //1 byte
//For Language Settings
#define EEPROM_ADDR_LANGUAGE             EEPROM_ADDR_BASE + 40  //1 byte
#define EEPROM_ADDR_AUDIOLANGUAGE        EEPROM_ADDR_BASE + 41  //1 byte
#define EEPROM_ADDR_SUBTITLE             EEPROM_ADDR_BASE + 42  //2 byte
#define EEPROM_ADDR_TELETEXT             EEPROM_ADDR_BASE + 44  //2 byte
// For Parental Control
#define EEPROM_ADDR_LOCK                 EEPROM_ADDR_BASE + 50
#define EEPROM_ADDR_LOCK_AGE             EEPROM_ADDR_LOCK       //1 byte
#define EEPROM_ADDR_LOCK_PRIVATE         EEPROM_ADDR_LOCK + 1   //1 byte
#define EEPROM_ADDR_LOCK_EDIT_CHANNEL    EEPROM_ADDR_LOCK + 2   //1 byte
#define EEPROM_ADDR_LOCK_SETTING         EEPROM_ADDR_LOCK + 3   //1 byte
#define EEPROM_ADDR_LOCK_BOX             EEPROM_ADDR_LOCK + 4   //1 byte
#define EEPROM_ADDR_LOCK_CHANNEL         EEPROM_ADDR_LOCK + 5   //1 byte
#define EEPROM_ADDR_LOCK_PASSWORD        EEPROM_ADDR_LOCK + 6   //4 bytes

// For Demo counter
#define EEPROM_ADDR_DEMO_COUNTER         EEPROM_ADDR_BASE + 70  //2 bytes
////// End Block #5 - Only for Flash saving 64kb //////

////// Begin Block #6 - Only for Flash saving 64kb //////
//For Timer
#define EEPROM_ADDR_TIMER_MANAGER_BASE   EEPROM_ADDR_BASE + 80  //13 bytes * numOfRecord
// mode         : 1 byte; // Standby - Wakeup - Record
// programNumber: 2 byte;
// period       : 1 byte;
// weekDay      : 1 byte; // [0->6] [sun -> sat]day
// dayTime      : 4 byte; // in second
// duration     : 4 byte; // in second
////// End Block #6 - Only for Flash saving 64kb  //////

//////////////////////////////////////////////////////////////////////////////////////
//END: ROM address define for user settings location
//////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
//BEGIN: ROM address defined for application location
//////////////////////////////////////////////////////////////////////////////////////
#define ROM_BASE_ADDRESS_SLOT_0   0x0001FFE4   // from the end of boot loader
#define ROM_BASE_ADDRESS_SLOT_1   0x00700000   // from the second 7 MB, without boot loader

typedef enum
{
   GROM_SLOT_0 = 0,
   GROM_SLOT_1 = 1,
   GROM_SLOT_UNKNOWN
}
GROMSlots;

typedef enum
{
   GROM_APP_STATE_UNKNOWN      = 0x00000000,
   GROM_APP_STATE_MANUFACTURE  = 0x00110000,
   GROM_APP_STATE_DECLARATION  = 0x00110011,
   GROM_APP_STATE_REGISTRATION = 0x00111100,
   GROM_APP_STATE_RESERVATION  = 0x00111111
}
GROMAppStates;

//////////////////////////////////////////////////////////////////////////////////////
//END: ROM address defined for application location
//////////////////////////////////////////////////////////////////////////////////////

//namespace GBS {
//namespace STM {
//namespace Framework {

#define _ROM_ GROMManager::getInstance()

//////////////////////////////////////////////////////////////////////////////////////
class GROMManager
{
private:
   // address of memory that contain m_loadAddress
   U32 m_loadAddressAddress;

   U32 m_loadAddress;
   U32 m_imageSizeAddress;
   U32 m_imageCompressedAddress;
   U32 m_imageCompressedSizeAddress;
   U32 m_startAddress;
   U32 m_stackAddress;
   U32 m_dbgConnectedAddress;

   // 0 - ROM at first half of flash, 1 - ROM at second half
   GROMSlots m_activeSlot;
   GROMAppStates m_appState;

   GROMAppStates detectAppState(U8* buffer, U32 bufferSize);

   GROMManager();

public:
   static GROMManager& getInstance();

   ~GROMManager();

   GROMSlots getActiveSlot();
   GROMAppStates getAppState();
   BOOL activateSlot( const GROMSlots slotActivated );
   BOOL readROMInfo(bool detail = false);
   BOOL switchSlot();

   BOOL loadSetting(U32 address, U8* Buffer, U8 NumOfByte);
   BOOL saveSetting(U32 address, U8* Buffer, U8 NumOfByte);
   BOOL dumpSettings();

   BOOL isAppValid(U8* buffer, U32 bufferSize);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif /* GROMMANAGER_HPP_ */
