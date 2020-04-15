//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2013 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GProgBar.hpp                                                               //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GPROGBAR_HPP_
#define GPROGBAR_HPP_

#include "GDynamicControl.hpp"
#include "GListItem.hpp"
#include "GView.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GProgBar : public GStaticControl
{
private:
   int m_progSegment;
   S32 m_width;
   S32 m_height;

   GDynamicControl* m_pProgress;
   GStaticControl* m_pBackGround;

public:
   GProgBar(int MaxSegment, S32 width, S32 height);
   GProgBar(S32 width, S32 height);
   ~GProgBar();

   const char* classname() {return "GProgBar";};
   GError_t draw();
   void setProgress(int percent);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GPROGBAR_HPP_ */
