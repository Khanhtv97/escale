//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: SerialPort.cpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#include "SerialPort.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////
SerialPort::SerialPort()
{
   m_portName = SPN_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////
SerialPort::SerialPort(SerialPortName portName)
{
   m_portName = portName;
}

//////////////////////////////////////////////////////////////////////////////////////
SerialPort::~SerialPort()
{
}

//////////////////////////////////////////////////////////////////////////////////////
void SerialPort::readBytes(U8* buffer, U32& oNumberRead)
{
   if( m_portName >= SPN_COM1 && m_portName < SPN_NONE )
   {
      UART_Read((U32)m_portName, buffer, SERIAL_BUFFER_SIZE-1, 3, &oNumberRead);

      if( oNumberRead > 0 )
      {
         //the buffer should be deleted after used
         buffer[oNumberRead] = 0;
      }
   }
   else
   {
      oNumberRead = 0;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void SerialPort::writeBytes(U8* buffer, U32 buffLengh, U32& oNumberWritten)
{
   if( m_portName >= SPN_COM1 && m_portName < SPN_NONE )
   {
      //UART_Write((U32)m_portName, buffer, buffLengh, 1, numberWritten);
      U32 totalWritten = 0;
      for( U32 i = 0; i < buffLengh; ++i )
      {
         U32 numWritten = 0;
         U8 data = buffer[i];
         UART_Write((U32)m_portName, &data, 1, 10, &numWritten);
         totalWritten += numWritten;
      }
      oNumberWritten = totalWritten;
   }
   else
   {
      oNumberWritten = 0;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
SerialPortName SerialPort::getPortName()
{
   return m_portName;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
