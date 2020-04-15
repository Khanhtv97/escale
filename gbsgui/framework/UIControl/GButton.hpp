//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GButton.hpp                                                                //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GBUTTON_HPP_
#define GBUTTON_HPP_

#include "GFocusable.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GButton : public virtual GFocusable
{
protected:
   IExecutable* m_pOnPress;
   void actOnPressed();

public:
   GButton() {m_pOnPress = NULL;};
   GButton(const char* controlName) {m_pOnPress = NULL; m_name.assign(controlName);};

   const char* classname() {return "GButton";};

   bool processKey(U32 key);
   virtual void onPressed();
   void setOnPressed(IExecutable* pdelegate);
   void turnOnDrawBtn();
   void destroyViews()
   {
      GFocusable::destroyViews();
   }
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GBUTTON_HPP_ */
