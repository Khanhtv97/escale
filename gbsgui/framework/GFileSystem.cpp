//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GFileSystem.cpp                                                            //
//////////////////////////////////////////////////////////////////////////////////////

#include "GFileSystem.hpp"
#include "GStringUtils.hpp"
#include "GROMManager.hpp"
#include "ROMBlower.hpp"

#define CURRENT_FIRMWARE_VERSION_CODE 1
#define CURRENT_FIRMWARE_VERSION "gbs-dev-2012-12-05" // must be less than 64 bytes

// signature dat o dau va cuoi phan du lieu cua moi serializable object de verify
#define SIGNATURE_START "#**S**#"
#define SIGNATURE_FINISH "#**E**#"

#define LOGO_BLOCK_SIZE       0x00010000 // 64kb
#define LOGO_HEADER_LEN       92
#define LOGO_PRE_HEADER_LEN   96
#define LOGO_PRE_DATA_LEN     188

//////////////////////////////////////////////////////////////////////////////////////
// Using as C++
typedef unsigned char byte;
extern "C" byte* lzw_encode(byte* in, size_t in_len, int max_bits, size_t* out_len);
extern "C" byte* lzw_decode(byte* in, size_t in_len, size_t* out_len);
extern "C" void lzw_free(byte*);

//////////////////////////////////////////////////////////////////////////////////////
byte* compress(const U8* data, size_t size, size_t& outlen)
{
   byte* out = lzw_encode((byte*)data, size, 9, &outlen);
   return out;
}

//////////////////////////////////////////////////////////////////////////////////////
byte* decompress(const U8* data, size_t size, size_t& outlen)
{
   byte *out = lzw_decode((byte*)data, size, &outlen);
   return out;
}

