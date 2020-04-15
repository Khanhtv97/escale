//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: ROMBlower.cpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#include "ROMBlower.hpp"
#include "GStringUtils.hpp"
#include "GROMManager.hpp"

#define ROMB_BOOTROM_BASE_ADDR   0x00000000
#define ROMB_BOOTROM_SIGNATURE   "_GBS_ST_IRD_DVBT2_BOOTOS21____"
#define ROMB_BOOTLIN_SIGNATURE   "_GBS_ST_IRD_DVBT2_BOOTLINUX___"
#define ROMB_MAINAPP_SIGNATURE   "_GBS_ST_IRD_DVBT2_APPLICATION_"
#define ROMB_MAINAPP_STSTRSIGN   "_GBS_ST_STR_DVBT2_APPLICATION_"
#define ROMB_USERDAT_SIGNATURE   "_GBS_ST_IRD_DVBT2_USER_DATA___"

#define ROMB_SIGNATURE_BLOCK_SIZE 65536 //it should be the same as signapp tool

#define ROMB_RETURN_IF_NOT_MATCH(sign1, sign2) \
{                                              \
   for( int k = 0; k < ROMB_SIGNATURE_LEN; ++k )\
   {                                           \
      if( (char)sign1[k] != (char)sign2[k] )   \
      {                                        \
         free(sign1);                          \
         free(sign2);                          \
         sign1 = NULL;                         \
         sign2 = NULL;                         \
         return GROMB_ERR_FILE_SIGN;           \
      }                                        \
   }                                           \
   _DTraceError("ROMB_SIGN_MATCHED\n");        \
}

#define ROMB_RETURN_IF_NOT_MATCH1(sign1, sign2)  \
{                                                \
   for( int k = 0; k < ROMB_SIGNATURE_LEN; ++k ) \
   {                                             \
      if( (char)sign1[k] != (char)sign2[k] )     \
      {                                          \
         free(sign1);                            \
         free(sign2);                            \
         sign1 = NULL;                           \
         sign2 = NULL;                           \
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);\
         FileBuffer = NULL;                      \
         SYS_FClose( FileHandle );               \
         return GROMB_ERR_FILE_SIGN;             \
      }                                          \
   }                                             \
   _DTraceError("ROMB_SIGN_MATCHED_1\n");        \
}

//namespace GBS {
//namespace STM {
//namespace API {
//////////////////////////////////////////////////////////////////////////////////////

BOOL calculateSignature(const char* fileName, const int fromAddress, char*& outSignature);

