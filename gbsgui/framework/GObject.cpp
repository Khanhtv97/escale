//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GObject.cpp                                                                //
//////////////////////////////////////////////////////////////////////////////////////

#include "GObject.hpp"
#include "GDelegate.hpp"
#include "GEvents.hpp"
#include "APIPower.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
// class GObject
//////////////////////////////////////////////////////////////////////////////////////
GObject::GObject()
{
}

//////////////////////////////////////////////////////////////////////////////////////
void GObject::dispatchEvent(U32 event, void* event_data)
{
   GDispatcher::getInstance()->dispatchEvent(this, event, event_data);
}

//////////////////////////////////////////////////////////////////////////////////////
void GObject::addEventListener(U32 event, Event_Handler_t fn)
{
   //_D_.Debug("GObject::addEventListener(%d,%u)\n",event, fn);
   GDispatcher::getInstance()->addEventListener(event, fn);
}

//////////////////////////////////////////////////////////////////////////////////////
void GObject::addEventListener(U32 event, IExecutable* pDelegate)
{
   //_D_.Debug("GObject::addEventListener(%d,%u)\n",event, fn);
   GDispatcher::getInstance()->addEventListener(event, pDelegate);
}

//////////////////////////////////////////////////////////////////////////////////////
void GObject::removeEventListener(U32 event, Event_Handler_t fn)
{
   GDispatcher::getInstance()->removeEventListener(event, fn);
}

//////////////////////////////////////////////////////////////////////////////////////
void GObject::removeEventListener(U32 event, IExecutable*& pDelegate)
{
   GDispatcher::getInstance()->removeEventListener(event, pDelegate);
}

//////////////////////////////////////////////////////////////////////////////////////
void GObject::removeAllEventListeners(U32 event)
{
   GDispatcher::getInstance()->removeAllEventListeners(event);
}

//////////////////////////////////////////////////////////////////////////////////////
void GObject::setPauseDispatchEvent(BOOL isPause)
{
#ifdef ASYNC_DISPATCH_EVENT
   GDispatcher::getInstance()->setPauseDispatchEvent(isPause);
#endif
}
//////////////////////////////////////////////////////////////////////////////////////
// end class GObject
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// class GDispatcher
//////////////////////////////////////////////////////////////////////////////////////
GDispatcher* GDispatcher::m_pInstance = NULL;

