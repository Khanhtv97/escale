//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GStaticControl.cpp - static item on OSD (1 state):                         //
//       static text, static bitmap, this is also base class for all UI Control     //
//////////////////////////////////////////////////////////////////////////////////////

#include "GStaticControl.hpp"
#include "GDebug.hpp"
#include "GNavigator.hpp"
#include "APIText.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
GStaticControl::GStaticControl() : GView(), m_pView(NULL), m_pData(NULL)
{
   m_isVisible = true;

   m_pUp = NULL;
   m_pDown = NULL;
   m_pLeft = NULL;
   m_pRight = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
GStaticControl::GStaticControl(GDrawable* drawable, int x, int y) : GView(), m_pView(NULL), m_pData(NULL)
{
   m_isVisible = true;

   m_pUp = NULL;
   m_pDown = NULL;
   m_pLeft = NULL;
   m_pRight = NULL;

   this->setView(drawable, x, y);
}

//////////////////////////////////////////////////////////////////////////////////////
GStaticControl::GStaticControl(const char* controlName) : GView(), m_pView(NULL), m_pData(NULL)
{
   m_isVisible = true;

   m_pUp = NULL;
   m_pDown = NULL;
   m_pLeft = NULL;
   m_pRight = NULL;

   this->m_name.assign(controlName);
}

//////////////////////////////////////////////////////////////////////////////////////
GStaticControl::GStaticControl(int layer) : GView(layer), m_pView(NULL), m_pData(NULL)
{
   m_isVisible = true;

   m_pUp = NULL;
   m_pDown = NULL;
   m_pLeft = NULL;
   m_pRight = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
GStaticControl::GStaticControl(int x, int y) : GView(x, y), m_pView(NULL), m_pData(NULL)
{
   m_isVisible = true;

   m_pUp = NULL;
   m_pDown = NULL;
   m_pLeft = NULL;
   m_pRight = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
GStaticControl::GStaticControl(int layer, int x, int y)
   : GView(layer, x, y), m_pView(NULL), m_pData(NULL)
{
   m_isVisible = true;

   m_pUp = NULL;
   m_pDown = NULL;
   m_pLeft = NULL;
   m_pRight = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
GStaticControl::GStaticControl(int x, int y, int ref_x, int ref_y)
   : GView(x, y, ref_x, ref_y), m_pView(NULL), m_pData(NULL)
{
   m_isVisible = true;

   m_pUp = NULL;
   m_pDown = NULL;
   m_pLeft = NULL;
   m_pRight = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
GStaticControl::GStaticControl(int layer, int x, int y, int ref_x, int ref_y)
   : GView(layer, x, y, ref_x, ref_y), m_pView(NULL), m_pData(NULL)
{
   m_isVisible = true;

   m_pUp = NULL;
   m_pDown = NULL;
   m_pLeft = NULL;
   m_pRight = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
GStaticControl::~GStaticControl()
{
   //m_pUp = m_pDown = m_pLeft = m_pRight = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
GStaticControl& GStaticControl::setView(GView* view)
{
   this->m_pView = view;
   // TODO check if using manual size, in that case, don't update w() and h() here
   if( (view != NULL) && (this->m_manualSize.w <0) )
   {
      this->w() = view->w();
      this->h() = view->h();
   }
   //GNavigator::getInstance()->pushUpdateRect(this->layer(), this);
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GStaticControl& GStaticControl::setView(GDrawable* drawable, int x, int y)
{
   GView* tmp = new GView(x, y);
   tmp->addView(drawable);

   return setView(tmp);
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GStaticControl::draw()
{
   //_DTraceError("GStaticControl::draw() \n");
   if( m_pView == NULL )
   {
      _DTraceDebug("%s #%d - NO view has been set for this control yet", classname(), id());
      return GERROR_NOT_FOUND;
   }

   //TODO: optimize this
   //int l = m_pView->layer();
   int l = this->layer();
   //if (layer > 0)
   //{
   //   l = 1;
   //}
   //else
   //{
   //   l = 0;
   //}
   //_DTraceError("%s this(%d,%d), refX(%d,%d), m_pView(%d,%d)\n",classname(),this->x(),this->y(),this->refX(),this->refY(),m_pView->x(),m_pView->y());
   int globalX = this->x() - this->refX() + m_pView->x();
   int globalY = this->y() - this->refY() + m_pView->y();

   ((GDrawable*)m_pView)->draw(l, globalX, globalY);
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GStaticControl::draw1(int nDraw1, int nClrBg)
{
   U8 a = 1;

   if( 0 < nClrBg )
   {
      GBLITfillRect( x()-refX()+m_pView->x(), y()-refY()+m_pView->y(), w(), h(), m_nARGB );
   }

   draw();
}

//////////////////////////////////////////////////////////////////////////////////////
GStaticControl& GStaticControl::updView(GDrawable* d, int x, int y, int w, int h, int nDraw1, int nClrBg)
{
   destroyViews();
   setView(d, x, y);
   setSize(w, h);
   draw1(nDraw1, nClrBg);
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GStaticControl::update()
{
   _DTraceError("GStaticControl::update() \n");
   this->draw();
}

//////////////////////////////////////////////////////////////////////////////////////
void GStaticControl::destroyViews()
{
   if( m_pView != NULL )
   {
      m_pView->destroyViews();
      delete m_pView;
      m_pView = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GStaticControl::setVisible(bool visible)
{
   if( visible != m_isVisible )
   {
      m_isVisible = visible;
      GNavigator::getInstance().pushUpdateRect(this->layer(), this);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
bool GStaticControl::isVisible()
{
   return m_isVisible;
}

//////////////////////////////////////////////////////////////////////////////////////
const char* GStaticControl::name()
{
   return m_name.c_str();
}

//////////////////////////////////////////////////////////////////////////////////////
void GStaticControl::setName(const char* s)
{
   m_name.assign(s);
}

//////////////////////////////////////////////////////////////////////////////////////
void* GStaticControl::getData()
{
   return m_pData;
}

//////////////////////////////////////////////////////////////////////////////////////
void GStaticControl::setData(void* data)
{
   m_pData = data;
   this->onSetData();
}

//////////////////////////////////////////////////////////////////////////////////////
void GStaticControl::onSetData()
{
   //To be overridden;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GStaticControl::intersect(GRect& rect)
{
   return rect.intersect(x(), y(), w(), h());
}

//////////////////////////////////////////////////////////////////////////////////////
// class GTxtCtl - 2013-05-15we_th66
//////////////////////////////////////////////////////////////////////////////////////
GTxtCtl::GTxtCtl(char* s, int x, int y, int w, int h, int xt, int yt, int nFid, U32 tcolor, S32 argb)
: GStaticControl(x, y)
{
   setSize(w, h);
   X = x;
   Y = y;
   W = w;
   H = h;
   _xt = xt;
   _yt = yt;
   _textID = -1;
   nFontId = nFid;
   m_nARGB = argb;
   nTColor = tcolor;
   setTxt(s, 0);
   setLayer(DEFAULT_DRAW_LAYER_ID);
}

//////////////////////////////////////////////////////////////////////////////////////
void GTxtCtl::setTxt(char* s, int nDraw1)
{
   memset(sTxt, 0, 128);
   memcpy(sTxt, s, strlen(s));
   //if( 0 < nDraw1 )
   //{
   //   dra1();
   //}
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GTxtCtl::draw()
{
   GError_t e = GERROR_NONE;
   int realX = x()-refX();
   int realY = y()-refY();

   //_DTraceError("GTxtCtl::draw(%s)x%d y%d w%d h%d\n",sTxt,X,Y,W,H);
   if( 0 <= _textID )
   {
      DeleteText(_textID);
      //FreeTextHandle(_textID);
   }
   _textID = getNextFreeTextId();

   //if( 0 <= _textID )
   //{
   //   er = DeleteText(_textID);
   //}
   //_textID=getNextFreeTextId();//2013-05-01we_th66

   if( _textID < 0 || TEXT_MAX_NUMBER_OF_TEXT_OBJECTS <= _textID )
   {
      _DTraceError("(%s)\t\t#%d, No more free text handle!\n",classname(),this->id());
      //_DTraceDebug("2013-05-01we_th66:GText::draw(%d.%d)e%u\n",io,_textID,er);
      //_DTraceDebug("(%s)\t\t#%d, Drawing text with ID %d\n", classname(), this->id(), _textID);
   }

   if( nARGB > 0 )
   {
      GBLITfillRect(X,Y,W,H,nARGB);
      //if(0<nDraw1)realX,realY,w(),h(),nARGB);
   }

   TextParams_t tp;
   tp.posX      = X+_xt;
   tp.posY      = Y+_yt;
   tp.color   = nTColor;
   tp.fontId   = nFontId;
   tp.planeId   = DEFAULT_DRAW_LAYER_ID;//1;//_nGfxPlaneDraw;//_layer;//_DTraceDebug("(%s)\t\t#%d, Drawing text \"%s\" with ID %d\n",classname(),this->id(),_text->c_str(),_textID);
   //_DTraceError("GTxtCtl::draw() PrintText %s \n",sTxt);
   PrintText(_textID,sTxt,tp,W,H);//else PrintAlignedText(_textID,_text->c_str(),textParams, textField_W, alignType);
   //_DTraceDebug("(%s)\t\t#%d, layer %d, z %2d, pos (%3d, %3d), ref (%3d, %3d), size (%3d, %3d), text: %s, font: %d, color: 0x%08x\n", classname(), this->id(), this->layer(), this->z(), realX, realY, this->refX(), this->refY(), this->w(), this->h(), _text->c_str(),_font->FontId(), _font->Color().Code());

   return e;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GTxtCtl::dra1()
{
   GError_t e = draw();
   //GBLITcopy();
   return e;
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