//////////////////////////////////////////////////////////////////////////////////////
//GROM_BurnFlash: Program routine for binary file
//////////////////////////////////////////////////////////////////////////////////////
GROMError GROM_BurnFlash(const U32 flashId,
                         const U32 fromAddress,
                         U32 sizeProgram,
                         const std::string sourceFile,
                         const GIMAGEType imageType,
                         const U32 signaturePos,
                         char* pSignature)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   FLASH_Parameters_t FLASH_Parameters[FLASH_MAX_NUMBER];

   U32 Address1;
   U32 Address2;
   U32 Size1;
   U32 Size2;
   U32 FileSize;
   U32 BlockIndex;
   U32 i;

   U8* FileBuffer;
   U8* FlashBuffer;

   void* FileHandle;

   // Check file format
   if( (imageType != GIMAGE_BOOTLOADER) &&
       (imageType != GIMAGE_BOOTLOADER_LINUX) &&
       (imageType != GIMAGE_APPLICATION) &&
       (imageType != GIMAGE_APPLICATION_STSTREAM) &&
       (imageType != GIMAGE_LOGO_SPLASH) &&
       (imageType != GIMAGE_LOGO_INFO) &&
       (imageType != GIMAGE_LOGO_MENU) &&
       (imageType != GIMAGE_USER_DATA) )
   {
      _DTraceError("GROM_BurnFlash: Incorrect file format...\"%s\" !\n", sourceFile.c_str());
      return GROMB_ERR_FILE_FORMAT;
   }

   //Check parameters
   if( flashId >= FLASH_MAX_NUMBER )
   {
      _DTraceError("GROM_BurnFlash: Identifier of the flash is not specified or is wrong !\n");
      return GROMB_ERR_FLASH_ID;
   }

   if( FLASH_GetParams(flashId, &FLASH_Parameters[flashId]) != ST_NO_ERROR )
   {
      _DTraceError("GROM_BurnFlash: Could not get Flash Params at FlashID: %d !\n", flashId);
      return GROMB_ERR_FLASH_PARAM;
   }

   if( FLASH_Parameters[flashId].Size == 0 )
   {
      _DTraceError("GROM_BurnFlash: Flash %d seems to be not initialized properly, please select an another flash id !\n", flashId);
      return GROMB_ERR_FLASH_SIZE;
   }

   if( fromAddress >= FLASH_Parameters[flashId].Size )
   {
      _DTraceError("GROM_BurnFlash: Address specified is too big, should be smaller than the flash size which is %d bytes !\n", FLASH_Parameters[flashId].Size);
      return GROMB_ERR_FLASH_SIZE;
   }

   if( (fromAddress + sizeProgram) > FLASH_Parameters[flashId].Size )
   {
      _DTraceError("GROM_BurnFlash: Size specified is too big, should be smaller than the flash size which is %d bytes !\n", FLASH_Parameters[flashId].Size);
      return GROMB_ERR_FLASH_SIZE;
   }

   char* pSignCalculated;
   if( calculateSignature((char*)sourceFile.c_str(), (int)signaturePos, pSignCalculated) != TRUE )
   {
      _DTraceError("GROM_BurnFlash: Could not calculate the signature...\"%s\" !\n", sourceFile.c_str());

      if( pSignature != NULL )
      {
         free(pSignature);
         pSignature = NULL;
      }

      if( pSignCalculated != NULL )
      {
         free(pSignCalculated);
         pSignCalculated = NULL;
      }

      return GROMB_ERR_FILE_SIGN;
   }

   _DTraceError("\n\n GROM_BurnFlash: Verify Signature OK -> Start install !\n\n");

   ROMB_RETURN_IF_NOT_MATCH(pSignCalculated, pSignature);

   _DTraceError("\n\n GROM_BurnFlash: Verify Signature OK -> Start install !\n\n");

   // Start install
   FileHandle = SYS_FOpen((char*)sourceFile.c_str(), "rb");
   if( FileHandle == NULL )
   {
      _DTraceError("GROM_BurnFlash: Cannot open data file \"%s\" !\n", sourceFile.c_str());
      return GROMB_ERR_FILE_SOURCE;
   }

   // Get size of file
   SYS_FSeek(FileHandle, 0, SEEK_END);
   FileSize = (U32)SYS_FTell(FileHandle);
   SYS_FSeek(FileHandle, 0, SEEK_SET);

   _DTraceError("GROM_BurnFlash: Data file opened, size is %d bytes, Flash size is %d bytes\n", FileSize, FLASH_Parameters[flashId].Size);

   if( sizeProgram == 0 )
   {
      sizeProgram = FileSize;

      if( (fromAddress + sizeProgram) > FLASH_Parameters[flashId].Size )
      {
         _DTraceError("GROM_BurnFlash: Be careful, the file size %d can't fit into the flash size which is %d bytes\n", FileSize,FLASH_Parameters[flashId].Size);
         _DTraceError("GROM_BurnFlash: Then programmation will be truncated to flash size but result is unexpected !\n");
         sizeProgram = FLASH_Parameters[flashId].Size - fromAddress;
      }
   }

   // Check again parameters compare to size of the file
   if( sizeProgram > FileSize )
   {
      _DTraceError("GROM_BurnFlash: Size specified is too big, should be smaller than the file size which is %d bytes !\n", FileSize);
      SYS_FClose(FileHandle);
      return GROMB_ERR_FILE_SIZE;
   }

   // Try to find first block index in the flash
   BlockIndex = 0;
   Address1 = 0;
   Size1 = 0;
   Size2 = 0;

   // Bypass the file header if any
   if( imageType == GIMAGE_BOOTLOADER ||
       imageType == GIMAGE_BOOTLOADER_LINUX ||
       imageType == GIMAGE_USER_DATA )
   {
      U32 byPassSize = ROMB_SIGNATURE_LEN*2;
      FileBuffer = (U8*)SYS_Memory_Allocate(SYS_MEMORY_NCACHED, byPassSize, 256);
      if( FileBuffer == NULL )
      {
         _DTraceError("GIMAGE_BOOTLOADER_LINUX: Size of block %d bytes is too big to fit in memory !\n", byPassSize);
         SYS_FClose(FileHandle);
         return GROMB_ERR_FILE_FORMAT;
      }

      if( SYS_FRead(FileBuffer, 1, byPassSize, FileHandle) != byPassSize )
      {
         _DTraceError("GIMAGE_BOOTLOADER_LINUX Error_SYS_FRead");
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
         FileBuffer = NULL;
         SYS_FClose(FileHandle);
         return GROMB_ERR_FILE_FORMAT;
      }
      // Free buffer
      SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
      FileBuffer = NULL;
      sizeProgram -= byPassSize;
      FileSize -= byPassSize;
   }
   // Bypass the file header if any

   // Detect start address
   if( imageType == GIMAGE_BOOTLOADER || imageType == GIMAGE_BOOTLOADER_LINUX )
   {
      Address2 = (U32)ROMB_BOOTROM_BASE_ADDR;
   }
   else
   {
      Address2 = (U32)fromAddress;
   }
   // Detect start address

   for( i = 0; i < FLASH_Parameters[flashId].NbBlocks; i++ )
   {
      while( (Address1 >= FLASH_Parameters[flashId].BlockMap[BlockIndex]) && (FLASH_Parameters[flashId].BlockMap[BlockIndex] != 0) )
      {
         BlockIndex += 2;
      }

      Size1 = FLASH_Parameters[flashId].BlockMap[BlockIndex+1];

      if( (Address1 <= Address2) && ((Address1 + Size1) > Address2) )
      {
         Size2 = Size1-(Address2-Address1);
         Size2 = (Size2 > sizeProgram) ? sizeProgram : Size2;
         break;
      }

      Address1 = Address1 + Size1;
   }

   // Verify if we have a contiguous block valid to burn the region
   if( FLASH_Parameters[flashId].Type == FLASH_NAND )
   {
      U32 Address1_Saved = Address1;
      U32 j,k;
      BOOL ContiguousRegionFound = FALSE;
      BOOL BadBlockIdentified = FALSE;

      // Look for a contiguous block regions without bad block to burn the flash
      while( ContiguousRegionFound == FALSE )
      {
         j = Address1;
         k = sizeProgram;
         BadBlockIdentified = FALSE;

         // Look if contiguous zone is valid
         while( (k != 0) && (BadBlockIdentified == FALSE) )
         {
            if( k > Size1 )
            {
               k = k - Size1;
            }
            else
            {
               k = 0;
            }

            if( FLASH_SectorErase(flashId, j, Size1) != ST_NO_ERROR )
            {
               _DTraceError("GROM_BurnFlash: Block at @ 0x%08x is a bad block on this flash, looking for a contiguous memory zone to burn the image !\n",j);
               BadBlockIdentified = TRUE;
               break;
            }

            j = j + Size1;
         }

         // If no bad block identified, contiguous zone is found
         if( BadBlockIdentified == FALSE )
         {
            ContiguousRegionFound = TRUE;
         }
         // Otherwise, try next valid block
         else
         {
            Address1 = j + Size1;
         }

         // We limit the contiguous region test to 512 block due to NAND IP controller specifications (see ADCS7864584)
         if( ContiguousRegionFound == FALSE )
         {
            if( ((Address1/Size1) > 512) || ((Address1/Size1) >= FLASH_Parameters[flashId].NbBlocks) )
            {
               _DTraceError("GROM_BurnFlash: Unable to burn this file as there are some bad block identified !\n");
               SYS_FClose( FileHandle );
               return GROMB_ERR_FILE_BAD_BLOCK;
            }
         }
      }

      if( Address1_Saved != Address1 )
      {
         _DTraceError("GROM_BurnFlash: Good contiguous blocks found at block @ 0x%08x, starting programming from this address !\n",Address1);
      }
   }

   // Perform the write operation block per block
   while( sizeProgram != 0 )
   {
      U32 LocalAddress;
      U32 LocalSize;

      // Program partial block or full block
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

      //Allocate memory to read this block
      FileBuffer = (U8*)SYS_Memory_Allocate(SYS_MEMORY_NCACHED, LocalSize, 256);

      if( FileBuffer == NULL )
      {
         _DTraceError("GROM_BurnFlash: Size of block %d bytes is too big to fit in memory !\n", LocalSize);
         SYS_FClose(FileHandle);
         return GROMB_ERR_FILE_BAD_BLOCK;
      }

      ROMB_RETURN_IF_NOT_MATCH1(pSignCalculated, pSignature);

      // Read the file
      if( SYS_FRead(FileBuffer, 1, LocalSize, FileHandle) != LocalSize )
      {
         _DTraceError("GROM_BurnFlash: Unable to read %d bytes into the \"%s\" file !\n", LocalSize, sourceFile.c_str());
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
         FileBuffer = NULL;
         SYS_FClose(FileHandle);
         return GROMB_ERR_FILE_BAD_BLOCK;
      }

      // Program the flash
      Task_Lock();

      _DTraceError("GROM_BurnFlash: Programming flash %d, address=0x%08x, size=%d\n", flashId, LocalAddress, LocalSize);

      ErrCode = FLASH_Write(flashId, (U8*)LocalAddress, LocalSize, FileBuffer);

      if( ErrCode != ST_NO_ERROR )
      {
         Task_Unlock();
         _DTraceError("GROM_BurnFlash: Error in flash %d program operation at @ 0x%08x !\n", flashId, LocalAddress);
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
         FileBuffer = NULL;
         SYS_FClose( FileHandle );
         return GROMB_ERR_MEMORY;
      }

      Task_Unlock();

      // Verify the flash programming

      // Allocate a tmp buffer
      FlashBuffer = (U8*)SYS_Memory_Allocate(SYS_MEMORY_NCACHED, LocalSize, 256);

      if( FlashBuffer == NULL )
      {
         _DTraceError("GROM_BurnFlash: Unable to allocate %d bytes memory to verify flash programming !\n", LocalSize);
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
         FileBuffer = NULL;
         SYS_FClose(FileHandle);
         return GROMB_ERR_MEMORY;
      }

      // Read back the flash to verify it
      ErrCode = FLASH_Read(flashId, (U8*)LocalAddress, LocalSize, FlashBuffer);

      if( ErrCode != ST_NO_ERROR )
      {
         _DTraceError("GROM_BurnFlash: Unable to read flash %d at @ 0x%08x and size = %d to compare flash content !\n", flashId, LocalAddress, LocalSize);
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FlashBuffer);
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer );
         FlashBuffer = NULL;
         FileBuffer = NULL;
         SYS_FClose(FileHandle);
         return GROMB_ERR_MEMORY;
      }

      // Compare the flash byte per byte
      for( i = 0; i < LocalSize; i++ )
      {
         if( FlashBuffer[i] != FileBuffer[i] )
         {
            _DTraceError("GROM_BurnFlash: Flash %d program/compare operation has failed at @ 0x%08x and size = %d, byte at offset 0x%08x is corrupted !\n",flashId,LocalAddress,LocalSize,i);
            SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer );
            SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FlashBuffer);
            FileBuffer = NULL;
            FlashBuffer = NULL;
            SYS_FClose(FileHandle);
            return GROMB_ERR_MEMORY;
         }
      }

      // Free tmp buffer
      SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FlashBuffer);
      FlashBuffer = NULL;

      // Free buffer
      SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
      FileBuffer = NULL;

      // Increase variables
      /* ------------------ */
      Address1 = Address1 + Size1;
      Address2 = Address1;

      sizeProgram = sizeProgram - Size2;

      while( (Address1 >= FLASH_Parameters[flashId].BlockMap[BlockIndex]) && (FLASH_Parameters[flashId].BlockMap[BlockIndex] !=0) )
      {
         BlockIndex += 2;
      }

      Size1 = FLASH_Parameters[flashId].BlockMap[BlockIndex+1];

      if( sizeProgram > Size1 )
      {
         Size2 = Size1;
      }
      else
      {
         Size2 = sizeProgram;
      }
   }

   // Close the file
   SYS_FClose(FileHandle);

   ROMB_RETURN_IF_NOT_MATCH(pSignature, pSignCalculated);

   if( pSignature != NULL )
   {
      free(pSignature);
      pSignature = NULL;
   }

   if( pSignCalculated != NULL )
   {
      free(pSignCalculated);
      pSignCalculated = NULL;
   }

   _DTraceError("GROM_BurnFlash: Program operation completed - OK\n");

   //Return no errors
   GROMError retNoError = GROMB_ERR_NONE;
   if( imageType == GIMAGE_BOOTLOADER )
   {
      retNoError = GROMB_ERR_NONE_BLD;
   }
   else if( imageType == GIMAGE_BOOTLOADER_LINUX )
   {
      retNoError = GROMB_ERR_NONE_BLD_LINUX;
   }

   return retNoError;
}

