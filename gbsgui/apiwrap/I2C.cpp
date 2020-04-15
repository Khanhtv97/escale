//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2013 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: I2C.cpp                                                                    //
//////////////////////////////////////////////////////////////////////////////////////

#include "I2C.hpp"
#include "GDebug.hpp"
#if defined(I2C_MAX_NUMBER)
#include "sti2c.h"
#endif

#if defined(I2C_MAX_NUMBER)

//////////////////////////////////////////////////////////////////////////////////////
//  Name:        GI2C_Write
//  Description: Performs a write on one of the I2C device
//////////////////////////////////////////////////////////////////////////////////////
BOOL GI2C_Write(U16 address, U8* Buff, U8 NumToWrite)
{
   BOOL           Error = TRUE;
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   S32            SlaveAddress, I2CPort, Token;
   U8             BufferWrite[66];
   U32            NumWritten;
   U16            AddressWrite;
   STI2C_OpenParams_t I2C_OpenParams;
   STI2C_Params_t     I2C_Params;
   STI2C_Handle_t     I2C_EepromHandle;

   // Now perform the write transaction on
   I2C_OpenParams.BusAccessTimeOut = 100000;
   I2C_OpenParams.AddressType      = STI2C_ADDRESS_7_BITS;
   I2C_OpenParams.I2cAddress       = (U16)EEPROM_WRITE_ADDRESS;
   I2C_OpenParams.BaudRate         = STI2C_RATE_NORMAL;

   ErrCode = STI2C_Open(I2C_DeviceName[EEPROM_PORT], &I2C_OpenParams, &I2C_EepromHandle);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("I2C_Write --> Unable to open i2c handle %d !\n",EEPROM_PORT);
      return FALSE;
   }

   BufferWrite[0] = (address >> 8) & 0x7F;
   BufferWrite[1] = (U8) (address & 0xFF);

   for( int i = 0; i < NumToWrite; i++ )
   {
      BufferWrite[i+2] = Buff[i];
   }

   NumToWrite += 2;

   ErrCode = STI2C_Write(I2C_EepromHandle, BufferWrite, NumToWrite, 0, &NumWritten);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("I2C_Write --> Unable to write on i2c !\n");
      Error = FALSE;
   }

   ErrCode = STI2C_Close(I2C_EepromHandle);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("I2C_Write --> Unable to close i2c handle !\n");
      Error = FALSE;
   }

   if( Error == TRUE )
   {
      _DTraceError("I2C has sent %d characters\n",NumWritten);
   }

   return Error;
}

