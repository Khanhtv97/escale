//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GFocusable.hpp - component that has 2 states (focused, unfocused)          //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GFOCUSABLE_HPP_
#define GFOCUSABLE_HPP_

#include "GStaticControl.hpp"
#include "GDelegate.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GFocusable : public GStaticControl
{
private:
   //disable this
   using GStaticControl::setView;

protected:
   GView* m_pNormalView;
   GView* m_pFocusedView;

   bool m_isFocused;
   bool m_isEnabled;

   IExecutable* m_pOnFocusChanged;
   IExecutable* m_pOnEnableChanged;

   void init();

   virtual void onFocusChanged();
   virtual void onEnableChanged();

public:
   GFocusable();
   GFocusable(int layer);
   GFocusable(int x, int y);
   GFocusable(int layer, int x, int y);
   GFocusable(int x, int y, int ref_x, int ref_y);
   GFocusable(int layer, int x, int y, int ref_x, int ref_y);

   const char* classname() {return "GFocusable";};

   virtual GFocusable& setViews(GView* normalView, GView* focuseView);

   virtual bool isFocused();
   virtual bool isEnabled();

   virtual void setFocus(bool focus);
   virtual void setEnable(bool enable);
   void setOnFocus(IExecutable* pdelegate);
   void setOnEnable(IExecutable* pdelegate);

   void destroyViews();

   virtual bool processKey(U32 key);

   static bool InstanceOfGFocusable(GStaticControl* control);
   virtual ~GFocusable();
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GFOCUSABLE_HPP_ */