//////////////////////////////////////////////////////////////////////////////////////
//GROM_DumpFlash: Dump routine to read flash to a binary fyle
//////////////////////////////////////////////////////////////////////////////////////
GROMError GROM_DumpFlash(const U32 flashId, const U32 fromAddress, U32 sizeDump, const std::string destinationFile)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   FLASH_Parameters_t FLASH_Parameters[FLASH_MAX_NUMBER];

   U32 Address1;
   U32 Address2;
   U32 Size1;
   U32 Size2;
   U32 BlockIndex;
   U32 i;

   U8* FlashBuffer;
   void* FileHandle;

   _DTraceError("GROM_DumpFlash: Dump from: %d, size: %d", fromAddress, sizeDump);

   // Check parameters
   if( flashId >= FLASH_MAX_NUMBER )
   {
      _DTraceError("GROM_DumpFlash: Identifier of the flash is not specified or is wrong !\n");
      return GROMB_ERR_FLASH_ID;
   }

   if( FLASH_GetParams(flashId, &FLASH_Parameters[flashId]) != ST_NO_ERROR )
   {
      _DTraceError("GROM_DumpFlash: Could not get Flash Params at FlashID: %d !\n", flashId);
      return GROMB_ERR_FLASH_PARAM;
   }

   if( FLASH_Parameters[flashId].Size == 0 )
   {
      _DTraceError("GROM_DumpFlash: Flash %d seems to be not initialized properly, please select an another flash id !\n", flashId);
      return GROMB_ERR_FLASH_SIZE;
   }

   if( fromAddress >= FLASH_Parameters[flashId].Size )
   {
      _DTraceError("GROM_DumpFlash: Address specified is too big, should be smaller than the flash size which is %d bytes !\n", FLASH_Parameters[flashId].Size);
      return GROMB_ERR_FLASH_SIZE;
   }

   if( (sizeDump > FLASH_Parameters[flashId].Size) || ((fromAddress + sizeDump) > FLASH_Parameters[flashId].Size) )
   {
      _DTraceError("GROM_DumpFlash: Size specified is too big, should be smaller than the flash size which is %d bytes !\n", FLASH_Parameters[flashId].Size);
      return GROMB_ERR_FLASH_SIZE;
   }

   if( sizeDump != 0 )
   {
      if( (fromAddress + sizeDump) > FLASH_Parameters[flashId].Size )
      {
         _DTraceError("GROM_DumpFlash: Size is too high, should be smaller than the flash size which is %d bytes !\n", FLASH_Parameters[flashId].Size);
         return GROMB_ERR_FLASH_SIZE;
      }
   }
   else
   {
      sizeDump = FLASH_Parameters[flashId].Size - fromAddress;
   }

   // Try to open the file
   FileHandle = SYS_FOpen((char*)destinationFile.c_str(), "wb");
   if( FileHandle == NULL )
   {
      _DTraceError("GROM_DumpFlash: Cannot open data file \"%s\" !\n", destinationFile.c_str());
      return GROMB_ERR_FILE_DEST;
   }

   // Try to find first block index in the flash
   BlockIndex = 0;
   Address1 = 0;
   Address2 = (U32)fromAddress;
   Size1 = 0;
   Size2 = 0;

   for( i = 0; i < FLASH_Parameters[flashId].NbBlocks; i++ )
   {
      while( (Address1 >= FLASH_Parameters[flashId].BlockMap[BlockIndex]) && (FLASH_Parameters[flashId].BlockMap[BlockIndex] != 0))
      {
         BlockIndex += 2;
      }

      Size1 = FLASH_Parameters[flashId].BlockMap[BlockIndex+1];

      if( (Address1 <= Address2) && ((Address1 + Size1) > Address2) )
      {
         Size2 = Size1 - (Address2 - Address1);
         Size2 = (Size2 > sizeDump) ? sizeDump : Size2;
         break;
      }

      Address1 = Address1 + Size1;
   }

   // Perform the dump operation block per block
   while( sizeDump != 0 )
   {
      U32 LocalAddress;
      U32 LocalSize;

      // Program partial block or full block
      if (Size1!=Size2)
      {
         LocalAddress = Address2;
         LocalSize = Size2;
      }
      else
      {
         LocalAddress = Address1;
         LocalSize = Size1;
      }

      // Allocate memory to read this block
      _DTraceError("GROM_DumpFlash: Dumping flash %d, address=0x%08x, size=%d\n", flashId, LocalAddress, LocalSize);

      FlashBuffer = (U8*)SYS_Memory_Allocate(SYS_MEMORY_NCACHED, LocalSize, 256);

      if( FlashBuffer == NULL )
      {
         _DTraceError("GROM_DumpFlash: Size of block %d bytes is too big to fit in memory !\n", LocalSize);
         SYS_FClose(FileHandle);
         return GROMB_ERR_MEMORY;
      }

      // Read the flash
      ErrCode = FLASH_Read(flashId, (U8*)LocalAddress, LocalSize, FlashBuffer);

      if( ErrCode != ST_NO_ERROR )
      {
         _DTraceError("GROM_DumpFlash: Error in flash %d dump operation at @ 0x%08x !\n", flashId, LocalAddress);
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FlashBuffer);
         FlashBuffer = NULL;
         SYS_FClose(FileHandle);
         return GROMB_ERR_MEMORY;
      }

      // Write to the file
      if( SYS_FWrite(FlashBuffer, 1, LocalSize, FileHandle) != LocalSize )
      {
         _DTraceError("GROM_DumpFlash: Unable to write %d bytes into the \"%s\" file !\n", LocalSize, destinationFile.c_str());
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FlashBuffer);
         FlashBuffer = NULL;
         SYS_FClose(FileHandle);
         return GROMB_ERR_MEMORY;
      }

      // Free buffer
      SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FlashBuffer);
      FlashBuffer = NULL;

      // Increase variables
      Address1 = Address1 + Size1;
      Address2 = Address1;
      sizeDump = sizeDump - Size2;

      while( (Address1 >= FLASH_Parameters[flashId].BlockMap[BlockIndex]) && (FLASH_Parameters[flashId].BlockMap[BlockIndex] != 0) )
      {
         BlockIndex += 2;
      }

      Size1 = FLASH_Parameters[flashId].BlockMap[BlockIndex+1];

      if( sizeDump > Size1 )
      {
         Size2 = Size1;
      }
      else
      {
         Size2 = sizeDump;
      }
   }

   // Close the file
   SYS_FClose(FileHandle);

   _DTraceError("GROM_DumpFlash: Dump operation completed - OK\n");

   // Return no errors
   return GROMB_ERR_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////
