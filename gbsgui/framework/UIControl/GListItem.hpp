//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GListItem.hpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GLISTITEM_HPP_
#define GLISTITEM_HPP_

#include "GListItemBase.hpp"
#include "GButton.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GListItem : public GButton, public GListItemBase
{
public:
   const char* classname()   {return "GListItem";};
   void destroyViews()
   {
      GListItemBase::destroyViews();
   };
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GLISTITEM_HPP_ */
