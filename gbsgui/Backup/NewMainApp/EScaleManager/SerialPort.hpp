//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: SerialPort.hpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef SERIALPORT_H_
#define SERIALPORT_H_

#include "stapp_main.h"
#include "GDebug.hpp"

#define SERIAL_BUFFER_SIZE  1024
#define DATA_ITEM_QUEUE_MAX 1024

enum SerialPortName
{
   SPN_COM1 = 0, //Configure PIO11[0,1] - used for Scale
   SPN_COM2 = 1, //Configure PIO02[6,7] - used for Barcode Reader
   SPN_COM3 = 2, //Configure PIO03[5,6] - used for Connection (3g/Wifi)
   SPN_COM4 = 3, //Configure PIO15[4,5] - used for Printer
   SPN_NONE
};

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////
class SerialPort
{
public:
   virtual bool init() = 0;
   virtual bool deinit() = 0;
   virtual bool isReady() = 0;

   SerialPortName getPortName();

protected:
   SerialPort();
   SerialPort(SerialPortName portName);
   virtual ~SerialPort();

   void readBytes(U8* buffer, U32& oNumberRead);
   void writeBytes(U8* buffer, U32 buffLengh, U32& oNumberWritten);

private:
   SerialPortName m_portName;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //SERIALPORT_H_
