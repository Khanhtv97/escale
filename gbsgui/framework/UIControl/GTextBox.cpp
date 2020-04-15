//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GTextBox.cpp                                                               //
//////////////////////////////////////////////////////////////////////////////////////

#include "GTextBox.hpp"
#include "GKey.hpp"
#include "GEvents.hpp"
#include "GNavigator.hpp"
#include "GStaticControl.hpp"
#include "GLanguage.hpp"
#include "GDebug.hpp"

#define MAX_TEXT_LINE_COUNT 6  // toi da 1 text box co 4 dong
#define MAX_CHAR_PER_LINE  50  // toi da 1 dong cua text box co 50 ky tu

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
bool GTextBox::InstanceOfGTextBox(GStaticControl* control)
{
   GTextBox* tmp = dynamic_cast<GTextBox*>(control);
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
void GTextBox::init()
{
   //_D_.Debug("GTextBox (%s) ::init\n",this->name());
   m_itemCount = 0;
   m_fullWidth = m_fullHeight = 0;
   m_maxItemCount = 1;
   m_rowCount = 0;
   m_xOffset = m_yOffset = 0;
   m_vSpace = 10;
}

//////////////////////////////////////////////////////////////////////////////////////
GTextBox::GTextBox(const char* text, int width, int height, int textHeight, int fontId, int color)
{
   m_ppItem = NULL;
   m_ppItem = new GStaticControl * [MAX_TEXT_LINE_COUNT];

   if( strlen(text) == 0 )
   {
      //_DTraceError("nothing here\n");
      m_isEmpty = true;

   }
   else
   {
      m_isEmpty = false;
   }

   init();

   m_textHeight = textHeight;
   m_width = width;
   m_fullWidth = width;
   m_fullHeight = height;

   this->setManualSize(width,height);

   m_fontId = fontId;
   m_color = color;

   // delta: dua vao font chu de tinh he so delta. Font cang lon thi delta cang lon -> so character tren 1 dong cang be'
   int delta;
   switch( fontId )
   {
   case VERA_SANS_14_N:
      delta = 2.3;
      break;

   case ARIAL_18_N:
      delta = 2.5;
      break;

   case ARIAL_20_H:
   case ARIAL_20_N:
      delta = 3;
      break;

   case ARIAL_22_H:
   case ARIAL_22_N:
      delta = 3.6;
      break;

   default:
      delta = 3;
   }

   m_delta = delta;

   int charPerLine = (int)(width / m_delta / 4);
   //_DTraceError("width = %d, charPerLine = %d, delta = %d\n",width,charPerLine,m_delta);
   char lineStr[MAX_TEXT_LINE_COUNT][MAX_CHAR_PER_LINE];

   int rowCount;
   rowCount = splitStr(text, lineStr, charPerLine); // ham nay sua gia tri cua rowCount, lineStr
   m_rowCount = m_maxItemCount = rowCount;
   //_DTraceError("rowCount = %d\n",rowCount);

   for( int i = 0; i < rowCount; i++ )
   {
      //_DTraceError("lineStr[%d] = %s\n",i,lineStr[i]);
      GStaticControl* tmpText = new GStaticControl();
      GText* title_txt = new GText(lineStr[i]);
      title_txt->setFont(fontId, color);
      tmpText->setView(title_txt, 0,0);

      //tmpText->setManualSize(width,textHeight);
      tmpText->setH(textHeight);
      tmpText->setW(width);
      this->addTextItem(tmpText);
   }

   this->setManualSize(width,height);
}

//////////////////////////////////////////////////////////////////////////////////////
void GTextBox::updateTextBox(const char* text)
{
   int charPerLine = m_width / m_delta / 4;

   //_DTraceError("charPerLine = %d\n",charPerLine);
   char lineStr[MAX_TEXT_LINE_COUNT][MAX_CHAR_PER_LINE];

   int rowCount;
   //ham nay sua gia tri cua rowCount, lineStr
   rowCount = splitStr(text, lineStr, charPerLine);
   m_rowCount = m_maxItemCount = rowCount;
   //_DTraceError("rowCount = %d\n",rowCount);

   for( int i = 0; i < rowCount; i++ )
   {
      //_DTraceError("lineStr[%d] = %s\n",i,lineStr[i]);
      GStaticControl* tmpText = new GStaticControl();
      GText* title_txt = new GText(lineStr[i]);
      title_txt->setFont(m_fontId, m_color);
      tmpText->setView(title_txt, 0, 0);
      //tmpText->setManualSize(width,textHeight);
      tmpText->setH(m_textHeight);
      tmpText->setW(m_width);
      this->addTextItem(tmpText);
   }

   this->turnOnDrawTextBox();
}

//////////////////////////////////////////////////////////////////////////////////////
int GTextBox::splitStr(const char* text, char lineStr[][50], int charPerLine)
{
   int i = 0;
   int j = 0;
   int rowCount = 0;
   int lastSpace_i  = 0; // danh dau vi tri la dau cach ' ' cua ca chuoi
   int lastSpace_j  = 0; // danh dau vi tri la dau cach ' ' cua tung dong

   int length = strlen(text);

   //_DTraceError("text = %s, length = %d\n",text,length);
   while( i < length )
   {
      if( j > charPerLine )
      {
         lineStr[rowCount][lastSpace_j+1] = '\0';
         i = lastSpace_i;
         lastSpace_j = 0;
         j = 0;
         rowCount++;
      }
      else
      {
         lineStr[rowCount][j] = text[i];
         if( text[i] == ' ' )
         {
            lastSpace_i = i;
            lastSpace_j = j;
         }
         j++;
      }
      i++;
   }

   lineStr[rowCount][j] = '\0';
   rowCount++;
   return rowCount;
}

//////////////////////////////////////////////////////////////////////////////////////
GTextBox& GTextBox::setRows(int rowCount)
{
   m_rowCount = rowCount;
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GTextBox::~GTextBox()
{
   delete[] m_ppItem;
   m_ppItem = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
GTextBox& GTextBox::addTextItem(GStaticControl* textItem)
{
   addTextItemAt(textItem, m_itemCount);
}

//////////////////////////////////////////////////////////////////////////////////////
GTextBox& GTextBox::addTextItemAt(GStaticControl* textItem, int index)
{
   //_DTraceError("GTextBox::addTextItemAt *** index = %d, itemCount = %d\n",index,m_itemCount);
   m_lastError = GERROR_NONE;

   if( textItem == NULL )
   {
      m_lastError = GERROR_INVALID_HANDLE;
      return *this;
   }

   if( m_itemCount >= m_maxItemCount )
   {
      m_lastError = GERROR_EXCEED_MAXIMUM;
      //_DTraceError("GTextBox::addTextItemAt *** Maximum number of items in TextBox #%d exceeded! *** itemCount = %d, maxItemCount = %d\n", this->_id,m_itemCount,m_maxItemCount);
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

   //_DTraceError("GTextBox::addTextItemAt *** 2 *** index = %d, itemCount = %d\n",index,m_itemCount);
   int i = m_itemCount-1;

   while( i >= index)
   {
      m_ppItem[i+1] = m_ppItem[i];
      i --;
   }

   m_ppItem[index] = textItem;
   textItem->setParent(this);

   m_itemCount ++;

   //_DTraceError("GTextBox::addTextItemAt *** 3 *** index = %d, itemCount = %d, rowCount = %d\n",index,m_itemCount, m_rowCount);
   for( int i = index; i < index + m_rowCount; i++ )
   {
      if( i >= m_itemCount )
      {
         return *this;
      }
      setItemPos(m_ppItem[i], i);
   }
   //_D_.Debug("Added item at #%d, _iFirstVisibleItem: %d\n", index, _iFirstVisibleItem);
}

//////////////////////////////////////////////////////////////////////////////////////
GTextBox& GTextBox::destroyAllItem()
{
   if( this->m_isEmpty )
   {
      return *this;
   }

   m_lastError = GERROR_NONE;
   //_DTraceError("GTextBox::destroyAllItem *** itemCount = %d", m_itemCount);
   for( int i = 0; i < m_itemCount; i++ )
   {
      if( m_ppItem[i] != NULL )
      {
         m_ppItem[i]->destroyViews();
         delete m_ppItem[i];
         m_ppItem[i] = NULL;
      }
   }

   removeAllTextItem();
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GTextBox& GTextBox::removeAllTextItem()
{
   m_lastError = GERROR_NONE;

   m_itemCount = 0;
   //Chau added *** begin
   //this->w() = m_xOffset;
   //this->h() = m_yOffset;
   //Chau added *** end
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
int GTextBox::findIndex(GStaticControl* textItem)
{
   int i = m_itemCount-1;
   while( i >= 0 )
   {
      if( m_ppItem[i] == textItem )
      {
         return i;
      }
      i--;
   }

   // not found
   return -1;
}

//////////////////////////////////////////////////////////////////////////////////////
GTextBox& GTextBox::removeTextItem(GStaticControl* textItem)
{
   m_lastError = GERROR_NOT_FOUND;
   int i = findIndex(textItem);

   if( i >= 0 )
   {
      m_lastError = GERROR_NONE;
      return removeTextItemAt(i);
   }

   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GTextBox& GTextBox::removeTextItemAt(int index)
{
   m_lastError = GERROR_NOT_FOUND;

   if( (index < 0) || (index >=m_itemCount) )
   {
      m_lastError = GERROR_NOT_FOUND;
      return *this;
   }

   int i = index;
   m_itemCount--;

   for( ; i < m_itemCount; i++ )
   {
      m_ppItem[i] = m_ppItem[i+1];
      //change position of items
      setItemPos(m_ppItem[i],i);
   }

   m_lastError = GERROR_NONE;
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
void GTextBox::setItemPos(GStaticControl* item, int index)
{
   //_DTraceError("GTextBox::setItemPos *** index = %d\n",index);
   int r = 0;
   int c = 0;
   int x, y;

   if( index == 0 )
   {
      // item dau tien tren man hinh
      item->setPos(m_xOffset, m_yOffset);
      return;
   }

   r = index;
   x = m_xOffset + c * item->w();
   y = m_yOffset + r * (item->h() + m_vSpace);

   item->setPos(x, y);
   //_DTraceError("GTextBox::setItemPos *** index = %d, x = %d, y = %d\n",index,x,y);
   //_D_.Debug("SetPos for '%s' at (%3d,%3d)\n", item->name(), x, y);
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GTextBox::draw()
{
   //_DTraceError("GTextBox::draw ***layer = %d, x = %d, y = %d\n", this->layer(), this->x(), this->y());
   GError_t er = GERROR_NONE;

   //TODO: optimize this
   //int l = _scrollBar->layer();
   int l = this->layer();

   //_D_.Debug("(%s)\t\t#%d, layer %d, z %d, pos (%3d, %3d), ref (%3d, %3d), size (%3d, %3d):\n", classname(), this->id(), this->layer(), this->z(), this->x(), this->y(), this->refX(), this->refY(), this->w(), this->h());
   if( !m_pView )
   {
      //_D_.Warning("%s #%d - List WITHOUT view (background)\n", classname(), id());
   }
   else
   {
      int globalX = this->x() - this->refX() + m_pView->x();
      int globalY = this->y() - this->refY() + m_pView->y();
      ((GDrawable*)m_pView)->draw(l, globalX, globalY);
   }

   //_DTraceError("GTextBox::draw *** rowCount = %d\n",m_rowCount);
   int i;
   for( i = 0; i < m_rowCount; i++ )
   {
      if( i >= m_itemCount )
      {
         break;
      }

      GDrawable* item = m_ppItem[i];
      //_DTraceError("GTextBox::draw *** item %d - x = %d, y = %d\n",i,item->x(),item->y());

      int globalX = this->x() - this->refX() + item->x();
      int globalY = this->y() - this->refY() + item->y();

      er = item->draw(l, globalX, globalY);

      if( er != GERROR_NONE )
      {
         m_lastError = er;
      }
      //_D_.Debug("(%s)\t\t#%d, End\n", item->classname(), item->id());
   }

   return er;
}

//////////////////////////////////////////////////////////////////////////////////////
void GTextBox::turnOnDrawAll()
{
   //why return here ???
   return; //trungkt add 17-12-2013
   GNavigator::getInstance().pushUpdateRect(
      this->layer(),
      this->x() - this->refX(),
      this->y() - this->refY(),
      this->w(), this->h());
}

//////////////////////////////////////////////////////////////////////////////////////
// pushUpdateRect rect->w = full list width, rect->h = full list height
void GTextBox::turnOnDrawTextBox()
{
   //why return here
   return; //trungkt add 17-12-2013

   //_DTraceError("GTextBox::turnOnDrawTextBox *** fullWidth = %d, fullHeight = %d, layer = %d\n",fullWidth,fullHeight,this->layer());
   GNavigator::getInstance().pushUpdateRect(
      this->layer(),
      this->x() - this->refX(),
      this->y() - this->refY(),
      m_fullWidth, m_fullHeight);
}

//////////////////////////////////////////////////////////////////////////////////////
void GTextBox::pushUpdateRect(int layer, int x, int y, int w, int h)
{
   //_D_.Debug("Push update rect for list item (%d, %d, %d, %d) on layer %d\n",x,y,w,h,layer);
   GNavigator::getInstance().pushUpdateRect(
         layer,
         x + this->x() - this->refX() + m_xOffset,
         y + this->y() - this->refY() + m_yOffset,
         w, h);
   m_relX = this->x();
   m_relY = this->y();
}

//////////////////////////////////////////////////////////////////////////////////////
void GTextBox::pushUpdateRect(int layer, GStaticControl* item)
{
   pushUpdateRect(layer, item->x(), item->y(), item->w(), item->h());
}

//////////////////////////////////////////////////////////////////////////////////////
bool GTextBox::pushUpdateRectIfIntersect(int layer, int x, int y, int w, int h)
{
   int _x = x + this->m_relX - this->refX() + m_xOffset;
   int _y = y + this->m_relY - this->refY() + m_yOffset;

   bool res = GNavigator::getInstance().pushUpdateRectIfIntersect(layer, _x,   _y,   w, h);

   //_D_.Debug("Push update rect if intersect (%d, %d, %d, %d) on layer %d, res = %s\n", _x, _y, w, h, layer, res?"true":"false");
   return res;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GTextBox::pushUpdateRectIfIntersect(int layer, GStaticControl* item)
{
   bool res =  pushUpdateRectIfIntersect(layer, item->x(), item->y(), item->w(), item->h());
   //_D_.Debug("PushUpdateRectIfIntersect for list item %s at (%d,%d,%d,%d), res: %s\n", item->name(), item->x(), item->y(), item->w(), item->h(), res?"true":"false");
   return res;
}

//////////////////////////////////////////////////////////////////////////////////////
void GTextBox::setPos(int x, int y)
{
   GStaticControl::setPos(x, y);
   m_relX = x; m_relY = y;
}

//////////////////////////////////////////////////////////////////////////////////////
GStaticControl* GTextBox::getItem(int i)
{
   if( (i>=0) && (i<m_itemCount) )
   {
      return m_ppItem[i];
   }
   else
   {
      return NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