GIMAGEType GROM_DetectImageType(const std::string sourceFile, int& signaturePos, char*& pSignature)
{
   GIMAGEType result = GIMAGE_UNKNOWN;
   U8* FileBuffer;
   void* FileHandle;

   // Try to open the file
   FileHandle = SYS_FOpen( (char*)sourceFile.c_str(), "rb");
   if( FileHandle == NULL )
   {
      _DTraceError("DetectImage: Cannot open data file \"%s\" !\n", (char*)sourceFile.c_str());
      return result;
   }

   // Get size of file
   SYS_FSeek(FileHandle, 0, SEEK_END);
   U32 FileSize = (U32)SYS_FTell(FileHandle);
   SYS_FSeek(FileHandle, 0, SEEK_SET);

   // Read the file header
   U32 readSize = 1024;

   FileBuffer = (U8*)SYS_Memory_Allocate(SYS_MEMORY_NCACHED, readSize, 256);
   if( FileBuffer == NULL )
   {
      _DTraceError("GROM_BurnFlash: Size of block %d bytes is too big to fit in memory !\n", readSize);
      SYS_FClose(FileHandle);
      return result;
   }

   if( SYS_FRead(FileBuffer, 1, readSize, FileHandle) != readSize )
   {
      _DTraceError("DetectImage: Unable to read %d bytes into the \"%s\" file !\n", readSize, (char*)sourceFile.c_str());
      SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
      FileBuffer = NULL;
      SYS_FClose(FileHandle);
      return result;
    }

   if( (signaturePos = GStringUtils::detectSignature(FileBuffer, readSize, ROMB_BOOTROM_SIGNATURE)) != -1 )
   {
      _DTraceError("DetectImage: Found IMAGE_BOOTLOADER\n");
      //BOOTROM should not be greater than 128kb
      if( (FileSize - (signaturePos + ROMB_SIGNATURE_LEN)) <= 128 * 1024 )
      {
         _DTraceError("DetectImage: IMAGE_BOOTLOADER\n");

         pSignature = new char[ROMB_SIGNATURE_LEN + 1];
         signaturePos -= ROMB_SIGNATURE_LEN;

         for( int j = 0; j < ROMB_SIGNATURE_LEN; j++)
         {
            pSignature[j] = (char)(FileBuffer[signaturePos + j]);
         }
         pSignature[ROMB_SIGNATURE_LEN] = '\0';

         result = GIMAGE_BOOTLOADER;
      }
      else
      {
         _DTraceError("DetectImage: BUT the image_compressed_size is invalid !!! \n");
      }
   }
   else if( (signaturePos = GStringUtils::detectSignature(FileBuffer, readSize, ROMB_BOOTLIN_SIGNATURE)) != -1)
   {
      _DTraceError("DetectImage: Found IMAGE_BOOTLOADER_LINUX\n");
      //UBOOT LINUX should not be greater than 6MB
      if( (FileSize - (signaturePos + ROMB_SIGNATURE_LEN)) <= 6*1024*1024 ) //6MB
      {
         _DTraceError("DetectImage: IMAGE_BOOTLOADER_LINUX\n");
         pSignature = new char[ROMB_SIGNATURE_LEN + 1];
         signaturePos -= ROMB_SIGNATURE_LEN;

         for( int j = 0; j < ROMB_SIGNATURE_LEN; j++)
         {
            pSignature[j] = (char)(FileBuffer[signaturePos + j]);
         }
         pSignature[ROMB_SIGNATURE_LEN] = '\0';

         result = GIMAGE_BOOTLOADER_LINUX;
      }
      else
      {
         _DTraceError("DetectImage: BUT the image_compressed_size is invalid !!! \n");
      }
   }
   else if( (signaturePos = GStringUtils::detectSignature(FileBuffer, readSize, ROMB_USERDAT_SIGNATURE)) != -1)
   {
      _DTraceError("DetectImage: Found IMAGE_USER_DATA \n");
      //UBOOT LINUX should not be greater than 6MB
      if( (FileSize - (signaturePos + ROMB_SIGNATURE_LEN)) <= 1024*1024+1024 ) //1.1MB
      {
         _DTraceError("DetectImage: IMAGE_USER_DATA\n");
         pSignature = new char[ROMB_SIGNATURE_LEN + 1];
         signaturePos -= ROMB_SIGNATURE_LEN;

         for( int j = 0; j < ROMB_SIGNATURE_LEN; j++)
         {
            pSignature[j] = (char)(FileBuffer[signaturePos + j]);
         }
         pSignature[ROMB_SIGNATURE_LEN] = '\0';

         result = GIMAGE_USER_DATA;
      }
      else
      {
         _DTraceError("DetectImage: BUT the image_compressed_size is invalid !!! \n");
      }
   }
   else if( ((signaturePos = GStringUtils::detectSignature(FileBuffer, readSize, ROMB_MAINAPP_SIGNATURE)) != -1) ||
            ((signaturePos = GStringUtils::detectSignature(FileBuffer, readSize, ROMB_MAINAPP_STSTRSIGN)) != -1) )
   {

      bool isSTStreamSupportedApp = false;
      if( GStringUtils::detectSignature(FileBuffer, readSize, ROMB_MAINAPP_STSTRSIGN) != -1 )
      {
         isSTStreamSupportedApp = true;
      }

      _DTraceError("DetectImage: Found IMAGE_APPLICATION\n");
      U32 image_compressed_size = GStringUtils::getU32FromBuffer(FileBuffer, 12);

      //Adjust final size if odd
      if( (image_compressed_size & 0x3) != 0 )
      {
         image_compressed_size += (4 - (image_compressed_size & 0x3));
      }

      if( image_compressed_size == (FileSize - (signaturePos + 32 * 3)) )
      {
         _DTraceError("DetectImage: IMAGE_APPLICATION\n");
         BOOL isValid = GROMManager::getInstance().isAppValid(FileBuffer, readSize);
         if( isValid == TRUE )
         {
            pSignature = new char[ROMB_SIGNATURE_LEN + 1];
            signaturePos -= ROMB_SIGNATURE_LEN;

            for( int j = 0; j < ROMB_SIGNATURE_LEN; j++)
            {
               pSignature[j] = (char)(FileBuffer[signaturePos + j]);
            }
            pSignature[ROMB_SIGNATURE_LEN] = '\0';

            if( isSTStreamSupportedApp )
            {
               result = GIMAGE_APPLICATION_STSTREAM;
            }
            else
            {
               result = GIMAGE_APPLICATION;
            }
         }
         else
         {
            _DTraceError("DetectImage: FAILED !!! - Image application is not valid\n");
            result = GIMAGE_APPLICATION_INVALID;
         }
      }
      else
      {
         _DTraceError("DetectImage: FAILED !!! - Image compressed size: 0x%08x (File size: 0x%08x)\n", image_compressed_size, (FileSize - (signaturePos + 32 * 4)));
      }
   }
   else
   {
      signaturePos = GStringUtils::detectSignature(FileBuffer, readSize, ROMB_LOGOSPLASH_SIGNATURE);
      GIMAGEType type = GIMAGE_LOGO_SPLASH;

      if( signaturePos == -1 )
      {
         signaturePos = GStringUtils::detectSignature(FileBuffer, readSize, ROMB_LOGOINFO_SIGNATURE);
         type = GIMAGE_LOGO_INFO;
      }

      if( signaturePos == -1 )
      {
         signaturePos = GStringUtils::detectSignature(FileBuffer, readSize, ROMB_LOGOMENU_SIGNATURE);
         type = GIMAGE_LOGO_MENU;
      }

      if( signaturePos != -1 && signaturePos >= ROMB_SIGNATURE_LEN )
      {
         _DTraceError("Image detected: LOGO - Type = %u", type);
         result = type;

         signaturePos -= ROMB_SIGNATURE_LEN;
         pSignature = new char[ROMB_SIGNATURE_LEN + 1];

         for( int i = 0; i < ROMB_SIGNATURE_LEN; i++ )
         {
            pSignature[i] = FileBuffer[signaturePos + i];
         }

         pSignature[ROMB_SIGNATURE_LEN] = '\0';
      }
      else
      {
         _DTraceError("!!! *ERROR*: Can't find any signature !!!");
      }
   }

   SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
   FileBuffer = NULL;
   SYS_FClose(FileHandle);

   return result;
}

