//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GNavigator.hpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GNAVIGATOR_HPP_
#define GNAVIGATOR_HPP_

#include "GObject.hpp"
#include "GActivity.hpp"
#include "GError.hpp"

#define MAX_ACTIVE_ACTIVITY_COUNT 20
#define MAX_UPDATE_RECTANGLE_COUNT 20

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GNavigator : GObject
{
private:
   int findActivity(GActivity* form);
   void closeActivityById(int i);
   void clearUpdateRects(int layer);
   GNavigator();

   // danh dau pham vi can phai ve lai tren moi layer
   GRect m_updateRectsL0[MAX_UICONTROLS_IN_ACTIVITY];
   GRect m_updateRectsL1[MAX_UICONTROLS_IN_ACTIVITY];

   GActivity* m_ppActivities[MAX_ACTIVE_ACTIVITY_COUNT];
   U32 m_keyCode;
   int m_activityCount;
   int m_updateRectCountL0;
   int m_updateRectCountL1;
   BOOL m_isDrawingAll;

public:
#ifdef ASYNC_DRAW
   void OnRequestDraw(EVENT_HANDLER_PARAM);
#endif
   static GNavigator& getInstance();

   const char* classname()   {return "GNavigator";};
   void onKeyPressed(EVENT_HANDLER_PARAM);
   bool processKey(U32 key);

   ~GNavigator();

   void closeActivity(GActivity* form);
   void closeAllActivity();
   void openActivity(GActivity* form);

   int getActivityCount() {return m_activityCount;};

   GActivity* getFocusedActivity();
   GActivity* getOpenedAvtivityByName(const char* name);

   //Edited by hiendv added param drawConflict (neu chi co 2 bien thi ko du xac dinh case drawConflic, co nhieu noi goi redrawAll(true), redrawAll())
   GError_t redrawAll(bool forceClearAll = false, bool reinitText = true, bool drawConflict = false);

   void pushUpdateRect(int layer, GStaticControl* control);

   void pushUpdateRect(int layer, int x, int y, int w, int h);

   bool pushUpdateRectIfIntersect(int layer, int x, int y, int w, int h);

   bool pushUpdateRectIfIntersect(int layer, GStaticControl* control);

   // lay activity ngay ben duoi
   GActivity* getUnderForm();
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GNAVIGATOR_HPP_ */
