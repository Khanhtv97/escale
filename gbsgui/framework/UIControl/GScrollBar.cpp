//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GScrollBar.cpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#include "GScrollBar.hpp"
#include "GError.hpp"
#include "GDebug.hpp"
#include "GNavigator.hpp"


//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
GScrollBar::GScrollBar(int layer)
{
   init();
   m_layer = layer;
}

//////////////////////////////////////////////////////////////////////////////////////
GScrollBar::GScrollBar(int layer, GBitmap* trackBmp, GBitmap* thumbBmp)
{
   m_layer = layer;
   init();
   setTrack(trackBmp);
   setThumb(thumbBmp);
}

//////////////////////////////////////////////////////////////////////////////////////
GScrollBar::GScrollBar(GBitmap* trackBmp, GBitmap* thumbBmp)
{
   init();
   setTrack(trackBmp);
   setThumb(thumbBmp);
}

//////////////////////////////////////////////////////////////////////////////////////
void GScrollBar::init()
{
   m_pTrack = NULL;
   m_pThumb = NULL;
   m_scroll = 0;
   m_trackLength = 0;
   m_orientation = SCROLLBAR_ORIENTATION_VERTICAL;
   m_isParentFocused = false;
}

//////////////////////////////////////////////////////////////////////////////////////
GScrollBar& GScrollBar::setTrack(GBitmap* trackBmp)
{
   m_pTrack = trackBmp;

   if( m_pTrack != NULL )
   {
      this->w() = m_pTrack->w(); this->h() = m_pTrack->h();
      m_trackLength = m_orientation==SCROLLBAR_ORIENTATION_VERTICAL?m_pTrack->h():m_pTrack->w();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GScrollBar& GScrollBar::setThumb(GBitmap* thumbBmp)
{
   m_pThumb = thumbBmp;
   if( (m_pTrack == NULL) && (m_pThumb != NULL) )
   {
      this->w() = m_pThumb->w(); this->h() = m_pThumb->h();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GScrollBar& GScrollBar::setOrientaton(ScrollBar_Orientation_t orientation)
{
   m_orientation = orientation;
}

//////////////////////////////////////////////////////////////////////////////////////
GScrollBar& GScrollBar::setTrackLength(int length)
{
   m_trackLength = length;

   if( m_orientation==SCROLLBAR_ORIENTATION_VERTICAL )
   {
      this->h() = m_trackLength;
   }
   else
   {
      this->w() = m_trackLength;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GScrollBar::draw()
{
   //_DTraceError("GScrollBar::draw() \n");
   if( m_pThumb == NULL )
   {
      _DTraceWarning("(%s)\t\t#%d, bitmap for 'thumb' not set yet\n", classname(), this->id());
      return GERROR_NOT_FOUND;
   }

   //updateBar(m_scroll);

   //TODO: optimize this
   int l = this->layer();

   int offetX = this->x() - this->refX();
   int offetY = this->y() - this->refY();

   if( m_pTrack != NULL )
   {
      ((GDrawable*)m_pTrack)->draw(l, offetX + m_pTrack->x(), offetY + m_pTrack->y());
   }

   ((GDrawable*)m_pThumb)->draw(l, offetX + m_pThumb->x(), offetY + m_pThumb->y());
   //_DTraceDebug("(%s)\t\t#%d, DRAW\n", classname(), m_id);

   return GERROR_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GScrollBar::scroll(int percent)
{
   if( !this->m_isParentFocused )
   {
      _DTraceError("list Parent is not focused, not scroll\n");
      return GERROR_NONE;
   }

   if( (m_pThumb == NULL) || (m_trackLength <=0) )
   {
      _DTraceWarning("(%s)\t\t#%d, bitmap for 'thumb' not set yet\n", classname(), this->id());
      return GERROR_NOT_FOUND;
   }

   if( percent < 0 )
   {
      percent = 0;
   }

   if( percent > 100 )
   {
      percent = 100;
   }

   int xoff = 0, yoff = 0;

   if( m_pTrack != NULL )
   {
      xoff = m_pTrack->x();
      yoff = m_pTrack->y();
   }

   m_scroll = percent;
   if( m_orientation == SCROLLBAR_ORIENTATION_VERTICAL )
   {
      if( percent == 0 )
      {
         m_pThumb->y() = yoff;
      }
      else if( percent == 100 )
      {
         m_pThumb->y() = yoff + (m_trackLength - m_pThumb->h());
      }
      else
      {
         m_pThumb->y() = yoff + (m_trackLength - m_pThumb->h()) * percent / 100;
      }
   }
   else
   {
      if( percent == 0 )
      {
         m_pThumb->x() = xoff;
      }
      else if( percent == 100 )
      {
         m_pThumb->x() = xoff + (m_trackLength - m_pThumb->w());
      }
      else
      {
         m_pThumb->x() = xoff + (m_trackLength - m_pThumb->w()) * percent / 100;
      }
   }

   _DTraceDebug("(%s)\t\t#%d, scroll to %d", classname(), m_id, m_scroll);

   return GERROR_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////
void GScrollBar::setParentFocused(bool isFocused)
{
   m_isParentFocused = isFocused;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
