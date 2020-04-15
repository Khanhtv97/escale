//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GFocusable.cpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#include "GFocusable.hpp"
#include "GEvents.hpp"
#include "GDebug.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
bool GFocusable::InstanceOfGFocusable(GStaticControl* control)
{
   GFocusable* tmp = dynamic_cast<GFocusable*>(control);
   //_DTraceDebug("Object at 0x%08x is Focusable: %s\n",control, (tmp!=NULL)?"YES":"NO");
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
void GFocusable::init()
{
   m_pFocusedView = NULL;
   m_pNormalView = NULL;
   m_pOnEnableChanged = NULL;
   m_pOnFocusChanged = NULL;
   m_isFocused = false;
   m_isEnabled = true;
   //TODO: check if "this" pointer has been initialized here??
   //m_pOnEnableChanged = new GDelegate<GFocusable>(this, &GFocusable::onEnableChanged);
   //m_pOnFocusChanged = new GDelegate<GFocusable>(this, &GFocusable::onFocusChanged);
}

//////////////////////////////////////////////////////////////////////////////////////
GFocusable::~GFocusable()
{
   //delete m_pOnEnableChanged;
   //delete m_pOnFocusChanged;
}

//////////////////////////////////////////////////////////////////////////////////////
GFocusable::GFocusable() : GStaticControl()
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
GFocusable::GFocusable(int layer) : GStaticControl(layer)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
GFocusable::GFocusable(int x, int y) : GStaticControl(x, y)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
GFocusable::GFocusable(int layer, int x, int y) : GStaticControl(layer, x, y)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
GFocusable::GFocusable(int x, int y, int ref_x, int ref_y)
   : GStaticControl(x, y, ref_x, ref_y)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
GFocusable::GFocusable(int layer, int x, int y, int ref_x, int ref_y)
   : GStaticControl(layer, x, y, ref_x, ref_y)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFocusable::isFocused()
{
   return m_isFocused;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFocusable::isEnabled()
{
   return m_isEnabled;
}

//////////////////////////////////////////////////////////////////////////////////////
void GFocusable::setFocus(bool focus)
{
   if( focus == this->m_isFocused )
   {
      return;
   }

   this->m_isFocused = focus;
   this->onFocusChanged();

   if( this->m_pOnFocusChanged != NULL )
   {
      m_pOnFocusChanged->execute(GEVENT_FOCUS_CHANGED, this, NULL);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GFocusable::setEnable(bool enable)
{
   if( enable == this->m_isEnabled )
   {
      return;
   }

   this->m_isEnabled = enable;
   this->onEnableChanged();

   if( this->m_pOnEnableChanged != NULL )
   {
      m_pOnEnableChanged->execute(GEVENT_ENABLE_CHANGED, this, NULL);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GFocusable& GFocusable::setViews(GView* normalView, GView* focuseView)
{
   this->m_pNormalView = normalView;
   this->m_pFocusedView = focuseView;
   this->m_pView = m_isFocused ? m_pFocusedView : m_pNormalView;

   if( m_pNormalView != NULL )
   {
      this->w() = m_pNormalView->w();
      this->h() = m_pNormalView->h();
   }

   if( m_pFocusedView != NULL )
   {
      if( this->w() < m_pFocusedView->w() )
      {
         this->w() = m_pFocusedView->w();
      }

      if( this->h() < m_pFocusedView->h() )
      {
         this->h() = m_pFocusedView->h();
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool GFocusable::processKey(U32 key)
{
   // nothing to process here, something will be in derived class
   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
void GFocusable::onFocusChanged()
{
   // to be overridden
   if( this->m_isFocused )
   {
      this->m_pView = this->m_pFocusedView;
   }
   else
   {
      this->m_pView = this->m_pNormalView;
   }

   //_DTraceDebug("_onFocusChanged of 0x%08x changed to %s\n", this, m_isFocused?"TRUE":"FALSE");
   if( m_pFocusedView != m_pNormalView )
   {
      // this should be done externaly by GActivity
      // this->update();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GFocusable::onEnableChanged()
{
   // to be overridden
}

//////////////////////////////////////////////////////////////////////////////////////
void GFocusable::setOnFocus(IExecutable* pdelegate)
{
   this->m_pOnFocusChanged = pdelegate;
}

//////////////////////////////////////////////////////////////////////////////////////
void GFocusable::setOnEnable(IExecutable* pdelegate)
{
   this->m_pOnEnableChanged = pdelegate;
}

void GFocusable::destroyViews()
{
   if( m_pNormalView != NULL )
   {
      m_pNormalView->destroyViews();
      delete m_pNormalView;
      m_pNormalView = NULL;
   }

   if( m_pFocusedView != NULL )
   {
      m_pFocusedView->destroyViews();
      delete m_pFocusedView;
      m_pFocusedView = NULL;
   }

   m_pView = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
