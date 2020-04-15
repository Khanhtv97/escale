//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GStaticControl.hpp - static item on OSD (1 state):                         //
//       static text, static bitmap, this is also base class for all UI Control     //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GSTATICCONTROL_HPP_
#define GSTATICCONTROL_HPP_

#include "GView.hpp"
#include "GObject.hpp"

#define GLabel GStaticControl

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GStaticControl : public virtual GView, public virtual GObject
{
private:
   //disable addView, and removeView methods, use setView instead
   using GView::addView;
   using GView::removeView;
   using GView::destroyViews;

protected:
   GView* m_pView;
   std::string m_name;
   void*  m_pData;
   bool   m_isVisible;

public:
   GStaticControl();
   GStaticControl(const char* controlName);
   GStaticControl(GDrawable* drawable, int x = 0, int y =0);
   GStaticControl(int layer);
   GStaticControl(int x, int y);
   GStaticControl(int layer, int x, int y);
   GStaticControl(int x, int y, int ref_x, int ref_y);
   GStaticControl(int layer, int x, int y, int ref_x, int ref_y);

   const char* classname() {return "GStaticControl";};

   const char* name();
   void setName(const char* s);

   bool isVisible();
   void setVisible(bool visible);

   virtual GStaticControl& setView(GView* view);
   virtual GStaticControl& setView(GDrawable* drawable, int x = 0, int y = 0);

   virtual GError_t draw();
   GError_t draw1(int nDraw1, int nClrBg);
   GStaticControl& updView(GDrawable* d, int x, int y, int w, int h, int nDraw1, int nClrBg);

   void* getData();
   void setData(void* data);
   virtual void onSetData();
   virtual void destroyViews();

   bool intersect(GRect& rect);

   // redraw
   virtual GError_t update();
   virtual bool processKey(U32 key) {return false;};
   virtual ~GStaticControl();

   //Con tro den cac controls se navigation den theo 4 phim mui ten,
   //mac dinh = NULL la tuc la de GActivity tu tinh toan
   GStaticControl* m_pUp;
   GStaticControl* m_pDown;
   GStaticControl* m_pLeft;
   GStaticControl* m_pRight;
};

//////////////////////////////////////////////////////////////////////////////////////
// class GTxtCtl - 2013-05-15we_th66
//////////////////////////////////////////////////////////////////////////////////////
class GTxtCtl : public GStaticControl
{
private:
   char sTxt[128];
   int X, Y, W, H, _xt, _yt, _textID, nFontId;
   S32 nARGB;
   U32 nTColor;

public:
   GTxtCtl(char* s, int x, int y, int w, int h, int xt, int yt, int nFid, U32 tcolor, S32 argb);
   void setTxt(char* s, int nDraw1 = 1);

   virtual GError_t draw();
   GError_t dra1();
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GSTATICCONTROL_HPP_ */
