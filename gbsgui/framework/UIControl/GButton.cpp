//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GButton.cpp                                                                //
//////////////////////////////////////////////////////////////////////////////////////

#include "GButton.hpp"
#include "GKey.hpp"
#include "GDebug.hpp"
#include "GEvents.hpp"
#include "GNavigator.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
bool GButton::processKey(U32 key)
{
   // nothing to process here, something will be in derived class
   if( !this->m_isFocused )
   {
      //return GFocusable::processKey(key);
      return false;
   }

   switch( key )
   {
   case KEY_KB_ENTER:
   case KEY_RC_OK:
      this->actOnPressed();
      return true;
      break;

   default:
      break;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
void GButton::actOnPressed()
{
   this->onPressed();

   if( this->m_pOnPress != NULL )
   {
      m_pOnPress->execute(GEVENT_BUTTON_PRESSED, this, NULL);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GButton::setOnPressed( IExecutable* pdelegate)
{
   this->m_pOnPress = pdelegate;
}

//////////////////////////////////////////////////////////////////////////////////////
void GButton::onPressed()
{
   // to be overridden
   //_DTraceDebug("%s #%d pressed\n", this->classname(), this->id());
}

//////////////////////////////////////////////////////////////////////////////////////
void GButton::turnOnDrawBtn()
{
   //GNavigator::getInstance()->pushUpdateRect(
   //      0,
   //      this->x() - this->refX(),
   //      this->y() - this->refY(),
   //      this->manualSize().w,this->manualSize().h);

   GNavigator::getInstance().pushUpdateRect(
         1,
         this->x() - this->refX(),
         this->y() - this->refY(),
         this->manualSize().w + 4,
         this->manualSize().h               );
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
