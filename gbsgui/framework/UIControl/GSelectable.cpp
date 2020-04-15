//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GSelectable.cpp                                                            //
//////////////////////////////////////////////////////////////////////////////////////

#include "GSelectable.hpp"
#include "GEvents.hpp"
#include "GDebug.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
bool GSelectable::InstanceOfGSelectable(GStaticControl* control)
{
   GSelectable* tmp = dynamic_cast<GSelectable*>(control);
   //_DTraceDebug("Control at 0x%08x is GButtonEx: %s\n",control, (tmp!=NULL)?"YES":"NO");
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
void GSelectable::init()
{
   m_pSelectedView = NULL;
   m_pFocusedSelectedView = NULL;
   m_pOnSelectChanged = NULL;
   m_isSelected = false;
   m_groupID = -1;
}

//////////////////////////////////////////////////////////////////////////////////////
GSelectable::GSelectable() : GFocusable()
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
GSelectable::GSelectable(int layer) : GFocusable(layer)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
GSelectable::GSelectable(int x, int y) : GFocusable(x, y)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
GSelectable::GSelectable(int layer, int x, int y) : GFocusable(layer, x, y)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
GSelectable::GSelectable(int x, int y, int ref_x, int ref_y) : GFocusable(x, y, ref_x, ref_y)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
GSelectable::GSelectable(int layer, int x, int y, int ref_x, int ref_y)
   : GFocusable(layer, x, y, ref_x, ref_y)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
void GSelectable::onFocusChanged()
{
   updateView();
}

//////////////////////////////////////////////////////////////////////////////////////
void GSelectable::onSelectChanged()
{
   updateView();
}

//////////////////////////////////////////////////////////////////////////////////////
GSelectable::~GSelectable()
{
}

//////////////////////////////////////////////////////////////////////////////////////
GSelectable& GSelectable::setViews(GView* normalView, GView* focusedView,
                                 GView* selectedView, GView* focusedSelectedView)
{
   this->m_pNormalView = normalView;
   this->m_pFocusedView = focusedView;
   this->m_pSelectedView = selectedView;
   this->m_pFocusedSelectedView = focusedSelectedView;

   updateView();

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

   if( m_pSelectedView != NULL )
   {
      if( this->w() < m_pSelectedView->w() )
      {
         this->w() = m_pSelectedView->w();
      }

      if( this->h() < m_pSelectedView->h() )
      {
         this->h() = m_pSelectedView->h();
      }
   }

   if( m_pFocusedSelectedView != NULL )
   {
      if( this->w() < m_pFocusedSelectedView->w() )
      {
         this->w() = m_pFocusedSelectedView->w();
      }

      if( this->h() < m_pFocusedSelectedView->h() )
      {
         this->h() = m_pFocusedSelectedView->h();
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GSelectable::updateView()
{
   // to be overridden
   if( this->m_isFocused )
   {
      if( this->m_isSelected )
      {
         this->m_pView = this->m_pFocusedSelectedView;
      }
      else
      {
         this->m_pView = this->m_pFocusedView;
      }
   }
   else
   {
      if( this->m_isSelected )
      {
         this->m_pView = this->m_pSelectedView;
      }
      else
      {
         this->m_pView = this->m_pNormalView;
      }
   }

   //_DTraceDebug("_onFocusChanged of 0x%08x changed to %s\n", this, m_isFocused?"TRUE":"FALSE");
}

//////////////////////////////////////////////////////////////////////////////////////
bool GSelectable::isSelected()
{
   return m_isSelected;
}

//////////////////////////////////////////////////////////////////////////////////////
void GSelectable::setSelect(bool select)
{
   if( select == this->m_isSelected )
   {
      return;
   }
   _DTraceDebug("%s %s (#%d) set select to - %s, m_pOnSelectChanged = 0x%08x", this->classname(), name(), id(), select ? "true":"false", m_pOnSelectChanged);

   this->m_isSelected = select;
   this->onSelectChanged();

   if( this->m_pOnSelectChanged != NULL )
   {
      m_pOnSelectChanged->execute(GEVENT_SELECT_CHANGED, this, NULL);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GSelectable::setOnSelect(IExecutable* pdelegate)
{
   this->m_pOnSelectChanged = pdelegate;
}

//////////////////////////////////////////////////////////////////////////////////////
void GSelectable::destroyViews()
{
   GFocusable::destroyViews();

   if( m_pSelectedView != NULL )
   {
      m_pSelectedView->destroyViews();
      delete m_pSelectedView;
      m_pSelectedView = NULL;
   }

   if( m_pFocusedSelectedView != NULL )
   {
      m_pFocusedSelectedView->destroyViews();
      delete m_pFocusedSelectedView;
      m_pFocusedSelectedView = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
int GSelectable::groupID()
{
   return m_groupID;
}

//////////////////////////////////////////////////////////////////////////////////////
void GSelectable::setGroupID(int id)
{
   m_groupID = id;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
