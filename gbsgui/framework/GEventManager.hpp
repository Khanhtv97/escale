//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GEventManager.hpp                                                          //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GEVENTMANAGER_HPP_
#define GEVENTMANAGER_HPP_

#include "GObject.hpp"
#include "GEvents.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
class GEventManager : public GObject
{
private:
   GEventManager();
   ~GEventManager();

public:
   static GEventManager& getInstance();
   void fire(GEvents event, void* pEvtData = NULL);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif // GEVENTMANAGER_HPP_
