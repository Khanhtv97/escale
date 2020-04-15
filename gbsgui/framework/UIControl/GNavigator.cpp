//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GNavigator.cpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#include "GNavigator.hpp"
#include "GDelegate.hpp"
#include "GSystem.hpp"
#include "GEvents.hpp"
#include "GKey.hpp"
#include "GDebug.hpp"
#include "GKeyManager.hpp"
#include "APIBitmap.hpp"
#include "APIText.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
GNavigator& GNavigator::getInstance()
{
   static GNavigator instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
GNavigator::GNavigator()
{
   APIBitmapInit();
   APITextInit();
   m_activityCount = 0;
   m_updateRectCountL0 = 0;
   m_updateRectCountL1 = 0;
   m_isDrawingAll = FALSE;
   m_keyCode = KEY_NUMBERS;

   for( int i = 0; i < MAX_ACTIVE_ACTIVITY_COUNT; ++i )
   {
      m_ppActivities[i] = NULL;
   }

   GKeyManager::getInstance().setInputMode(KEY_INPUT_RC | KEY_INPUT_UART);
}

//////////////////////////////////////////////////////////////////////////////////////
GNavigator::~GNavigator()
{
   APIBitmapTerm();
}

//////////////////////////////////////////////////////////////////////////////////////
bool GNavigator::processKey(U32 key)
{
   // send key to opened and focused form first

   m_keyCode = key;

   if( (m_activityCount > 0) && (m_ppActivities[m_activityCount-1]->processKey(m_keyCode)) )
   {
      //TODO: optimize the redrawAll method
      redrawAll();
      return true; // ko xu ly
   }

   switch( m_keyCode )
   {
      case KEY_RC_EXIT:
      case KEY_KB_DELETE:
         // close all opened form
         closeAllActivity();
         return true;
         break;

      case 'D':
         //_D_.SetLogLevel(LOG_LEVEL_DEBUG);
         return true;
         break;

      case 'Y':
         //_D_.SetLogLevel(LOG_LEVEL_INFO);
         return true;
         break;

      case 'E':
         //_D_.SetLogLevel(LOG_LEVEL_ERROR);
         return true;
         break;

      case 'i':
      case 'I':
         _S_.meminfo(0);
         _DTraceInfo("CPU Load: %.2f\%\n", (double)_S_.cpuload()/100);
         return true;
         break;

      case 'T':
         _S_.taskinfo();
         _DTraceInfo("CPU Load: %.2f\%\n", (double)_S_.cpuload()/100);
         return true;
         break;

      default:
         break;
   }

   //this event is addListener in gbsgui --> if there's no UIControls processKey so gbsgui will process this key
   dispatchEvent(GEVENT_KEY_PRESSED, (void*)&m_keyCode);

   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
void GNavigator::onKeyPressed(EVENT_HANDLER_PARAM)
{
   if( event != GEVENT_KEY_PRESSED )
   {
      return;
   }

   //U32 key = *((U32*)event_data);
   //_DTraceError("Key: %d", key);
   //char keyname[30];
   //GKeyManager::getInstance().getKeyName(key, keyname);
   //this->processKey(key);
}

//////////////////////////////////////////////////////////////////////////////////////
int GNavigator::findActivity(GActivity* form)
{
   int i = m_activityCount - 1;

   if( i >= MAX_ACTIVE_ACTIVITY_COUNT )
   {
      i = MAX_ACTIVE_ACTIVITY_COUNT - 1;
   }

   while( i >= 0 )
   {
      if( m_ppActivities[i] == form )
      {
         return i;
      }
      i--;
   }
   return -1; // not found
}

//////////////////////////////////////////////////////////////////////////////////////
void GNavigator::closeActivityById(int i)
{
   if( m_activityCount > 0 )
   {
      m_activityCount--;
   }

   if( i == m_activityCount && m_ppActivities[i] != NULL )
   {
      m_ppActivities[i]->setFocus(false);

      if( 0 < i )
      {
         m_ppActivities[i-1]->setFocus(true);
      }
   }
   else
   {
      while( i < m_activityCount )
      {
         m_ppActivities[i] = m_ppActivities[i+1];
         i++;
      }
   }//for(i=0;i<m_activityCount;i++){   m_ppActivities[i]->turnOnDrawAll();}

   //redrawAll(true);
}

//////////////////////////////////////////////////////////////////////////////////////
void GNavigator::closeAllActivity()
{
   int i;
   int countDisabledForm = 0;

   if( m_activityCount > MAX_ACTIVE_ACTIVITY_COUNT )
   {
      m_activityCount = MAX_ACTIVE_ACTIVITY_COUNT;
   }

   for( i = 0; i < m_activityCount; i++ )
   {
      if( (m_ppActivities[i] != NULL) && (m_ppActivities[i]->m_enabled) )
      {
         m_ppActivities[i]->close();
      }
      else
      {
         countDisabledForm++;
      }
   }

   m_activityCount = countDisabledForm;
}

//////////////////////////////////////////////////////////////////////////////////////
void GNavigator::closeActivity(GActivity* form)
{
   int i = findActivity(form);

   _DTraceDebug("(%s)\t, closeActivity #%d, found at index: %d", classname(), form->id(), i);

   if( i < 0 )
   {
      _DTraceError("(%s)\t - Trying to Close unopened %s #%d", this->classname(), form->classname(), form->id());
      return;
   }

   closeActivityById(i);
}

//////////////////////////////////////////////////////////////////////////////////////
void GNavigator::openActivity(GActivity* form)
{
   int i = findActivity(form);
   //_DTraceError("GNavigator::openActivity(%s) openFormId%d preOpenedId%d Forms%d\n",form->classname(),form->id(),i,m_activityCount);
   if( 0 <= i && i == m_activityCount-1 )
   {
      _DTraceError("GNavigator::openActivity(%s)Form's already opened&focused!\n",form->classname());
      return;
   }//form already opened&focused

   if( i < 0 )
   {
      m_ppActivities[m_activityCount] = form;
      m_activityCount++;
   }
   //_DTraceError("GNavigator::openActivity(%s) openFormId%d preOpenedId%d Forms%d\n",form->classname(),form->id(),i,m_activityCount);
   if( form->m_enabled )
   {
      form->setFocus(true);

      //swap the form has top layer to the top displaying
      for( int m = 0; m < m_activityCount-1; m++ )
      {
         for( int n = m+1; n < m_activityCount; n++ )
         {
            if( m_ppActivities[m]->m_isTopLayer && !m_ppActivities[n]->m_isTopLayer)
            {
               GActivity* pTemp = m_ppActivities[m];
               m_ppActivities[m] = m_ppActivities[n];
               m_ppActivities[n] = pTemp;
               break;
            }
         }
      }
   }
   else if( 1 < m_activityCount )
   { // dua disabled form len vi tri dau tien cua array
      for( i = m_activityCount-1; 0 < i; i-- )
      {
         m_ppActivities[i] = m_ppActivities[i-1];
      }

      m_ppActivities[0] = form;

      if( m_ppActivities[m_activityCount-1] != NULL )
      {
         m_ppActivities[m_activityCount-1]->setFocus(true);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GActivity* GNavigator::getOpenedAvtivityByName(const char* name)
{
   int i = m_activityCount-1;

   if( i >= MAX_ACTIVE_ACTIVITY_COUNT )
   {
      i = MAX_ACTIVE_ACTIVITY_COUNT - 1;
   }

   while( i >= 0 )
   {
      if( (m_ppActivities[i] != NULL) && (strcmp(m_ppActivities[i]->name(), name) == 0) )
      {
         return m_ppActivities[i];
      }
      i--;
   }
   return NULL; // not found
}

//////////////////////////////////////////////////////////////////////////////////////
GActivity* GNavigator::getFocusedActivity()
{
   if( m_activityCount < 1 )
   {
      return NULL;
   }

   int i = m_activityCount-1;

   if( i >= MAX_ACTIVE_ACTIVITY_COUNT )
   {
      i = MAX_ACTIVE_ACTIVITY_COUNT - 1;
   }

   while( i >= 0 )
   {
      if( (m_ppActivities[i] != NULL) && (m_ppActivities[i]->m_enabled) )
      {
         return m_ppActivities[i];
      }
      else
      {
         i--;
      }
   }

   return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GNavigator::redrawAll(bool forceClearAll, bool reinitText, bool drawConflict)
{
   if( m_isDrawingAll == TRUE )
   {
      return GERROR_NONE;
   }

   m_isDrawingAll = TRUE;

   if( reinitText )
   {
      APITextInit();
   }

   ClearPlane(DEFAULT_DRAW_LAYER_ID);

   for( int i = 0; i < m_activityCount; i++ )
   {
      if( m_ppActivities[i] != NULL )
      {
         m_ppActivities[i]->draw(DEFAULT_DRAW_LAYER_ID);
      }
   }

   GBLITcopy(0, DEFAULT_DRAW_LAYER_ID, CVBS_DRAW_LAYER_ID);
   GBLITcopy(0, DEFAULT_DRAW_LAYER_ID, HDMI_DRAW_LAYER_ID);

   m_isDrawingAll = FALSE;

   return GERROR_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////
void GNavigator::clearUpdateRects(int layer)
{
   int count = (layer <= 0)? m_updateRectCountL0 : m_updateRectCountL1;

   if( count < 1 )
   {
      return;
   }

   GRect* rect = (layer <= 0)? m_updateRectsL0 : m_updateRectsL1;

   if( m_activityCount > 0 )
   {
      int i;
      for( i=0; i<count; i++ )
      {
         int x = rect[i].x() + m_ppActivities[m_activityCount-1]->x() - m_ppActivities[m_activityCount-1]->refX();
         int y = rect[i].y() + m_ppActivities[m_activityCount-1]->y() - m_ppActivities[m_activityCount-1]->refY();
         int w = rect[i].w();
         int h = rect[i].h();

         //_DTraceDebug("Clear rect (%d, %d, %d, %d) on layer %d\n", x, y, w, h, layer);
         //TODO
         ClearRect(layer, x, y, w, h);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool GNavigator::pushUpdateRectIfIntersect(int layer, GStaticControl* control)
{
   //todo: why return here ???
   return false;//2013-04-27sa_th66
   //return pushUpdateRectIfIntersect(layer, control->x(), control->y(), control->w(), control->h());
}

//////////////////////////////////////////////////////////////////////////////////////
bool GNavigator::pushUpdateRectIfIntersect(int layer, int x, int y, int w, int h)
{
   //todo: why return here ???
   return false;//2013-04-27sa_th66
   //_DTraceDebug("GNavigator PushUpdateRectIfIntersect on layer %d at (%d,%d,%d,%d)\n", layer, x, y, w, h);
   //   int i;
   //   if( layer <= 0 )
   //   {
   //      for( i = 0; i < m_updateRectCountL0; i++ )
   //      {
   //         if( m_updateRectsL0[i].intersect(x, y, w, h) )
   //         {
   //            //_DTraceDebug("PushUpdateRectIfIntersect on layer 0 at (%d,%d,%d,%d)\n", x, y, w, h);
   //            pushUpdateRect(0, x, y, w, h);
   //            return true;
   //         }
   //      }
   //   }
   //   else
   //   {
   //      for( i = 0; i < m_updateRectCountL1; i++ )
   //      {
   //         if( m_updateRectsL1[i].intersect(x, y, w, h) )
   //         {
   //            //_DTraceDebug("PushUpdateRectIfIntersect on layer 1 at (%d,%d,%d,%d)\n", x, y, w, h);
   //            pushUpdateRect(1, x, y, w, h);
   //            return true;
   //         }
   //      }
   //   }
   //
   //   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
void GNavigator::pushUpdateRect(int layer, int x, int y, int w, int h)
{
   if( layer <= 0 )
   {
      if( m_updateRectCountL0 >= MAX_UICONTROLS_IN_ACTIVITY )
      {
         _DTraceDebug("(%s)\t, CANNOT add more rectangles to be updated on layer 0", classname());
         return;
      }
      m_updateRectsL0[m_updateRectCountL0].reset(x,y,w,h);
      m_updateRectCountL0++;
      //_DTraceDebug("pushUpdateRect (%d,%d,%d,%d) on layer %d, rect count: %d\n", x, y, w, h, layer, m_updateRectCountL0);
   }
   else
   {
      if( m_updateRectCountL1 >= MAX_UICONTROLS_IN_ACTIVITY )
      {
         _DTraceDebug("(%s)\t, CANNOT add more rectangles to be updated on layer 0", classname());
         return;
      }
      m_updateRectsL1[m_updateRectCountL1].reset(x, y, w, h);
      m_updateRectCountL1++;
      //_DTraceDebug("pushUpdateRect (%d,%d,%d,%d) on layer %d, rect count: %d\n", x, y, w, h, layer, m_updateRectCountL1);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GNavigator::pushUpdateRect(int layer, GStaticControl* control)
{
   //todo: why return here ???
   return;//2013-04-27sa_th66
   //pushUpdateRect(layer, control->x(), control->y(), control->w(), control->h());
   //_DTraceDebug("pushUpdateRect: control (%s %s) with rect (%d,%d,%d,%d) on layer %d\n", control->classname(), control->name(), control->x(), control->y(), control->w(), control->h(), layer);
}

//////////////////////////////////////////////////////////////////////////////////////
GActivity* GNavigator::getUnderForm()
{
   GActivity* pUnderForm = NULL;
   if( m_activityCount >= 2 )
   {
      pUnderForm = m_ppActivities[m_activityCount - 2];
   }
   return pUnderForm;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