GDispatcher::GDispatcher() : m_pHead(NULL)
{
   m_eventPollingTaskId = (Task_t)NULL;

#ifdef ASYNC_DISPATCH_EVENT
   m_pFront = NULL;
   m_pRear = NULL;
   m_isPauseDispatchEvent = FALSE;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////
// Add listener to list, return its index
// if it was already in list, will not add
int GDispatcher::addNode(U32 event, IExecutable* pDelegate, Event_Handler_t fn)
{
   //_D_.Debug("GDispatcher::addNode(%d,%u,%u)\n",event, pDelegate, fn);

   if( (pDelegate == NULL) && (fn == NULL) )
   {
      return -1;
   }

   if( findListener(m_pHead, event, pDelegate, fn) != NULL )
   {
      //_D_.Debug("Handle exists, will NOT be added\n");
      return -1; // already exists
   }

   //_D_.Debug("Handle not exists, will be added\n");
   EventHandlerNode* newNode = new EventHandlerNode(event, pDelegate, fn);

   if( m_pHead == NULL )
   {
      m_pHead = newNode;
      //_D_.Debug("Fisrt node added (at index: %d)\n", newNode->m_id);
      return 0;
   }

   EventHandlerNode* tmp = m_pHead;
   while( tmp->getNext() != NULL )
   {
      tmp = tmp->getNext();
   }

   newNode->m_id = tmp->m_id + 1;

   tmp->setNext(newNode);
   //_D_.Debug("New node added at index: %d\n", newNode->m_id);

   return newNode->m_id;
}

//////////////////////////////////////////////////////////////////////////////////////
// xoa handler trong list
// neu fn == NULL, remove all listeners with "event", return number of listeners deleted
// neu fn != NULL, remove the specific listener, return its index
int GDispatcher::removeNode(U32 event, IExecutable*& pDelegate, Event_Handler_t fn)
{
   if( m_pHead == NULL )
   {
      //_D_.Debug("List empty\n");
      return -1;
   }

   EventHandlerNode* tmp = m_pHead;
   EventHandlerNode* btmp = NULL;
   int i = -1, count = 0;

   while( tmp != NULL )
   {
      if( (((pDelegate != NULL) || (fn != NULL)) && tmp->equal(event, pDelegate, fn)) ||
          ((pDelegate == NULL) && (fn == NULL) && (tmp->getEvent() == event)) )
      {
         if( btmp == NULL )
         { // tmp = m_pHead
            i = tmp->m_id;
            m_pHead = tmp->getNext();
            delete tmp;
            tmp = m_pHead;
         }
         else
         {
            //_D_.Debug("tmp node: %u (%d,%u), btmp: %u (%d, %u), btmp->next: %u\n",tmp,tmp->getEvent(), tmp->getHandler(),btmp, btmp->getEvent(), btmp->getHandler(), btmp->getNext());
            btmp->setNext(tmp->getNext());
            //_D_.Debug("bmp next: %u \n",btmp->getNext());
            i = tmp->m_id;
            delete tmp;
            tmp = btmp->getNext();
         }

         count ++;
         //_D_.Debug("Removed node %d: (event = %d, handler = %u)\n",i,event, fn);
         if( (pDelegate != NULL) || (fn != NULL) )
         {
            // pDelegate was deleted in the EventHandlerNode's destructor
            // just set to NULL here
            pDelegate = NULL;
            return i;
         }
      }
      else
      {
         btmp = tmp;
         tmp = tmp->getNext();
      }
   }

   return count;
}

//////////////////////////////////////////////////////////////////////////////////////
// tim listener trong list, tra la node tim dc, NULL if not found
EventHandlerNode* GDispatcher::findListener(EventHandlerNode* pFromNode, U32 event, IExecutable* pDelegate, Event_Handler_t fn)
{
   EventHandlerNode* tmp = pFromNode;
   while( tmp != NULL )
   {
      if( (((pDelegate != NULL) || (fn != NULL)) && tmp->equal(event, pDelegate, fn)) ||
          ((pDelegate == NULL) && (fn == NULL) && (tmp->getEvent() == event)) )
      {
         return tmp;
      }
      tmp = tmp->getNext();
   }

   return NULL;
}


#ifdef ASYNC_DISPATCH_EVENT

//////////////////////////////////////////////////////////////////////////////////////
void GDispatcher::queueEvent(EventNode* event)
{
   event->setNext(NULL);

   if( m_pFront == NULL )
   {
      m_pFront = event;
   }
   else
   {
      m_pRear->setNext(event);
   }
   m_pRear = event;
}

//////////////////////////////////////////////////////////////////////////////////////
EventNode* GDispatcher::dequeueEvent()
{
   if( m_pFront == NULL )
   {
      return NULL;
   }
   else
   {
      EventNode* event = m_pFront;
      m_pFront = m_pFront->getNext();
      return event;
   }
}

#endif

//////////////////////////////////////////////////////////////////////////////////////
void GDispatcher::dispatchEvent(GObject* sender, U32 event, void* event_data)
{

#ifdef ASYNC_DISPATCH_EVENT

   //add option to choose async dispatching mode'
   //_DTraceError("queue event: %d, sender: 0x%x, data: 0x%x\n", event, sender, event_data);
   EventNode* eventNode = new EventNode(sender, event, event_data);
   //_DTraceError("queue event: %d, sender: 0x%x, data:0x%x\n", eventNode->getEvent(), eventNode->sender(), eventNode->data());
   queueEvent(eventNode);

#else

   EventHandlerNode* tmp = m_pHead;
   while( tmp != NULL )
   {
      tmp = findListener(tmp, event, NULL, NULL);
      if( tmp == NULL )
      {
         return;
      }

      // call listener
      IExecutable* pdelegate = tmp->getHandler();

      if( pdelegate != NULL )
      {
         if( CPower::getInstance().getState() != GPS_STANDBY_SYS )
         {
            pdelegate->execute(event, sender, event_data);
         }
      }
      else
      {
         Event_Handler_t fn = tmp->getGlobalHandler();
         (*fn)(event, sender, event_data);
      }

      tmp = tmp->getNext();
   }

#endif

}

#ifdef ASYNC_DISPATCH_EVENT

//////////////////////////////////////////////////////////////////////////////////////
void GDispatcher::evenPollingTask(void* Parameter)
{
   while( 1 )
   {
      EventNode* nextEvent = GDispatcher::getInstance()->dequeueEvent();
      if(nextEvent == NULL)
      {
         //_DTraceError("event polling task: no pending event\n");
      }
      else
      {
         //_DTraceError("event polling task, polled event: %d, sender: 0x%x, data: 0x%x\n", nextEvent->getEvent(), nextEvent->sender(), nextEvent->data());
         EventHandlerNode* tmp = GDispatcher::getInstance()->m_pHead;
         while( tmp != NULL)
         {
            tmp = GDispatcher::getInstance()->findListener(tmp, nextEvent->getEvent(), NULL, NULL);
            if( tmp != NULL )
            {
               // call listener
               IExecutable* pdelegate = tmp->getHandler();

               if( pdelegate != NULL )
               {
                  if( CPower::getInstance().getState() != GPS_STANDBY_SYS )
                  {
                     pdelegate->execute(nextEvent->getEvent(), nextEvent->sender(), nextEvent->data());
                  }
               }
               else
               {
                  Event_Handler_t fn = tmp->getGlobalHandler();
                  (*fn)(nextEvent->getEvent(), nextEvent->sender(), nextEvent->data());
               }

               tmp = tmp->getNext();
            }
         }
      }

      Task_Delay(100);
   }
}

#endif

//////////////////////////////////////////////////////////////////////////////////////
void GDispatcher::dumpListeners()
{
   return; //why return here
   _DTraceError("Listener list: m_pHead = %u\n", m_pHead);
   EventHandlerNode* tmp = m_pHead;

   while( tmp != NULL )
   {
      _DTraceError("Listener %d (%u): event = %u, handler = %u, globalhandler = %u\n", tmp->m_id, tmp, tmp->getEvent(), tmp->getHandler(), tmp->getGlobalHandler());
      tmp = tmp->getNext();
   }
   _DTraceError("\n");
}

//////////////////////////////////////////////////////////////////////////////////////
void GDispatcher::addEventListener(U32 event, Event_Handler_t fn)
{
   addNode(event, NULL, fn);
   dumpListeners();
}

//////////////////////////////////////////////////////////////////////////////////////
void GDispatcher::addEventListener(U32 event, IExecutable* pDelegate)
{
   addNode(event, pDelegate, NULL);
   //AddNode(event, pDelegate, NULL);
   dumpListeners();
}

//////////////////////////////////////////////////////////////////////////////////////
void GDispatcher::removeEventListener(U32 event, Event_Handler_t fn)
{
   IExecutable* pDelegate = NULL;
   removeNode(event, pDelegate, fn);
   dumpListeners();
}

//////////////////////////////////////////////////////////////////////////////////////
void GDispatcher::removeEventListener(U32 event, IExecutable*& pDelegate)
{
   removeNode(event, pDelegate, NULL);
   dumpListeners();
}

//////////////////////////////////////////////////////////////////////////////////////
void GDispatcher::removeAllEventListeners(U32 event)
{
   IExecutable* pDelegate = NULL;
   removeNode(event, pDelegate, NULL);
   dumpListeners();
}

//////////////////////////////////////////////////////////////////////////////////////
GDispatcher* GDispatcher::getInstance()
{
   if( m_pInstance == NULL )
   {
      m_pInstance = new GDispatcher();
   }

#ifdef ASYNC_DISPATCH_EVENT

   if( m_eventPollingTaskId != (Task_t)NULL )
   {
      task_kill( m_eventPollingTaskId, 0, 0 );
      Task_Delete( m_eventPollingTaskId );
      m_eventPollingTaskId = (Task_t)NULL;
   }

   if( m_eventPollingTaskId == (Task_t)NULL )
   {
      U32 ParamId = 0;
      m_eventPollingTaskId = Task_Create(GDispatcher::evenPollingTask, (void*)ParamId, 8192, MIN_USER_PRIORITY, "WatchDogTask", 0, m_eventPollingTaskId);
      if( m_eventPollingTaskId == (Task_t)NULL )
      {
         _DTraceError("Cannot create eventPolling Task\n");
      }
      else
      {
         _DTraceError("eventPolling Task created\n");
      }
   }

#endif

   return m_pInstance;
}

//////////////////////////////////////////////////////////////////////////////////////
GDispatcher::~GDispatcher()
{
   //TODO: remove all nodes in *m_pHead
   if( m_eventPollingTaskId != (Task_t)NULL )
   {
      task_kill( m_eventPollingTaskId, 0, 0 );
      Task_Delete( m_eventPollingTaskId );
      m_eventPollingTaskId = (Task_t)NULL;
   }
}
//////////////////////////////////////////////////////////////////////////////////////
// end class GDispatcher
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
// class EventHandlerNode
//////////////////////////////////////////////////////////////////////////////////////
EventHandlerNode::EventHandlerNode()
{
   m_event = GEVENT_NONE;
   m_pHandler = NULL;
   m_pNextHandler = NULL;
   m_id = 0;
}

//////////////////////////////////////////////////////////////////////////////////////
EventHandlerNode::EventHandlerNode(U32 evt, IExecutable* pdelegate, Event_Handler_t fn)
{
   m_event = evt;
   m_pHandler = pdelegate;
   m_globalHandler = fn;
   m_pNextHandler = NULL;
   m_id = 0;
}

//////////////////////////////////////////////////////////////////////////////////////
EventHandlerNode::~EventHandlerNode()
{
   m_event = GEVENT_NONE;
   m_pNextHandler = NULL;
   m_globalHandler = NULL;
   m_id = 0;
   if( m_pHandler != NULL )
   {
      _DTraceError("Release m_pHandler %d", m_pHandler);
      delete m_pHandler;
      m_pHandler = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
EventHandlerNode* EventHandlerNode::getNext()
{
   return m_pNextHandler;
}

//////////////////////////////////////////////////////////////////////////////////////
void EventHandlerNode::setNext(EventHandlerNode* node)
{
   m_pNextHandler = node;
}

//////////////////////////////////////////////////////////////////////////////////////
U32 EventHandlerNode::getEvent()
{
   return m_event;
}

//////////////////////////////////////////////////////////////////////////////////////
IExecutable* EventHandlerNode::getHandler()
{
   return m_pHandler;
}

//////////////////////////////////////////////////////////////////////////////////////
Event_Handler_t EventHandlerNode::getGlobalHandler()
{
   return m_globalHandler;
}

//////////////////////////////////////////////////////////////////////////////////////
int EventHandlerNode::equal(U32 evt, IExecutable* pDelegate, Event_Handler_t fn)
{
   return ((evt == m_event) && (pDelegate == m_pHandler) && (fn == m_globalHandler));
}

//////////////////////////////////////////////////////////////////////////////////////
void EventHandlerNode::setEvent(U32 event)
{
   m_event = event;
}

//////////////////////////////////////////////////////////////////////////////////////
void EventHandlerNode::setExecutable(IExecutable* handler)
{
   m_pHandler = handler;
}

//////////////////////////////////////////////////////////////////////////////////////
void EventHandlerNode::setGlobalHandle(Event_Handler_t gHandler)
{
   m_globalHandler = gHandler;
}
//////////////////////////////////////////////////////////////////////////////////////
// end class EventHandlerNode
//////////////////////////////////////////////////////////////////////////////////////

#ifdef ASYNC_DISPATCH_EVENT

//////////////////////////////////////////////////////////////////////////////////////
// class EventNode
//////////////////////////////////////////////////////////////////////////////////////
EventNode::EventNode(GObject* sender, U32 evt, void* data)
{
   m_event = evt;
   _sender = sender;
   _event_data = data;
   m_pNextHandler = NULL;

   if( isKeyEvent() )
   {
      _keyCode = *((U32*)_event_data);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
EventNode* EventNode::getNext()
{
   return m_pNextHandler;
}

//////////////////////////////////////////////////////////////////////////////////////
void EventNode::setNext(EventNode* node)
{
   m_pNextHandler = node;
}

//////////////////////////////////////////////////////////////////////////////////////
U32 EventNode::getEvent()
{
   return m_event;
}

//////////////////////////////////////////////////////////////////////////////////////
void* EventNode::data()
{
   if( isKeyEvent() )
   {
      return (void*) &_keyCode;
   }
   else
   {
      return _event_data;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GObject* EventNode::sender()
{
   return _sender;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL EventNode::isKeyEvent()
{
   return (m_event == GEVENT_KEY_PRESSED || m_event == GEVENT_KEY_PRESSED_2) ? TRUE : FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////
U32 EventNode::keyCode()
{
   return _keyCode;
}

//////////////////////////////////////////////////////////////////////////////////////
EventNode::~EventNode()
{
}

//////////////////////////////////////////////////////////////////////////////////////
// end class EventNode
//////////////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
