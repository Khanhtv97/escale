//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GFileSystem.hpp                                                            //
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// This is a trivial file system for saving persistent data on Flash Memory within
// Kachiusa project
//
// To simplify the implementation, following conventions are considered during
// development:
//
//  - The start address of the filesystem on Flash memory is fixed in code (doesn't
//    require Master Boot Record or something like that). This start address is defined
//    in FS_BASED_ADDRESS macro below
//
//  - Storage unit using in this trivial filesystem is "block", instead of sector and
//    cluster
//
//  - Block size is fixed in code (instead of Boot Sector). This value is
//    defined in FS_BLOCK_SIZE macro below
//
//  - Number of FAT copy - 2
//
//  - Total number of block is auto calculated by the FS_BASED_ADDRESS and the size of
//    flash memory
//
//  - This file system is flat, that means it cannot contain directories, every files
//    are store at the root level of hierarchy. And the maximum number of files are
//    limited to 256?????
//
//  - The File Allocation Table has one 16-bit entry per block. Each entry records
//    one of following things
//     . block number of next block in a chain
//     . a special end of chain (EoC) entry: let's say 0x00FF00FF
//     . a zero to note that the cluster is unused: 0x00000000
//     . other special entries while needed
//
//  - The FAT size will be: "total number of blocks" x "2 bytes (16-bit) per entry"
//
//////////////////////////////////////////////////////////////////////////////////////
// !!! IMPORTANT:
//       Everything said above has been DISCARDED!!!!!!!!!!!!!!!!
//       Replaced with quicker, simpler approach - limited number of serializable
//       objects at fixed addresses. Addresses are calculated upon maximum size will
//       be allocated to each of those objects (SERIALIZABLE_MAX_SIZE array).
//
//       Structure of `Descriptor` partition:
//
//       offset           size      value
//
//       00               XX         signature start
//       XX                4         firmware version code (XX la length cua signature start)
//                        64         firmware version name
//       +0x44 (+68)       4         part_0 address
//                         4         part_0 size
//                         4         part_0 compress size
//                        32         part_0 checksum
//       +0x70 (+112)      4         part_1 address
//                         4         part_1 size
//                         4         part_1 compress size
//                        32         part_1 checksum
//      ...
//      +68 + 44*i         4         part_i address
//                         4         part_i size
//                         4         part_i compress size
//                        32         part_i checksum
//       ...
//       ...             ...         signature finish
//////////////////////////////////////////////////////////////////////////////////////
#ifndef GFILESYSTEM_HPP_
#define GFILESYSTEM_HPP_

#include "APIFlashIO.hpp"
#include "GObject.hpp"
#include "GDebug.hpp"

//////////////////////////////////////////////////////////////////////////////////////
// Used the last 1MB in the Flash memory for File System (Channel data + Settings)
// Used 1MB of 32MB Flash (0x01FFFFFF - 0x00100000 = 0x01EFFFFF)
// Used 1MB of 16MB Flash (0x01000000 - 0x00100000 = 0x00F00000)
//////////////////////////////////////////////////////////////////////////////////////
// |Boot|------- Slot1 ---------|--------- Slot2 ---------| Partition..| Settings | Logo | //
// |----|-----------------------|-------------------------|------------|----------|------| //
//////////////////////////////////////////////////////////////////////////////////////

#define FS_SYSTEM_ADDRESS  0x00000000
//#define FS_BASED_ADDRESS FS_SYSTEM_ADDRESS + 0x01EFFFFF   // Used for 32MB flash
#define FS_BASED_ADDRESS   FS_SYSTEM_ADDRESS    +  0x00A00000   // from 10MB
#define FS_LOGO_ADDRESS    FS_BASED_ADDRESS     +  0x00400000   // from 14MB
#define FS_SETTING_ADDRESS FS_LOGO_ADDRESS      +  0x00100000   // from 15MB

