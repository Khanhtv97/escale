//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GDynamicControl.cpp                                                        //
//////////////////////////////////////////////////////////////////////////////////////

#include "GDynamicControl.hpp"
#include "GActivity.hpp"
#include "GDebug.hpp"
#include <typeinfo>

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
bool GDynamicControl::InstanceOfGDynamicControl(GStaticControl* control)
{
   GDynamicControl* tmp = dynamic_cast<GDynamicControl*> (control);
   if( tmp != NULL )
   {
      return true;
   }
   else
   {
      return false;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GDynamicControl& GDynamicControl::updateView(GView* view)
{
   if( (this->parent() != NULL) && (GActivity::InstanceOfGActivity(this->parent())) )
   {
      GActivity* container = dynamic_cast<GActivity*>(this->parent());
      //_DTraceDebug("GDynamicControl::updateView\n");
      container->forceControlRedraw(this);
      //_DTraceDebug("inside GDynamicControl w x h: %d x %d\n", this->w(), this->h());
   }

   GStaticControl::setView(view);
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GDynamicControl& GDynamicControl::updateView(GView* view, int posX, int posY)
{
   setPos(posX, posY);
   updateView(view);
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GDynamicControl& GDynamicControl::updateView(GView* view, int posX, int posY, int manualW, int manualH)
{
   setPos(posX, posY);
   setManualSize(manualW, manualH);
   updateView(view);
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
