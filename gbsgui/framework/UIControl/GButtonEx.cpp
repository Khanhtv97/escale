//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GButtonEx.cpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#include "GButtonEx.hpp"
#include "GKey.hpp"
#include "GActivity.hpp"
#include "GNavigator.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
bool GButtonEx::processKey(U32 key)
{
   // nothing to process here, something will be in derived class
   if( !this->m_isFocused )
   {
      return false;
   }

   switch( key )
   {
   case KEY_KB_ENTER:
   case KEY_RC_OK:
      setSelect(!this->m_isSelected);
      return true;
      break;

   default:
      break;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
void GButtonEx::setSelect(bool select)
{
   if( m_groupID < 0 )
   {
      // checkbox mode
      if( select != m_isSelected )
      {
         GNavigator::getInstance().pushUpdateRect(this->layer(), this);
      }
      GSelectable::setSelect(select);
      return;
   }

   //int foundControlWithSameGroup = 0;
   if( (this->parent() != NULL) && (GActivity::InstanceOfGActivity(this->parent())) )
   {
      GActivity* myParent = dynamic_cast<GActivity*>(this->parent());
      int i;

      for( int i = 0; i < myParent->m_controlCount; i++ )
      {
         if( (myParent->m_pControls[i] != this) &&
             (InstanceOfGSelectable(myParent->m_pControls[i])) )
         {
            GSelectable* control = dynamic_cast<GSelectable*>(myParent->m_pControls[i]);

            if( control->groupID() == this->m_groupID )
            {
               //foundControlWithSameGroup = 1;
               if( control->isSelected() )
               {
                  control->GSelectable::setSelect(false); // goi ham setSelect cua lop me de tranh bi de quy o day
                  GNavigator::getInstance().pushUpdateRect(control->layer(), control);
               }
            }
         }
      }
   }

   // radio button mode
   if( !m_isSelected )
   {
      GNavigator::getInstance().pushUpdateRect(this->layer(), this);
   }

   GSelectable::setSelect(true);
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
