//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: APIFlashIO.cpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#include "APIFlashIO.hpp"

static FLASH_Parameters_t FLASH_Parameters[FLASH_MAX_NUMBER];
static BOOL g_isFlashInitialized[FLASH_MAX_NUMBER];
static BOOL g_apiFlashHasInstance = FALSE;
static BOOL g_isFlashNeedLockTask = TRUE;

//////////////////////////////////////////////////////////////////////////////////////
//lock task before writing flash
void APIFlashNeedLockTask(BOOL isNeedLockTask)
{
   g_isFlashNeedLockTask = isNeedLockTask;
}

//////////////////////////////////////////////////////////////////////////////////////
bool APIInitFlash(U32 flashID)
{
   U32 FLASH_SIZE = FLASH_SIZE_SPI; //DEFAULT_FLASH_ID
   if( flashID == NAND_FLASH_ID )
   {
      FLASH_SIZE = FLASH_SIZE_NAND;
   }

   if( g_apiFlashHasInstance == FALSE )
   {
      for( int i = 0; i < FLASH_MAX_NUMBER; ++i )
      {
         g_isFlashInitialized[flashID] = FALSE;
      }

      g_apiFlashHasInstance = TRUE;
   }

   if( g_isFlashInitialized[flashID] == TRUE )
   {
      return true;
   }

   _DTraceDebug("Flash not initialized yet, initializing... !\n");

   ST_ErrorCode_t ErrCode = FLASH_InitType( flashID );

   if( ErrCode == ST_NO_ERROR )
   {
      ErrCode = FLASH_GetParams(flashID, &FLASH_Parameters[flashID]);

      if( ErrCode == ST_NO_ERROR )
      {
         _DTraceDebug("Type: %u DeviceName:%s, Size: %u KBytes, Nblocks: %u, BlockSize: %u KBytes, BlockMap: %x %x %x\n",
                       FLASH_Parameters[flashID].Type,
                       FLASH_Parameters[flashID].DeviceName,
                       FLASH_Parameters[flashID].Size/1024,
                       FLASH_Parameters[flashID].NbBlocks,
                       FLASH_Parameters[flashID].Size/FLASH_Parameters[flashID].NbBlocks/1024,
                       FLASH_Parameters[flashID].BlockMap[0],
                       FLASH_Parameters[flashID].BlockMap[1],
                       FLASH_Parameters[flashID].BlockMap[2]);

         if( FLASH_Parameters[flashID].Size != 0 )
         {
            if( FLASH_Parameters[flashID].Size < (FLASH_SIZE - BLOCK_SIZE) )
            {
               _DTraceDebug("WARNING !!! The Flash Size is smaller than configuration as %d\n", FLASH_SIZE);
            }

            g_isFlashInitialized[flashID] = TRUE;
         }
      }
   }

   return (bool)(g_isFlashInitialized[flashID] == TRUE);
}

