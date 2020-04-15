//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GDelegate.cpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#include "GDelegate.hpp"

// IMPORTANT: G++ requires requires template implementation in the same file as header
// therefore implementation cannot be here, it's in .hpp file

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
//template <class CLASS>
//GDelegate<CLASS>::GDelegate(CLASS* obj, void (CLASS::*method)(EVENT_HANDLER_PARAM), void* data)
//{
//	this->containerObject = obj;
//	this->methodToExecute = method;
//	this->_data = data;
//}

//////////////////////////////////////////////////////////////////////////////////////
//template <class CLASS>
//GDelegate<CLASS>::~GDelegate()
//{
//	//nothing to do
//}

//////////////////////////////////////////////////////////////////////////////////////
//template <class CLASS>
//void GDelegate<CLASS>::execute(EVENT_HANDLER_PARAM)
//{
//	if (event_data==NULL) event_data = this->_data;
//
//	(containerObject->*methodToExecute)(event, sender, event_data);
//}

//////////////////////////////////////////////////////////////////////////////////////
//template <class CLASS>
//GDelegate<CLASS>* GDelegate<CLASS>::create(CLASS* obj, void (CLASS::*method)(EVENT_HANDLER_PARAM), void* data)
//{
//	return new GDelegate<CLASS>(obj, method, data);
//}

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
