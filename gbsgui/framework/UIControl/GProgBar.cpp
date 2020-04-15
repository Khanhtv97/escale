//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2013 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GProgBar.cpp                                                               //
//////////////////////////////////////////////////////////////////////////////////////

#include "GProgBar.hpp"
#include "GDebug.hpp"
#include "GError.hpp"
#include "GNavigator.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

#define BORDER_SPACING 2

//////////////////////////////////////////////////////////////////////////////////////
GProgBar::GProgBar(S32 width, S32 height)
{
   m_progSegment = 0;
   m_width = width;
   m_height = height;
   m_pProgress = NULL;
   m_pBackGround = NULL;
   m_pBackGround = new GStaticControl(NULL, 0, 0);
   m_pProgress = new GDynamicControl(new GFillRect(0,0,0,height,COLOR_FORM_BGL),0, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
GProgBar::~GProgBar()
{
   if( m_pProgress != NULL )
   {
      m_pProgress->destroyViews();
      delete m_pProgress;
      m_pProgress = NULL;
   }

   if( m_pBackGround != NULL )
   {
      m_pBackGround->destroyViews();
      delete m_pBackGround;
      m_pBackGround = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GProgBar::draw()
{
   if( m_pBackGround != NULL )
   {
      ((GDrawable*)m_pBackGround)->draw(layer(), this->x()-this->refX()+m_pBackGround->x(),this->y()-this->refY()+m_pBackGround->y());
   }

   if( m_pProgress != NULL )
   {
      ((GDrawable*)m_pProgress)->draw(layer(), this->x()-this->refX()+m_pProgress->x(),this->y()-this->refY()+m_pProgress->y());
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GProgBar::setProgress(int percent)
{
   if( percent < 0 )
   {
      percent = 0;
   }
   else if( percent > 100 )
   {
      percent = 100;
   }

   int tmpW = ((float)percent/100)*m_width;
   int tmpH;

   tmpW = (tmpW >= BORDER_SPACING) ? (tmpW - BORDER_SPACING) : 0;
   tmpH = (m_height >= BORDER_SPACING) ? (m_height - BORDER_SPACING) : 0;

   _DTraceError("GProgBar::setProgress %d", tmpW);

   if( m_pProgress != NULL )
   {
      m_pProgress->destroyViews();
      delete m_pProgress;
      m_pProgress = NULL;
   }

   m_pProgress = new GDynamicControl(new GFillRect(BORDER_SPACING / 2, BORDER_SPACING / 2, tmpW, tmpH, COLOR_FOCUS_BG), 0, 0);
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
