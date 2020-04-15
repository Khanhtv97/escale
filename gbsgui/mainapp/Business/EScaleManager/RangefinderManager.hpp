//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2014 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: RangefinderManager.hpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef RANGEMANAGER_H_
#define RANGEMANAGER_H_

#include "LaserSensor.hpp"
#include <queue>

#include "Json/json.h"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace EScaleManager {
//////////////////////////////////////////////////////////////////////////////////////

class RangefinderManager
{
public:
   static RangefinderManager& getInstance();

   bool init();
   bool deinit();

protected:

private:
   RangefinderManager();
   virtual ~RangefinderManager();

   LaserSensor m_laserSensorL;
   LaserSensor m_laserSensorW;
   LaserSensor m_laserSensorH;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //EScaleManager
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //RANGEMANAGER_H_
