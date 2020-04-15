//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2013 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: CommonData.cpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#include "CommonData.hpp"
#include "GDebug.hpp"

//namespace GBS {
//namespace STM {
//namespace Application {
//namespace Business {
//namespace DataController {

//////////////////////////////////////////////////////////////////////////////////////
CommonData::CommonData()
{
}

//////////////////////////////////////////////////////////////////////////////////////
CommonData::~CommonData()
{
}

//////////////////////////////////////////////////////////////////////////////////////
CommonData& CommonData::getInstance()
{
   static CommonData instance;
   return instance;
}
//////////////////////////////////////////////////////////////////////////////////////
//} //DataController
//} //Business
//} //Application
//} //STM
//} //GBS