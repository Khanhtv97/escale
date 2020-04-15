//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GListItemBase.hpp                                                          //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GLISTITEMBASE_HPP_
#define GLISTITEMBASE_HPP_

//#include "GList.hpp"
#include "GFocusable.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

class GList;

//////////////////////////////////////////////////////////////////////////////////////
class GListItemBase : public virtual GFocusable
{
protected:
   GList* m_pParent;

public:
   GListItemBase();
   GList* parent();
   void setParent(GList* parent);
   virtual ~GListItemBase(){}
   virtual void destroyViews()
   {
      GFocusable::destroyViews();
   };
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GLISTITEMBASE_HPP_ */
