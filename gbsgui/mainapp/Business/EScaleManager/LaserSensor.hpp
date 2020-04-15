//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: LaserSensor.hpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef LASERSENSOR_H_
#define LASERSENSOR_H_

#include "SerialPort.hpp"
#include "GSystem.hpp"
#include "GUserSettings.hpp"
#include "GStringUtils.hpp"
#include "JsonParser.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////
class LaserSensor : public SerialPort
{
public:
   virtual bool init();
   virtual bool deinit();
   virtual bool isReady();

   LaserSensor();
   LaserSensor(SerialPortName portName);
   LaserSensor(SerialPortName portName, U8 id);
   virtual ~LaserSensor();

   void setId(U8 id);
   U8 getId();

private:
   U8 m_id;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //LASERSENSOR_H_
