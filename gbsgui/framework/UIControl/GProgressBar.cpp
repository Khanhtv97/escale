//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GProgressBar.cpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#include "GProgressBar.hpp"
#include "GDebug.hpp"
#include "GError.hpp"
#include "GNavigator.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
GProgressBar::GProgressBar(int maxSegmentCount, GAMLOAD_Object_t* normalPiece, GAMLOAD_Object_t* focusPiece,
                         int itemSpace_H, int itemSpace_V, int listWidth, int listHeight)
{
   //_DTraceError("GProgressBar::GProgressBar\n");
   m_pNormalPiece = NULL;
   m_pFocusPiece = NULL;
   m_pNormalList = NULL;
   m_pFocusList = NULL;

   m_maxSegmentCount = maxSegmentCount;
   m_segmentCount = 0;
   m_deltaSegment = 0;
   m_pNormalPiece = normalPiece;
   m_pFocusPiece = focusPiece;

   m_itemSpaceV = itemSpace_V;
   m_itemSpaceH = itemSpace_H;

   m_listW = listWidth;
   m_listH = listHeight;

   //m_pViewNormalPiece = new GView(m_pNormalPiece);
   //m_pViewFocusPiece = new GView(m_pFocusPiece);
   m_pNormalList = new GList(maxSegmentCount);
   m_pNormalList->setListDirection(LIST_DIRECTION_HORIZONTAL);
   m_pNormalList->setCols(m_maxSegmentCount);
   m_pNormalList->setRows(1);
   m_pNormalList->setItemSpace(itemSpace_H, itemSpace_V);
   m_pNormalList->setManualSize(listWidth,listHeight);

   for( int i = 0; i < m_maxSegmentCount; i++ )
   {
      GListItem* item = new GListItem();
      GView* viewNormalPiece = new GView(new GBitmap(LoadPreDefinedGam(m_pNormalPiece)));
      item->setViews(viewNormalPiece, NULL);
      m_pNormalList->addItem(item);
      //_DTraceError("GProgressBar::ADD ITEM TO LIST OF PROGRESSBAR   \n");
   }

   m_pFocusList = new GList(maxSegmentCount);
   m_pFocusList->setListDirection(LIST_DIRECTION_HORIZONTAL);
   m_pFocusList->setCols(m_maxSegmentCount);
   m_pFocusList->setRows(1);
   m_pFocusList->setItemSpace(m_itemSpaceH, m_itemSpaceV);
   m_pFocusList->setManualSize(m_listW, m_listH);
}

//////////////////////////////////////////////////////////////////////////////////////
GProgressBar::GProgressBar(int maxSegmentCount,   GAMLOAD_Object_t* focusPiece,
                         int itemSpace_H, int itemSpace_V, int listWidth, int listHeight)
{
   //_DTraceError("GProgressBar::GProgressBar\n");
   m_pNormalPiece = NULL;
   m_pFocusPiece = NULL;
   m_pNormalList = NULL;
   m_pFocusList = NULL;

   m_maxSegmentCount = maxSegmentCount;
   m_segmentCount = 0;
   m_deltaSegment = 0;
   m_pFocusPiece = focusPiece;

   m_itemSpaceV = itemSpace_V;
   m_itemSpaceH = itemSpace_H;

   m_listW = listWidth;
   m_listH = listHeight;

   //m_pViewFocusPiece = new GView(m_pFocusPiece);
   m_pFocusList = new GList(maxSegmentCount);
   m_pFocusList->setListDirection(LIST_DIRECTION_HORIZONTAL);
   m_pFocusList->setCols(m_maxSegmentCount);
   m_pFocusList->setRows(1);
   m_pFocusList->setItemSpace(m_itemSpaceH, m_itemSpaceV);
   m_pFocusList->setManualSize(m_listW, m_listH);
   //_DTraceError("SET LIST WIDTH: %d\n",m_listW);
   //_DTraceError("SET LIST HEIGHT: %d\n",m_listH);
}

//////////////////////////////////////////////////////////////////////////////////////
void GProgressBar::turnOnDrawProgress()
{
   //_DTraceError("GProgressBar::turnOnDrawProgress *** fullWidth = %d, fullHeight = %d, layer = %d\n",fullWidth,fullHeight,this->layer());
   GNavigator::getInstance().pushUpdateRect(
      this->layer(),
      this->x() - this->refX(),
      this->y() - this->refY(),
      m_fullWidth, m_fullHeight);
}

