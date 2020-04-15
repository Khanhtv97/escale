//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GObject.hpp                                                                //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GOBJECT_HPP_
#define GOBJECT_HPP_

#include "stapp_main.h"
#include <list>
#include "GDebug.hpp"

//#define ASYNC_DISPATCH_EVENT

class IExecutable;
class GObject;

#define EVENT_HANDLER_PARAM U32 event, const GObject* sender, const void* event_data

typedef void(*Event_Handler_t)(EVENT_HANDLER_PARAM);

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
class EventHandlerNode
{
private:
   U32 m_event;
   IExecutable* m_pHandler;
   Event_Handler_t m_globalHandler;
   EventHandlerNode* m_pNextHandler;

public:
   int m_id; // for index

   EventHandlerNode();
   EventHandlerNode(U32 evt, IExecutable* pdelegate, Event_Handler_t fn);
   ~EventHandlerNode();

   U32 getEvent();
   IExecutable* getHandler();
   Event_Handler_t getGlobalHandler();
   EventHandlerNode* getNext();

   void setEvent(U32 event);
   void setExecutable(IExecutable* handler);
   void setGlobalHandle(Event_Handler_t gHandler);
   void setNext(EventHandlerNode* node);

   int equal(U32 evt, IExecutable* pDelegate, Event_Handler_t fn);
};
// end class EventHandlerNode

#ifdef ASYNC_DISPATCH_EVENT

//////////////////////////////////////////////////////////////////////////////////////
//class EventNode
class EventNode
{
private:
   U32 m_event;
   void* m_pEventData;
   GObject* m_pSender;
   EventNode* m_pNextHandler;
   U32 m_keyCode;

public:
   EventNode(GObject* sender, U32 evt, void* data);
   ~EventNode();

   EventNode* getNext();
   void setNext(EventNode* node);
   U32 event();

   void* data();
   GObject* sender();
   BOOL isKeyEvent();
   U32 keyCode();
};
// end class EventNode

#endif

//////////////////////////////////////////////////////////////////////////////////////
class GObject
{
protected:
   void dispatchEvent(U32 event, void* event_data);

public:
   GObject();

   //void testFn(EVENT_HANDLER_PARAM) {};
   virtual void removeAllEventListeners(U32 event);
   virtual void addEventListener(U32 event, IExecutable* pDelegate);
   virtual void removeEventListener(U32 event, IExecutable*& pDelegate);
   virtual void addEventListener(U32 event, Event_Handler_t fn);
   virtual void removeEventListener(U32 event, Event_Handler_t fn);
   void setPauseDispatchEvent(BOOL isPause);
};
// end class GObject

//////////////////////////////////////////////////////////////////////////////////////
class GDispatcher
{
private:
   EventHandlerNode* findListener(EventHandlerNode* pFromNode, U32 event, IExecutable* pDelegate, Event_Handler_t);
   int addNode(U32 event, IExecutable* pDelegate, Event_Handler_t fn);
   int removeNode(U32 event, IExecutable*& pDelegate, Event_Handler_t fn); // neu pDelegate == fn == NULL, remove all listener with "event"

   static GDispatcher* m_pInstance;
   EventHandlerNode* m_pHead; // list of all listceners
   BOOL m_isPauseDispatchEvent;
   Task_t m_eventPollingTaskId;

#ifdef ASYNC_DISPATCH_EVENT

   // Thuc add option to use async dispatch event mode
   // event queue
   EventNode* m_pFront;
   EventNode* m_pRear;

   void queueEvent(EventNode* event);
   EventNode* dequeueEvent();

#endif

   GDispatcher();
   ~GDispatcher();

public:
   void dumpListeners();
   void dispatchEvent(GObject* sender, U32 event, void* event_data);
   void addEventListener(U32 event, IExecutable* pDelegate);
   void removeEventListener(U32 event, IExecutable*& pDelegate);
   void addEventListener(U32 event, Event_Handler_t fn);
   void removeEventListener(U32 event, Event_Handler_t fn);
   void removeAllEventListeners(U32 event);

#ifdef ASYNC_DISPATCH_EVENT

   static void evenPollingTask(void* Parameter);
   void setPauseDispatchEvent(BOOL isPause) { m_isPauseDispatchEvent = isPause;}

#endif

   static GDispatcher* getInstance();
};
//end class GDispatcher

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif // GOBJECT_HPP_
