//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GView.hpp - this is a compound of multiple drawable                        //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GVIEW_HPP_
#define GVIEW_HPP_

#include "GDrawable.hpp"
#include "GError.hpp"

#define MAX_INBOUND_DRAWABLES 20

//! IMPORTANT!!! moi toa do ben trong View deu dc tinh tuong doi so voi View chua no
//! IMPORTANT!!! moi z-index ben trong View chi co tac dung trong View hien tai, z-index voi be ngoai la z-index cua ban than View

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {
//////////////////////////////////////////////////////////////////////////////////////
class GView: public virtual GDrawable
{
private:
   void init();

protected:
   // array of containing views, sorted asc
   GDrawable* m_pViews[MAX_INBOUND_DRAWABLES]; // thuc te chua moi object thuoc clases ke thua tu class GDrawable
   int m_viewCount;

   int findIndexById(int id);

public:
   static bool InstanceOfGView(GDrawable* drawable);

   GView();
   GView(GDrawable* drawable1, GDrawable* drawable2 = NULL, GDrawable* drawable3 = NULL, GDrawable* drawable4 = NULL);
   GView(int layer);
   GView(int x, int y);
   GView(int layer, int x, int y);
   GView(int x, int y, int ref_x, int ref_y);
   GView(int layer, int x, int y, int ref_x, int ref_y);
   virtual ~GView();

   const char* classname() {return "GView";};

   GView& addView(GDrawable* view, int x = 0, int y = 0);
   GView& removeView(int id);

   //re-arrange order of children
   GError_t bringForward(int id);
   GError_t sendBackward(int id);
   GError_t bringToFront(int id);
   GError_t sendToBack(int id);

   virtual GError_t draw();

   //!!!Be careful when using this method, be sure that all child views of this object are not used anywhere else
   void destroyViews();
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GVIEW_HPP_ */