//////////////////////////////////////////////////////////////////////////////////////
//GROM_DumpFlash: sign on file
GROMError GROM_SignOnFile(const std::string sourceFile, GIMAGEType fileType)
{
   U8* FileBuffer = NULL;
   void* FileHandleRead = NULL;
   void* FileHandleWrite = NULL;
   U32 blockSize = 0;
   U32 totalRead = 0;
   U32 FileSize = 0;
   std::string tempFile = sourceFile;
   std::size_t extPos = sourceFile.find(".");
   if( extPos != std::string::npos )
   {
      tempFile = sourceFile.substr(0, extPos);
   }

   tempFile.append(".tmp");

   /////////////////////////////
   // Begin create temp file
   /////////////////////////////
   FileHandleWrite = SYS_FOpen((char*)tempFile.c_str(), "wb");
   if( FileHandleWrite == NULL )
   {
      _DTraceError("Cannot create temp file \"%s\" !\n", tempFile.c_str());
      return GROMB_ERR_FILE_DEST;
   }

   blockSize = ROMB_SIGNATURE_LEN *2;
   FileBuffer = (U8*)SYS_Memory_Allocate(SYS_MEMORY_NCACHED, blockSize, 256);
   if( FileBuffer == NULL )
   {
      _DTraceError("Size of block %d bytes is too big to fit in memory !\n", blockSize);
      SYS_FClose(FileHandleWrite);
      return GROMB_ERR_MEMORY;
   }

   memset((U8*)FileBuffer, 0, blockSize);
   char* pCharHeader = NULL;
   if( fileType == GIMAGE_USER_DATA )
   {
      pCharHeader = ROMB_USERDAT_SIGNATURE;
      for(int i = 0; i < strlen(pCharHeader); ++i)
      {
         FileBuffer[ROMB_SIGNATURE_LEN + i] = (U8)pCharHeader[i];
      }
   }

   if( SYS_FWrite(FileBuffer, 1, blockSize, FileHandleWrite) != blockSize )
   {
      _DTraceError("Unable to write %d bytes into the \"%s\" file !\n", blockSize, tempFile.c_str());
      SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
      FileBuffer = NULL;
      SYS_FClose(FileHandleWrite);
      SYS_FRemove(tempFile.c_str());
      return GROMB_ERR_FILE_DEST;
   }

   SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
   FileBuffer = NULL;
   // finish writing header

   FileHandleRead = SYS_FOpen( sourceFile.c_str(), "rb");
   if( FileHandleRead == NULL )
   {
      _DTraceError("Cannot open file \"%s\" !\n", sourceFile.c_str());
      SYS_FClose(FileHandleWrite);
      SYS_FRemove(tempFile.c_str());
      return GROMB_ERR_FILE_DEST;
   }

   // Get size of file
   SYS_FSeek(FileHandleRead, 0, SEEK_END);
   FileSize = (U32)SYS_FTell(FileHandleRead);
   SYS_FSeek(FileHandleRead, 0, SEEK_SET);

   totalRead = 0;
   blockSize = 0;
   while( totalRead < FileSize )
   {
      if( (FileSize - totalRead) > ROMB_SIGNATURE_BLOCK_SIZE)
      {
         blockSize = ROMB_SIGNATURE_BLOCK_SIZE;
      }
      else
      {
         blockSize = FileSize - totalRead;
      }

      FileBuffer = (U8*)SYS_Memory_Allocate(SYS_MEMORY_NCACHED, blockSize, 256);
      if( FileBuffer == NULL )
      {
         _DTraceError("Size of block %d bytes is too big to fit in memory !\n", blockSize);
         SYS_FClose(FileHandleWrite);
         SYS_FClose(FileHandleRead);
         SYS_FRemove(tempFile.c_str());
         return GROMB_ERR_MEMORY;
      }

      if( SYS_FRead(FileBuffer, 1, blockSize, FileHandleRead) != blockSize )
      {
         _DTraceError("Unable to read %d bytes into the \"%s\" file !\n", blockSize, sourceFile.c_str());
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
         FileBuffer = NULL;
         SYS_FClose(FileHandleWrite);
         SYS_FClose(FileHandleRead);
         SYS_FRemove(tempFile.c_str());
         return GROMB_ERR_FILE_DEST;
      }

      if( SYS_FWrite(FileBuffer, 1, blockSize, FileHandleWrite) != blockSize )
      {
         _DTraceError("Unable to write %d bytes into the \"%s\" file !\n", blockSize, tempFile.c_str());
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
         FileBuffer = NULL;
         SYS_FClose(FileHandleWrite);
         SYS_FClose(FileHandleRead);
         SYS_FRemove(tempFile.c_str());
         return GROMB_ERR_FILE_DEST;
      }

      totalRead += blockSize;
      SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
      FileBuffer = NULL;
   }

   SYS_FClose(FileHandleRead);
   SYS_FClose(FileHandleWrite);
   SYS_FRemove(sourceFile.c_str());
   /////////////////////////////
   // Finish create temp file
   /////////////////////////////


   /////////////////////////////
   // Calculate signature on temp file
   /////////////////////////////
   char* pSignCalculated = NULL;
   if( calculateSignature((char*)tempFile.c_str(), (int)ROMB_SIGNATURE_LEN, pSignCalculated) != TRUE )
   {
      _DTraceError("Could not calculateSignature !!!");
      if( pSignCalculated != NULL )
      {
         free(pSignCalculated);
         pSignCalculated = NULL;
      }

      return GROMB_ERR_FILE_SIGN;
   }
   /////////////////////////////
   // Calculate signature on temp file
   /////////////////////////////

   /////////////////////////////
   // Begin create signed file
   /////////////////////////////
   FileHandleWrite = SYS_FOpen((char*)sourceFile.c_str(), "wb");
   FileHandleRead = SYS_FOpen((char*)tempFile.c_str(), "rb");
   if( FileHandleWrite == NULL || FileHandleRead == NULL )
   {
      _DTraceError("FileHandleWrite == NULL or FileHandleRead == NULL !!!");
      SYS_FClose(FileHandleRead);
      SYS_FClose(FileHandleWrite);
      if( pSignCalculated != NULL )
      {
         free(pSignCalculated);
         pSignCalculated = NULL;
      }
      return GROMB_ERR_FILE_DEST;
   }

   // Get size of file
   SYS_FSeek(FileHandleRead, 0, SEEK_END);
   FileSize = (U32)SYS_FTell(FileHandleRead);
   SYS_FSeek(FileHandleRead, 0, SEEK_SET);

   totalRead = 0;
   blockSize = 0;
   while( totalRead < FileSize )
   {
      if( (FileSize - totalRead) > ROMB_SIGNATURE_BLOCK_SIZE)
      {
         blockSize = ROMB_SIGNATURE_BLOCK_SIZE;
      }
      else
      {
         blockSize = FileSize - totalRead;
      }

      FileBuffer = (U8*)SYS_Memory_Allocate(SYS_MEMORY_NCACHED, blockSize, 256);
      if( FileBuffer == NULL )
      {
         _DTraceError("Size of block %d bytes is too big to fit in memory !\n", blockSize);
         SYS_FClose(FileHandleWrite);
         SYS_FClose(FileHandleRead);
         SYS_FRemove(tempFile.c_str());
         if( pSignCalculated != NULL )
         {
            free(pSignCalculated);
            pSignCalculated = NULL;
         }
         return GROMB_ERR_MEMORY;
      }

      if( SYS_FRead(FileBuffer, 1, blockSize, FileHandleRead) != blockSize )
      {
         _DTraceError("Unable to read %d bytes into the \"%s\" file !\n", blockSize, tempFile.c_str());
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
         FileBuffer = NULL;
         SYS_FClose(FileHandleWrite);
         SYS_FClose(FileHandleRead);
         SYS_FRemove(tempFile.c_str());
         if( pSignCalculated != NULL )
         {
            free(pSignCalculated);
            pSignCalculated = NULL;
         }
         return GROMB_ERR_FILE_DEST;
      }

      if( totalRead == 0 && pSignCalculated != NULL )
      {
         for(int i = 0; i < ROMB_SIGNATURE_LEN; ++i )
         {
            FileBuffer[i] = pSignCalculated[i];
         }
         free(pSignCalculated);
         pSignCalculated = NULL;
      }

      if( SYS_FWrite(FileBuffer, 1, blockSize, FileHandleWrite) != blockSize )
      {
         _DTraceError("Unable to write %d bytes into the \"%s\" file !\n", blockSize, tempFile.c_str());
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
         FileBuffer = NULL;
         SYS_FClose(FileHandleWrite);
         SYS_FClose(FileHandleRead);
         SYS_FRemove(tempFile.c_str());
         return GROMB_ERR_FILE_DEST;
      }

      totalRead += blockSize;
      SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
      FileBuffer = NULL;
   }
   /////////////////////////////
   // Finish create signed file
   /////////////////////////////
   SYS_FClose(FileHandleWrite);
   SYS_FClose(FileHandleRead);
   SYS_FRemove(tempFile.c_str());

   return GROMB_ERR_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL calculateSignature(const char* fileName, const int fromAddress, char*& outSignature)
{
   BOOL result = FALSE;

   U8* FileBuffer;
   void* FileHandle;

   // Try to open the file
   FileHandle = SYS_FOpen( fileName, "rb");
   if( FileHandle == NULL )
   {
      _DTraceError("calculateSignature: Cannot open data file \"%s\" !\n", fileName);
      return result;
   }

   // Get size of file
   SYS_FSeek(FileHandle, 0, SEEK_END);
   U32 FileSize = (U32)SYS_FTell(FileHandle);
   SYS_FSeek(FileHandle, 0, SEEK_SET);

   // Bypass the file header
   U32 blockSize = (U32)fromAddress;

   FileBuffer = (U8*)SYS_Memory_Allocate(SYS_MEMORY_NCACHED, blockSize, 256);
   if( FileBuffer == NULL )
   {
      _DTraceError("calculateSignature: Size of block %d bytes is too big to fit in memory !\n", blockSize);
      SYS_FClose(FileHandle);
      return result;
   }

   if( SYS_FRead(FileBuffer, 1, blockSize, FileHandle) != blockSize )
   {
      _DTraceError("calculateSignature: Unable to read %d bytes into the \"%s\" file !\n", blockSize, fileName);
      SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
      FileBuffer = NULL;
      SYS_FClose(FileHandle);
      return result;
    }

   //read the file content
   U32 totalRead = blockSize;
   U32 blockId = 0;
   blockSize = ROMB_SIGNATURE_BLOCK_SIZE;
   SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
   FileBuffer = NULL;

   U32 tempMd5BlocksSize = (U32)(ROMB_SIGNATURE_LEN*((FileSize - totalRead)/blockSize + 1));
   char* pTempMd5Blocks = (char*)malloc(sizeof(char) *tempMd5BlocksSize);

   if( pTempMd5Blocks == NULL )
   {
      SYS_FClose(FileHandle);
      return result;
   }

   while( totalRead < FileSize )
   {
      if( (FileSize - totalRead) > ROMB_SIGNATURE_BLOCK_SIZE)
      {
         blockSize = ROMB_SIGNATURE_BLOCK_SIZE;
      }
      else
      {
         blockSize = FileSize - totalRead;
      }

      FileBuffer = (U8*)SYS_Memory_Allocate(SYS_MEMORY_NCACHED, blockSize, 256);
      if( FileBuffer == NULL )
      {
         _DTraceError("calculateSignature: Size of block %d bytes is too big to fit in memory !\n", blockSize);
         SYS_FClose(FileHandle);
         return result;
      }

      if( SYS_FRead(FileBuffer, 1, blockSize, FileHandle) != blockSize )
      {
         _DTraceError("calculateSignature: Unable to read %d bytes into the \"%s\" file !\n", blockSize, fileName);
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
         FileBuffer = NULL;
         SYS_FClose(FileHandle);
         return result;
       }

      char* pBlockMd5 = GStringUtils::md5(FileBuffer, blockSize);
      for( int i = 0; i < ROMB_SIGNATURE_LEN; ++i )
      {
         pTempMd5Blocks[ROMB_SIGNATURE_LEN*blockId + i] = pBlockMd5[i];
      }

      SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)FileBuffer);
      FileBuffer = NULL;

      blockId++;
      totalRead += blockSize;
      free(pBlockMd5);
      pBlockMd5 = NULL;
   }

   outSignature = GStringUtils::md5((U8*)pTempMd5Blocks, tempMd5BlocksSize);

   free(pTempMd5Blocks);
   pTempMd5Blocks = NULL;
   SYS_FClose(FileHandle);

   result = (outSignature != NULL);
   return result;
}

