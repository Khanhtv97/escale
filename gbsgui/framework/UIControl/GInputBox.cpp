//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GInputBox.cpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#include "GInputBox.hpp"
#include "GDebug.hpp"
#include "GKey.hpp"
#include "GSystem.hpp"

//namespace GBS {
//namespace STM {
//namespace Framework {
//namespace UIControl {

//////////////////////////////////////////////////////////////////////////////////////
void GInputBox::init()
{
   m_xOffset = m_yOffset = 0;
   m_inputIndex = 0;
   m_inputMode = INPUT_MODE_NORMAL;
   m_pTextView = NULL;
   m_pNormalText = NULL;
   m_pFocusedText = NULL;
   m_lastKeyTime = 0;
   m_lastKey = -1;
   m_repeatCount = 0;
   m_maxCharCount = 20;
   m_forceProcessKey = false;
}

//////////////////////////////////////////////////////////////////////////////////////
GInputBox::GInputBox(int x, int y)
{
   init();
   this->setPos(x, y);
}

//////////////////////////////////////////////////////////////////////////////////////
void GInputBox::onFocusChanged()
{
   // to be overridden
   if( this->m_isFocused )
   {
      this->m_pTextView = this->m_pFocusedText;
   }
   else
   {
      this->m_pTextView = this->m_pNormalText;
   }

   m_inputIndex = m_text.length();
   GFocusable::onFocusChanged();
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GInputBox::draw()
{
   _DTraceError("");
   GFocusable::draw();
   //return GERROR_NONE;

   if( m_pTextView == NULL )
   {
      _DTraceError("%s #%d - GText objects for this NOT initialized", classname(), id());
      return GERROR_NOT_FOUND;
   }

   //TODO: optimize this
   //int l = m_pTextView->layer();
   int l = this->layer();

   int globalX = this->x() - this->refX() + m_pTextView->x() + m_xOffset;
   int globalY = this->y() - this->refY() + m_pTextView->y() + m_yOffset;

   if( (this->m_isFocused) || (this->m_forceProcessKey) )
   {
      std::string tmp = m_text;
      m_pTextView->setText(tmp.insert(m_inputIndex, "|"));
   }
   else
   {
      m_pTextView->setText(m_text);
   }

   //_DTraceDebug("(%s)\t\t#%d, draw m_text: %s at (%d, %d) on layer %d\n", classname(), this->id(), m_text.c_str(), globalX, globalY, l);
   ((GDrawable*)m_pTextView)->draw(l, globalX, globalY);

   return GERROR_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////
// return 0 if there is no processing
U32 GInputBox::key2Char(U32 key)
{
   //(key>='a') && (key <= 'z')
   if( (key >= 32) && (key <= 255) )
   {
      if( (m_inputMode == INPUT_MODE_CAPLOCK) || (m_inputMode == INPUT_MODE_SHIFT) )
      {
         if( (key>='a') && (key<='z') )
         {
            key -= 'a'-'A';
         }

         if( m_inputMode == INPUT_MODE_SHIFT )
         {
            setInputMode(INPUT_MODE_NORMAL);
         }
      }

      return key;
   }

   if( (key >= KEY_RC_0) && (key <= KEY_RC_9) )
   {
      if( m_inputMode == INPUT_MODE_NUMERIC )
      {
         return key - KEY_RC_0 + '0';
      }
      else
      {
         return key;
      }
   }

   return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
bool GInputBox::processKey(U32 key)
{
   //Chau added *** begin
   //trong truong hop ko can focus vao inputbox nhung van muon thuc hien processKey cho no thi set _forceProcessKey =  true;
   if( (!this->m_isFocused) && (!this->m_forceProcessKey) )
   {
      return false;
   }

   //Chau added *** end
   if( m_text.length() >= m_maxCharCount )
   {
      if( (key == KEY_RC_SUBTITLES) && (key == KEY_KB_BACKSPACE) )
      {
         return false;
      }
   }

   int durationFromLastKey = GSystem::now() - m_lastKeyTime;
   m_lastKeyTime += durationFromLastKey;

   U32 chr = key2Char(key);
   if( chr > 0 )
   {
      if( (chr >= KEY_RC_0) && (chr <= KEY_RC_9) )
      {
         // da bao gom ca dieu kien m_inputMode != INPUT_MODE_NUMERIC, do ham key2Char
         if( (chr == m_lastKey) && (durationFromLastKey<KEY_REPEAT_TIMEOUT) )
         { // TODO: change this 800ms
            m_repeatCount++;
            U32 chr2 = getCharOnRepeatKey(chr-KEY_RC_0, m_repeatCount);
            m_text.replace(m_inputIndex-1,1,1, (char)key2Char(chr2));
         }
         else
         {
            m_repeatCount = 0;
            U32 chr2 = getCharOnRepeatKey(chr-KEY_RC_0, m_repeatCount);
            std::string tmp = "";
            tmp += (char)key2Char(chr2);  // de turn on case check on CAPLOCK and SWIFT;
            m_text.insert(m_inputIndex, tmp);
            m_inputIndex ++;
            //m_lastKey = -1;
         }

         m_lastKey = chr;
      }
      else
      {
         std::string tmp = "";
         tmp += (char)chr;
         m_text.insert(m_inputIndex, tmp);
         m_inputIndex ++;
      }

      return true;
   }

   m_lastKey = -1;
   m_repeatCount = 0;

   switch( key )
   {
   case KEY_KB_BACKSPACE:
   case KEY_RC_SUBTITLES:
      if( m_inputIndex > 0 )
      {
         m_text.erase(m_inputIndex-1, 1);
         m_inputIndex--;
      }
      m_lastKey = -1;
      m_repeatCount = 0;
      return true;
      break;

   case KEY_KB_LEFT:
   case KEY_RC_LEFT:
      if( m_inputIndex > 0 )
      {
         m_inputIndex--;
         m_lastKey = -1;
         m_repeatCount = 0;
         return true;
      }
      break;

   case KEY_KB_RIGHT:
   case KEY_RC_RIGHT:
      if( m_inputIndex<m_text.length() )
      {
         m_inputIndex++;
         m_lastKey = -1;
         m_repeatCount = 0;
         return true;
      }
      break;

   case KEY_KB_CTRL_UP:
   //case KEY_RC_INFO:
      if( m_inputMode != INPUT_MODE_CAPLOCK )
      {
         setInputMode(INPUT_MODE_CAPLOCK);
      }
      else
      {
         setInputMode(INPUT_MODE_NORMAL);
      }
      m_lastKey = -1;
      m_repeatCount = 0;
      break;

   case KEY_KB_CTRL_DOWN:
      setInputMode(INPUT_MODE_SHIFT);
      break;

   case KEY_RC_INFO:
      if( m_inputMode == INPUT_MODE_NORMAL )
      {
         setInputMode(INPUT_MODE_CAPLOCK);
      }
      else if( m_inputMode == INPUT_MODE_CAPLOCK )
      {
         setInputMode(INPUT_MODE_NUMERIC);
      }
      else
      {
         setInputMode(INPUT_MODE_NORMAL);
      }
      m_lastKey = -1;
      m_repeatCount = 0;
      break;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////////////////
GInputBox& GInputBox::setFont(GFont* normalFont, GFont* focusFont)
{
   if( focusFont == NULL )
   {
      focusFont = normalFont;
   }

   if( m_pNormalText != NULL )
   {
      delete m_pNormalText;
      m_pNormalText = NULL;
   }

   if( m_pFocusedText != NULL )
   {
      delete m_pFocusedText;
      m_pFocusedText = NULL;
   }

   m_pNormalText = new GText(this->layer(), this->m_text.c_str());
   m_pFocusedText = new GText(this->layer(), this->m_text.c_str());
   m_pNormalText->setFont(*normalFont);
   m_pFocusedText->setFont(*focusFont);

   if( this->m_isFocused )
   {
      m_pTextView = m_pFocusedText;
   }
   else
   {
      m_pTextView = m_pNormalText;
   }

   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GInputBox::~GInputBox()
{
   if( m_pNormalText != NULL )
   {
      delete m_pNormalText;
      m_pNormalText = NULL;
   }

   if( m_pFocusedText != NULL )
   {
      delete m_pFocusedText;
      m_pFocusedText = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
U8 GInputBox::getCharOnRepeatKey(int key, int repetition)
{
   int i;
   std::string s = " 0.,:@?!\"1abc2def3ghi4jkl5mno6pqrs7tuv8wxyz9";
   U8 pos[11];
   pos[0] = 0;

   for( i = 0; i < 10; i++ )
   {
      char tmp[2];
      sprintf(tmp,"%d",i);
      pos[i+1] = s.find_first_of(tmp) + 1;
   }

   return s.at(pos[key] + repetition % (pos[key+1] - pos[key]));
}

//////////////////////////////////////////////////////////////////////////////////////
GInputBox& GInputBox::setText(std::string text)
{
   m_text.assign(text);
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GInputBox& GInputBox::resizeText(int length)
{
   m_text.resize(length);
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GInputBox& GInputBox::setTextOffset(int xOffset, int yOffset)
{
   this->m_xOffset = xOffset;
   this->m_yOffset = yOffset;
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GInputBox& GInputBox::setHintText(std::string text)
{
   m_hintText.assign(text);
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
GInputBox& GInputBox::setInputMode(InputMode_t inputMode)
{
   m_inputMode = inputMode;
   _DTraceInfo("(%s)\t\t#%d, input mode set to: %s\n", classname(), this->id(), m_inputMode == INPUT_MODE_NORMAL?"INPUT_MODE_NORMAL":m_inputMode == INPUT_MODE_CAPLOCK?"INPUT_MODE_CAPLOCK":m_inputMode == INPUT_MODE_NUMERIC?"INPUT_MODE_NUMERIC":m_inputMode == INPUT_MODE_SHIFT?"INPUT_MODE_SHIFT":m_inputMode == INPUT_MODE_SYMBOLS1?"INPUT_MODE_SYMBOLS1":m_inputMode == INPUT_MODE_SYMBOLS2?"INPUT_MODE_SYMBOLS2":"INPUT_MODE_UNKNOWN");
   return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
const char* GInputBox::getText()
{
   return m_text.c_str();
}

//////////////////////////////////////////////////////////////////////////////////////
//} //UIControl
//} //Framework
//} //STM
//} //GBS
