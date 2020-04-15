//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: resource.hpp                                                               //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_HPP_
#define RESOURCES_HPP_

#include "APIDrawParams.hpp"
#include "images_extern.hpp"

//#include "../extra/gam2h/gamload-sh4.c"

typedef enum FontID_e
{
   VERA_SANS_14_N,
   ARIAL_18_N,
   ARIAL_20_N,
   ARIAL_20_H,
   ARIAL_22_N,
   ARIAL_22_H,
   //FONT_ROBOTO_22_N,
   //FONT_ROBOTO_22_H,
   //FONT_ROBOTO_24_H,
   //FONT_ROBOTO_24_N,

   // always puts this at the end of this list
   MAX_FONT_ID
}
FontID_t;

extern int FONT_OFFSETY[MAX_FONT_ID];

typedef enum Color_Const_e
{
   // mau den 000000 ko ve dc vi no la gia tri mac dinh de lay may cuoi cung
   COLOR_Black         = 0x000001,
   COLOR_Navy          = 0x000080,
   COLOR_DarkBlue      = 0x00008B,
   COLOR_MediumBlue    = 0x0000CD,
   COLOR_Blue          = 0x0000FF,
   COLOR_DarkGreen     = 0x006400,
   COLOR_Green         = 0x008000,
   COLOR_Teal          = 0x008080,
   COLOR_DarkCyan      = 0x008B8B,
   COLOR_DeepSkyBlue   = 0x00BFFF,
   COLOR_DarkTurquoise = 0x00CED1,
   COLOR_MediumSpringGreen = 0x00FA9A,
   COLOR_Lime          = 0x00FF00,
   COLOR_SpringGreen   = 0x00FF7F,
   COLOR_Aqua          = 0x00FFFF,
   COLOR_Cyan          = 0x00FFFF,
   COLOR_MidnightBlue  = 0x191970,
   COLOR_DodgerBlue    = 0x1E90FF,
   COLOR_LightSeaGreen = 0x20B2AA,
   COLOR_ForestGreen   = 0x228B22,
   COLOR_SeaGreen      = 0x2E8B57,
   COLOR_DarkSlateGray = 0x2F4F4F,
   COLOR_DarkSlateGrey = 0x2F4F4F,
   COLOR_LimeGreen     = 0x32CD32,
   COLOR_MediumSeaGreen = 0x3CB371,
   COLOR_Turquoise     = 0x40E0D0,
   COLOR_RoyalBlue     = 0x4169E1,
   COLOR_SteelBlue     = 0x4682B4,
   COLOR_DarkSlateBlue = 0x483D8B,
   COLOR_MediumTurquoise = 0x48D1CC,
   COLOR_Indigo        = 0x4B0082,
   COLOR_DarkOliveGreen  = 0x556B2F,
   COLOR_CadetBlue     = 0x5F9EA0,
   COLOR_CornflowerBlue  = 0x6495ED,
   COLOR_MediumAquaMarine = 0x66CDAA,
   COLOR_DimGray       = 0x696969,
   COLOR_DimGrey       = 0x696969,
   COLOR_SlateBlue     = 0x6A5ACD,
   COLOR_OliveDrab     = 0x6B8E23,
   COLOR_SlateGray     = 0x708090,
   COLOR_SlateGrey     = 0x708090,
   COLOR_LightSlateGray = 0x778899,
   COLOR_LightSlateGrey = 0x778899,
   COLOR_MediumSlateBlue = 0x7B68EE,
   COLOR_LawnGreen     = 0x7CFC00,
   COLOR_Chartreuse    = 0x7FFF00,
   COLOR_Aquamarine    = 0x7FFFD4,
   COLOR_Maroon        = 0x800000,
   COLOR_Purple        = 0x800080,
   COLOR_Olive         = 0x808000,
   COLOR_Gray          = 0x808080,
   COLOR_Grey          = 0x808080,
   COLOR_SkyBlue       = 0x87CEEB,
   COLOR_LightSkyBlue  = 0x87CEFA,
   COLOR_BlueViolet    = 0x8A2BE2,
   COLOR_DarkRed       = 0x8B0000,
   COLOR_DarkMagenta   = 0x8B008B,
   COLOR_SaddleBrown   = 0x8B4513,
   COLOR_DarkSeaGreen  = 0x8FBC8F,
   COLOR_LightGreen    = 0x90EE90,
   COLOR_MediumPurple  = 0x9370D8,
   COLOR_DarkViolet    = 0x9400D3,
   COLOR_PaleGreen     = 0x98FB98,
   COLOR_DarkOrchid    = 0x9932CC,
   COLOR_YellowGreen   = 0x9ACD32,
   COLOR_Sienna        = 0xA0522D,
   COLOR_Brown         = 0xA52A2A,
   COLOR_DarkGray      = 0xA9A9A9,
   COLOR_DarkGrey      = 0xA9A9A9,
   COLOR_LightBlue     = 0xADD8E6,
   COLOR_GreenYellow   = 0xADFF2F,
   COLOR_PaleTurquoise = 0xAFEEEE,
   COLOR_LightSteelBlue = 0xB0C4DE,
   COLOR_PowderBlue    = 0xB0E0E6,
   COLOR_FireBrick     = 0xB22222,
   COLOR_DarkGoldenRod = 0xB8860B,
   COLOR_MediumOrchid  = 0xBA55D3,
   COLOR_RosyBrown     = 0xBC8F8F,
   COLOR_DarkKhaki     = 0xBDB76B,
   COLOR_Silver        = 0xC0C0C0,
   COLOR_MediumVioletRed = 0xC71585,
   COLOR_IndianRed     = 0xCD5C5C,
   COLOR_Peru          = 0xCD853F,
   COLOR_Chocolate     = 0xD2691E,
   COLOR_Tan           = 0xD2B48C,
   COLOR_LightGray     = 0xD3D3D3,
   COLOR_LightGrey     = 0xD3D3D3,
   COLOR_PaleVioletRed = 0xD87093,
   COLOR_Thistle       = 0xD8BFD8,
   COLOR_Orchid        = 0xDA70D6,
   COLOR_GoldenRod     = 0xDAA520,
   COLOR_Crimson       = 0xDC143C,
   COLOR_Gainsboro     = 0xDCDCDC,
   COLOR_Plum          = 0xDDA0DD,
   COLOR_BurlyWood     = 0xDEB887,
   COLOR_LightCyan     = 0xE0FFFF,
   COLOR_Lavender      = 0xE6E6FA,
   COLOR_DarkSalmon    = 0xE9967A,
   COLOR_Violet        = 0xEE82EE,
   COLOR_PaleGoldenRod = 0xEEE8AA,
   COLOR_LightCoral    = 0xF08080,
   COLOR_Khaki         = 0xF0E68C,
   COLOR_AliceBlue     = 0xF0F8FF,
   COLOR_HoneyDew      = 0xF0FFF0,
   COLOR_Azure         = 0xF0FFFF,
   COLOR_SandyBrown    = 0xF4A460,
   COLOR_Wheat         = 0xF5DEB3,
   COLOR_Beige         = 0xF5F5DC,
   COLOR_WhiteSmoke    = 0xF5F5F5,
   COLOR_MintCream     = 0xF5FFFA,
   COLOR_GhostWhite    = 0xF8F8FF,
   COLOR_Salmon        = 0xFA8072,
   COLOR_AntiqueWhite  = 0xFAEBD7,
   COLOR_Linen         = 0xFAF0E6,
   COLOR_LightGoldenRodYellow = 0xFAFAD2,
   COLOR_OldLace       = 0xFDF5E6,
   COLOR_Red           = 0xFF0000,
   COLOR_Fuchsia       = 0xFF00FF,
   COLOR_Magenta       = 0xFF00FF,
   COLOR_DeepPink      = 0xFF1493,
   COLOR_OrangeRed     = 0xFF4500,
   COLOR_Tomato        = 0xFF6347,
   COLOR_HotPink       = 0xFF69B4,
   COLOR_Coral         = 0xFF7F50,
   COLOR_Darkorange    = 0xFF8C00,
   COLOR_LightSalmon   = 0xFFA07A,
   COLOR_Orange        = 0xFFA500,
   COLOR_LightPink     = 0xFFB6C1,
   COLOR_Pink          = 0xFFC0CB,
   COLOR_Gold          = 0xFFD700,
   COLOR_PeachPuff     = 0xFFDAB9,
   COLOR_NavajoWhite   = 0xFFDEAD,
   COLOR_Moccasin      = 0xFFE4B5,
   COLOR_Bisque        = 0xFFE4C4,
   COLOR_MistyRose     = 0xFFE4E1,
   COLOR_BlanchedAlmond = 0xFFEBCD,
   COLOR_PapayaWhip    = 0xFFEFD5,
   COLOR_LavenderBlush = 0xFFF0F5,
   COLOR_SeaShell      = 0xFFF5EE,
   COLOR_Cornsilk      = 0xFFF8DC,
   COLOR_LemonChiffon  = 0xFFFACD,
   COLOR_FloralWhite   = 0xFFFAF0,
   COLOR_Snow          = 0xFFFAFA,
   COLOR_Yellow        = 0xFFFF00,
   COLOR_LightYellow   = 0xFFFFE0,
   COLOR_Ivory         = 0xFFFFF0,
   COLOR_White         = 0xFFFFFF,

   MAX_COLOR_CONST
}
Color_Const_t;

//////////////////////////////////////////////////////////////////////////////////////
// Copy gam object from fre-defined in .h file (Src) to Object
// this will allocate memory for the Object as well,
// so Object should be NULL before calling this function
GAMLOAD_Object_t* LoadPreDefinedGam(GAMLOAD_Object_t* Src);

ST_ErrorCode_t GetGamObjById(int FileNameIndex, GAMLOAD_Object_t* object);

ST_ErrorCode_t DrawGamById(int FileNameIndex, APIDrawParams obj_APIDrawParams);

//////////////////////////////////////////////////////////////////////////////////////
#endif //RESOURCES_HPP_
