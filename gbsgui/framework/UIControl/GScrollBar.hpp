//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GScrollBar.hpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GSCROLLBAR_HPP_
#define GSCROLLBAR_HPP_

#include "GStaticControl.hpp"
#include "GDrawable.hpp"


typedef enum ScrollBar_Orientation_e
{
   SCROLLBAR_ORIENTATION_VERTICAL,
   SCROLLBAR_ORIENTATION_HORIZONTAL
}
ScrollBar_Orientation_t;

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GScrollBar : public GStaticControl
{
private:
   //GStaticControl::setView;

protected:
   GBitmap* m_pTrack;
   GBitmap* m_pThumb;
   int m_trackLength;
   int m_scroll; // in percent

   bool m_isParentFocused;

   ScrollBar_Orientation_t m_orientation;

   void init();

public:
   GScrollBar(int layer);
   GScrollBar(int layer, GBitmap* trackBmp, GBitmap* thumbBmp);
   GScrollBar(GBitmap* trackBmp, GBitmap* thumbBmp);
   ~GScrollBar() {};

   const char* classname() {return "GScrollBar";};

   GScrollBar& setTrack(GBitmap* trackBmp);
   GScrollBar& setThumb(GBitmap* thumbBmp);
   GScrollBar& setOrientaton(ScrollBar_Orientation_t orientation);
   GScrollBar& setTrackLength(int length);

   GError_t draw();
   GError_t scroll(int percent);

   void setParentFocused(bool isFocused);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GSCROLLBAR_HPP_ */
