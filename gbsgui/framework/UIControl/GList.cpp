//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GList.cpp                                                                  //
//////////////////////////////////////////////////////////////////////////////////////

#include "GList.hpp"
#include "GKey.hpp"
#include "GEvents.hpp"
#include "GNavigator.hpp"
#include "GListItemEx.hpp"
#include "APIBitmap.hpp"
#include "APIText.hpp"
#include "GDebug.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
bool GList::InstanceOfGList(GStaticControl* control)
{
   GList* tmp = dynamic_cast<GList*>(control);

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
void GList::initGList()
{
   //_DTraceDebug("GList (%s) ::initGList\n",this->name());
   m_circleNavigation = false;
   m_itemCount = 0;
   m_fullWidth = 0;
   m_fullHeight = 0;
   m_maxItemCount = 1;
   m_iFocused = -1;
   m_iSelected = -1;
   m_iFirstVisibleItem = -1;
   m_iFocPre = -1;
   m_iSelPre = -1;
   m_i1visiPre = -2;//2013-05-06mo_th66
   m_rowCount = 0;
   m_columnCount = 0;
   m_xOffset = 0;
   m_yOffset = 0;
   m_vSpace = 10;
   m_hSpace = 10;
   m_direction = LIST_DIRECTION_VERTICAL;
   m_needRedrawAll = true;
   m_pScrollBar = NULL;
   m_ppListItem = NULL;
   m_isControlBar = false;
}

//////////////////////////////////////////////////////////////////////////////////////
GList::GList(int maxItemCount)
{
   initGList();
   m_maxItemCount = maxItemCount;
   m_ppListItem = new GListItemBase*[m_maxItemCount];
}

//////////////////////////////////////////////////////////////////////////////////////
GList::GList(int maxItemCount, int rowCount, int colCount)
{
   initGList();
   m_maxItemCount = maxItemCount;
   m_ppListItem = new GListItemBase*[m_maxItemCount];
   m_rowCount = rowCount;
   m_columnCount = colCount;
}

//////////////////////////////////////////////////////////////////////////////////////
GList& GList::setCols(int colCount)
{
   m_columnCount = colCount;
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GList & GList::setRows(int rowCount)
{
   m_rowCount = rowCount;
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GList::~GList()
{
   if( m_ppListItem != NULL )
   {
      delete[] m_ppListItem;
      m_ppListItem = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GList::onFocusChanged()
{
   GFocusable::onFocusChanged();
   // Chau added *** begin
   if( this->m_pScrollBar != NULL )
   {
      this->m_pScrollBar->setParentFocused(this->isFocused());
   }

   if( this->isFocused() )
   {
      //_DTraceDebug("%s change to focused\n",this->name());
      if( (this->getRows() == 1) && (this->getCols() == 1) )
      {
         setFocusedItem(m_iFirstVisibleItem);
      }
      else
      {
         setFocusedItem(m_iFocused);
      }
   }
   else
   {
      //_DTraceDebug("%s change to unfocused\n",this->name());
      if( this->getItemCount() > 0 )
      {
         if(this->m_isControlBar)
         {
            this->getFocusedItem()->setFocus(true);
         }
         else
         {
            this->getFocusedItem()->setFocus(false);
         }
      }

      dispatchEvent(GEVENT_LIST_UNFOCUSED, (void*) this);
   }
   // Chau added *** end

   //setFocusedItem(m_iFocused); //Chau commented *** su dung doan code tren

   if( m_pView != NULL )
   {
      turnOnDrawAll();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GList::setItemPos(GListItemBase* item, int index)
{
   int r = 0;
   int c = 0;
   int x, y;

   if( index == 0 )
   {
      // item dau tien tren man hinh
      item->setPos(m_xOffset, m_yOffset);
      return;
   }

   if( this->m_direction == LIST_DIRECTION_VERTICAL )
   {
      r = index / m_columnCount;
      c = index % m_columnCount;
   }
   else
   {
      r = index % m_rowCount;
      c = index / m_rowCount;
   }

   x = m_xOffset + c * (item->w() + m_hSpace);
   y = m_yOffset + r * (item->h() + m_vSpace);
   item->setPos(x, y);
//   _DTraceError("SetPos for '%s' at (%3d,%3d)\n", item->name(), x, y);
}

//////////////////////////////////////////////////////////////////////////////////////
void GList::updateFirstVisibleItemIndex(int index)
{
   if( index < 0 )
   {
      index = 0;
   }

   m_iFirstVisibleItem = index;
   for( int i = index; i < index + m_rowCount * m_columnCount; i++)
   {
      if( i >= m_itemCount )
      {
         return;
      }

      setItemPos(m_ppListItem[i], i - index);
   }

   // update list width height
   this->w() = m_xOffset + m_columnCount * (m_ppListItem[0]->w() + m_hSpace);
   this->h() = m_yOffset + m_rowCount * (m_ppListItem[0]->h() + m_vSpace);
   //   _DTraceDebug("Item size - (%d,%d), list size - (%d,%d)\n", m_ppListItem[0]->w(), m_ppListItem[0]->h(), w(), h());
}

//////////////////////////////////////////////////////////////////////////////////////
void GList::upd1visi(int index)
{
   if( index < 0 )
   {
      index=0;
   }

   if( m_itemCount < index )
   {
      index = m_itemCount-1;
   }

   m_i1visiPre = m_iFirstVisibleItem;
   m_iFirstVisibleItem = index;

   for( int i = index; i < index + m_rowCount * m_columnCount; i++)
   {
      if( i >= m_itemCount )
      {
         return;
      }
      setItemPos(m_ppListItem[i], i - index);
   }

   // update list width height
   this->w() = m_xOffset + m_columnCount * (m_ppListItem[0]->w() + m_hSpace);
   this->h() = m_yOffset + m_rowCount * (m_ppListItem[0]->h() + m_vSpace);
   //_DTraceDebug("Item size - (%d,%d), list size - (%d,%d)\n", m_ppListItem[0]->w(), m_ppListItem[0]->h(), w(), h());
}

//////////////////////////////////////////////////////////////////////////////////////
GList& GList::setListDirection(List_Direction_t direction)
{
   m_direction = direction;
}

//////////////////////////////////////////////////////////////////////////////////////
GList& GList::addItem(GListItemBase* listItem)
{
   addItemAt(listItem, m_itemCount);
}

//////////////////////////////////////////////////////////////////////////////////////
GList& GList::addItemAt(GListItemBase* listItem, int index)
{
   m_lastError = GERROR_NONE;

   if( listItem == NULL )
   {
      m_lastError = GERROR_INVALID_HANDLE;
      return *this;
   }

   if( m_itemCount >= m_maxItemCount )
   {
      m_lastError = GERROR_EXCEED_MAXIMUM;
      //_DTraceError("Maximum number of items in List #%d exceeded!\n", this->_id);
      return *this;
   }

   if( index < 0 )
   {
      m_lastError = GERROR_NOT_FOUND;
      return *this;
   }

   if( index >= m_itemCount )
   {
      // add vao cuoi
      index = m_itemCount;
   }

   int i = m_itemCount-1;
   while( i >= index )
   {
      m_ppListItem[i+1] = m_ppListItem[i];
      i --;
   }

   m_ppListItem[index] = listItem;
   listItem->setParent(this);
   m_itemCount ++;
   //_DTraceDebug("Added item at #%d, m_iFirstVisibleItem: %d\n", index, m_iFirstVisibleItem);
   //updateFirstVisibleItemIndex(m_iFirstVisibleItem<0 ? index : m_iFirstVisibleItem);
   upd1visi(m_iFirstVisibleItem < 0 ? index : m_iFirstVisibleItem);
   // set position for new item

   if( m_iFocused < 0 )
   {
      // if this is first focusable control, make it focused by default
      m_iFocused = index;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
int GList::findIndex(GListItemBase* listItem)
{
   int i = m_itemCount-1;
   while( i >= 0 )
   {
      if( m_ppListItem[i] == listItem )
      {
         return i;
      }
      i--;
   }
   return -1; // not found
}

//////////////////////////////////////////////////////////////////////////////////////
GList& GList::destroyAllItem()
{
   m_lastError = GERROR_NONE;

   //_DTraceError("GList::destroyAllItem  *** itemCount = %d\n", m_itemCount);
   for( int i = 0; i < m_itemCount; i++ )
   {
      if( m_ppListItem[i] != NULL )
      {
         m_ppListItem[i]->destroyViews();
         delete m_ppListItem[i];
         m_ppListItem[i] = NULL;
      }
   }

   removeAllItem();

   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GList& GList::removeAllItem()
{
   m_lastError = GERROR_NONE;

   m_itemCount = 0;

   //Chau added *** begin
   this->w() = m_xOffset;
   this->h() = m_yOffset;

   //Chau added *** end
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GList& GList::removeItem(GListItemBase* listItem)
{
   m_lastError = GERROR_NOT_FOUND;
   int i = findIndex(listItem);

   if( i >= 0 )
   {
      m_lastError = GERROR_NONE;
      return removeItemAt(i);
   }

   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GList& GList::removeItemAt(int index)
{
   int page;
   m_lastError = GERROR_NOT_FOUND;

   if( (index < 0) || (index >= m_itemCount) )
   {
      m_lastError = GERROR_NOT_FOUND;
      return *this;
   }

   int i = index;
   m_itemCount--;

   if( m_ppListItem[i] != NULL )
   {
      m_ppListItem[i]->destroyViews();
      delete m_ppListItem[i];
      m_ppListItem[i] = NULL;
   }

   page = index/(m_rowCount*m_columnCount);
   for(; i < m_itemCount; i++ )
   {
      m_ppListItem[i] = m_ppListItem[i+1];

      if( i/(m_rowCount*m_columnCount) == page )
      {
         setItemPos(m_ppListItem[i], i%(m_rowCount*m_columnCount));
      }
   }

   m_ppListItem[m_itemCount] = NULL;

   m_lastError = GERROR_NONE;
   return *this;
}

#if 0
//////////////////////////////////////////////////////////////////////////////////////
GError_t GList::draw()
{
   GError_t er = GERROR_NONE;

   //TODO: optimize this
   //int l = m_pScrollBar->layer();
   int l = this->layer();

   _DTraceError("(%s)\t\t#%d, layer %d, z %d, pos (%3d, %3d), ref (%3d, %3d), size (%3d, %3d):\n", classname(), this->id(), this->layer(), this->z(), this->x(), this->y(), this->refX(), this->refY(), this->w(), this->h());

   if( !m_pView )
   {
      //_DTraceWarning("%s #%d - List WITHOUT view (background)\n", classname(), id());
   }
   else
   {
      if( (m_needRedrawAll) || (pushUpdateRectIfIntersect(l, m_pView->x(), m_pView->y(), m_pView->w(), m_pView->h())) )
      {
         int globalX = this->x() - this->refX() + m_pView->x();
         int globalY = this->y() - this->refY() + m_pView->y();

         ((GDrawable*)m_pView)->draw(l, globalX, globalY);
      }
   }

   //if ( (m_pScrollBar != NULL) && (m_rowCount*m_columnCount < m_itemCount) ) { FIXME *** Ly comment de test
   if( m_pScrollBar != NULL )
   {
      if( (m_needRedrawAll) || (pushUpdateRectIfIntersect(l, m_pScrollBar->x(), m_pScrollBar->y(), m_pScrollBar->w(), m_pScrollBar->h())) )
      {
         //_DTraceDebug("%s #%d - scrollbar available\n", classname(), id());
         int globalX = this->x() - this->refX() + m_pScrollBar->x();
         int globalY = this->y() - this->refY() + m_pScrollBar->y();
         ((GDrawable*)m_pScrollBar)->draw(l, globalX, globalY);
      }
   }

   //updateFirstVisibleItemIndex(m_iFirstVisibleItem);

   //_DTraceDebug("Draw items\n");

   if( (m_iFirstVisibleItem < 0) || (m_rowCount <= 0) || (m_columnCount <= 0) )
   {
      //_DTraceError("%s #%d - NO visible items in list. m_iFirstVisibleItem = %d, row = %d, col = %d\n", classname(), id(), m_iFirstVisibleItem, m_rowCount, m_columnCount);
      return GERROR_NOT_FOUND;
   }

   int i;
   for( i = m_iFirstVisibleItem; i < m_iFirstVisibleItem + m_rowCount*m_columnCount; i++ )
   {
      //TODO: handle this _layer for rendering optimization
      //int l = this->_layer;
      if( i >= m_itemCount )
      {
         break;
      }

      GDrawable* item = m_ppListItem[i];
      //TODO: optimize this
      //int l = item->layer();
      int l = this->layer();

      int globalX = this->x() - this->refX() + item->x();
      int globalY = this->y() - this->refY() + item->y();

      if( (m_needRedrawAll) || (pushUpdateRectIfIntersect(l, m_ppListItem[i])) )
      {
         //_DTraceDebug("(%s #%d), item #%d (%s - %d) layer %d, z %2d, pos (%3d, %3d), size (%3d, %3d), drawAll: %s\n", this->classname(), this->id(), i, item->classname(), item->id(), l, item->z(), globalX, globalY, item->w(), item->h(), m_needRedrawAll?"true":"false");
         if( er = item->draw(l, globalX, globalY) != GERROR_NONE )
         {
            m_lastError = er;
         }
      }
      //_DTraceDebug("(%s)\t\t#%d, End\n", item->classname(), item->id());
   }
   m_needRedrawAll = false;
   //_DTraceDebug("(%s)\t\t#%d, End\n\n", classname(), this->id());

   return er;
}

#else   //2013-04-27sa_th66

//////////////////////////////////////////////////////////////////////////////////////
GError_t GList::draw()
{
   GError_t er = GERROR_NONE;
   GView* pv = parent();
   if( pv == NULL )
   {
      return er;
   }

   int x = this->x(), y=this->y(), w=this->w(), h=this->h(), i, gx, gy, l=layer();

   GDrawable* item = NULL;
   //_DTraceDebug("GList::draw(%d %d %d %d)%di1st%d items%drs%dcs%d\n",x,y,w,h,m_i1visiPre,_i1visiCur,m_itemCount,m_rowCount,m_columnCount);//2013-04-28su_th66   GBLITfillRect(l,pv->x()-pv->refX()+this->x(),pv->y()-pv->refY()+this->y(),m_fullWidth,m_fullHeight,1,R,G,B);
   if( m_pView != NULL )
   {
      ((GDrawable*)m_pView)->draw(l,x-refX()+m_pView->x(),y+refY()+m_pView->y());
   }

   if( (m_iFirstVisibleItem < 0) || (m_rowCount <= 0) || (m_columnCount <= 0) )
   {
      return GERROR_NOT_FOUND;
   }

   for( i = m_iFirstVisibleItem; i < m_iFirstVisibleItem + m_rowCount*m_columnCount; i++ )
   {
      if( m_itemCount <= i )
      {
         break;
      }

      //_DTraceError("GList::draw() cur %d, %d  iFoc %d\n",i,m_iFocPre,m_iFocused);
      if( 0 < m_nDraw1 && m_i1visiPre == m_iFirstVisibleItem && 0 <= m_iFocPre && i != m_iFocPre && i!=m_iFocused && m_iFocPre!=m_iFocused )
      {
         continue;
      }

      item = m_ppListItem[i];

      if( item != NULL )
      {
         item->draw(l,x-refX()+item->x(), y-refY()+item->y());
      }
   }//if(er=...!=GERROR_NONE)m_lastError=er;}

   if( m_pScrollBar != NULL )
   {
      ((GDrawable*)m_pScrollBar)->draw( l, x-refX()+m_pScrollBar->x(), y+refY()+m_pScrollBar->y() );
   }
   return er;
}

//////////////////////////////////////////////////////////////////////////////////////
void GList::draw1(int nClrBg)
{
#if 1
   GView* pv = parent();
   if( pv == NULL )
   {
      return;//if(m_iFocused==m_iFocPre&&m_iSelected==m_iSelPre&&_i1visiCur==m_i1visiPre)return;
   }

   int l=layer(), x = pv->x()-pv->refX() + this->x();
   int y = pv->y()-pv->refY()+this->y();
   //APITextInit(); //trungkt comment 18-10-2010
   if( m_i1visiPre != m_iFirstVisibleItem || 0 < nClrBg )
   {
      GBLITfillRect(x, y, m_fullWidth, m_fullHeight, m_nARGB, l);;//ClearRect(l,x,y,m_fullWidth,m_fullHeight);
   }

   if( 0 < nClrBg )
   {
      m_i1visiPre=-1;//ClearRect(l,x,y,m_fullWidth,m_fullHeight,a);//this->w(),this->h(),a);//2013-04-30tue_th66
   }

   m_nDraw1 = 1;

   GDrawable::draw(l, x, y);

   m_nDraw1 = 0;

#else

   GView* pv = parent();
   int l = layer();
   int x = x = pv->x()-pv->refX() + this->x(),y=pv->y()-pv->refY()+this->y();
   GDrawable::draw(l,x,y);

#endif
//   GBLITcopy();
}//_DTraceDebug("GList::draw1()END\n");}

//////////////////////////////////////////////////////////////////////////////////////
void GList::draw0(int nBLITcp)
{
   GView* pv = parent();
   if( pv == NULL )
   {
      return;//2013-08-14we_th66
   }

   int l =layer();
   int x = pv->x()-pv->refX()+this->x();
   int y = pv->y()-pv->refY()+this->y();
   GBLITfillRect(x,y,m_fullWidth,m_fullHeight,0,l);
   //if(nBLITcp)GBLITcopy();
}

#endif   //2013-04-27sa_th66

//////////////////////////////////////////////////////////////////////////////////////
GListItemBase* GList::getItem(int i)
{
   if( (i >= 0) && (i < m_itemCount) )
   {
      return m_ppListItem[i];
   }
   else
   {
      return NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
int GList::getItemCount()
{
   return m_itemCount;
}

//////////////////////////////////////////////////////////////////////////////////////
GListItemBase* GList::getFocusedItem()
{
   if( (m_iFocused <0) || (m_itemCount<=0) )
   {
      return NULL;
   }

   return m_ppListItem[m_iFocused];
}

//////////////////////////////////////////////////////////////////////////////////////
int GList::getFocusedItemId()
{
   if( (m_iFocused <0) || (m_itemCount<=0) )
   {
      return -1;
   }

   return m_iFocused;
}

#if 1   //2013-05-06mo_th66

//////////////////////////////////////////////////////////////////////////////////////
GError_t GList::setFocusedItem(int i, bool dispatchEvt)
{
   if( (i < 0) || (i >= m_itemCount) )
   {
      return GERROR_NOT_FOUND;
   }

   if( (i < m_iFirstVisibleItem) || (i >= m_iFirstVisibleItem + m_columnCount * m_rowCount) )
   {
      int page = i / (m_rowCount * m_columnCount);
      updateFirstVisibleItemIndex(page * (m_rowCount * m_columnCount) );
      //_DTraceInfo("Scroll to page: %d\n", page);
      turnOnDrawAll();
   }

   GListItemBase* focusedItem = getFocusedItem();
   if( focusedItem != NULL )
   {
      focusedItem->setFocus(false);
      pushUpdateRect(this->layer(), focusedItem);

      if( dispatchEvt )
      {
         dispatchEvent(GEVENT_LIST_ITEM_UNFOCUSED, (void *) focusedItem);
      }
   }

   m_iFocused = i;
   focusedItem = getFocusedItem();
   focusedItem->setFocus(true);
   pushUpdateRect(this->layer(), focusedItem);

   if( dispatchEvt )
   {
      dispatchEvent(GEVENT_LIST_ITEM_FOCUSED, (void *) focusedItem);
   }

   if( m_pScrollBar != NULL )
   {
      int percent = 0;
      if( m_direction == LIST_DIRECTION_VERTICAL )
      {
         if( (m_columnCount >0) && ((m_itemCount-1) / m_columnCount > 0) )
         {
            if( i == 0 )
            {
               percent = 0;
            }
            else
            {
               //percent = (i / m_columnCount) * 100 / ((m_itemCount)/ m_columnCount);
               /*Ly modified*/
               percent = (i / m_columnCount) * 100 / ((m_itemCount -1)/ m_columnCount);
               /*Ly modified*/
            }
         }
      }
      else
      {
         if( (m_rowCount >0) && ((m_itemCount-1) / m_rowCount > 0) )
         {
            //percent = (i / m_rowCount) * 100 / ((m_itemCount) / m_rowCount);
            /*Ly modified*/
            percent = (i / m_rowCount) * 100 / ((m_itemCount -1) / m_rowCount);
            /*Ly modified*/
         }
      }

      m_pScrollBar->scroll(percent);
      pushUpdateRect(this->layer(), m_pScrollBar->x(), m_pScrollBar->y(), m_pScrollBar->w(), m_pScrollBar->h());
   }

   //_DTraceDebug("Focused item changed to: #%d - 0x%08x\n",i, m_ppListItem[i]);
   m_lastError = GERROR_NONE;
   return m_lastError;
}

#else   //2013-05-06mo_th66

//////////////////////////////////////////////////////////////////////////////////////
GError_t GList::setFocusedItem(int i, bool dispEvt)
{
   if( i < 0 || m_itemCount <= i )
   {
      return GERROR_NOT_FOUND;
   }

   GListItemBase* fi;

#if 0

   int n0 = m_iFirstVisibleItem, n1=m_columnCount*m_rowCount;
   if( i < n0 )
   {
      upd1visi(i);//n0-1);//2013-05-06mo_th66
   }

   if( n0+n1 <= i)
   {
      upd1visi(i+(i-m_iFocused)-n1);//upd1visi(i+1-n1);//n0+1+i-n0-n1);}//turnOnDrawAll();}//int page=i/(m_rowCount*m_columnCount);//2013-05-06mo_th66 page*m_rowCount*m_columnCount);//_DTraceInfo("Scroll to page: %d\n", page);
   }

   //_DTraceDebug("GList::setFocusedItem(%dfoc%d,dispE%d)%di1visi%d\n",m_iFocused,i,dispEvt,m_i1visiPre,_i1visiCur);
   fi = getFocusedItem();

   if( fi != NULL )
   {
      //_DTraceError("GList::setFocusedItem %d false \n",m_iFocused);
      fi->setFocus(false);
   }//if(dispEvt)dispatchEvent(GEVENT_LIST_ITEM_UNFOCUSED,(void*)fi);}//pushUpdateRect(layer(),fi);

   m_iFocPre  = m_iFocused;
   m_iFocused = i;
   fi = getFocusedItem();

   if( fi != NULL )
   {
      fi->setFocus(true);
      if(dispEvt) dispatchEvent(GEVENT_LIST_ITEM_FOCUSED,(void*)fi);
   }//pushUpdateRect(layer(),fi);

   if( m_pScrollBar != NULL )
   {
      int percent=0;
      if( m_direction==LIST_DIRECTION_VERTICAL )
      {
         if( (m_columnCount >0) && ((m_itemCount-1) / m_columnCount > 0) )
         {
            if( i == 0 )
            {
               percent = 0;
            }
            else
            {
               percent=(i/m_columnCount)*100/((m_itemCount-1)/m_columnCount);
            }   /*Ly modified*///percent = (i / m_columnCount) * 100 / ((m_itemCount)/ m_columnCount);      /*Ly modified*/
         }
      }
      else
      {
         if( m_rowCount > 0 && (m_itemCount-1)/m_rowCount > 0 )
         {
            percent=(i/m_rowCount)*100 / ((m_itemCount -1) / m_rowCount);
         }
      } /*Ly modified*///percent = (i / m_rowCount) * 100 / ((m_itemCount) / m_rowCount);         /*Ly modified*/
      m_pScrollBar->scroll(percent);
      pushUpdateRect(layer(),m_pScrollBar->x(),m_pScrollBar->y(),m_pScrollBar->w(),m_pScrollBar->h());
   }//_DTraceDebug("Focused item changed to: #%d - 0x%08x\n",i, m_ppListItem[i]);

#else

   if( (i < m_iFirstVisibleItem) || (i >= m_iFirstVisibleItem + m_columnCount * m_rowCount) )
   {
      int page = i / (m_rowCount * m_columnCount);
      updateFirstVisibleItemIndex(page * (m_rowCount * m_columnCount) );
      //_DTraceInfo("Scroll to page: %d\n", page);
      turnOnDrawAll();
   }

   GListItemBase* focusedItem = getFocusedItem();
   if( focusedItem != NULL )
   {
      focusedItem->setFocus(false);
      pushUpdateRect(this->layer(), focusedItem);
      if( dispEvt )
      {
         dispatchEvent(GEVENT_LIST_ITEM_UNFOCUSED, (void *) focusedItem);
      }
   }

   m_iFocused = i;
   focusedItem = getFocusedItem();
   focusedItem->setFocus(true);
   pushUpdateRect(this->layer(), focusedItem);

   if( dispEvt )
   {
      dispatchEvent(GEVENT_LIST_ITEM_FOCUSED, (void *) focusedItem);
   }

   if( m_pScrollBar != NULL )
   {
      int percent = 0;
      if( m_direction == LIST_DIRECTION_VERTICAL )
      {
         if( (m_columnCount > 0) && ((m_itemCount-1) / m_columnCount > 0) )
         {
            if( i == 0 )
            {
               percent = 0;
            }
            else
            {
               //percent = (i / m_columnCount) * 100 / ((m_itemCount)/ m_columnCount);
               /*Ly modified*/
               percent = (i / m_columnCount) * 100 / ((m_itemCount -1)/ m_columnCount);
               /*Ly modified*/
            }
         }
      }
      else
      {
         if( (m_rowCount > 0) && ((m_itemCount-1) / m_rowCount > 0) )
         {
            //percent = (i / m_rowCount) * 100 / ((m_itemCount) / m_rowCount);
            /*Ly modified*/
            percent = (i / m_rowCount) * 100 / ((m_itemCount -1) / m_rowCount);
            /*Ly modified*/
         }
      }

      m_pScrollBar->scroll(percent);
      pushUpdateRect(this->layer(), m_pScrollBar->x(), m_pScrollBar->y(), m_pScrollBar->w(), m_pScrollBar->h());
   }

#endif

   m_lastError=GERROR_NONE;
   return m_lastError;
}

#endif   //2013-05-06mo_th66

//////////////////////////////////////////////////////////////////////////////////////
GError_t GList::updateListItem(GListItemBase* item)
{
   //Chau added: chi ve lai phan rect item moi dc add them vao hoac cbi xoa di,
   //su dung cho progress bar (volume, waiting)
   //m_needRedrawAll = true;
   GNavigator::getInstance().pushUpdateRect(
      this->layer(),
      this->x() - this->refX() + item->x(),
      this->y() - this->refY() + item->y(),
      item->w(), item->h());

   return GERROR_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GList::setFocusedItem(GListItemBase* listItem, bool dispatchEvt)
{
   m_lastError = GERROR_NOT_FOUND;

   int i = findIndex(listItem);
   if( i >= 0 )
   {
      return setFocusedItem(i, dispatchEvt);
   }

   return m_lastError;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GList::setSelectedItem(GListItemBase* listItem, bool dispatchEvt)
{
   m_lastError = GERROR_NOT_FOUND;

   int i = findIndex(listItem);
   if( i >= 0 )
   {
      return setSelectedItem(i, dispatchEvt);
   }

   return m_lastError;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GList::setSelectedItem(int i, bool dispatchEvt)
{
   if( (i < 0) || (i >= m_itemCount) )
   {
      return GERROR_NOT_FOUND;
   }

   m_iSelPre = m_iSelected;
   m_iSelected = i;

   if( m_columnCount == 1 && m_rowCount == 1)
   {
      m_i1visiPre = m_iFirstVisibleItem;
      m_iFirstVisibleItem = i;
   }

   GListItemEx* selectedItem = dynamic_cast<GListItemEx*>(getSelectedItem());

   if( selectedItem != NULL )
   {
      selectedItem->setSelect(true);
   }

   if( dispatchEvt )
   {
      dispatchEvent(GEVENT_LIST_ITEM_SELECT, (void*)getSelectedItem());
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GListItemBase* GList::getSelectedItem()
{
   if( (m_iSelected < 0 ) || (m_itemCount <= 0) )
   {
      return NULL;
   }

   return m_ppListItem[m_iSelected];
}

//////////////////////////////////////////////////////////////////////////////////////
int GList::getSelectedItemId()
{
   if( (m_iSelected < 0) || (m_itemCount <= 0) )
   {
      return -1;
   }

   return m_iSelected;
}

//////////////////////////////////////////////////////////////////////////////////////
int GList::findNextItem(int currentItem, Direction_t direction)
{
   if( currentItem < 0 )
   {
      return -1;
   }

   int iFound = -1;

   if( currentItem < m_itemCount-1 )
   {
      iFound = currentItem + 1;
   }

   if( m_direction == LIST_DIRECTION_VERTICAL )
   {
      switch( direction )
      {
      case UP:
         iFound = currentItem - m_columnCount;
         if( m_circleNavigation && iFound < 0 )
         {
            iFound = m_itemCount - 1;
         }
         break;

      case DOWN:
         iFound = currentItem + m_columnCount;
         if( m_circleNavigation && iFound >= m_itemCount && currentItem >= m_itemCount-1 )
         {
            iFound = 0;
         }

         if( iFound >= m_itemCount )
         {
            iFound = m_itemCount - 1;
         }
         break;

      case LEFT:
         iFound = currentItem - 1;
         if( (iFound / m_columnCount) != (currentItem / m_columnCount) )
         {
            // nhay qua hang khac
            iFound = -1;
         }
         break;

      case RIGHT:
         iFound = currentItem + 1;
         if( (iFound / m_columnCount) != (currentItem / m_columnCount) )
         {
            // nhay qua hang khac
            iFound = -1;
         }
         break;
      }
   }
   else
   {
      switch( direction )
      {
      case UP:
         iFound = currentItem - 1;
         if( (iFound / m_rowCount) != (currentItem / m_rowCount) )
         {
            // nhay qua cot khac
            iFound = -1;
         }
         break;

      case DOWN:
         iFound = currentItem + 1;
         if( (iFound / m_rowCount) != (currentItem / m_rowCount) )
         {
            // nhay qua cot khac
            iFound = -1;
         }
         break;

      case LEFT:
         iFound = currentItem - m_rowCount;
         if( m_circleNavigation && iFound < 0 )
         {
            iFound = m_itemCount - 1;
         }
         break;

      case RIGHT:
         iFound = currentItem + m_rowCount;
         if( m_circleNavigation && iFound >= m_itemCount && currentItem >= m_itemCount-1 )
         {
            iFound = 0;
         }

         if(iFound >= m_itemCount)
         {
            iFound = m_itemCount - 1;
         }
         break;
      }
   }

   if( (iFound <0) || (iFound >= m_itemCount) )
   {
      iFound = -1;
   }

   return iFound;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GList::scrollNextPage()
{
   if( m_iFirstVisibleItem + m_rowCount * m_columnCount < m_itemCount )
   {
      updateFirstVisibleItemIndex( m_iFirstVisibleItem + m_rowCount * m_columnCount );
      _DTraceInfo("Scroll next page\n");
      turnOnDrawAll();
      return true;
   }
   else
   {
      return false;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool GList::scrollPreviousPage()
{
   if( m_iFirstVisibleItem - m_rowCount * m_columnCount >= 0)
   {
      updateFirstVisibleItemIndex(m_iFirstVisibleItem - m_rowCount * m_columnCount );
      _DTraceInfo("Scroll previous page\n");
      turnOnDrawAll();
      return true;
   }
   else
   {
      return false;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool GList::nextPage()
{
   if( scrollNextPage() )
   {
      setFocusedItem(m_iFirstVisibleItem);
      return true;
   }
   else
   {
      return false;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool GList::previousPage()
{
   if( scrollPreviousPage() )
   {
      setFocusedItem(m_iFirstVisibleItem);
      return true;
   }
   else
   {
      return false;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GList::setCircleNavigation(bool enableCircleNav)
{
   m_circleNavigation = enableCircleNav;
}

//////////////////////////////////////////////////////////////////////////////////////
void GList::setPos(int x, int y)
{
   GFocusable::setPos(x, y);
   m_relX = x;
   m_relY = y;
}

//////////////////////////////////////////////////////////////////////////////////////
void GList::setLastVisibleItemIndex(int index)
{
   index = index - m_columnCount*m_rowCount + 1;

   if( (index >= 0) && (index < m_itemCount) )
   {
      updateFirstVisibleItemIndex(index);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GList::turnOnDrawItem(GListItemBase * item)
{
   //todo: Why return here ???
   return; //trungkt add 17-12-2013
   pushUpdateRect(this->layer(), item);
}

//////////////////////////////////////////////////////////////////////////////////////
void GList::turnOnDrawAll()
{
   //todo: why return here ???
   return; //trungkt add 17-12-2013
   m_needRedrawAll = true;
   GNavigator::getInstance().pushUpdateRect(
      this->layer(),
      this->x() - this->refX(),
      this->y() - this->refY(),
      this->w(), this->h());
}


//Chau added *** begin
// pushUpdateRect rect->w = full list width, rect->h = full list height
//////////////////////////////////////////////////////////////////////////////////////
void GList::turnOnDrawList()
{
   //todo why return here ???
   return;//2013-05-05su_th66
   m_needRedrawAll = true;
   GNavigator::getInstance().pushUpdateRect(
      this->layer(),
      this->x() - this->refX(),
      this->y() - this->refY(),
      m_fullWidth, m_fullHeight);
}
//Chau added *** end

//////////////////////////////////////////////////////////////////////////////////////
void GList::pushUpdateRect(int layer, int x, int y, int w, int h)
{
   //todo: why return here ???
   return;//2013-05-06mo_th66
   //   _DTraceDebug("Push update rect for list item (%d, %d, %d, %d) on layer %d\n",x,y,w,h,layer);
   GNavigator::getInstance().pushUpdateRect(
         layer,
         x + this->x() - this->refX() + m_xOffset,
         y + this->y() - this->refY() + m_yOffset,
         w, h);
   m_relX = this->x();
   m_relY = this->y();
}

//////////////////////////////////////////////////////////////////////////////////////
void GList::pushUpdateRect(int layer, GListItemBase* item)
{
   //todo: why return here ???
   return;//2013-05-06mo_th66
   pushUpdateRect(layer, item->x(), item->y(), item->w(), item->h());
}

//////////////////////////////////////////////////////////////////////////////////////
bool GList::pushUpdateRectIfIntersect(int layer, int x, int y, int w, int h)
{
   //todo: why return here ???
   return 0;//2013-05-06mo_th66
   int _x = x + this->m_relX - this->refX() + m_xOffset;
   int _y = y + this->m_relY - this->refY() + m_yOffset;

   bool res = GNavigator::getInstance().pushUpdateRectIfIntersect(layer, _x,   _y,   w, h);

   //_DTraceDebug("Push update rect if intersect (%d, %d, %d, %d) on layer %d, res = %s\n", _x, _y, w, h, layer, res?"true":"false");
   return res;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GList::pushUpdateRectIfIntersect(int layer, GListItemBase* item)
{
   bool res =  pushUpdateRectIfIntersect(layer, item->x(), item->y(), item->w(), item->h());
   //_DTraceDebug("PushUpdateRectIfIntersect for list item %s at (%d,%d,%d,%d), res: %s\n", item->name(), item->x(), item->y(), item->w(), item->h(), res?"true":"false");
   return res;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GList::processKey(U32 key, bool forced)
{
   if( (!this->m_isFocused) && (forced) )
   {
      this->m_isFocused = true;
      this->processKey(key);
      this->m_isFocused = false;
   }
   else
   {
      this->processKey(key);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool GList::processKey(U32 key)
{
   //_DTraceInfo("***before check*** \nGList (%s) try to processKey - ItemCount= %d\n",this->name(),this->getItemCount());
   if( !this->m_isFocused)
   {
      //_DTraceInfo("GList (%s) not focused, not processKey\n",this->name());
      return false;
   }

   //Chau added *** begin
   // neu ko co item nao thi ko xu ly key
   if(this->getItemCount() < 1)
   {
      //_DTraceInfo("GList (%s) item count = 0, not processKey\n",this->name());
      return false;
   }
   //_DTraceInfo("*** checked *** \nGList (%s) try to processKey - ItemCount= %d\n",this->name(),this->getItemCount());
   //Chau added *** end

   GListItemBase* focusedItem = getFocusedItem();

   //_DTraceDebug("Focused control is at: 0x%08x\n",focusedCtrl);

   if( focusedItem != NULL )
   {
      if( (key == KEY_RC_OK) || (key == KEY_KB_ENTER) )
      {
         setSelectedItem(m_iFocused);
         return true;
      }
#if 0 //trungkt comment 07-10-2013
      //de xu ly dc key OK cho focusedItem thi 4 dong duoi day dua len truoc "if (focusedItem != NULL) {"
      if( focusedItem->processKey(key) )
      {
         pushUpdateRect(this->layer(), focusedItem);
         return true;
      }
#endif
   }
   //_DTraceDebug("Focused control at 0x%08x didn't process key\n",focusedCtrl);

   // TODO: find next focusable based on "key"
   Direction_t direction = NONE;
   switch( key )
   {
   case KEY_RC_UP:
   case KEY_KB_UP:
      direction = UP;
      break;

   case KEY_RC_DOWN:
   case KEY_KB_DOWN:
      direction = DOWN;
      break;

   case KEY_RC_LEFT:
   case KEY_KB_LEFT:
      direction = LEFT;
      break;

   case KEY_RC_RIGHT:
   case KEY_KB_RIGHT:
      direction = RIGHT;
      break;
   }

   if( direction != NONE )
   {
      m_i1visiPre = m_iFirstVisibleItem;
      int iNextFocusable = findNextItem(m_iFocused, direction);

      if( iNextFocusable >= 0 )
      {
         //_DTraceDebug("GList (%s) - Next control to be set focused: #%d - 0x%08x\n",this->name(),iNextFocusable, _controls[iNextFocusable]);
         setFocusedItem(iNextFocusable);
         return true;
      }
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
