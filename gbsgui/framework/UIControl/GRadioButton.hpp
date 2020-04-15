//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GRadioButton.cpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GRADIOBUTTON_HPP_
#define GRADIOBUTTON_HPP_

#include "GButtonEx.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
// * GRadioButton class, all logic of this class has been implemented in GButtonEx
// * if there is any GButtonEx with same groupID, it's will act as radio button
//////////////////////////////////////////////////////////////////////////////////////
class GRadioButton : public GButtonEx
{
public:
   GRadioButton() : GButtonEx() {};
   GRadioButton(int groupID) : GButtonEx()   {this->m_groupID = groupID;};
   GRadioButton(int groupID, const char* controlName)    {this->m_groupID = groupID; m_name.assign(controlName);};
   const char* classname() {return "GRadioButton";};
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GRADIOBUTTON_HPP_ */