//////////////////////////////////////////////////////////////////////////////////////
bool APIFlashInfo(U32 flashId, GFlash_Info_t& flashInfo)
{
   if( !APIInitFlash(flashId) )
   {
      _DTraceDebug("Unable to probe flash devices !\n");
      return false;
   }

   // Check parameters
   if( flashId >= FLASH_MAX_NUMBER )
   {
      _DTraceDebug("Identifier of the flash is not specified or is wrong !\n");
      return false;
   }

   if( FLASH_Parameters[flashId].Size == 0 )
   {
      _DTraceDebug("Flash %d seems to be not initialized properly, please select an another flash id !\n",flashId);
      return false;
   }

   flashInfo.deviceID = flashId;
   flashInfo.DeviceName.assign(FLASH_Parameters[flashId].DeviceName);
   flashInfo.DeviceCode = FLASH_Parameters[flashId].DeviceCode;
   flashInfo.ManufacturerCode = FLASH_Parameters[flashId].ManufacturerCode;
   flashInfo.Type = FLASH_Parameters[flashId].Type;
   flashInfo.Size = FLASH_Parameters[flashId].Size;
   flashInfo.NbBlocks = FLASH_Parameters[flashId].NbBlocks;

   for( int i = 0; i < 12; i++ )
   {
      flashInfo.BlockMap[i] = FLASH_Parameters[flashId].BlockMap[i];
   }

   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
bool APIFlashRead(U32 flashId, U32 startAddress, U32 readSize,U8* buffer, U32* actualReadSize)
{
   _DTraceDebug("Flash #%d Read started: addr: 0x%08X, size: %08d!\n",flashId, startAddress, readSize);

   if( !APIInitFlash(flashId) )
   {
      _DTraceDebug("romtool: Unable to probe flash devices !\n");
      return(false);
   }

   ST_ErrorCode_t ErrCode=ST_NO_ERROR;
   U32 Address1, Address2, Size1, Size2, BlockIndex, i;
   U8* FlashBuffer;

   *actualReadSize = 0;

   // Check parameters
   if( flashId >= FLASH_MAX_NUMBER )
   {
      _DTraceDebug("Identifier of the flash is not specified or is wrong !\n");
      return(false);
   }

   if( FLASH_Parameters[flashId].Size == 0 )
   {
      _DTraceDebug("Flash %d seems to be not initialized properly, please select an another flash id !\n",flashId);
      return(false);
   }

   if( startAddress >= FLASH_Parameters[flashId].Size )
   {
      _DTraceDebug("Address specified is too big, should be smaller than the flash size which is %d bytes !\n",FLASH_Parameters[flashId].Size);
      return(false);
   }

   if( (readSize>FLASH_Parameters[flashId].Size)||((startAddress+readSize)>FLASH_Parameters[flashId].Size) )
   {
      _DTraceDebug("Size specified is too big, should be smaller than the flash size which is %d bytes !\n",FLASH_Parameters[flashId].Size);
      return(false);
   }

   if( readSize != 0 )
   {
      if( (startAddress+readSize)>FLASH_Parameters[flashId].Size )
      {
         _DTraceDebug("Size is too high, should be smaller than the flash size which is %d bytes !\n",FLASH_Parameters[flashId].Size);
         return(false);
      }
   }
   else
   {
      readSize = FLASH_Parameters[flashId].Size-startAddress;
   }

   // Read the flash
   ErrCode = FLASH_Read(flashId, (U8*)startAddress, readSize, buffer);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceDebug("Error in flash %d dump operation at @ 0x%08x !\n",flashId,startAddress);
      return(false);
   }

   *actualReadSize = readSize;

   return(true);
}

