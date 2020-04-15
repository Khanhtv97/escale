//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: UsbDevice.hpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef USBDEVICE_H_
#define USBDEVICE_H_

#include "stapp_main.h"
#include "GEvents.hpp"
#include <list>

//Corresponding to OS plus 3.2.5
enum UsbDeviceType
{
   USB_DEVICE_UNKNOW = 0,
   USB_DEVICE_AUDIO,
   USB_DEVICE_COMM,
   USB_DEVICE_ETHERNET,
   USB_DEVICE_HID,
   USB_DEVICE_HOTTOHOT,
   USB_DEVICE_HUB,
   USB_DEVICE_STORAGE,
   USB_DEVICE_SERIAL,
   USB_DEVICE_VOIP
};

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace UsbManager {
//////////////////////////////////////////////////////////////////////////////////////
class UsbDevice
{
public:
   UsbDevice();
   UsbDevice(UsbDeviceType type);
   virtual ~UsbDevice();

   virtual bool init() = 0;
   virtual bool deInit() = 0;
   virtual bool mount() = 0;
   virtual bool unmount() = 0;
   virtual bool isMounted() = 0;
   virtual bool isPlugged() = 0;
   virtual bool isReady() = 0;

   UsbDeviceType getType();

private:
   UsbDeviceType m_type;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UsbManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //USBDEVICE_H_
