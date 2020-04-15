//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GList.hpp                                                                  //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GLIST_HPP_
#define GLIST_HPP_

#include "GFocusable.hpp"
#include "GScrollBar.hpp"
#include "GListItemBase.hpp"
#include "GActivity.hpp"

typedef enum List_Direction_e
{
   LIST_DIRECTION_VERTICAL,
   LIST_DIRECTION_HORIZONTAL,
   LIST_DIRECTION_GRID            //2013-05-09thu_th66 for FullEPG
} List_Direction_t;

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GList: public GFocusable
{
public:
   bool m_isControlBar; //Chau added: == true -> dung GList thay cho ControlBar
   int m_fullWidth; //Chau added: su dung de update lai toan bo list
   int m_fullHeight; //Chau added: su dung de update lai toan bo list

   GList(int maxItemCount);
   GList(int maxItemCount, int rowCount, int colCount);

   const char* classname()   {return "GList";};

   bool nextPage();
   bool previousPage();

   void setCircleNavigation(bool enableCircleNav);
   void setPos(int x, int y);
   void setLastVisibleItemIndex(int index);

   GList& addItem(GListItemBase* listItem);
   GList& addItemAt(GListItemBase* listItem, int index);
   GList& removeItem(GListItemBase* listItem);
   GList& removeAllItem();
   GList& destroyAllItem();
   GList& removeItemAt(int index);

   GListItemBase* getFocusedItem();
   int getFocusedItemId();
   GListItemBase* getSelectedItem();
   int getSelectedItemId();
   int getItemCount();

   GListItemBase* getItem(int i);

   GError_t setFocusedItem(GListItemBase* listItem, bool dispatchEvt = true);
   GError_t setFocusedItem(int index, bool dispatchEvt = true);
   GError_t setSelectedItem(GListItemBase* listItem, bool dispatchEvt = true);
   GError_t setSelectedItem(int index, bool dispatchEvt = true);
   GError_t updateListItem(GListItemBase* item); // //Chau added: chi ve them item dc add them vao. Su dung cho progress bar (volume, waiting)
   void updateFirstVisibleItemIndex(int index);

   GList& setCols(int colCount);
   int getCols() { return m_columnCount; };
   GList& setRows(int rowCount);
   int getRows() { return m_rowCount; };
   GList& setListDirection(List_Direction_t direction);
   GList& setItemSpace(int hSpace, int vSpace)    {m_hSpace = hSpace; m_vSpace = vSpace;};
   GList& setItemOffset(int xoffset, int yoffset) {m_xOffset = xoffset; m_yOffset = yoffset;};
   GList& setScrollBar(GScrollBar* scrollBar)     {m_pScrollBar = scrollBar; };
   int getXOffset() {return m_xOffset;};
   int getYOffset() {return m_yOffset;};

   virtual GError_t draw();
   void draw1(int nClrBg=0);
   void draw0(int nBLITcp=0);
   bool processKey(U32 key, bool forced);
   bool processKey(U32 key);

   void pushUpdateRect(int layer, int x, int y, int w, int h);
   void pushUpdateRect(int layer, GListItemBase* item);
   bool pushUpdateRectIfIntersect(int layer, int x, int y, int w, int h);
   bool pushUpdateRectIfIntersect(int layer, GListItemBase* item);

   void turnOnDrawItem(GListItemBase* item);
   void turnOnDrawAll();
   void turnOnDrawList();

   static bool InstanceOfGList(GStaticControl* control);

   ~GList();

protected:
   GListItemBase** m_ppListItem;
   int m_itemCount;
   int m_maxItemCount;
   int m_iFocused;
   int m_iFocPre;            //2013-05-06mo_th66;
   int m_iSelected;
   int m_iSelPre;
   int m_iFirstVisibleItem;
   int m_i1visiPre; //2013-05-07tu_th66;
   int m_columnCount; // so luong hang, cot trong list box
   int m_rowCount;
   int m_vSpace; // khoang cach giua cac items
   int m_hSpace;
   int m_xOffset;
   int m_yOffset;
   bool m_circleNavigation;
   List_Direction_t m_direction;
   GScrollBar* m_pScrollBar;

   bool m_needRedrawAll;

   // for use internally to save actual x & y of list when drawing (x, y in draw() method has been changed externally)
   int m_relX;
   int m_relY;

   int findIndex(GListItemBase* listItem);
   int findNextItem(int currentItem, Direction_t direction);

   // index - so thu tu cua item trong so cac item can hien thi tren man hinh
   void setItemPos(GListItemBase* item, int index);
   void upd1visi(int index);
   void initGList();
   bool scrollNextPage();
   bool scrollPreviousPage();
   virtual void onFocusChanged();
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GLIST_HPP_ */