//////////////////////////////////////////////////////////////////////////////////////
GProgressBar::~GProgressBar()
{
   if( m_pNormalList != NULL )
   {
      //_DTraceError("GProgressBar::~GProgressBar *** delete m_pNormalList\n");
      m_pNormalList->destroyAllItem();
      delete m_pNormalList;
      m_pNormalList = NULL;
   }

   if( m_pFocusList != NULL )
   {
      //_DTraceError("GProgressBar::~GProgressBar *** delete m_pFocusList\n");
      m_pFocusList->destroyAllItem();
      delete m_pFocusList;
      m_pFocusList = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GProgressBar::setProgress(int percent)
{
   if( percent < 0 )
   {
      return;
   }
   m_deltaSegment = (int)(percent * m_maxSegmentCount / 100) - m_segmentCount;
   m_segmentCount = (int)(percent * m_maxSegmentCount / 100);
   //_DTraceError("percent: %d, m_deltaSegment: %d, m_segmentCount: %d \n",percent,m_deltaSegment,m_segmentCount);

   this->draw();
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GProgressBar::draw()
{
   //   _DTraceError("GProgressBar::draw *** m_segmentCount = %d - m_deltaSegment = %d\n",m_segmentCount,m_deltaSegment);
   int i;

#if 0

   if( m_segmentCount > 0 )
   {
      if( m_pFocusList == NULL )
      {
         _DTraceError("m_pFocusList == NULL \n");
         m_pFocusList = new GList(m_maxSegmentCount);
         m_pFocusList->setListDirection(LIST_DIRECTION_HORIZONTAL);
         m_pFocusList->setCols(m_maxSegmentCount);
         m_pFocusList->setRows(1);
         m_pFocusList->setItemSpace(m_itemSpaceH, m_itemSpaceV);
         m_pFocusList->setManualSize(m_listW, m_listH);
      }

      int curItemCount = m_pFocusList->getItemCount();
      _DTraceError("curItemCount = %d, m_deltaSegment = %d, m_segmentCount = %d \n",curItemCount,m_deltaSegment,m_segmentCount);
   }

   if( m_deltaSegment > 0 )
   {
      for( i = curItemCount; i < m_segmentCount; i++ )
      {
         _DTraceError("GProgressBar::draw() item %d \n",i);
         GListItem* item = new GListItem();
         GView* viewFocusPiece = new GView(new GBitmap(LoadPreDefinedGam(m_pFocusPiece)));
         //GView *viewFocusPiece = new GView(new GBitmap(gamObj));
         item->setViews(viewFocusPiece,NULL);
         m_pFocusList->addItem(item);
         m_pFocusList->updateListItem(m_pFocusList->getItem(i)); // ve lai m_pFocusList, chi ve them item moi add them vao
      }
   }
   else if( m_deltaSegment < 0 )
   {
      for( i = m_segmentCount-m_deltaSegment-1; i >= m_segmentCount; i-- )
      {
         m_pFocusList->updateListItem(m_pFocusList->getItem(i)); // ve lai m_pFocusList, chi ve lai phan dien tich truoc do removed item chiem
         GListItemBase* item = m_pFocusList->getItem(i);
         m_pFocusList->removeItemAt(i); // goi lenh updateListItem truoc khi removeItem
         item->destroyViews();
         delete item;
         item = NULL;
         //m_pNormalList->updateListItem(m_pNormalList->getItem(i));
      }
   }
   else if( m_segmentCount == 0 )
   {
      //m_pFocusList->removeAllItem();
      m_pFocusList->destroyAllItem();
      //_DTraceError("DESTROY ALL ITEM OF PROGRESS BAR\n");
      m_pFocusList->m_fullWidth = m_listW;
      m_pFocusList->m_fullHeight = m_listH;
      m_pFocusList->turnOnDrawList();
      //GNavigator::getInstance()->redrawAll();
      if( m_pNormalList != NULL )
      {
         //_DTraceError("m_pNormalList->draw\n");
         m_pNormalList->turnOnDrawAll();
      }
   }

#else

   m_pFocusList->destroyAllItem();
   for( i = 0; i < m_segmentCount; i++ )
   {
      GListItem* item = new GListItem();
      GView* viewFocusPiece = new GView(new GBitmap(LoadPreDefinedGam(m_pFocusPiece)));
      item->setViews(viewFocusPiece,NULL);
      m_pFocusList->addItem(item);
   }

#endif

   //_DTraceError("GProgressBar::draw *** finished\n");
   return GERROR_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////
// posX, posY la toa do so voi goc toa do O (0,0)
void GProgressBar::addToActivity(GActivity* form, int posX, int posY, int layer)
{
   _DTraceError("BEGIN ADD TO ACTIVITY");
   if( m_pNormalList != NULL )
   {
      //_DTraceError("m_pNormalList != NULL\n");
      form->addControl(this->m_pNormalList, layer);
      this->m_pNormalList->setPos(posX,posY);
   }

   if( m_pFocusList != NULL )
   {
      //_DTraceError("m_pFocusList != NULL\n");
      form->addControl(this->m_pFocusList, layer);
      this->m_pFocusList->setPos(posX,posY);
   }

   _DTraceError("END ADD TO ACTIVITY\n");
}

//////////////////////////////////////////////////////////////////////////////////////
void GProgressBar::removeFromActivity(GActivity* form)
{
   if( form != NULL )
   {
      form->removeControl(this->m_pNormalList);
      form->removeControl(this->m_pFocusList);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GProgressBar::setEnable(bool enable)
{
   this->m_pNormalList->setEnable(enable);
   this->m_pFocusList->setEnable(enable);
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
