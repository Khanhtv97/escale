//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GDelegate.hpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GDELEGATE_HPP_
#define GDELEGATE_HPP_

#include "GObject.hpp"
#include "stapp_main.h"

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
//Base Interface
class IExecutable
{
public:
   virtual ~IExecutable(){}
   virtual void execute(EVENT_HANDLER_PARAM) = 0;
};

// IMPORTANT: G++ requires requires template implementation in the same file as header
// therefore implementation is not in .cpp file, but right here

template <class CLASS> class GDelegate;

#define DELEGATE(CLASS, OBJ, METHOD) new GDelegate<CLASS>(OBJ, &CLASS::METHOD)
#define DELEGATE_D(CLASS, OBJ, METHOD, DATA) new GDelegate<CLASS>(OBJ, &CLASS::METHOD,DATA)

//GDelegate class
template <class CLASS>
class GDelegate : public IExecutable
{
private:
   void* m_pData;
   CLASS* m_pContainerObject;

   void(CLASS::*methodToExecute)(EVENT_HANDLER_PARAM); //(U32 event, GObject * sender, void * event_data)

public:
   GDelegate(CLASS* obj, void (CLASS::*method)(EVENT_HANDLER_PARAM), void* data = NULL)
   {
      this->m_pContainerObject = obj;
      this->methodToExecute = method;
      this->m_pData = data;
   }

   static GDelegate* create(CLASS* obj, void (CLASS::*method)(EVENT_HANDLER_PARAM), void* data = NULL)
   {
      return new GDelegate<CLASS>(obj, method, data);
   }

   ~GDelegate() {};

   void execute(EVENT_HANDLER_PARAM)
   {
      if( event_data == NULL )
      {
         event_data = this->m_pData;
      }

      (m_pContainerObject->*methodToExecute)(event, sender, event_data);
   }
};

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif /* GDELEGATE_HPP_ */