//namespace GBS {
//namespace STM {
//namespace Framework {
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
GFileSystem& GFileSystem::getInstance()
{
   static GFileSystem instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFileSystem::init()
{
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
void GFileSystem::initialize()
{
   if( APIInitFlash( DEFAULT_FLASH_ID ) )
   {
      GFlash_Info_t flashInfo;
      APIFlashInfo(DEFAULT_FLASH_ID, flashInfo);

      if( flashInfo.Size > FS_BASED_ADDRESS )
      {
          this->m_basedAddress = FS_BASED_ADDRESS;
      }
      else
      {
         _DTraceDebug("Warning!!! Flash Size: %d, not enough for File system at: %d !\n", flashInfo.Size, FS_BASED_ADDRESS);
         this->m_basedAddress = flashInfo.Size;
      }

      if( flashInfo.NbBlocks > 0 )
      {
         this->m_flashBlockSize = flashInfo.Size / flashInfo.NbBlocks;
      }
      else
      {
         this->m_flashBlockSize = flashInfo.BlockMap[1];
      }
   }
   else
   {
      _DTraceDebug("Could not get Flash Params at FlashID: %d !\n", DEFAULT_FLASH_ID);
   }

   // initialize max size including START & END signatures
   SERIALIZABLE_MAX_SIZE[SERIALIZABLE_DESCRIPTORS] = 1024;     // 1KB
   SERIALIZABLE_MAX_SIZE[SERIALIZABLE_PARTITION_1] = 1024*3;   // 3KB   - settings
   SERIALIZABLE_MAX_SIZE[SERIALIZABLE_PARTITION_2] = 1024*512; // 512KB - rate #1
   SERIALIZABLE_MAX_SIZE[SERIALIZABLE_PARTITION_3] = 1024*512; // 512KB - rate #2
   SERIALIZABLE_MAX_SIZE[SERIALIZABLE_PARTITION_4] = 1024*512; // 512KB - rate #3
   SERIALIZABLE_MAX_SIZE[SERIALIZABLE_PARTITION_5] = 1024*8;   // 8KB   - offline data
   // 1 + 3 + 4 + 128 + 8 + 8 = 152 kb used
   // 256 - 152 = 104 kb reserved

   strcpy(SERIALIZABLE_NAME[SERIALIZABLE_DESCRIPTORS], "Descriptors");
   strcpy(SERIALIZABLE_NAME[SERIALIZABLE_PARTITION_1], "Partition #1");
   strcpy(SERIALIZABLE_NAME[SERIALIZABLE_PARTITION_2], "Partition #2");
   strcpy(SERIALIZABLE_NAME[SERIALIZABLE_PARTITION_3], "Partition #3");
   strcpy(SERIALIZABLE_NAME[SERIALIZABLE_PARTITION_4], "Partition #4");
   strcpy(SERIALIZABLE_NAME[SERIALIZABLE_PARTITION_5], "Partition #5");

   initDescriptor();
   //formatFileSystem();
   readDescriptor();
   //dumpDescriptor();
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFileSystem::formatFileSystem()
{
   //initDescriptor();
   return writeDescriptor(-1);
}

//////////////////////////////////////////////////////////////////////////////////////
void GFileSystem::initDescriptor()
{
   int i;
   m_descriptors[0].address = 0;
   m_descriptors[0].size = SERIALIZABLE_MAX_SIZE[SERIALIZABLE_DESCRIPTORS];
   m_descriptors[0].compressed_size = 0;
   m_descriptors[0].checksum[0] = '\0';
   _DTraceDebug("GFileSystem::initDescriptor...");

   for( i = 1; i < SERIALIZABLE_OBJECTS_COUNT; i++ )
   {
      m_descriptors[i].address = m_descriptors[i-1].address + SERIALIZABLE_MAX_SIZE[i-1];
      m_descriptors[i].size = m_descriptors[i].compressed_size = 0;
      _DTraceDebug("m_descriptors[%d].address = 0x%08X", i, m_descriptors[i].address);
   }

   m_isDescriptorRead = false;
}

//////////////////////////////////////////////////////////////////////////////////////
void GFileSystem::dumpDescriptor()
{
   //_DTraceDebug("Descriptor dump: Number of partitions: %d", SERIALIZABLE_OBJECTS_COUNT);
   int i;
   for( i = 0; i < SERIALIZABLE_OBJECTS_COUNT; i++ )
   {
      //_DTraceDebug("Partition #%d ('%s'): \t\t offset: 0x%08X, size: %08d bytes, compressed_size: %08d bytes, md5: %s", i, SERIALIZABLE_NAME[i], m_descriptors[i].address, m_descriptors[i].size, m_descriptors[i].compressed_size, m_descriptors[i].checksum);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
U8* GFileSystem::readSerializable(SerializableObject_t obj, U32& outlen)
{
   _DTraceDebug("GFileSystem::readSerializable Start ");
   if( obj != SERIALIZABLE_DESCRIPTORS )
   {
      if( m_isDescriptorRead == false )
      {
         if( !readDescriptor() )
         {
            //_DTraceDebug("readSerializable '%s': Cannot read descriptor!", SERIALIZABLE_NAME[obj]);
            return NULL;
         }
      }
   }

   U8* outStr = NULL;

   U32 size = (obj == SERIALIZABLE_DESCRIPTORS) ? m_descriptors[obj].size : m_descriptors[obj].compressed_size;
   if( size <= 0 )
   {
      //_DTraceDebug("readSerializable '%s': size is 0 in descriptor", SERIALIZABLE_NAME[obj]);
      return NULL;
   }

   size += strlen(SIGNATURE_FINISH) + strlen(SIGNATURE_START);

   if( obj != SERIALIZABLE_DESCRIPTORS && size >= SERIALIZABLE_MAX_SIZE[obj] )
   {
      //_DTraceDebug("readSerializable '%s': size exceeds limit", SERIALIZABLE_NAME[obj]);
      return NULL;
   }

   //if( m_descriptors[obj].size < m_descriptors[obj].compressed_size )
   //{
   //   _DTraceDebug("readSerializable '%s': wrong compressed size ", SERIALIZABLE_NAME[obj]);
   //   return false;
   //}

   _DTraceDebug("readSerializable '%s', size to read: %d ", SERIALIZABLE_NAME[obj], size);

   //_DTraceDebug("before locating mem\n");
   U8* buffer = (U8*)malloc(sizeof(U8) * (size+1));
   if( buffer == NULL )
   {
      //_DTraceDebug("readSerializable '%s': Can not allocate memory ", SERIALIZABLE_NAME[obj]);
      return NULL;
   }

   U32 realSize;
   //_DTraceDebug("after locating mem\n");
   //Task_Delay(ST_GetClocksPerSecond()/10);
   if( fastRawRead(m_descriptors[obj].address, size, buffer, &realSize) )
   {
      //buffer[size] = '\0';
      _DTraceDebug("raw data: %s", buffer);
      int b = GStringUtils::detectSignature(buffer, realSize, SIGNATURE_START);

      if( b < 0 )
      {
         _DTraceDebug("readSerializable '%s' Failed: SIGNATURE_START not found", SERIALIZABLE_NAME[obj]);
         if( obj == SERIALIZABLE_DESCRIPTORS )
         {
            // neu chua co descriptor thi ghi desriptor mac dinh vao (format)
            if( formatFileSystem() )
            {
               m_isDescriptorRead = true;
            }
         }

         free(buffer);
         buffer = NULL;
         return NULL;
      }

      b += strlen(SIGNATURE_START);

      int e = GStringUtils::detectSignature(buffer, realSize, SIGNATURE_FINISH);
      if( e < 0 )
      {
         _DTraceDebug("readSerializable '%s' Failed: SIGNATURE_FINISH not found", SERIALIZABLE_NAME[obj]);
         _DTraceDebug("readSerializable '%s' Failed: SIGNATURE_START not found", SERIALIZABLE_NAME[obj]);
         if( obj == SERIALIZABLE_DESCRIPTORS )
         {
            // neu chua co descriptor thi ghi desriptor mac dinh vao (format)
            if( formatFileSystem() )
            {
               m_isDescriptorRead = true;
            }
         }
         free(buffer);
         buffer = NULL;
         return NULL;
      }

      if( (b >= realSize) || (b >= e) )
      {
         _DTraceDebug("readSerializable '%s', Signatures found, data empty", SERIALIZABLE_NAME[obj]);
         free(buffer);
         buffer = NULL;
         return NULL;
      }
      else
      {
         _DTraceDebug("readSerializable, Signatures found, real data: offset: 0x%08X, size: %08d", b, e-b);
         //_DTraceDebug("Out string: , b = %d",b);
         //for(int i =0;i<200; i++) _DTraceDebug("%d: %#x ",i,buffer[i]);
         //_DTraceDebug("");
         // giai nen, ko ap dung voi descriptor
         if( obj != SERIALIZABLE_DESCRIPTORS )
         {
            size_t decoded_size;
            if( m_descriptors[obj].compressed_size != e-b )
            {
               _DTraceDebug("readSerializable '%s': verification FAILED, compressed size mismatch!", SERIALIZABLE_NAME[obj]);
               outlen = 0;
               free(buffer);
               buffer = NULL;
               return NULL;
            }

            byte* decodedStr = decompress(buffer + b, e-b, decoded_size);
            _DTraceDebug("readSerializable '%s': data size: %d, decompressed size: %d", SERIALIZABLE_NAME[obj],e-b,decoded_size);

            //_DTraceDebug("raw data: %s", buffer);
            //_DTraceDebug("decoded data 0: %s", decodedStr);
            // TODO cai nay thinh thoang bi crash, can doi het sang su dung char* thay vi C++ string
            outStr = (U8*)malloc((decoded_size+1));
            memcpy(outStr, decodedStr, decoded_size);

            outlen = decoded_size;
            //GStringUtils::getStringFromArray(outStr, (char*)decodedStr, 0, decoded_size);
            outStr[decoded_size] = '\0';
            //_DTraceDebug("decoded data: %s", outStr);
            lzw_free(decodedStr); // ko free vi bo nho nay dc dung trong outStr, do o tren chi ep kieu chu ko copy mem

            //_DTraceDebug("calculateing checksum...");
            char* schecksum = GStringUtils::md5(outStr, decoded_size);
            //_DTraceDebug("checksum: %s", _checksum);

            if( (m_descriptors[obj].size != outlen) || (strcmp(m_descriptors[obj].checksum, schecksum)) )
            {
               _DTraceDebug("readSerializable '%s': veryfication FAILED, size or checksum mismatch!", SERIALIZABLE_NAME[obj]);
               //_DTraceDebug("m_descriptors[%s].size = %d ", SERIALIZABLE_NAME[obj], m_descriptors[obj].size);
               //_DTraceDebug("outStr.length() = %d ", outStr.length());
               outlen = 0;

               if( buffer != NULL )
               {
                  free(buffer);
                  buffer = NULL;
               }

               if( schecksum != NULL )
               {
                  free(schecksum);
                  schecksum = NULL;
               }

               if( outStr != NULL )
               {
                  free(outStr);
                  outStr = NULL;
               }
               return NULL;
            }
            else
            {
               //delete _checksum;
               if( schecksum != NULL )
               {
                  free(schecksum);
                  schecksum = NULL;
               }
               _DTraceDebug("readSerializable '%s': veryfication OK!", SERIALIZABLE_NAME[obj]);
            }
         }
         else
         {
            //_DTraceDebug("Descriptor data found from %d to %d", b, e);
            if( outStr != NULL )
            {
               free(outStr);
               outStr = NULL;
            }

            outStr = GStringUtils::subArray(buffer, size, outlen, b, e-b);
            //_DTraceDebug("Descriptor data found from %d to %d: %s, length: %d", b, e, outStr, outlen);
            //outlen = e-b; // cai nay tu dong duoc gan boi ham subArray tren
         }
         // END OF PARSE DESCRIPTOR

         if( buffer != NULL )
         {
            free(buffer);
            buffer = NULL;
         }

         //_DTraceDebug("Descriptor data found: %s, length: %d", outStr, outlen);
         return outStr;
      }
   }
   else
   {
      //_DTraceDebug("readSerializable '%s' Failed", SERIALIZABLE_NAME[obj]);
       if( outStr != NULL )
       {
          free(outStr);
          outStr = NULL;
       }

       if( buffer != NULL )
       {
          free(buffer);
          buffer = NULL;
       }

      outlen = 0;

      return NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFileSystem::writeSerializable(SerializableObject_t obj, U8* inStr, int size)
{
   bool ret = false;
   size_t encoded_size;
   byte* encodedStr = compress(inStr, size, encoded_size);

   m_descriptors[obj].size = size;
   m_descriptors[obj].compressed_size = encoded_size;

   char* schecksum = GStringUtils::md5(inStr, size);
   strcpy(m_descriptors[obj].checksum, schecksum);

   if( schecksum != NULL )
   {
      free(schecksum);
      schecksum = NULL;
   }

   int lb = strlen(SIGNATURE_START);
   int le = strlen(SIGNATURE_FINISH);
   int write_size = encoded_size + lb + le;
   U8* dataToWrite = (U8*)malloc(sizeof(U8) * (write_size+3));
   //memcpy(dataToWrite, SIGNATURE_START, lb);
   //memcpy(dataToWrite + lb, encodedStr, encoded_size);
   //memcpy(dataToWrite + lb + encoded_size, SIGNATURE_FINISH, le);

   for( int i = 0; i < lb; i++ )
   {
      dataToWrite[i] = SIGNATURE_START[i];
   }

   for( int i = 0; i < encoded_size; i++)
   {
      dataToWrite[i+lb] = encodedStr[i];
   }

   for( int i = 0; i < le; i++ )
   {
      dataToWrite[i+lb+encoded_size] = SIGNATURE_FINISH[i];
   }

   dataToWrite[write_size] = '\0';
   //_DTraceDebug("writeSerializable about to write '%s', offset: 0x%8x, size: %d, compressed size: %d", SERIALIZABLE_NAME[obj], m_descriptors[obj].address, m_descriptors[obj].size, m_descriptors[obj].compressed_size);
   //de debug co the loai bo 1 phan tu cuoi cung de ham _DTraceDebug ko bi loi
   inStr[size-1] = '\0';
   //_DTraceDebug("raw data: %s", inStr);
   //_DTraceDebug("encoded data: %s", (char*)dataToWrite);

   if( fastRawWrite(m_descriptors[obj].address, write_size, dataToWrite) )
   {
      //_DTraceDebug("writeSerializable '%s' successful, offset: 0x%8x, size: %d, compressed size: %d, size to write: %d", SERIALIZABLE_NAME[obj], m_descriptors[obj].address, m_descriptors[obj].size, m_descriptors[obj].compressed_size, write_size);
      _DTraceDebug("writeSerializable: %s, data: %s", SERIALIZABLE_NAME[obj], (char*)dataToWrite);
      ret = writeDescriptor(obj);
   }
   else
   {
      _DTraceDebug("writeSerializable '%s' FAILED!", SERIALIZABLE_NAME[obj]);
   }

   lzw_free(encodedStr);

   if( dataToWrite != NULL )
   {
      free(dataToWrite);
      dataToWrite = NULL;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
U8* GFileSystem::readFlashData(SerializableObject_t obj, U32& outlen)
{
   _DTraceDebug("Start ");
   if( obj != SERIALIZABLE_DESCRIPTORS )
   {
      if( m_isDescriptorRead == false )
      {
         if( !readDescriptor() )
         {
            //_DTraceDebug("readSerializable '%s': Cannot read descriptor!", SERIALIZABLE_NAME[obj]);
            return NULL;
         }
      }
   }

   U8* outStr = NULL;

   U32 size = m_descriptors[obj].size;
   if( size <= 0 )
   {
      //_DTraceDebug("readSerializable '%s': size is 0 in descriptor", SERIALIZABLE_NAME[obj]);
      return NULL;
   }

   size += strlen(SIGNATURE_FINISH) + strlen(SIGNATURE_START);

   if( obj != SERIALIZABLE_DESCRIPTORS && size >= SERIALIZABLE_MAX_SIZE[obj] )
   {
      //_DTraceDebug("readSerializable '%s': size exceeds limit", SERIALIZABLE_NAME[obj]);
      return NULL;
   }
   //TODO: change to Debug
   _DTraceDebug("readSerializable '%s', size to read: %d ", SERIALIZABLE_NAME[obj], size);

   U8* buffer = (U8*)malloc(sizeof(U8) * (size+1));
   if( buffer == NULL )
   {
      //_DTraceDebug("readSerializable '%s': Can not allocate memory ", SERIALIZABLE_NAME[obj]);
      return NULL;
   }

   U32 realSize;
   //_DTraceDebug("after locating mem\n");
   //Task_Delay(ST_GetClocksPerSecond()/10);
   if( fastRawRead(m_descriptors[obj].address, size, buffer, &realSize) )
   {
      //buffer[size] = '\0';
      _DTraceDebug("GFileSystem::readFlashData size = %d", realSize);
      int b = GStringUtils::detectSignature(buffer, realSize, SIGNATURE_START);

      if( b < 0 )
      {
         _DTraceDebug("readSerializable '%s' Failed: SIGNATURE_START not found", SERIALIZABLE_NAME[obj]);
         if( obj == SERIALIZABLE_DESCRIPTORS )
         {
            // neu chua co descriptor thi ghi desriptor mac dinh vao (format)
            if( formatFileSystem() )
            {
               m_isDescriptorRead = true;
            }
         }
         free(buffer);
         buffer = NULL;
         return NULL;
      }

      b += strlen(SIGNATURE_START);
      int e = GStringUtils::detectSignature(buffer, realSize, SIGNATURE_FINISH);

      if( e < 0 )
      {
         _DTraceDebug("readSerializable '%s' Failed: SIGNATURE_FINISH not found", SERIALIZABLE_NAME[obj]);
         free(buffer);
         buffer = NULL;
         return NULL;
      }

      if( (b >= realSize) || (b >= e) )
      {
         _DTraceDebug("readSerializable '%s', Signatures found, data empty", SERIALIZABLE_NAME[obj]);
         free(buffer);
         buffer = NULL;
         return NULL;
      }
      else
      {
         _DTraceDebug("readFlashData -> Signatures found, real data: offset: 0x%08X, size: %08d", b, e-b);
         outStr = GStringUtils::subArray(buffer, size, outlen, b, e-b);
         free(buffer);
         buffer = NULL;
         //_DTraceDebug("Descriptor data found: %s, length: %d", outStr, outlen);
         return outStr;
      }
   }
   else
   {
      _DTraceDebug("readSerializable '%s' Failed", SERIALIZABLE_NAME[obj]);
      free(buffer);
      buffer = NULL;
      outlen = 0;
      return NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFileSystem::writeFlashData(SerializableObject_t obj, U8* inStr, int size)
{
   bool ret = false;
   m_descriptors[obj].size = size;

   char* schecksum = GStringUtils::md5(inStr, size);

   strcpy(m_descriptors[obj].checksum, schecksum);

   if( schecksum != NULL )
   {
      free(schecksum);
      schecksum = NULL;
   }

   _DTraceDebug("GFileSystem::writeFlashData start ... \n ");

   int lb = strlen(SIGNATURE_START);
   int le = strlen(SIGNATURE_FINISH);
   int write_size = size + lb + le;

   U8* dataToWrite = (U8*)malloc(sizeof(U8) * (write_size+3));
   //memcpy(dataToWrite, SIGNATURE_START, lb);
   //memcpy(dataToWrite + lb, encodedStr, encoded_size);
   //memcpy(dataToWrite + lb + encoded_size, SIGNATURE_FINISH, le);

   for( int i = 0; i < lb; i++ )
   {
      dataToWrite[i] = SIGNATURE_START[i];
   }

   for( int i = 0; i < size; i++ )
   {
      dataToWrite[i+lb] = inStr[i];
   }

   for( int i = 0; i < le; i++ )
   {
      dataToWrite[i+lb+size] = SIGNATURE_FINISH[i];
   }

   dataToWrite[write_size] = '\0';
   //_DTraceDebug("writeSerializable about to write '%s', offset: 0x%8x, size: %d, compressed size: %d", SERIALIZABLE_NAME[obj], m_descriptors[obj].address, m_descriptors[obj].size, m_descriptors[obj].compressed_size);
   //de debug co the loai bo 1 phan tu cuoi cung de ham _DTraceDebug ko bi loi
   inStr[size-1] = '\0';
   //_DTraceDebug("raw data: %s", inStr);
   //_DTraceDebug("encoded data: %s", (char*)dataToWrite);
   if( fastRawWrite(m_descriptors[obj].address, write_size, dataToWrite) )
   {
      _DTraceDebug("writeSerializable '%s' successful, offset: 0x%8x, size: %d, compressed size: %d, size to write: %d", SERIALIZABLE_NAME[obj], m_descriptors[obj].address, m_descriptors[obj].size, m_descriptors[obj].compressed_size, write_size);
      ret = writeDescriptor(obj);
   }
   else
   {
      _DTraceDebug("writeSerializable '%s' FAILED!", SERIALIZABLE_NAME[obj]);
   }

   free(dataToWrite);
   dataToWrite = NULL;

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFileSystem::readDescriptor()
{
   m_isDescriptorRead = false;
   _DTraceDebug("Start ");
   U32 descriptorSize;
   U8* buffer = readSerializable(SERIALIZABLE_DESCRIPTORS, descriptorSize);

   U32 expectedSize = 68 + 44*SERIALIZABLE_OBJECTS_COUNT;

   if( buffer != NULL )
   {
      _DTraceDebug("readDescriptor, size: %d", descriptorSize);

      if( descriptorSize != expectedSize )
      {
         _DTraceDebug("Read Descriptors: read size (%d) mismatched (%d), reformating file system!", descriptorSize, expectedSize);
         if( formatFileSystem() )
         {
            m_isDescriptorRead = true;
         }

         free(buffer);
         buffer = NULL;
         return false;
      }

      // PARSE DESCRIPTOR
      // set default value first
      initDescriptor();

      int pos = 0;
      m_firmwareVersionCode = GStringUtils::getU32FromBuffer(buffer, pos);
      pos += 4;
      GStringUtils::getStringFromArray(m_firmwareVersion, (char*)buffer, pos, 64);
      pos += 64;

      for( int i = 0; i < SERIALIZABLE_OBJECTS_COUNT; i++ )
      {
         m_descriptors[i].address  = GStringUtils::getU32FromBuffer(buffer, pos);
         pos += 4;
         m_descriptors[i].size  = GStringUtils::getU32FromBuffer(buffer, pos);
         pos += 4;
         m_descriptors[i].compressed_size  = GStringUtils::getU32FromBuffer(buffer, pos);
         pos += 4;
         GStringUtils::getStringFromArray(m_descriptors[i].checksum, (char*)buffer, pos, 32);
         pos += 32;
      }

      if( (m_descriptors[0].size != SERIALIZABLE_MAX_SIZE[SERIALIZABLE_DESCRIPTORS]) ||
            strcmp(m_descriptors[0].checksum, "d41d8cd98f00b204e9800998ecf8427e") )
      {
         //_DTraceDebug("Read Descriptors: veryfication FAILED, reformating file system!");
         if( formatFileSystem() )
         {
            m_isDescriptorRead = true;
         }

         free(buffer);
         buffer = NULL;
         return false;
      }
      else
      {
         _DTraceDebug("Read Descriptors: veryfication OK!");
         m_isDescriptorRead = true;
      }
      // END OF PARSE DESCRIPTOR

      dumpDescriptor();

      _DTraceDebug("\n\n###############################################################\n");
      _DTraceDebug("## Flash firmware: `%s` (code %d)\n", m_firmwareVersion, m_firmwareVersionCode);
      _DTraceDebug("## Current firmware: `%s` (code %d)\n", CURRENT_FIRMWARE_VERSION, CURRENT_FIRMWARE_VERSION_CODE);
      _DTraceDebug("###############################################################\n\n\n");

      free(buffer);
      buffer = NULL;
      _DTraceDebug("End ");
      return true;
   }
   else
   {
      //_DTraceDebug("Read descriptor Failed");
      return false;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFileSystem::writeDescriptor(int partition)
{
   bool ret = false;

   char* buffer = (char*)malloc(SERIALIZABLE_OBJECTS_COUNT * 44 + 68 + strlen(SIGNATURE_START) + strlen(SIGNATURE_FINISH) + 10); // 10 bytes reserved

   int size = 0;

   if( partition <= 0 )
   {
      strcpy(buffer, SIGNATURE_START);
      size += strlen(SIGNATURE_START);
      GStringUtils::putU32ToBuffer(CURRENT_FIRMWARE_VERSION_CODE, (U8*)buffer, size);
      size += 4;
      strcpy(buffer+size, CURRENT_FIRMWARE_VERSION);
      size += 64;

      m_descriptors[0].size = SERIALIZABLE_MAX_SIZE[SERIALIZABLE_DESCRIPTORS];
      strcpy(m_descriptors[0].checksum, "d41d8cd98f00b204e9800998ecf8427e");
      int i;

      for( i = 0; i < SERIALIZABLE_OBJECTS_COUNT; i++ )
      {
         GStringUtils::putU32ToBuffer(m_descriptors[i].address, (U8*)buffer, size);
         size += 4;
         GStringUtils::putU32ToBuffer(m_descriptors[i].size, (U8*)buffer, size);
         size += 4;
         GStringUtils::putU32ToBuffer(m_descriptors[i].compressed_size, (U8*)buffer, size);
         size += 4;
         m_descriptors[i].checksum[32] = 0;
         strcpy(buffer + size, m_descriptors[i].checksum);
         size += 32;
      }

      strcpy(buffer + size, SIGNATURE_FINISH);
      size += strlen(SIGNATURE_FINISH);
   }
   else
   {
      GStringUtils::putU32ToBuffer(m_descriptors[partition].address, (U8*)buffer, size);
      size += 4;
      GStringUtils::putU32ToBuffer(m_descriptors[partition].size, (U8*)buffer, size);
      size += 4;
      GStringUtils::putU32ToBuffer(m_descriptors[partition].compressed_size, (U8*)buffer, size);
      size += 4;
      m_descriptors[partition].checksum[32] = 0;
      strcpy(buffer + size, m_descriptors[partition].checksum);
      size += 32;
   }

   if( fastRawWrite(m_descriptors[0].address + ( partition <= 0 ? 0 : strlen(SIGNATURE_START)+68+44*partition), size, (U8*)buffer) )
   {
      //_DTraceDebug("Write descriptor of partition %d successful: %d bytes", partition, size);
      _DTraceDebug("Write descriptor %d, data: %s", partition, buffer);
      dumpDescriptor();
      ret = true;
   }
   else
   {
      _DTraceDebug("Write descriptor FAILED!");
      ret = false;
   }

   if( buffer != NULL )
   {
      free(buffer);
      buffer = NULL;
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFileSystem::writeFirmware(std::string& inStr)
{
   rawFwWrite(0, inStr.length(), (U8*)inStr.c_str());
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFileSystem::rawRead(U32 offset, U32 readSize, U8* buffer, U32* actualReadSize, bool useRelativeOffset)
{
   return APIFlashRead(DEFAULT_FLASH_ID, useRelativeOffset ? offset + m_basedAddress : offset, readSize, buffer, actualReadSize);
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFileSystem::rawWrite(U32 offset, U32 writeSize,U8* buffer, bool useRelativeOffset)
{
   return APIFlashWrite(DEFAULT_FLASH_ID, useRelativeOffset ? offset + m_basedAddress : offset, writeSize, buffer);
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFileSystem::fastRawRead(U32 offset, U32 readSize, U8* buffer, U32* actualReadSize, bool useRelativeOffset)
{
   return APIFastRead(DEFAULT_FLASH_ID, useRelativeOffset ? offset + m_basedAddress : offset, readSize, buffer, actualReadSize);
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFileSystem::fastRawWrite(U32 offset, U32 writeSize, U8* buffer, bool useRelativeOffset)
{
   return APIFastWrite(DEFAULT_FLASH_ID, useRelativeOffset ? offset + m_basedAddress : offset, writeSize, buffer);
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFileSystem::rawFwWrite(U32 offset, U32 writeSize, U8*buffer)
{
   return APIFlashWrite(DEFAULT_FLASH_ID, offset + FS_SYSTEM_ADDRESS, writeSize, buffer);
}

//////////////////////////////////////////////////////////////////////////////////////
GFileSystem::GFileSystem()
{
   initialize();
}

//////////////////////////////////////////////////////////////////////////////////////
GFileSystem::~GFileSystem()
{
}

//////////////////////////////////////////////////////////////////////////////////////
//write partition
bool GFileSystem::writeSavePartition(int partition, std::string& inStr)
{
   bool ret = false;

   SerializableObject_t object = SERIALIZABLE_OBJECTS_COUNT;
   switch( partition )
   {
   case 0:
      object = SERIALIZABLE_DESCRIPTORS;
      break;

   case 1:
      object = SERIALIZABLE_PARTITION_1;
      break;

   case 2:
      object = SERIALIZABLE_PARTITION_2;
      break;

   case 3:
      object = SERIALIZABLE_PARTITION_3;
      break;

   case 4:
      object = SERIALIZABLE_PARTITION_4;
      break;

   case 5:
      object = SERIALIZABLE_PARTITION_5;
      break;

   default:
      _DTraceDebug("Partition is not valid !!!");
      return false;
      break;
   }

   if( object == SERIALIZABLE_PARTITION_2 )
   {
      //Switch off task locking due to save setting
      APIFlashNeedLockTask(FALSE);
   }

   ret = writeFlashData( object, (U8*)inStr.c_str(), inStr.length());

   if( object == SERIALIZABLE_PARTITION_2 )
   {
      //Switch on task locking
      APIFlashNeedLockTask(TRUE);
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
//Read partition
bool GFileSystem::readPartitionFromFlash(int partition, U8*& data, U32& size)
{
   _DTraceDebug("GFileSystem::readPartitionFromFlash Start ");
   //data = readSerializable((SerializableObject_t)partition, size);
   data = readFlashData((SerializableObject_t)partition, size);
   if( data == NULL )
   {
      _DTraceDebug("%s: data is NULL", __FUNCTION__);
      return false;
   }
   _DTraceDebug("End ");
   return true;
}

//////////////////////////////////////////////////////////////////////////////////////
U32 GFileSystem::toSettingAddress(U32 romAddress)
{
   U32 settingAddress = FS_SETTING_ADDRESS;
   U32 blockIndex = 6; // reserved block

   if( romAddress >= EEPROM_ADDR_CHANNEL && romAddress < EEPROM_ADDR_VOLUME )
   {
      //Flash 65kb, block #1
      blockIndex = 0;
   }
   else if( romAddress >= EEPROM_ADDR_VOLUME && romAddress < EEPROM_ADDR_STANDBY )
   {
      //Flash 65kb, block #2
      blockIndex = 1;
   }
   else if( romAddress >= EEPROM_ADDR_STANDBY && romAddress < EEPROM_ADDR_ANTENPOWER )
   {
      //Flash 65kb, block #3
      blockIndex = 2;
   }
   else if( romAddress >= EEPROM_ADDR_ANTENPOWER && romAddress < EEPROM_ADDR_FAV )
   {
      //Flash 65kb, block #4
      blockIndex = 3;
   }
   else if( romAddress >= EEPROM_ADDR_FAV && romAddress < EEPROM_ADDR_TIMER_MANAGER_BASE )
   {
      //Flash 65kb, block #5
      blockIndex = 4;
   }
   else if( romAddress >= EEPROM_ADDR_TIMER_MANAGER_BASE )
   {
      //Flash 65kb, block #6
      blockIndex = 5;
   }

   settingAddress += (m_flashBlockSize*blockIndex + romAddress);

   return settingAddress;
}

//////////////////////////////////////////////////////////////////////////////////////
// Read user settings
bool GFileSystem::readSettings(U32 address, U8* Buffer, U8 NumOfByte)
{
   U32 localAddress = toSettingAddress(address);
   U32 actualReadSize = 0;

   if( fastRawRead(localAddress, NumOfByte, Buffer, &actualReadSize, false) )
   {
      _DTraceDebug("Read at FLASH local addr: 0x%08X, size: %08d, actual read size %08d!\n", localAddress, NumOfByte, actualReadSize);
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
// Write user settings
bool GFileSystem::writeSettings(U32 address, U8* Buffer, U8 NumOfByte)
{
   U32 localAddress = toSettingAddress(address);

   //Switch off task locking due to save setting
   APIFlashNeedLockTask(FALSE);

   if( fastRawWrite(localAddress, NumOfByte, Buffer, false) )
   {
      _DTraceDebug("Written at FLASH addr: 0x%08X, size: %08d!\n", localAddress, NumOfByte);
      //Switch on task locking
      APIFlashNeedLockTask(TRUE);
      return true;
   }

   //Switch on task locking
   APIFlashNeedLockTask(TRUE);
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFileSystem::getLogo(FS_Logo_t type, GAMLOAD_Object_t*& pGamObj)
{
   U8 buffer[LOGO_BLOCK_SIZE];

   U32 address = toLogoAddress(type);
   if( address < FS_LOGO_ADDRESS || address + LOGO_BLOCK_SIZE > 0x01000000 )
   {
      _DTraceDebug("!!! *ERROR*: ID is not valid !!!");
      return false;
   }

   U32 actualReadSize = 0;
   if( fastRawRead(address, LOGO_BLOCK_SIZE, buffer, &actualReadSize, false) ) // Read OK
   {
      _DTraceDebug("Read at FLASH local addr: 0x%08X, %08d!\n", address, actualReadSize);

      const char* sign = NULL;

      switch( type )
      {
      case GLOGO_SPLASH:
         sign = ROMB_LOGOSPLASH_SIGNATURE;
         break;

      case GLOGO_INFO:
         sign = ROMB_LOGOINFO_SIGNATURE;
         break;

      case GLOGO_MENU:
         sign = ROMB_LOGOMENU_SIGNATURE;
         break;
      }

      if( sign != NULL && GStringUtils::detectSignature(buffer, LOGO_PRE_HEADER_LEN, sign) != -1 )
      {
         STGXOBJ_Bitmap_t* pBitmap = (STGXOBJ_Bitmap_t*)malloc(sizeof(STGXOBJ_Bitmap_t));
         getBitmapFromBuffer(buffer + LOGO_PRE_HEADER_LEN, pBitmap);

         if( pBitmap->Size1 <= (actualReadSize - LOGO_PRE_DATA_LEN) ) // Verify OK
         {
            if( pGamObj != NULL )
            {
               if( pGamObj->Bitmap != NULL )
               {
                  SYS_Memory_Deallocate(SYS_MEMORY_CACHED, (U32)pGamObj->Bitmap->Data1_Cp);
                  free(pGamObj->Bitmap->Data1_NCp);
                  pGamObj->Bitmap->Data1_NCp = NULL;
                  free(pGamObj->Bitmap);
                  pGamObj->Bitmap = NULL;
               }
               free(pGamObj);
               pGamObj = NULL;
            }

            U8* pData = (U8*)malloc(pBitmap->Size1 * sizeof(U8));
            memcpy(pData, buffer + LOGO_PRE_DATA_LEN, pBitmap->Size1);
            pBitmap->Data1_NCp = pData;

            pGamObj = (GAMLOAD_Object_t*)malloc(sizeof(GAMLOAD_Object_t));
            memset(pGamObj, 0, sizeof(GAMLOAD_Object_t));
            pGamObj->Bitmap = pBitmap;

            return true;
         }
         else
         {
            free(pBitmap);
            pBitmap = NULL;
         }
      }
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFileSystem::resetLogo(FS_Logo_t type)
{
   U32 address = toLogoAddress(type);
   if( address < FS_LOGO_ADDRESS || address + LOGO_BLOCK_SIZE > 0x01000000 )
   {
      _DTraceDebug("!!! *ERROR*: ID is not valid !!!");
      return false;
   }

   U8 buffer[LOGO_BLOCK_SIZE];
   memset(buffer, 0, LOGO_BLOCK_SIZE);

   if( fastRawWrite(address, LOGO_BLOCK_SIZE, buffer, false) )
   {
      _DTraceDebug("Written at FLASH - addr: 0x%08X!\n", address);
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
U32 GFileSystem::toLogoAddress(FS_Logo_t type)
{
   switch( type )
   {
   case GLOGO_SPLASH:
      return FS_LOGO_ADDRESS;

   case GLOGO_INFO:
      return FS_LOGO_ADDRESS + LOGO_BLOCK_SIZE;

   case GLOGO_MENU:
      return FS_LOGO_ADDRESS + LOGO_BLOCK_SIZE + LOGO_BLOCK_SIZE;
   }

   return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
void GFileSystem::getBitmapFromBuffer(U8* buffer, STGXOBJ_Bitmap_t* pBitmap)
{
   if( buffer == NULL || pBitmap == NULL )
   {
      return;
   }

   memset(pBitmap, 0, sizeof(STGXOBJ_Bitmap_t));
   int index = 0;
   U32 temp;

   pBitmap->ColorType = (STGXOBJ_ColorType_t)GStringUtils::getU32FromBuffer(buffer, index);
   index += 4;
   pBitmap->BitmapType = (STGXOBJ_BitmapType_t)GStringUtils::getU32FromBuffer(buffer, index);
   index += 4;
   temp = GStringUtils::getU32FromBuffer(buffer, index);
   pBitmap->PreMultipliedColor = (temp == 1) ? TRUE : FALSE;
   index += 4;
   pBitmap->ColorSpaceConversion = (STGXOBJ_ColorSpaceConversionMode_t)GStringUtils::getU32FromBuffer(buffer, index);
   index += 4;
   pBitmap->AspectRatio = (STGXOBJ_AspectRatio_t)GStringUtils::getU32FromBuffer(buffer, index);
   index += 4;
   pBitmap->Width = GStringUtils::getU32FromBuffer(buffer, index);
   index += 4;
   pBitmap->Height = GStringUtils::getU32FromBuffer(buffer, index);
   index += 4;
   pBitmap->Pitch = GStringUtils::getU32FromBuffer(buffer, index);
   index += 4;
   pBitmap->Offset = GStringUtils::getU32FromBuffer(buffer, index);
   index += 16;
   pBitmap->Size1 = GStringUtils::getU32FromBuffer(buffer, index);
   index += 20;
   pBitmap->SubByteFormat = (STGXOBJ_SubByteFormat_t)GStringUtils::getU32FromBuffer(buffer, index);
   index += 4;
   pBitmap->Pitch2 = GStringUtils::getU32FromBuffer(buffer, index);
   index += 4;
   pBitmap->Offset2 = GStringUtils::getU32FromBuffer(buffer, index);
   index += 4;
   temp = GStringUtils::getU32FromBuffer(buffer, index);
   pBitmap->BigNotLittle = (temp == 1) ? TRUE : FALSE;
   index += 4;
   pBitmap->YUVScaling.ScalingFactorY = (YUV_Scaling_t)GStringUtils::getU32FromBuffer(buffer, index);
   index += 4;
   pBitmap->YUVScaling.ScalingFactorUV = (YUV_Scaling_t)GStringUtils::getU32FromBuffer(buffer, index);
}

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
