//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2013 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: CommonData.hpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef COMMONDATA_HPP_
#define COMMONDATA_HPP_

#include "GObject.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace DataController {

//////////////////////////////////////////////////////////////////////////////////////
class CommonData : public GObject
{
public:
   static CommonData& getInstance();
   CommonData();
   ~CommonData();
};

//////////////////////////////////////////////////////////////////////////////////////
//} //DataController
//} //Business
//} //Application
//} //STM
//} //GBS
#endif //COMMONDATA_HPP_
