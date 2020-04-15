//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GListItemEx.hpp                                                            //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GLISTITEMEX_HPP_
#define GLISTITEMEX_HPP_


#include "GListItemBase.hpp"
#include "GButtonEx.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GListItemEx : public GButtonEx, public GListItemBase
{
public:
   const char* classname() {return "GListItemEx";};
   virtual void setSelect(bool select);
   void destroyViews()
   {
      GButtonEx::destroyViews();
   };
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS

#endif /* GLISTITEMEX_HPP_ */
