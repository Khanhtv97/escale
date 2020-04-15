//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GListItemEx.cpp                                                            //
//////////////////////////////////////////////////////////////////////////////////////

#include "GListItemEx.hpp"
#include "GList.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
void GListItemEx::setSelect(bool select)
{
   if( (this->parent() != NULL) && (GList::InstanceOfGList(this->parent())) )
   {
      GList* list = NULL;
      list = dynamic_cast<GList*>(this->parent());

      if( list == NULL )
      {
         return;
      }

      for( int i = 0; i < list->getItemCount(); i++ )
      {
         GListItemEx* item = dynamic_cast<GListItemEx*>(list->getItem(i));

         if( item == NULL )
         {
            continue;
         }

         if( (item != this) && (select) && (item->isSelected()) )
         {
            item->GSelectable::setSelect(false); // goi ham setSelect cua lop me de tranh bi de quy o day
            list->pushUpdateRect(list->layer(), item);
         }
      }

      if( select )
      {
         list->pushUpdateRect(list->layer(), this);
      }
   }

   if( select )
   {
      GSelectable::setSelect(true);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
