//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GProgressBar.hpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GPROGRESSBAR_HPP_
#define GPROGRESSBAR_HPP_

#include "GDynamicControl.hpp"
#include "GListItem.hpp"
#include "GList.hpp"
#include "GView.hpp"

#define ProgressSegment 0.5  // chia track thanh 20 phan bang nhau

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GProgressBar : public GStaticControl
{
protected:
   GAMLOAD_Object_t* m_pNormalPiece;
   GAMLOAD_Object_t* m_pFocusPiece;

   int m_itemSpaceH;
   int m_itemSpaceV;
   int m_listW;
   int m_listH;
   int m_maxSegmentCount;
   int m_segmentCount;
   int m_deltaSegment;

public:
   GProgressBar(int maxSegmentCount, GAMLOAD_Object_t* normalPiece, GAMLOAD_Object_t* focusPiece, int itemSpace_H, int itemSpace_V, int listWidth, int listHeight);
   GProgressBar(int maxSegmentCount, GAMLOAD_Object_t* focusPiece, int itemSpace_H, int itemSpace_V, int listWidth, int listHeight);
   ~GProgressBar();

   const char* classname() {return "GProgressBar";};

   GError_t draw();

   void setProgress(int percent);
   void setEnable(bool enable);
   void addToActivity(GActivity* form, int posX, int posY, int layer = 1);
   void removeFromActivity(GActivity* form);
   void turnOnDrawProgress();

   GList* m_pNormalList;
   GList* m_pFocusList;
   int m_fullWidth;
   int m_fullHeight;
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GPROGRESSBAR_HPP_ */
