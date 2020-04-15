//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2013 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: I2C.hpp                                                                    //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef I2C_HPP_
#define I2C_HPP_

#include "stapp_main.h"

#define EEPROM_WRITE_ADDRESS 0xA0
#define EEPROM_READ_ADDRESS  0xA1
#define EEPROM_PORT          1

// standard defines used in this driver
#if defined(I2C_MAX_NUMBER)
#define I2C_BUS_RD_TIMEOUT 1000 // Read access timeout in ms units
#define I2C_BUS_WR_TIMEOUT 1000 // Write access timeout in ms units
#endif

#ifdef __cplusplus
    extern "C" {
#endif

//BOOL I2C_Detect();
//BOOL I2C_EepromInit();
BOOL GI2C_Read(U16 address, U8* Buffer, U8 NumtoRead);
BOOL GI2C_Write(U16 address, U8* Buffer, U8 NumtoWrite);
BOOL GI2C_Dump();
#ifdef __cplusplus
    }
#endif

#endif
