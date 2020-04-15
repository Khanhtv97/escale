//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GButtonEx.hpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////
#ifndef GBUTTONEX_HPP_
#define GBUTTONEX_HPP_

#include "GSelectable.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GButtonEx : public GSelectable
{
public:
   GButtonEx() {};
   GButtonEx(const char* controlName) {m_name.assign(controlName);};

   const char* classname() {return "GButtonEx";}
   bool processKey(U32 key);
   virtual void setSelect(bool select);

   void destroyViews()
   {
      GSelectable::destroyViews();
   }

};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GBUTTONEX_HPP_ */
