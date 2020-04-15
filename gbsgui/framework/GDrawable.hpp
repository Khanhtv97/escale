//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GDrawable.hpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////
#ifndef GDRAWABLE_HPP_
#define GDRAWABLE_HPP_

#include "stapp_main.h"  // for Gam_Object_t
#include "resource.hpp"
#include "APIBitmap.hpp"
#include "APIText.hpp"
#include "GError.hpp"
#include "GDebug.hpp"

#define SCREEN_WIDTH       720
#define SCREEN_HEIGHT      576
#define SCREEN_WIDTH_HDMI  1920 // 1280
#define SCREEN_HEIGHT_HDMI 1080 // 720
#define MIN_SAFE_X 60
#define MIN_SAFE_Y 50
#define MAX_SAFE_X SCREEN_WIDTH  -MIN_SAFE_X
#define MAX_SAFE_Y SCREEN_HEIGHT -MIN_SAFE_Y
#define DEFAULT_COLOR      0x00000000
#define DEFAULT_FONT_ID    0
#define DEFAULT_LAYER_ID   1    //0 2013-05-02thu_th66
#define DEFAULT_DRAW_LAYER_ID 3 //trungkt add 2013-12-16
#define HDMI_DRAW_LAYER_ID 0
#define CVBS_DRAW_LAYER_ID 1
#define MAX_LAYER_ID       1
#define DEFAULT_Z_INDEX    32
#define DEFAULT_ALPHA      20 //alpha = 0 <--> do nhin thay == 100 (percent)

#define COLOR_NONE        -1
#define COLOR_FORM_BGD    0x1252525 //0x1151515
#define COLOR_FORM_BGLD   0x1303030 //0x1202020
#define COLOR_FORM_BGL    0x1404040 //0x1303030
#define COLOR_FORM_BGL2   0x1505050 //0x1404040
#define COLOR_FORM_FGL    0x1FFFFFF //0x1FFFFFF
#define COLOR_FOCUS_BG    0x1105070 //0x1004060
#define COLOR_FOCUS_BGL   0x11090d0 //0x10080c0
#define COLOR_TIMELINE_BGL 0xFFFFFFF //0xFFFFFFF
#define COLOR_FORM_PLAY   0x1707070  //0x1606060

#define COLOR_SELECTED_SUDOKU     0x11E90FF
#define COLOR_STARTED_SUDOKU      0x1CD853F
#define COLOR_DUPLICATED_SUDOKU   0x1FF4500
#define COLOR_SUDOKU_NONE         0x1FFC0CB
#define COLOR_LINE_NONE           0x1BAB9B9

class GSize;
class GView;

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
class GPosition
{
public:
   int x;
   int y;

   GPosition() : x(0), y(0) {};
   GPosition(int _x, int _y)  : x(_x), y(_y)  {};

   GPosition& operator+=(const int delta);
   GPosition& operator+(const int delta);
   GPosition& operator+=(const GSize& size);
   GPosition& operator+(const GSize& size);
   GPosition& operator-=(const int delta);
   GPosition& operator-(const int delta);
   GPosition& operator-=(const GSize& size);
   GPosition& operator-(const GSize& size);
   GSize& operator-(const GPosition& pos);
   bool operator==(const GPosition& pos);
   bool operator!=(const GPosition& pos);
};
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
class GSize
{
public:
   int w;
   int h;
   GSize() : w(0), h(0) {};
   GSize(int _w, int _h) : w(_w), h(_h) {};

   GSize& operator-();
   GSize& operator+=(const GSize& size);
   GSize& operator+(const GSize& size);
   GSize& operator+=(const int delta);
   GSize& operator+(const int delta);
   GSize& operator-=(const GSize& size);
   GSize& operator-(const GSize& size);
   GSize& operator-=(const int delta);
   GSize& operator-(const int delta);
   GSize& operator*=(const int mul);
   GSize& operator*(const int mul);
   GSize& operator/=(const int div);
   GSize& operator/(const int div);
   GPosition& operator+(const GPosition& pos);
   bool operator==(const GSize& size);
   bool operator!=(const GSize& size);
};
//////////////////////////////////////////////////////////////////////////////////////

