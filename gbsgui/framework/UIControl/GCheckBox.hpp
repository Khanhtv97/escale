//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GCheckBox.hpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GCHECKBOX_HPP_
#define GCHECKBOX_HPP_

#include "GButtonEx.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
// * GCheckBox class, all logic of this class has been implemented in GButtonEx
// * if there is no other GButtonEx with same groupID, it's will act as checkbox
//////////////////////////////////////////////////////////////////////////////////////
class GCheckBox : public GButtonEx
{
public:
   GCheckBox() {};
   GCheckBox(const char* controlName) {m_name.assign(controlName);};
   const char* classname() {return "GCheckBox";}
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GCHECKBOX_HPP_ */