//////////////////////////////////////////////////////////////////////////////////////
bool APIFlashWrite(U32 flashId, U32 startAddress, U32 writeSize, U8* buffer)
{
   _DTraceDebug("Flash #%d Write started: addr: 0x%08X, size: %08d!\n",flashId, startAddress, writeSize);

   U32 FLASH_SIZE = FLASH_SIZE_SPI; //DEFAULT_FLASH_ID
   if( flashId == NAND_FLASH_ID )
   {
      FLASH_SIZE = FLASH_SIZE_NAND;
   }

   if( startAddress + writeSize > FLASH_SIZE )
   {
      _DTraceDebug("NOT ENOUGH FLASH MEMORY, SEE THE CONFIGURATION FOR FLASH_SIZE AT APIFlastIO.hpp");
      return false;
   }

   if( !APIInitFlash(flashId) )
   {
      _DTraceDebug("Unable to probe flash devices !\n");
      return(false);
   }

   ST_ErrorCode_t ErrCode=ST_NO_ERROR;
   U32 Address1, Address2, Size1, Size2, BlockIndex, i;
   U8* FileBuffer;
   U8* FlashBuffer;
   U32 bufferIndex = 0;

   // Check parameters
   if( flashId >= FLASH_MAX_NUMBER )
   {
      _DTraceDebug("Identifier of the flash is not specified or is wrong !\n");
      return(false);
   }

   if( FLASH_Parameters[flashId].Size == 0 )
   {
      _DTraceDebug("Flash %d seems to be not initialized properly, please select an another flash id !\n",flashId);
      return(false);
   }

   if( startAddress >= FLASH_Parameters[flashId].Size )
   {
      _DTraceDebug("Address specified is too big, should be smaller than the flash size which is %d bytes !\n",FLASH_Parameters[flashId].Size);
      return(false);
   }

   if( (writeSize>FLASH_Parameters[flashId].Size)||((startAddress+writeSize)>FLASH_Parameters[flashId].Size) )
   {
      _DTraceDebug("Size specified is too big, should be smaller than the flash size which is %d bytes !\n",FLASH_Parameters[flashId].Size);
      return(false);
   }

   if( (writeSize+startAddress)>FLASH_Parameters[flashId].Size )
   {
      _DTraceWarning("Be careful, the size %d can't fit into the flash size which is %d bytes\n",writeSize,FLASH_Parameters[flashId].Size);
      _DTraceWarning("Then programmation will be truncated to flash size but result is unexpected !\n");
      writeSize=FLASH_Parameters[flashId].Size-startAddress;
   }


   if( g_isFlashNeedLockTask == TRUE )
   {
      Task_Lock();
   }

   ErrCode = FLASH_Write(flashId, (U8*)startAddress, writeSize, buffer);

   if( ErrCode != ST_NO_ERROR )
   {
      if( g_isFlashNeedLockTask == TRUE )
      {
         Task_Unlock();
      }
      _DTraceDebug("Error in flash %d program operation at @ 0x%08x !\n",flashId,startAddress);
      return(false);
   }

   if( g_isFlashNeedLockTask == TRUE )
   {
      Task_Unlock();
   }

   // Verify the flash programming
   // Read back the flash to verify it

   _DTraceDebug(("Flash Write completed - OK\n"));

   // Return no errors
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
bool APIFastRead(U32 flashId, U32 startAddress, U32 readSize, U8* buffer, U32* actualReadSize)
{
   _DTraceDebug("Flash #%d Fast read started: addr: 0x%08X, size: %08d!\n",flashId, startAddress, readSize);

   U32 FLASH_SIZE = FLASH_SIZE_SPI; //DEFAULT_FLASH_ID
   if( flashId == NAND_FLASH_ID )
   {
      FLASH_SIZE = FLASH_SIZE_NAND;
   }

   if( (startAddress / BLOCK_SIZE) != ((startAddress+readSize-1)/ BLOCK_SIZE) )
   {
      // vung can doc KHONG nam tron trong 1 block
      return APIFlashRead(flashId, startAddress, readSize, buffer, actualReadSize);
   }
   else
   {
      // vung can doc nam tron trong 1 block
      ST_ErrorCode_t ErrCode = ST_NO_ERROR;
      ErrCode = FLASH_SectorRead(flashId, startAddress, readSize, buffer);

      if( ErrCode != ST_NO_ERROR )
      {
         _DTraceDebug("Error in flash %d dump operation at @ 0x%08x !\n",flashId,startAddress);
         return(false);
      }

      *actualReadSize = readSize;

      _DTraceDebug("Fast Read operation completed - OK, readSize = %d\n",readSize);
      return(true);
   }
}

//////////////////////////////////////////////////////////////////////////////////////

static bool isFlashUnlocked = false;

//////////////////////////////////////////////////////////////////////////////////////
bool APIFastWrite(U32 flashId, U32 startAddress, U32 writeSize, U8* buffer)
{
   _DTraceDebug("Flash #%d fast Write started: addr: 0x%08X, size: %08d!\n",flashId, startAddress, writeSize);

   U32 FLASH_SIZE = FLASH_SIZE_SPI; //DEFAULT_FLASH_ID
   if( flashId == NAND_FLASH_ID )
   {
      FLASH_SIZE = FLASH_SIZE_NAND;
   }

   if( startAddress + writeSize > FLASH_SIZE )
   {
      _DTraceDebug("NOT ENOUGH FLASH MEMORY, SEE THE CONFIGURATION FOR FLASH_SIZE AT APIFlastIO.hpp");
      return false;
   }

   // code o duoi ko chay, dung tam cai cu
   return APIFlashWrite(flashId, startAddress, writeSize, buffer);

   if( (startAddress / BLOCK_SIZE) != ((startAddress + writeSize - 1)/ BLOCK_SIZE))
   {
      // vung can doc KHONG nam tron trong 1 block
      return APIFlashWrite(flashId, startAddress, writeSize, buffer);
   }
   else
   {
      // vung can doc nam tron trong 1 block
      if( g_isFlashNeedLockTask == TRUE )
      {
         Task_Lock();
      }

      ST_ErrorCode_t ErrCode=ST_NO_ERROR;

      //ErrCode=FLASH_SectorErase(flashId,startAddress,writeSize);
      //if (ErrCode!=ST_NO_ERROR)
      //{
         //if (!isFlashUnlocked)
         //{
               ErrCode = FLASH_Unlock(flashId);
               if( ErrCode != ST_NO_ERROR )
               {
                  _DTraceDebug("%s: Unable to unlock the flash !!!\n", __FUNCTION__);
                  if( g_isFlashNeedLockTask == TRUE )
                  {
                     Task_Unlock();
                  }
                  return false;
               }

               isFlashUnlocked = true;
           //}

            ErrCode = FLASH_SectorUnlock(flashId,startAddress,writeSize);
            if( ErrCode != ST_NO_ERROR)
            {
               _DTraceDebug("%s: Unable to unlock a block at @ 0x%08x !!!\n", __FUNCTION__, startAddress);
               if( g_isFlashNeedLockTask == TRUE )
               {
                  Task_Unlock();
               }
               return false;
            }

            //ErrCode = FLASH_SectorErase(flashId,startAddress,writeSize);
            //if( ErrCode != ST_NO_ERROR )
            //{
               //_DTraceDebug("%s: Unable to erase a sector at @ 0x%08x !!!\n", __FUNCTION__, startAddress);
               //Task_Unlock();
               //return false;
            //}
         //}
         //else
         //{
            //_DTraceDebug("%s: Unable to ERASE a sector at @ 0x%08x !!!\n", __FUNCTION__, startAddress);
            //Task_Unlock();
            //return false;
         //}

         //ErrCode = FLASH_SectorErase(flashId,startAddress,writeSize);
         //if (ErrCode != ST_NO_ERROR)
         //{
         //   _DTraceDebug("%s: Unable to earse a block at @ 0x%08x !!!\n", __FUNCTION__, startAddress);
         //   Task_Unlock();
         //   return false;
         //}

      ErrCode = FLASH_SectorWrite(flashId, startAddress, writeSize, buffer);
      if( ErrCode != ST_NO_ERROR )
      {
         if( g_isFlashNeedLockTask == TRUE )
         {
            Task_Unlock();
         }
         _DTraceDebug("Error in flash %d program operation at @ 0x%08x !\n",flashId,startAddress);
         return(false);
      }

      if( g_isFlashNeedLockTask == TRUE )
      {
         Task_Unlock();
      }

      _DTraceDebug(("Flash fast Write completed - OK\n"));

      return true;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool APIFlashErase(U32 flashId, U32 startAddress, U32 eraseSize)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   U32 Address1, Address2, Size1, Size2, i, BlockIndex;

   U32 FLASH_SIZE = FLASH_SIZE_SPI; //DEFAULT_FLASH_ID
   if( flashId == NAND_FLASH_ID )
   {
      FLASH_SIZE = FLASH_SIZE_NAND;
   }

   // Check parameters
   if( flashId >= FLASH_MAX_NUMBER )
   {
      _DTraceDebug("Identifier of the flash is not specified or is wrong !\n");
      return(false);
   }

   if( FLASH_Parameters[flashId].Size == 0 )
   {
      _DTraceDebug("Flash %d seems to be not initialized properly, please select an another flash id !\n",flashId);
      return(false);
   }

   if( startAddress >= FLASH_Parameters[flashId].Size )
   {
      _DTraceDebug("Address is too high, should be smaller than the flash size which is %d bytes !\n",FLASH_Parameters[flashId].Size);
      return(false);
   }

   if( eraseSize != 0 )
   {
      if( (startAddress+eraseSize) > FLASH_Parameters[flashId].Size )
      {
         _DTraceDebug("Size is too high, should be smaller than the flash size which is %d bytes !\n",FLASH_Parameters[flashId].Size);
         return(false);
      }
   }
   else
   {
      eraseSize = FLASH_Parameters[flashId].Size-startAddress;
   }

   _DTraceDebug("Starting flash %d erase from address 0x%08x and size=%d\n",flashId,startAddress,eraseSize);

   // Look for block begin address
   for( i = 0, BlockIndex = 0, Address1 = 0, Address2 = startAddress, Size1 = Size2 = 0; i < FLASH_Parameters[flashId].NbBlocks; i++ )
   {
      while( (Address1 >= FLASH_Parameters[flashId].BlockMap[BlockIndex]) && (FLASH_Parameters[flashId].BlockMap[BlockIndex] != 0) )
      {
         BlockIndex+=2;
      }

      Size1 = FLASH_Parameters[flashId].BlockMap[BlockIndex+1];

      if( (Address1 <= Address2) && ((Address1+Size1) > Address2) )
      {
         Size2 = Size1-(Address2-Address1);
         Size2 = (Size2>eraseSize)?eraseSize:Size2;
         break;
      }

      Address1 = Address1+Size1;
   }

   // Perform the erase operation block per block
   while( eraseSize != 0 )
   {
      U32 LocalSize,LocalAddress;

      // Erase partial block or full block
      if( Size1 != Size2 )
      {
         LocalAddress = Address2;
         LocalSize = Size2;
      }
      else
      {
         LocalAddress = Address1;
         LocalSize = Size1;
      }

      // Do the erase now
      _DTraceDebug("Erasing flash %d, address=0x%08x, size=%d\n",flashId,LocalAddress,LocalSize);

      if( g_isFlashNeedLockTask == TRUE )
      {
         Task_Lock();
      }

      ErrCode = FLASH_Erase(flashId, (U8*)LocalAddress, LocalSize);
      if( ErrCode != ST_NO_ERROR )
      {
         if( g_isFlashNeedLockTask == TRUE )
         {
            Task_Unlock();
         }
         _DTraceDebug("Error in flash %d erase operation at @ 0x%08x !\n",flashId,LocalAddress);
         return(false);
      }

      if( g_isFlashNeedLockTask == TRUE )
      {
         Task_Unlock();
      }

      // Increase variables
      Address1 = Address1+Size1;
      Address2 = Address1;
      eraseSize = eraseSize-Size2;

      while( (Address1 >= FLASH_Parameters[flashId].BlockMap[BlockIndex]) && (FLASH_Parameters[flashId].BlockMap[BlockIndex]!=0) )
      {
         BlockIndex += 2;
      }

      Size1 = FLASH_Parameters[flashId].BlockMap[BlockIndex+1];

      if( eraseSize > Size1 )
      {
         Size2=Size1;
      }
      else
      {
         Size2 = eraseSize;
      }
   }

   _DTraceDebug("Blocks are fully erased and verified - OK\n");

   // Return no errors
   return(true);
}

//////////////////////////////////////////////////////////////////////////////////////
//} //API
//} //STM
//} //GBS