//////////////////////////////////////////////////////////////////////////////////////
//  Name:        GI2C_Read
//  Description: Performs a read on one of the I2C device
//////////////////////////////////////////////////////////////////////////////////////
BOOL GI2C_Read(U16 address, U8* BufferRead, U8 _numRead)
{
   BOOL               Error = TRUE;
   ST_ErrorCode_t     ErrCode = ST_NO_ERROR;
   U8                 BufferWrite[4];
   S32                NumToRead, NumToWrite, SlaveAddress, i, j, I2CPort;
   U32                NumRead, NumWritten;
   STI2C_OpenParams_t I2C_OpenParams;
   STI2C_Params_t     I2C_Params;
   STI2C_Handle_t     I2C_EepromHandle;

   // Now perform the write transaction on
   I2C_OpenParams.BusAccessTimeOut = 100000;
   I2C_OpenParams.AddressType      = STI2C_ADDRESS_7_BITS;
   I2C_OpenParams.I2cAddress       = (U16)EEPROM_WRITE_ADDRESS;
   I2C_OpenParams.BaudRate         = STI2C_RATE_NORMAL;

   ErrCode = STI2C_Open(I2C_DeviceName[EEPROM_PORT], &I2C_OpenParams, &I2C_EepromHandle);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("I2C_Read --> Unable to open i2c handle %d !\n",EEPROM_PORT);
      return FALSE;
   }

   BufferWrite[0] = (address >> 8) & 0x7F;
   BufferWrite[1] = (U8) (address & 0xFF);

   ErrCode = STI2C_WriteNoStop(I2C_EepromHandle, BufferWrite, 2, 0, &NumWritten);

   if( ErrCode != ST_NO_ERROR )
   {
      STI2C_Close(I2C_EepromHandle);
      _DTraceError("I2C_Read --> Unable to write on i2c !\n");
      return FALSE;
   }

   I2C_Params.BusAccessTimeOut = 100;
   I2C_Params.AddressType      = STI2C_ADDRESS_7_BITS;
   I2C_Params.I2cAddress       = EEPROM_READ_ADDRESS;

   ErrCode = STI2C_SetParams(I2C_EepromHandle, &I2C_Params);

   if( ErrCode != ST_NO_ERROR )
   {
      STI2C_Close(I2C_EepromHandle);
      _DTraceError("I2C_Read--> Unable to set i2c params !\n");
      return FALSE;
   }

   ErrCode = STI2C_Read(I2C_EepromHandle, BufferRead, _numRead, 0, &NumRead);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("I2C_Read--> Unable to read on i2c !\n");
      Error = FALSE;
   }

   ErrCode = STI2C_Close(I2C_EepromHandle);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("I2C_Read--> Unable to close i2c handle !\n");
      Error = FALSE;
   }

   if( Error == TRUE )
   {
      _DTraceError("Received %d bytes :\n",NumRead);
   }

   return Error;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GI2C_Dump()
{
   BOOL               Error = FALSE;
   ST_ErrorCode_t     ErrCode = ST_NO_ERROR;
   STI2C_OpenParams_t I2C_OpenParams;
   STI2C_Handle_t     I2C_Handle;
   U8                 BufferRead[64],BufferWrite[4];
   U8                 i,_numRead;
   U32                NumRead,NumWritten;
   STI2C_Handle_t     I2C_EepromHandle;
   STI2C_Params_t     I2C_Params;

   _numRead = 64;

   // Now perform the write transaction on
   I2C_OpenParams.BusAccessTimeOut = 100000;
   I2C_OpenParams.AddressType      = STI2C_ADDRESS_7_BITS;
   I2C_OpenParams.I2cAddress       = (U16)EEPROM_WRITE_ADDRESS;
   I2C_OpenParams.BaudRate         = STI2C_RATE_NORMAL;

   ErrCode = STI2C_Open(I2C_DeviceName[EEPROM_PORT], &I2C_OpenParams, &I2C_EepromHandle);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("I2C_Read --> Unable to open i2c handle %d !\n",EEPROM_PORT);
      return(TRUE);
   }

   BufferWrite[0] = 0;
   BufferWrite[1] = 0;

   ErrCode = STI2C_WriteNoStop(I2C_EepromHandle, BufferWrite, 2, 0, &NumWritten);

   if( ErrCode != ST_NO_ERROR )
   {
      STI2C_Close(I2C_EepromHandle);
      _DTraceError("I2C_Read --> Unable to write on i2c !\n");
      return(TRUE);
   }

   I2C_Params.BusAccessTimeOut = 100;
   I2C_Params.AddressType      = STI2C_ADDRESS_7_BITS;
   I2C_Params.I2cAddress       = EEPROM_READ_ADDRESS;

   ErrCode = STI2C_SetParams(I2C_EepromHandle, &I2C_Params);

   if( ErrCode != ST_NO_ERROR )
   {
      STI2C_Close(I2C_EepromHandle);
      _DTraceError("I2C_Read--> Unable to set i2c params !\n");
      return(TRUE);
   }

   for( i = 0; i < 512; i++ )
   {
      ErrCode = STI2C_Read(I2C_EepromHandle, BufferRead, _numRead, I2C_BUS_RD_TIMEOUT, &NumRead);

      if( ErrCode != ST_NO_ERROR )
      {
         _DTraceError("I2C_Read--> Unable to read on i2c !\n");
         Error = TRUE;
      }

      for( int k = 0, j = 1; k < NumRead; k++ )
      {
         printf("%02X ",BufferRead[k]);
         if (j==16) {  printf("\n"); j=0; }
         j++;
      }
   }

   ErrCode = STI2C_Close(I2C_EepromHandle);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("I2C_Read--> Unable to close i2c handle !\n");
      Error = TRUE;
   }
   return Error;
}
#endif
//////////////////////////////////////////////////////////////////////////////////////
