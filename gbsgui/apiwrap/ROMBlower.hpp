//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: ROMBlower.hpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef ROM_BLOWER_API_HPP_
#define ROM_BLOWER_API_HPP_

#include "stapp_main.h"
#include "GDebug.hpp"

typedef enum
{
   GROMB_ERR_NONE = 0,
   GROMB_ERR_NONE_BLD,
   GROMB_ERR_NONE_BLD_LINUX,
   GROMB_ERR_FLASH_ID,
   GROMB_ERR_FLASH_PARAM,
   GROMB_ERR_FLASH_SIZE,
   GROMB_ERR_FILE_SOURCE,
   GROMB_ERR_FILE_DEST,
   GROMB_ERR_FILE_SIZE,
   GROMB_ERR_FILE_BAD_BLOCK,
   GROMB_ERR_FILE_FORMAT,
   GROMB_ERR_FILE_SIGN,
   GROMB_ERR_MEMORY,
}
GROMError;

typedef enum
{
   GIMAGE_BOOTLOADER = 0,
   GIMAGE_BOOTLOADER_LINUX,
   GIMAGE_APPLICATION,
   GIMAGE_APPLICATION_STSTREAM,
   GIMAGE_USER_DATA,
   GIMAGE_APPLICATION_INVALID,
   GIMAGE_LOGO_SPLASH,
   GIMAGE_LOGO_INFO,
   GIMAGE_LOGO_MENU,
   GIMAGE_UNKNOWN
}
GIMAGEType;

#define ROMB_SIGNATURE_LEN          32
#define ROMB_LOGOSPLASH_SIGNATURE   "_GBS_ST_IRD_IMAGE_LOGO_SPLA___"
#define ROMB_LOGOINFO_SIGNATURE     "_GBS_ST_IRD_IMAGE_LOGO_INFO___"
#define ROMB_LOGOMENU_SIGNATURE     "_GBS_ST_IRD_IMAGE_LOGO_MENU___"

//namespace GBS {
//namespace STM {
//namespace API {

//////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
   extern "C"
   {
#endif

   GIMAGEType GROM_DetectImageType(const std::string sourceFile, int& signaturePos, char*& pSignature);

   //GROM_BurnFlash: Program routine for binary file
   GROMError GROM_BurnFlash(const U32 flashId, const U32 fromAddress, U32 sizeProgram,
                            const std::string sourceFile, const GIMAGEType imageType,
                            const U32 signaturePos, char* pSignature);

   //GROM_DumpFlash: Dump routine to read flash to a binary fyle
   GROMError GROM_DumpFlash(const U32 flashId, const U32 fromAddress, U32 sizeDump, const std::string destinationFile);

   //GROM_EraseFlash: Program routine for empty
   GROMError GROM_EraseFlash(const U32 flashId, const U32 fromAddress, U32 sizeErasing, BOOL isEraseAll);

   //GROM_DumpFlash: sign on file
   GROMError GROM_SignOnFile(const std::string sourceFile, GIMAGEType fileType);

#ifdef __cplusplus
   }
#endif

//////////////////////////////////////////////////////////////////////////////////////
//} //API
//} //STM
//} //GBS
#endif //ROM_BLOWER_API_HPP_