typedef enum SerializableObject_e
{
   // to save info about all other Objects, always at first place
   SERIALIZABLE_DESCRIPTORS = 0,
   SERIALIZABLE_PARTITION_1, // User settings
   SERIALIZABLE_PARTITION_2, // Rate table #1
   SERIALIZABLE_PARTITION_3, // Rate table #2
   SERIALIZABLE_PARTITION_4, // Rate table #3
   SERIALIZABLE_PARTITION_5, // Offline data
   SERIALIZABLE_OBJECTS_COUNT
}
SerializableObject_t;

typedef enum
{
   GLOGO_SPLASH,
   GLOGO_INFO,
   GLOGO_MENU
}
FS_Logo_t;

typedef struct SerializableDesciptor_s
{
   U32 address;          // relative to FS_BASED_ADDRESS
   U32 size;             // in bytes, not included size of signatures
   U32 compressed_size;  // in bytes, not included size of signatures
   char checksum [33];   // md5 checksum of real data (signatures excluded)
}
SerializableDesciptor_t;

// block (sector) size
//#define FS_BLOCK_SIZE 1024
//#define FS_FILE_NAME_MAX_LENGTH 16

//namespace GBS {
//namespace STM {
//namespace Framework {
//////////////////////////////////////////////////////////////////////////////////////
class GFileSystem : public GObject
{
private:
   void initialize();
   void initDescriptor();
   GFileSystem();
   ~GFileSystem();

   bool formatFileSystem();
   bool rawFwWrite(U32 offset, U32 writeSize, U8* buffer);
   bool rawRead(U32 offset, U32 readSize, U8* buffer, U32* actualReadSize, bool useRelativeOffset = true);
   bool rawWrite(U32 offset, U32 writeSize, U8* buffer, bool useRelativeOffset = true);

   bool writeDescriptor(int partition = -1); //partition < 0 --> save all partitions
   bool writeFirmware(std::string& inStr);
   bool readDescriptor();
   void dumpDescriptor();

   //return NULL if error
   bool writeSerializable(SerializableObject_t obj, U8* inBuf, int size);
   bool writeFlashData(SerializableObject_t obj, U8* inBuf, int size);
   U8* readSerializable(SerializableObject_t obj, U32& outlen);
   U8* readFlashData(SerializableObject_t obj, U32& outlen);

   U32 toSettingAddress(U32 romAddress);

   void getBitmapFromBuffer(U8* buffer, STGXOBJ_Bitmap_t* bitmap);

   U32 m_basedAddress;
   U32 m_flashBlockSize; //in bytes

   U32 m_firmwareVersionCode;
   char m_firmwareVersion[65];
   // here define maximum sizes of all partitions
   char SERIALIZABLE_NAME[SERIALIZABLE_OBJECTS_COUNT][32];
   int SERIALIZABLE_MAX_SIZE[SERIALIZABLE_OBJECTS_COUNT];
   bool m_isDescriptorRead;

   SerializableDesciptor_t m_descriptors[SERIALIZABLE_OBJECTS_COUNT];

public:
   static GFileSystem& getInstance();

   bool init();
   bool fastRawRead(U32 offset, U32 readSize, U8* buffer, U32* actualReadSize, bool useRelativeOffset = true);
   bool fastRawWrite(U32 offset, U32 writeSize, U8* buffer, bool useRelativeOffset = true);

   bool readPartitionFromFlash(int partition, U8*& data, U32& size);
   bool writeSavePartition(int partition, std::string& inStr);

   bool readSettings(U32 address, U8* Buffer, U8 NumOfByte);
   bool writeSettings(U32 address, U8* Buffer, U8 NumOfByte);

   bool getLogo(FS_Logo_t type, GAMLOAD_Object_t*& pGamObj);
   bool resetLogo(FS_Logo_t type);
   U32 toLogoAddress(FS_Logo_t type);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif /* GFILESYSTEM_HPP_ */
