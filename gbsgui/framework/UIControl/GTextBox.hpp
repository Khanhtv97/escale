//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GTextBox.hpp                                                               //
//////////////////////////////////////////////////////////////////////////////////////

// Example:
// char* inputStr = "Truye^`n hi`nh ki~ thua^.t so^' ve^. tinh vs Truye^`n hi`nh ki~ thua^.t so^' ma(.t dda^'t";
// textBox_test = new GTextBox(_L_.VN_Convert(inputStr),300,100,25,ARIAL_18_N,0x8c8c8c);
// textBox_test->setPos(100,250);
// Other:
// GTextBox co dang giong GList theo chieu doc. Moi item la 1 text row.
// Text string truoc khi dua vao text box can split thanh cac row co width <=  GTextBox width

#ifndef GTEXTBOX_HPP_
#define GTEXTBOX_HPP_

#define   ALIGN_LEFT   0
#define   ALIGN_RIGHT  1
#define   ALIGN_CENTER 2

#include "GStaticControl.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
class GTextBox: public GStaticControl
{
protected:
   GStaticControl** m_ppItem;
   int m_maxItemCount;
   int m_itemCount;
   int m_rowCount;
   int m_vSpace; //khoang cach giua cac dong text

   //for use internally to save actual x & y of list when drawing (x, y in draw() method has been changed externally)
   int m_relX;
   int m_relY;
   int m_xOffset;
   int m_yOffset;

   int m_fullWidth;  //Chau added: su dung de update lai toan bo list
   int m_fullHeight; //Chau added: su dung de update lai toan bo list

   int m_textHeight;
   int m_width;
   int m_delta;  //he so de tinh so character per line
   int m_fontId;
   int m_color;
   bool m_isEmpty;

   void init();
   int findIndex(GStaticControl* textItem);

public:
   GTextBox(const char* text, int width, int height, int textHeight, int fontId, int color);
   ~GTextBox();

   const char* classname() {return "GTextBox";};
   int splitStr(const char* text, char lineStr[][50], int charPerLine); //todo: will be optimized
   int getItemCount() {return m_itemCount;};
   void setPos(int x, int y);
   void updateTextBox(const char* text);

   GTextBox& addTextItem(GStaticControl* textItem);
   GTextBox& addTextItemAt(GStaticControl* textItem, int index);
   GTextBox& removeTextItem(GStaticControl* textItem);
   GTextBox& removeAllTextItem();
   GTextBox& removeTextItemAt(int index);
   GTextBox& destroyAllItem();

   GStaticControl* getItem(int i);

   GTextBox& setRows(int rowCount);
   GTextBox& setItemSpace(int vSpace) {m_vSpace = vSpace;};

   void setItemPos(GStaticControl* item, int index);
   int getXOffset() {return m_xOffset;};
   int getYOffset() {return m_yOffset;};

   GError_t draw();

   void pushUpdateRect(int layer, int x, int y, int w, int h);
   void pushUpdateRect(int layer, GStaticControl* item);
   bool pushUpdateRectIfIntersect(int layer, int x, int y, int w, int h);
   bool pushUpdateRectIfIntersect(int layer, GStaticControl* item);

   void turnOnDrawAll();
   void turnOnDrawTextBox();

   static bool InstanceOfGTextBox(GStaticControl* control);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
#endif /* GTEXTBOX_HPP_ */
