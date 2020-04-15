//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GDynamicControl.hpp                                                        //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GDYNAMICCONTROL_HPP_
#define GDYNAMICCONTROL_HPP_

#include "GStaticControl.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GDynamicControl : public GStaticControl
{
public:
   GDynamicControl() : GStaticControl() {};
   GDynamicControl(GDrawable* drawable, int x = 0, int y =0) : GStaticControl(drawable, x, y){};
   static bool InstanceOfGDynamicControl(GStaticControl* control);

   //updateView need be called after setManualSize and setPos if any
   virtual GDynamicControl& updateView(GView* view);
   virtual GDynamicControl& updateView(GView* view, int posX, int posY);
   virtual GDynamicControl& updateView(GView* view, int posX, int posY, int manualW, int manualH);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GDYNAMICCONTROL_HPP_ */
