//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GActivity.cpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#include "GActivity.hpp"
#include "GList.hpp"
#include "GTextBox.hpp"
#include "GKey.hpp"
#include "GDebug.hpp"
#include "GNavigator.hpp"
#include "GDynamicControl.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
bool GActivity::InstanceOfGActivity(GView* view)
{
   GActivity* tmp = dynamic_cast<GActivity*>(view);
   //_DTraceDebug("View at 0x%08x is GActivity: %s\n",control, (tmp!=NULL)?"YES":"NO");
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
void GActivity::initActivity()
{
   m_controlCount = 0;
   m_defaultFocusedControlIdx = m_focusedControlIdx = -1;
   m_needRedrawAll = true;
   m_enabled = true; // form bthuong thi m_enabled = true
   m_isTopLayer = false;
   m_isVisible = false;

   for( int i = 0; i < MAX_UICONTROLS_IN_ACTIVITY; ++i )
   {
      m_pControls[i] = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GActivity::~GActivity()
{
}

//////////////////////////////////////////////////////////////////////////////////////
GActivity::GActivity() : GFocusable()
{
   initActivity();
}

//////////////////////////////////////////////////////////////////////////////////////
GActivity::GActivity(int layer) : GFocusable(layer)
{
   initActivity();
}

//////////////////////////////////////////////////////////////////////////////////////
GActivity::GActivity(int x, int y) : GFocusable(x, y)
{
   initActivity();
}

//////////////////////////////////////////////////////////////////////////////////////
GActivity::GActivity(int layer, int x, int y) : GFocusable(layer, x, y)
{
   initActivity();
}

//////////////////////////////////////////////////////////////////////////////////////
GActivity* GActivity::getOpenedAvtivityByName(const char* name)
{
   return GNavigator::getInstance().getOpenedAvtivityByName(name);
}

//////////////////////////////////////////////////////////////////////////////////////
int GActivity::findIndex(GStaticControl* control)
{
   int i = m_controlCount - 1;

   if( i >= MAX_UICONTROLS_IN_ACTIVITY )
   {
      i = MAX_UICONTROLS_IN_ACTIVITY - 1;
   }

   while( i >= 0 )
   {
      if( m_pControls[i] == control )
      {
         return i;
      }
      i--;
   }

   return -1; // not found
}

//////////////////////////////////////////////////////////////////////////////////////
int GActivity::forceControlRedraw(GStaticControl* control)
{
   int i = findIndex(control);
   if( i < 0 )
   {
      _DTraceWarning("forceControlRedraw failed: control not in Activity");
      return -1;
   }

   GNavigator::getInstance().pushUpdateRect(m_pControls[i]->layer(), m_pControls[i]);
   return i;
}

//////////////////////////////////////////////////////////////////////////////////////
GActivity& GActivity::addControl(GStaticControl* control, int inLayer)
{
   if( control == NULL )
   {
      return *this;
   }

   m_lastError = GERROR_NONE;

   if( m_controlCount >= MAX_UICONTROLS_IN_ACTIVITY )
   {
      m_lastError = GERROR_EXCEED_MAXIMUM;
      _DTraceError("Maximum number of controls in Activity #%d exceeded!\n", this->m_id);
      return *this;
   }

   // insert to proper place, sort asc by z-index
   int i = m_controlCount-1;
   while( (i >= 0) && (m_pControls[i]->z() > control->z()) )
   {
      m_pControls[i+1] = m_pControls[i];
      i--;
   }

   if( i >= 0 )
   {
      if( m_pControls[i]->id() == control->id() )
      {
         m_lastError = GERROR_ALREADY_EXISTED;
         return *this;
      }
   }

   m_pControls[i+1] = control;
   control->setParent(this);
   m_controlCount++;

   //TODO: set lai x, y, refX, refY khi control->x() - control->refX() < 0 hoac control->y() - control->refY() <0
   int cW =  control->x() - control->refX() + control->w();
   int cH =  control->y() - control->refY() + control->h();

   if( cW > this->w() )
   {
      this->w() = cW;
   }

   if( cH > this->h() )
   {
      this->h() = cH;
   }

   control->setLayer(inLayer);

   //_DTraceError("GActivity %d: Added control at pos #%d/%d (%s - 0x%08x), z = %d, focus idx: %d, layer: %d\n", id(), i+1, m_controlCount, control->classname(), control, control->z(), m_focusedControlIdx, control->layer());

   if( m_focusedControlIdx < 0 )
   {
      // if this is first focusable control, make it focused by default
      if( GFocusable::InstanceOfGFocusable(control) )
      {
         m_focusedControlIdx = i+1;
      }
   }

   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GActivity& GActivity::removeControl(GStaticControl* control)
{
   m_lastError = GERROR_NOT_FOUND;

   int i = findIndex(control);

   if( i >= 0 && i < m_controlCount )
   {
      m_controlCount--;

      if( m_controlCount < 0 )
      {
         m_controlCount = 0;
      }
      else if( m_controlCount >= MAX_UICONTROLS_IN_ACTIVITY )
      {
         m_controlCount = MAX_UICONTROLS_IN_ACTIVITY - 1;
      }

      for( ; i < m_controlCount; i++ )
      {
         m_pControls[i] = m_pControls[i+1];
      }

      m_pControls[m_controlCount] = NULL;
      m_lastError = GERROR_NONE;
      return *this;
   }

   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GActivity& GActivity::removeAllControl()
{
   m_controlCount = 0;
   for( int i = 0; i < MAX_UICONTROLS_IN_ACTIVITY; ++i )
   {
      m_pControls[i] = NULL;
   }
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GActivity::setDefaultFocus(GFocusable* control)
{
   m_lastError = GERROR_NOT_FOUND;

   int i = findIndex(control);

   if( i >= 0 )
   {
      m_defaultFocusedControlIdx = i;
      setFocusedControl(i);
      m_lastError = GERROR_NONE;
   }

   return m_lastError;
}

//////////////////////////////////////////////////////////////////////////////////////
GFocusable* GActivity::getFocusedControl()
{
   if( (m_focusedControlIdx <0) || (m_controlCount<=0) )
   {
      return NULL;
   }

   return dynamic_cast<GFocusable *>(m_pControls[m_focusedControlIdx]);
}

//////////////////////////////////////////////////////////////////////////////////////
// ham nay chi dc goi khi da chac chan _control[i] la GFocusable
GError_t GActivity::setFocusedControl(int i)
{
   GFocusable* focusedCtrl = getFocusedControl();

   if( focusedCtrl != NULL )
   {
      focusedCtrl->setFocus(false);
      if( !GList::InstanceOfGList(focusedCtrl) )
      {
         GNavigator::getInstance().pushUpdateRect(focusedCtrl->layer(), focusedCtrl);
      }
   }

   m_focusedControlIdx = i;
   focusedCtrl = getFocusedControl();
   focusedCtrl->setFocus(true);

   if( !GList::InstanceOfGList(focusedCtrl) )
   {
      GNavigator::getInstance().pushUpdateRect(focusedCtrl->layer(), focusedCtrl);
   }
   //   _DTraceDebug("Focused control changed to: #%d - 0x%08x\n",i, m_pControls[i]);
   m_lastError = GERROR_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GActivity::setFocusedControl(GFocusable* control)
{
   m_lastError = GERROR_NOT_FOUND;
   if( control == NULL )
   {
      return m_lastError;
   }

   int i = findIndex(control);
   if( i >= 0 )
   {
      return setFocusedControl(i);
   }

   return m_lastError;
}

//////////////////////////////////////////////////////////////////////////////////////
#define SQR(x) ((x) * (x))

//////////////////////////////////////////////////////////////////////////////////////
int GActivity::findNextFocusable(int currentControl, Direction_t direction)
{
   //_DTraceDebug("GActivity::findNextFocusable\n");
   if( currentControl < 0 || currentControl >= MAX_UICONTROLS_IN_ACTIVITY || m_pControls[currentControl] == NULL )
   {
      return -1;
   }

   int i = -1;

   if( (direction == LEFT) && (m_pControls[currentControl]->m_pLeft != NULL) )
   {
      i = findIndex(m_pControls[currentControl]->m_pLeft);
      //if (i >= 0) return i;
   }

   if( (direction == RIGHT) && (m_pControls[currentControl]->m_pRight != NULL) )
   {
      i = findIndex(m_pControls[currentControl]->m_pRight);
      //if (i >= 0) return i;
   }

   if( (direction == UP) && (m_pControls[currentControl]->m_pUp != NULL) )
   {
      i = findIndex(m_pControls[currentControl]->m_pUp);
      //if (i >= 0) return i;
   }

   if( (direction == DOWN) && (m_pControls[currentControl]->m_pDown != NULL) )
   {
      i = findIndex(m_pControls[currentControl]->m_pDown);
      //if (i >= 0) return i;
   }

   //Chau added *** begin
   // neu m_pControls[i] tim dc la 1 GList thi can kiem tra itemCount cua list day
   // neu itemCount == 0 thi tiep tuc tim
   if( i >= 0 )
   {
      if( GList::InstanceOfGList(m_pControls[i]) )
      {
         GList* list = dynamic_cast<GList*>(m_pControls[i]);
         if( list->getItemCount() > 0 )
         {
            return i;
         }
      }
      else
      {
         return i;
      }
   }
   //Chau added *** end

   //TODO edit this logic
   int iFound = -1;
   int minDistance = 0x7FFFFFFF;

   // trong so theo chieu doc va ngang
   int wV = 300;
   int wH = 300;

   // left, top, right, bottom:
   int L, T, R, B;
   L = m_pControls[currentControl]->x() - m_pControls[currentControl]->refX();
   T = m_pControls[currentControl]->y() - m_pControls[currentControl]->refY();
   R = L + m_pControls[currentControl]->w();
   B = T + m_pControls[currentControl]->h();

   if( GList::InstanceOfGList(m_pControls[currentControl]) )
   {
      // tranh truong hop list->getFocusedItem == NULL
      GList* list = dynamic_cast<GList*>(m_pControls[currentControl]);
      if( list == NULL )
      {
         _DTraceError("list == NULL");
         return -1;
      }

      if( list->getItemCount() < 1 )
      {
         _DTraceError("this is %s, this list has no item",list->name());
      }
      else
      {
         if( (direction == LEFT) || (direction == RIGHT) )
         {
            T = list->y() - list->refY()
                  + list->getFocusedItem()->y() - list->getFocusedItem()->refY()
                  + list->getYOffset();
            B = T + list->getFocusedItem()->h();
         }
         else
         {
            L = list->x() - list->refX()
                  + list->getFocusedItem()->x() - list->getFocusedItem()->refX()
                  + list->getXOffset();
            R = L + list->getFocusedItem()->w();
         }
      }
   }

   //_DTraceInfo("\n\nOrig #%d - %s: (%d, %d) (%d, %d)\n", m_pControls[currentControl]->id(), m_pControls[currentControl]->name(), L, T, R, B);
   for( i = 0; i < m_controlCount; i ++ )
   {
      if( !m_pControls[i]->isVisible() )
      {
         continue;
      }

      // neu m_pControls[i] la GList va ko co listItem nao thi cung ignore
      if( GList::InstanceOfGList(m_pControls[i]) )
      {
         GList* list = dynamic_cast<GList*>(m_pControls[i]);
         if( list->getItemCount() < 1 )
         {
            continue;
         }
      }

      if( (i != currentControl) && (GFocusable::InstanceOfGFocusable(m_pControls[i])) )
      {
         if( !((GFocusable*)(m_pControls[i]))->isEnabled() )
         {
            continue;
         }
         int curL = m_pControls[i]->x() - m_pControls[i]->refX();
         int curT = m_pControls[i]->y() - m_pControls[i]->refY();
         int curR = curL + m_pControls[i]->w();
         int curB = curT + m_pControls[i]->h();
         //_DTraceInfo("Cur #%d - %s: (%d, %d) (%d, %d)\n", m_pControls[i]->id(), m_pControls[i]->name(), curL, curT, curR, curB);
         int distance = minDistance;
         if( (direction == LEFT) && (curR <= L) )
         {
            distance = wH * SQR(L - curR) + SQR((T+B)/2 - (curT+curB)/2) + 10;
            //_DTraceInfo("LEFT, Dis: %d, MinDis: %d\n", distance, minDistance);
         }
         else if( (direction == RIGHT) && (curL >= R) )
         {
            distance = wH * SQR(R - curL) + SQR((T+B)/2 - (curT+curB)/2);
            //_DTraceInfo("RIGHT, Dis: %d, MinDis: %d\n", distance, minDistance);
         }
         else if( (direction == UP) && (curB <= T) )
         {
            distance = SQR((L+R)/2 - (curL+curR)/2) + wV * SQR(T - curB);
            //_DTraceInfo("UP, Dis: %d, MinDis: %d\n", distance, minDistance);
         }
         else if( (direction == DOWN) && (curT >= B) )
         {
            distance = SQR((L+R)/2 - (curL+curR)/2) + wV * SQR(B - curT);
            //_DTraceInfo("DOWN, Dis: %d, MinDis: %d\n", distance, minDistance);
         }

         if( distance < minDistance )
         {
            minDistance = distance;
            iFound = i;
         }
      }
   }

   //   _DTraceInfo("Found: %d, %s\n", iFound, (i>=0)?m_pControls[iFound]->name():"");
   return iFound;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GActivity::processKey(U32 key)
{
   if( !this->m_enabled )
   {
      _DTraceError("GActivity::processKey *** enabled == false\n");
      return false;
   }

   if( !this->m_isFocused )
   {
      _DTraceError("GActivity::processKey *** isFocused == false\n");
      return false;
   }

   GFocusable* focusedCtrl = getFocusedControl();
   //_DTraceError("GActivity::processKey %s\n",focusedCtrl->classname());
   //_DTraceError("Focused control is at: 0x%08x\n",focusedCtrl);

   if( focusedCtrl != NULL )
   {
      if( focusedCtrl->processKey(key) )
      {
         //_DTraceError("control (%s - %d) processed key %d\n", focusedCtrl->classname(), focusedCtrl, key);
         return true;
      }
   }
   //_DTraceError("Focused control at 0x%08x didn't process key\n",focusedCtrl);

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
      int iNextFocusable    = findNextFocusable(m_focusedControlIdx, direction);
      if( (iNextFocusable >= 0) && (m_pControls[iNextFocusable]) != NULL )
      {
         setFocusedControl(dynamic_cast<GFocusable*>(m_pControls[iNextFocusable]));
         //_DTraceError("Next control to be set focused: #%d - 0x%08x\n",iNextFocusable, m_pControls[iNextFocusable]);
         return true;
      }
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GActivity::draw(int layer)
{
   GError_t er = GERROR_NONE;

   for( int i = 0; i < m_controlCount; i++ )
   {
      if( m_pControls[i] == NULL || !m_pControls[i]->isVisible() )
      {
         //TODO: handle this _layer for rendering optimization   //int l=_layer;
         continue;
      }

      GDrawable* item = m_pControls[i];

      if( item != NULL )
      {
         er = item->draw(layer, x()-refX()+item->x(), y()-refY()+item->y());
      }
   }

   return er;
}

//////////////////////////////////////////////////////////////////////////////////////
void GActivity::open()
{
   //_DTraceError("(%s)\t\t#%d, open()", classname(), this->id());
   m_isVisible = true;
   GNavigator::getInstance().openActivity(this);
   onOpen();
   GNavigator::getInstance().redrawAll();
}

//////////////////////////////////////////////////////////////////////////////////////
void GActivity::close()
{
   _DTraceError("(%s)\t\t#%d, close()", classname(), this->id());

   GNavigator::getInstance().closeActivity(this);
   m_isVisible = false;
   m_isTopLayer = false;
   onClose();
   GNavigator::getInstance().redrawAll();
}

//////////////////////////////////////////////////////////////////////////////////////
void GActivity::confirm(bool cmdOK)
{
   _DTraceError("(%s)\t\t#%d, confirm()", classname(), this->id());
   if( m_isVisible )
   {
      onConfirm(cmdOK);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GActivity::turnOnDrawAll()
{
   m_needRedrawAll = true;
   int i;
   for( i = 0; i < m_controlCount; i++ )
   {
      if( m_pControls[i]->isVisible() && GList::InstanceOfGList(m_pControls[i]) )
      {
         GList* list = dynamic_cast<GList*>(m_pControls[i]);
         list->turnOnDrawAll();
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