//////////////////////////////////////////////////////////////////////////////////////
//GROM_EraseFlash: Program routine for empty
//////////////////////////////////////////////////////////////////////////////////////
#if defined(DEMO_VERSION)

GROMError GROM_EraseFlash(const U32 flashId, const U32 fromAddress, U32 sizeErasing, BOOL isEraseAll)
{

   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   FLASH_Parameters_t FLASH_Parameters[FLASH_MAX_NUMBER];

   U32 SizeProgram;
   U32 Address1;
   U32 Address2;
   U32 Size1;
   U32 Size2;
   U32 BlockIndex;
   U32 i;

   U8* EraseBuffer;

   //Check parameters
   if( flashId >= FLASH_MAX_NUMBER )
   {
      _DTraceError("GROM_EraseFlash: Identifier of the flash is not specified or is wrong !\n");
      return GROMB_ERR_FLASH_ID;
   }

   if( FLASH_GetParams(flashId, &FLASH_Parameters[flashId]) != ST_NO_ERROR )
   {
      _DTraceError("GROM_EraseFlash: Could not get Flash Params at FlashID: %d !\n", flashId);
      return GROMB_ERR_FLASH_PARAM;
   }

   if( FLASH_Parameters[flashId].Size == 0 )
   {
      _DTraceError("GROM_EraseFlash: Flash %d seems to be not initialized properly, please select an another flash id !\n", flashId);
      return GROMB_ERR_FLASH_SIZE;
   }

   // Try to find first block index in the flash
   BlockIndex = 0;
   Address1 = 0;
   Size1 = 0;
   Size2 = 0;

   if( isEraseAll == TRUE )
   {
      Address2 = (U32)ROMB_BOOTROM_BASE_ADDR;
      SizeProgram = FLASH_Parameters[flashId].Size - 512*1024;
   }
   else
   {
      Address2 = (U32)fromAddress;
      SizeProgram = sizeErasing;
   }

   for( i = 0; i < FLASH_Parameters[flashId].NbBlocks; i++ )
   {
      while( (Address1 >= FLASH_Parameters[flashId].BlockMap[BlockIndex]) && (FLASH_Parameters[flashId].BlockMap[BlockIndex] != 0) )
      {
         BlockIndex += 2;
      }

      Size1 = FLASH_Parameters[flashId].BlockMap[BlockIndex+1];

      if( (Address1 <= Address2) && ((Address1 + Size1) > Address2) )
      {
         Size2 = Size1-(Address2-Address1);
         Size2 = (Size2 > SizeProgram) ? SizeProgram : Size2;
         break;
      }

      Address1 = Address1 + Size1;
   }

   // Verify if we have a contiguous block valid to burn the region
   if( FLASH_Parameters[flashId].Type == FLASH_NAND )
   {
      U32 Address1_Saved = Address1;
      U32 j,k;
      BOOL ContiguousRegionFound = FALSE;
      BOOL BadBlockIdentified = FALSE;

      // Look for a contiguous block regions without bad block to burn the flash
      while( ContiguousRegionFound == FALSE )
      {
         j = Address1;
         k = SizeProgram;
         BadBlockIdentified = FALSE;

         // Look if contiguous zone is valid
         while( (k != 0) && (BadBlockIdentified == FALSE) )
         {
            if( k > Size1 )
            {
               k = k - Size1;
            }
            else
            {
               k = 0;
            }

            if( FLASH_SectorErase(flashId, j, Size1) != ST_NO_ERROR )
            {
               _DTraceError("GROM_EraseFlash: Block at @ 0x%08x is a bad block on this flash, looking for a contiguous memory zone to burn the image !\n",j);
               BadBlockIdentified = TRUE;
               break;
            }

            j = j + Size1;
         }

         // If no bad block identified, contiguous zone is found
         if( BadBlockIdentified == FALSE )
         {
            ContiguousRegionFound = TRUE;
         }
         // Otherwise, try next valid block
         else
         {
            Address1 = j + Size1;
         }

         // We limit the contiguous region test to 512 block due to NAND IP controller specifications (see ADCS7864584)
         if( ContiguousRegionFound == FALSE )
         {
            if( ((Address1/Size1) > 512) || ((Address1/Size1) >= FLASH_Parameters[flashId].NbBlocks) )
            {
               _DTraceError("GROM_EraseFlash: Unable to burn this file as there are some bad block identified !\n");
               return GROMB_ERR_FILE_BAD_BLOCK;
            }
         }
      }

      if( Address1_Saved != Address1 )
      {
         _DTraceError("GROM_EraseFlash: Good contiguous blocks found at block @ 0x%08x, starting programming from this address !\n",Address1);
      }
   }

   // Perform the write operation block per block
   while( SizeProgram != 0 )
   {
      U32 LocalAddress;
      U32 LocalSize;

      // Program partial block or full block
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

      //Allocate memory to fill this block
      EraseBuffer = (U8*)SYS_Memory_Allocate(SYS_MEMORY_NCACHED, LocalSize, 256);

      if( EraseBuffer == NULL )
      {
         _DTraceError("GROM_EraseFlash: Size of block %d bytes is too big to fit in memory !\n", LocalSize);
         return GROMB_ERR_FILE_BAD_BLOCK;
      }

      memset( (U8*)EraseBuffer, 0, LocalSize );

      // Program the flash
      Task_Lock();

      _DTraceError("GROM_EraseFlash: Programming flash %d, address=0x%08x, size=%d\n", flashId, LocalAddress, LocalSize);

      ErrCode = FLASH_Write(flashId, (U8*)LocalAddress, LocalSize, EraseBuffer);

      if( ErrCode != ST_NO_ERROR )
      {
         Task_Unlock();
         _DTraceError("GROM_EraseFlash: Error in flash %d program operation at @ 0x%08x !\n", flashId, LocalAddress);
         SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)EraseBuffer);
         EraseBuffer = NULL;
         return GROMB_ERR_MEMORY;
      }

      Task_Unlock();

      // Free tmp buffer
      SYS_Memory_Deallocate(SYS_MEMORY_NCACHED, (U32)EraseBuffer);
      EraseBuffer = NULL;

      // Increase variables
      Address1 = Address1 + Size1;
      Address2 = Address1;

      SizeProgram = SizeProgram - Size2;

      while( (Address1 >= FLASH_Parameters[flashId].BlockMap[BlockIndex]) && (FLASH_Parameters[flashId].BlockMap[BlockIndex] !=0) )
      {
         BlockIndex += 2;
      }

      Size1 = FLASH_Parameters[flashId].BlockMap[BlockIndex+1];

      if( SizeProgram > Size1 )
      {
         Size2 = Size1;
      }
      else
      {
         Size2 = SizeProgram;
      }
   }

   _DTraceError("GROM_EraseFlash: Program operation completed - OK\n");

   //Return no errors
   return GROMB_ERR_NONE;
}

#else

GROMError GROM_EraseFlash(const U32 flashId, const U32 fromAddress, U32 sizeErasing, BOOL isEraseAll)
{
   _DTraceError("FEATURE IS NOT SUPPORTED");
   return GROMB_ERR_NONE;
}

#endif
//////////////////////////////////////////////////////////////////////////////////////
//} //API
//} //STM
//} //GBS