class GRect
{
public:
   GPosition pos;
   GSize size;

   GRect();
   GRect(int w, int h);
   GRect(int x, int y, int w, int h);

   int& x() {return pos.x;};
   int& y() {return pos.y;};
   int& w() {return size.w;};
   int& h() {return size.h;};

   void reset(int _x, int _y, int _w, int _h);
   bool intersect(int _x, int _y, int _w, int _h);
   bool intersect(GRect* rect);
   bool intersect(GRect& rect);
};

//////////////////////////////////////////////////////////////////////////////////////
class GColor
{
public:
   U8 A;
   U8 R;
   U8 G;
   U8 B;

   GColor();
   GColor(U32 color);
   GColor(U8 a, U8 r, U8 g, U8 b);
   U32 Code();
   bool operator==(const GColor& color);
   bool operator!=(const GColor& color);
};

//////////////////////////////////////////////////////////////////////////////////////
class GFont
{
private:
   int _fontId;
   GColor* _color;

public:
   GFont();
   GFont(int font_id);
   GFont(int font_id, U32 Color);
   GFont(const GColor& color);
   GFont(int font_id, const GColor& color);
   GFont(const GFont& font);

   int& FontId(); // use int& de gan 2 chieu dc
   GColor& Color();
   void setFont(int font_id);
   void setColor(const GColor& color);

   GFont& operator=(const GFont& font);
   bool operator==(const GFont& font);
   bool operator!=(const GFont& font);

   ~GFont();
};

//////////////////////////////////////////////////////////////////////////////////////
class GDrawable
{
protected:
   //TODO: implement this to be used in GNavigator::pushUpdateRect;
   //GPosition* m_pAbsPos;   //absolute position on screen

   GPosition m_position;     // relative position in parent
   GPosition m_refPosition;  //reference point,relative to m_position, top-left point to render to OSD//should be(m_position->x-m_refPosition->x,m_position->y-m_refPosition->y)
   GSize m_size;
   GSize m_manualSize;
   int m_layer;
   int m_alpha;
   int m_zindex;             // m_zindex lon hon se o phia tren (ve sau)
   int m_id;
   static int idPool;        //object's m_id, unique and automatically generated//void generateId();

   GView* m_pParent;
   GError_t m_lastError;

   U32 m_nARGB;
   int m_nDraw1; //2013-04-28su_th66 GList.draw(){if(m_nDraw1)ClearRec();}

   void init();

public:
   GDrawable();
   virtual ~GDrawable();
   GDrawable(int layer);
   GDrawable(int x, int y);
   GDrawable(int layer, int x, int y);
   GDrawable(int x, int y, int ref_x, int ref_y);
   GDrawable(int layer, int x, int y, int ref_x, int ref_y);
   GDrawable(const GPosition& pos);

   virtual const char* classname()   {return "GDrawable";};

   bool instanceOf(const char* classname);
   void setParent(GView* parent);
   GView* parent();
   GError_t getLastError();

   // rearrange my order
   GError_t bringForward();
   GError_t sendBackward();
   GError_t bringToFront();
   GError_t sendToBack();

   virtual int id();
   virtual int& x();
   virtual int& y();
   virtual int& z();
   virtual int& w();
   virtual int& h();
   virtual int& refX();
   virtual int& refY();

   virtual GPosition& pos();
   virtual GPosition& refPos();

   virtual GSize& size();
   virtual GSize& manualSize();

   virtual void setX(int x);
   virtual void setY(int y);
   virtual void setW(int w);
   virtual void setH(int h);
   virtual void setZ(int z);

   virtual void setPos(int x, int y);
   virtual void setPos(const GPosition& pos);
   virtual void setSize(const GSize& size);
   virtual void setSize(int w, int h);
   virtual void setManualSize(int w, int h);
   virtual void setRefPos(const GPosition& pos);
   virtual void setRefPos(int x, int y);

   virtual int& layer();
   virtual int& alpha();
   virtual void setLayer(int layerid);

   virtual void setBgRGB(U32 argb);

   GError_t draw(int x, int y);
   GError_t draw(int layerid);
   GError_t draw(int layerid, int x, int y);

