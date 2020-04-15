//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GSelectable.hpp                                                            //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GSELECTABLE_HPP_
#define GSELECTABLE_HPP_

#include "GFocusable.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GSelectable : public virtual GFocusable
{
private:
   using GFocusable::setViews;

protected:
   GView* m_pSelectedView;
   GView* m_pFocusedSelectedView;
   IExecutable* m_pOnSelectChanged;

   bool m_isSelected;
   //default = _id
   int m_groupID;

   void updateView();
   void onFocusChanged();
   void init();
   virtual void onSelectChanged();

public:
   GSelectable();
   GSelectable(int layer);
   GSelectable(int x, int y);
   GSelectable(int layer, int x, int y);
   GSelectable(int x, int y, int ref_x, int ref_y);
   GSelectable(int layer, int x, int y, int ref_x, int ref_y);
   ~GSelectable();

   const char* classname()   {return "GSelectable";};

   GSelectable& setViews(GView* normalView, GView* focusedView, GView* selectedView, GView* focusedSelectedView);

   void destroyViews();
   void setOnSelect(IExecutable* pdelegate);
   void setGroupID(int id);
   int groupID();

   virtual bool isSelected();
   virtual void setSelect(bool select);
   static bool InstanceOfGSelectable(GStaticControl* control);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GSELECTABLE_HPP_ */
