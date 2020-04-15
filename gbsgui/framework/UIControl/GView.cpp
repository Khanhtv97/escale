//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GView.cpp - this is a compound of multiple drawable                        //
//////////////////////////////////////////////////////////////////////////////////////

#include "GView.hpp"
#include "GDebug.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
GView::GView() : GDrawable(), m_viewCount(0)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
void GView::init()
{
   for( int i = 0; i < MAX_INBOUND_DRAWABLES; ++i )
   {
      m_pViews[i] = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool GView::InstanceOfGView(GDrawable* drawable)
{
   GView* tmp = dynamic_cast<GView*>(drawable);
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
void GView::destroyViews()
{
   for( int i = 0; i < MAX_INBOUND_DRAWABLES; i ++ )
   {
      if( m_pViews[i] != NULL )
      {
         delete m_pViews[i];
         m_pViews[i] = NULL;
      }
   }

   m_viewCount = 0;
}

//////////////////////////////////////////////////////////////////////////////////////
GView::~GView()
{
   destroyViews();
}

//////////////////////////////////////////////////////////////////////////////////////
GView::GView(GDrawable* drawable1, GDrawable* drawable2, GDrawable* drawable3, GDrawable* drawable4)
   : GDrawable(), m_viewCount(0)
{
   init();

   addView(drawable1);

   if( drawable2 != NULL )
   {
      addView(drawable2);
   }

   if( drawable3 != NULL )
   {
      addView(drawable3);
   }

   if( drawable4 != NULL )
   {
      addView(drawable4);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GView::GView(int layer) : GDrawable(layer), m_viewCount(0)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
GView::GView(int x, int y) : GDrawable(x, y), m_viewCount(0)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
GView::GView(int layer, int x, int y) : GDrawable(layer, x, y), m_viewCount(0)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
GView::GView(int x, int y, int ref_x, int ref_y) : GDrawable(x, y, ref_x, ref_y), m_viewCount(0)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
GView::GView(int layer, int x, int y, int ref_x, int ref_y)
   : GDrawable(layer, x, y, ref_x, ref_y), m_viewCount(0)
{
   init();
}

//////////////////////////////////////////////////////////////////////////////////////
int GView::findIndexById(int id)
{
   int i = m_viewCount-1;

   while( i >= 0 )
   {
      if( m_pViews[i]->id() == id )
      {
         return i;
      }
      i--;
   }

   // not found
   return -1;
}

//////////////////////////////////////////////////////////////////////////////////////
GView& GView::addView(GDrawable* view, int x, int y)
{
   m_lastError = GERROR_NONE;

   if( m_viewCount >= MAX_INBOUND_DRAWABLES )
   {
      m_lastError = GERROR_EXCEED_MAXIMUM;
      return *this;
   }

   // insert to proper place, sort asc by z-index
   int i = m_viewCount-1;

   while( (i>=0) && (m_pViews[i]->z() > view->z()) )
   {
      m_pViews[i+1] = m_pViews[i];
      i --;
   }

   if( i >= 0 )
   {
      if( m_pViews[i]->id() == view->id() )
      {
         m_lastError = GERROR_ALREADY_EXISTED;
         return *this;
      }
   }

   m_pViews[i+1] = view;
   if( x != 0 )
   {
      view->x() = x;
   }

   if( y != 0 )
   {
      view->y() = y;
   }

   //TODO: set lai x, y, refX, refY khi view->x() - view->refX() < 0 hoac view->y() - view->refY() <0
   int viewW =  view->x() - view->refX() + view->w();
   int viewH =  view->y() - view->refY() + view->h();

   if( viewW > this->w() )
   {
      this->w() = viewW;
   }

   if( viewH > this->h() )
   {
      this->h() = viewH;
   }

   view->setParent(this);
   m_viewCount++;

   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GView& GView::removeView(int id)
{
   m_lastError = GERROR_NOT_FOUND;

   int i = findIndexById(id);

   if( i >= 0 )
   {
      m_viewCount--;
      for( ; i < m_viewCount; i++)
      {
         m_pViews[i] = m_pViews[i+1];
      }
      m_lastError = GERROR_NONE;
      return *this;
   }

   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GView::bringForward(int id)
{
   GError_t er = GERROR_NOT_FOUND;

   int i = findIndexById(id);

   if( (i >= 0) && (i < m_viewCount-1) )
   {
      int tmpz = m_pViews[i]->z();
      m_pViews[i]->z() = m_pViews[i+1]->z();
      m_pViews[i+1]->z() = tmpz;

      GDrawable* tmp = m_pViews[i];
      m_pViews[i] = m_pViews[i+1];
      m_pViews[i+1] = tmp;

      return GERROR_NONE;
   }

   return er;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GView::sendBackward(int id)
{
   GError_t er = GERROR_NOT_FOUND;

   int i = findIndexById(id);
   _DTraceDebug("found i = %d",i);

   if( i > 0 )
   {
      int tmpz = m_pViews[i]->z();
      m_pViews[i]->z() = m_pViews[i-1]->z();
      m_pViews[i-1]->z() = tmpz;

      GDrawable* tmp = m_pViews[i];
      m_pViews[i] = m_pViews[i-1];
      m_pViews[i-1] = tmp;

      return GERROR_NONE;
   }

   return er;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GView::bringToFront(int id)
{
   GError_t er = GERROR_NOT_FOUND;

   int i = findIndexById(id);

   if( (i >= 0) && (i < m_viewCount-1) )
   {
      m_pViews[i]->z() = m_pViews[m_viewCount-1]->z() + 1;
      GDrawable* tmp = m_pViews[i];

      for( ; i < m_viewCount-1; i++ )
      {
         m_pViews[i] = m_pViews[i+1];
      }

      m_pViews[m_viewCount-1] = tmp;

      return GERROR_NONE;
   }

   return er;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GView::sendToBack(int id)
{
   GError_t er = GERROR_NOT_FOUND;

   int i = findIndexById(id);

   if( i > 0 )
   {
      m_pViews[i]->z() = m_pViews[0]->z() - 1;

      GDrawable* tmp = m_pViews[i];
      //m_pViews[i] = m_pViews[0];

      for( ; i > 0; i-- )
      {
         m_pViews[i] = m_pViews[i-1];
      }
      m_pViews[0] = tmp;

      return GERROR_NONE;
   }

   return er;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GView::draw()
{
   GError_t er = GERROR_NONE;
   //_DTraceError("(%s)\t\t#%d, layer %d, z %d, pos (%3d, %3d), ref (%3d, %3d), size (%3d, %3d):\n", classname(), this->id(), this->layer(), this->z(), this->x(), this->y(), this->refX(), this->refY(), this->w(), this->h());
   //_DTraceError("GView::draw() \n");
   int i;
   for( i = 0; i < m_viewCount; i++ )
   {
      GDrawable* item = m_pViews[i];

      //TODO: optimize this
      //int l = item->layer();
      int l = this->m_layer;
      //if (this->m_layer > 0)
      //{
      //   l = item->layer();
      //}
      //else
      //{
      //   l = 0;
      //}

      int globalX = this->x() - this->refX() + item->x();
      int globalY = this->y() - this->refY() + item->y();
      //_DTraceError("(%s)\t\t#%d, layer %d, z %2d, pos (%3d, %3d)\n", item->classname(), item->id(), l, item->z(), globalX, globalY);

      er = item->draw(l, globalX, globalY);
      if( er != GERROR_NONE )
      {
         m_lastError = er;
      }
   }

   //_DTraceError("(%s)\t\t#%d, End\n", classname(), this->id());
   return er;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
