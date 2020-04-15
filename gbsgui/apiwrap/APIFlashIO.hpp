//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: APIFlashIO.hpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef APIFLASHIO_HPP_
#define APIFLASHIO_HPP_

#include "stapp_main.h"
#include "GDebug.hpp"

// Flash size 32 MB
//#define FLASH_SIZE 0x01FFFFFF
// Flash size 16 MB
#define DEFAULT_FLASH_ID 0x00000001 //SPI
#define NAND_FLASH_ID    0x00000002 //NAND
#define FLASH_SIZE_SPI   0x01000000 //16MB
#define FLASH_SIZE_NAND  0x10000000 //256MB
#define BLOCK_SIZE       65536
#define BLOCK_SIZE_NAND  131072

//namespace GBS {
//namespace STM {
//namespace API {

//////////////////////////////////////////////////////////////////////////////////////
typedef struct GFlash_Info
{
   U32 deviceID;
   FLASH_Type_t Type;    // Return flash type (nor/spi/nand/mtd)
   std::string DeviceName;    // Flash device name
   U32 DeviceCode;       // Return device code of the flash
   U32 ManufacturerCode; // Return manufacturer code of the flash
   U32 Size;             // Return size of the flash, in bytes
   U32 NbBlocks;         // Return number of blocks of the flash
   U32 BlockMap[12];     // Give list of block mapping

   GFlash_Info()
   {
      deviceID = 0xFFFFFFFF;
      Type = (FLASH_Type_t) 0;
      DeviceName.assign("");
      DeviceCode = 0;
      ManufacturerCode = 0;
      Size = 0;
      NbBlocks = 0;
      for( int i = 0; i < 12; i++ )
      {
         BlockMap[i] = 0;
      }
   }
}
GFlash_Info_t;

//////////////////////////////////////////////////////////////////////////////////////

bool APIInitFlash(U32 flashId);
bool APIFlashInfo(U32 flashId, GFlash_Info_t& flashInfo);

//buffer phai duoc cap phat truoc
bool APIFlashRead(U32 flashId, U32 startAddress, U32 readSize, U8* buffer, U32* actualReadSize);
bool APIFlashWrite(U32 flashId,U32 startAddress,U32 writeSize,U8* buffer);
bool APIFastRead(U32 flashId, U32 startAddress, U32 readSize, U8* buffer, U32* actualReadSize);
bool APIFastWrite(U32 flashId, U32 startAddress, U32 writeSize, U8* buffer);

//fill with 0xff pattern
bool APIFlashErase(U32 flashId, U32 startAddress, U32 eraseSize);

//lock task before writing flash
void APIFlashNeedLockTask(BOOL isNeedLockTask);

//////////////////////////////////////////////////////////////////////////////////////
//} //API
//} //STM
//} //GBS
#endif // APIFLASHIO_HPP_