   virtual GError_t draw() = 0;//virtual void setText(const string & s)=0;//2013-05-03fri_th66
};

//////////////////////////////////////////////////////////////////////////////////////
class GText : public virtual GDrawable
{
private:
   GFont* m_pFont;
   std::string* m_pText;
   int m_textID;
   void init();

public:
   int textField_W; //default = -1
   int textField_H; //default = -1
   int alignType;   //default = -1

   GText();
   GText(int layer);
   GText(int layer, const std::string& s);
   GText(const std::string& s);
   GText(int layer, const std::string& s, int fontid, U32 color);
   GText(const std::string& s, int fontid, U32 color);
   GText(int x, int y);
   GText(int layer, int x, int y);
   GText(int layer, int x, int y, const std::string& s);
   GText(int layer, int x, int y, const std::string& s, int fontid, U32 color);
   GText(int x, int y, const std::string& s, int fontid, U32 color);
   GText(int x, int y, const std::string& s, int fontid, U32 color, int textField_w);
   GText(int x, int y, int ref_x, int ref_y);
   GText(int layer, int x, int y, int ref_x, int ref_y);
   GText(const GPosition& pos);

   const char* classname() {return "GText";};
   const std::string text() {return *m_pText;};
   const GFont& getFont() {return *m_pFont;};

   GError_t draw();
   GError_t clr(); //2013-05-02thu_th66
   void setText(const std::string& s);
   void calculateTextSize();
   void setFont(GFont& font);
   void setFont(int fontid, U32 color);

   ~GText();
};

//////////////////////////////////////////////////////////////////////////////////////
class GDrawRec : public virtual GDrawable
{
public:
   int plane, X, Y, W, H;
   U32 m_nARGB; //U8 alfa,R,G,B;
   GDrawRec(int x, int y, int w, int h, U32 argb, int l = DEFAULT_DRAW_LAYER_ID);
   GError_t draw();
};

//////////////////////////////////////////////////////////////////////////////////////
class GFillRect:public virtual GDrawable
{
public:
   int plane, X, Y, W, H;
   U32 m_nARGB; //U8 alfa,R,G,B;
   GFillRect(int x, int y, int w, int h, U32 argb, int l = DEFAULT_DRAW_LAYER_ID);
   GError_t draw();
};

//////////////////////////////////////////////////////////////////////////////////////
//2013-05-07tu_th66
void GBLITfillProcBar(int val, int min=0, int max=100, int x=200, int y=500, int w=200, int h=40,
                    U32 argb0=COLOR_FORM_BGL, U32 argb1=COLOR_FOCUS_BG, int nHorV=0, int Plane=1);
//{
//   int d0=val-min,d1=max-min,w1;if(!d1)return;
//   w1=w*d0/d1;
//   GBLITfillRect(Plane,x,y,w,h,argb0);
//   GBLITfillRect(Plane,x,y,w1,h,argb1);
//}
//////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////
class GBitmap : public virtual GDrawable
{
private:
   GAMLOAD_Object_t* m_pGamObject;
   GError_t setWH(GAMLOAD_Object* gamObj);
   GError_t setWHManual(GAMLOAD_Object* gamObj, int width, int height);
   void init();

   int m_width; // default = -1
   int m_height;// default = -1

public:
   GBitmap();
   GBitmap(int layer);
   GBitmap(int layer, GAMLOAD_Object* gamObj);
   GBitmap(GAMLOAD_Object* gamObj);
   GBitmap(GAMLOAD_Object* gamObj, int width, int height);
   GBitmap(GBitmap* bitmapToCopy);
   GBitmap(int x, int y);
   GBitmap(int layer, int x, int y, GAMLOAD_Object* gamObj);
   GBitmap(int layer, int x, int y);
   GBitmap(int x, int y, int ref_x, int ref_y);
   GBitmap(int layer, int x, int y, int ref_x, int ref_y);
   GBitmap(const GPosition& pos);

   const char* classname() {return "GBitmap";};

   void setGam(GAMLOAD_Object* gamObj);
   void setGamManual(GAMLOAD_Object* gamObj, int width, int height);
   const GAMLOAD_Object_t* getGam();

   GError_t draw();
};

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif /* GDRAWABLE_HPP_ */
