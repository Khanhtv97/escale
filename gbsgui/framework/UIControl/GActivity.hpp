//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GActivity.hpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GACTIVITY_HPP_
#define GACTIVITY_HPP_

#include "GObject.hpp"
#include "GFocusable.hpp"
#include "GButtonEx.hpp"

#define GForm GActivity

#define MAX_UICONTROLS_IN_ACTIVITY 128

typedef enum Direction_e
{
   NONE,
   UP,
   DOWN,
   LEFT,
   RIGHT
} Direction_t;

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GActivity : public GFocusable
{
private:
   void initActivity();

   friend void GButtonEx::setSelect(bool selected);
   friend void GFocusable::setFocus(bool selected);

protected:
   GStaticControl* m_pControls[MAX_UICONTROLS_IN_ACTIVITY];

   int m_controlCount;
   int m_focusedControlIdx;
   int m_defaultFocusedControlIdx;
   bool m_needRedrawAll;

   int findIndex(GStaticControl* control);
   int findNextFocusable(int currentControl, Direction_t direction);
   GError_t setFocusedControl(int i); // ham nay chi dc goi khi da chac chan _control[i] la GFocusable

public:
   bool m_enabled;    //true, neu == false thi form nay ko focus duoc, ko processKeyz
   bool m_isTopLayer; //default is false. If true, the form always displays on the top layer

   GActivity();
   GActivity(int layer);
   GActivity(int x, int y);
   GActivity(int layer, int x, int y);
   const char* classname()   {return "GActivity";}

   GActivity& addControl(GStaticControl* control, int inLayer = DEFAULT_DRAW_LAYER_ID);
   GActivity& removeControl(GStaticControl* control);
   GActivity& removeAllControl();
   GError_t setDefaultFocus(GFocusable* control);
   GFocusable* getFocusedControl();
   GError_t setFocusedControl(GFocusable* control);

   int forceControlRedraw(GStaticControl* control);

   bool processKey(U32 key);

   ~GActivity();

   static bool InstanceOfGActivity(GView* view);

   GError_t draw(int layer = 1);

   void open();
   void close();
   void confirm(bool cmdOK = false);

   //override this to perform action after opened
   virtual void onOpen(){};
   //override this to perform action before closed
   virtual void onClose(){};
   //override this to perform action after confirmed
   virtual void onConfirm(bool cmdOK){};

   static GActivity* getOpenedAvtivityByName(const char* name);

   void turnOnDrawAll();
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif // GACTIVITY_HPP_
