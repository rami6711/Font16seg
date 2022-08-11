/*
 * Vykresovanie textu na graficky display v tvare 16-segmentovkoveho displeja
 */

#include "text16seg.h"

volatile struct FONT_PARAMS textFont;
	
/*
   14 segment charset lookup table
        0
    --------- 
   |\   |   /|
 5 | \D |6 /7| 1     O E
   |  \ | /  |
    -C-- --8-  
   |  / | \  |
 4 | /B |A \9| 2     O F
   |/   |   \|
    ---------  
        3
*/

static int text16segmentVectors[][4]=
{	// X1,Y1,X2,Y2
	{ 0, 0, 2, 0},	// segment 0
	{ 2, 0, 2, 1},	// segment 1
	{ 2, 1, 2, 2},	// segment 2
	{ 0, 2, 2, 2},	// segment 3
	{ 0, 1, 0, 2},	// segment 4
	{ 0, 0, 0, 1},	// segment 5
  
	{ 1, 1, 0, 1},	// segment 6
	{ 1, 1, 0, 2},	// segment 7
	{ 1, 1, 2, 1},	// segment 8
	{ 1, 1, 2, 2},	// segment 9
	{ 1, 1, 1, 2},	// segment A
	{ 1, 1, 0, 2},	// segment B
	{ 1, 1, 0, 1},	// segment C
	{ 1, 1, 0, 0},	// segment D
};

// Character table of ASCII from 32..127
//static U16 text16segmentCharset[]=
static unsigned long text16segmentCharset[]=
{           // FEDC BA98 7654 3210
	0x0000,   // 0000 0000 0000 0000 (space)
	0x0000,   // 1000 0000 0100 0000 !
	0x0180,   // 0000 0001 1000 0000 "
	0x553C,   // 0101 0101 0011 1100 #
	0x55BB,   // 0101 0101 1011 1011 $
	0x7799,   // 0111 0111 1001 1001 %
	0xC979,   // 1100 1001 0111 1001 &
	0x0200,   // 0000 0010 0000 0000 '
	0x0A00,   // 0000 1010 0000 0000 (
	0xA000,   // 1010 0000 0000 0000 )
	0xFF00,   // 1111 1111 0000 0000 *
	0x5500,   // 0101 0101 0000 0000 +
	0x0000,   // 0000 0000 0000 0000 ,
	0x4400,   // 0100 0100 0000 0000 -
	0x0000,   // 0000 0000 0000 0000 .
	0x2200,   // 0010 0010 0000 0000 /
	0x22FF,   // 0010 0010 1111 1111 0
	0x1100,   // 0001 0001 0000 0000 1
	0x4477,   // 0100 0100 0111 0111 2
	0x443F,   // 0100 0100 0011 1111 3
	0x448C,   // 0100 0100 1000 1100 4
	0x44BB,   // 0100 0100 1011 1011 5
	0x44FB,   // 0100 0100 1111 1011 6
	0x000F,   // 0000 0000 0000 1111 7
	0x44FF,   // 0100 0100 1111 1111 8
	0x44BF,   // 0100 0100 1011 1111 9
	0x0000,   // 0000 0000 0010 0001 :
	0x0000,   // 0010 0000 0000 0001 ;
	0x2430,   // 0010 0100 0011 0000 <
	0x4430,   // 0100 0100 0011 0000 =
	0x4830,   // 0100 1000 0011 0000 >
	0x1407,   // 0001 0100 0000 0111 ?

	0x507F,   // 0101 0000 0111 1111 @
	0x44CF,   // 0100 0100 1100 1111 A
	0x153F,   // 0001 0101 0011 1111 B
	0x00F3,   // 0000 0000 1111 0011 C
	0x113F,   // 0001 0001 0011 1111 D
	0x40F3,   // 0100 0000 1111 0011 E
	0x40C3,   // 0100 0000 1100 0011 F
	0x04FB,   // 0000 0100 1111 1011 G
	0x44CC,   // 0100 0100 1100 1100 H
	0x1133,   // 0001 0001 0011 0011 I
	0x007C,   // 0000 0000 0111 1100 J
	0x4AC0,   // 0100 1010 1100 0000 K
	0x00F0,   // 0000 0000 1111 0000 L
	0x82CC,   // 1000 0010 1100 1100 M
	0x88CC,   // 1000 1000 1100 1100 N
	0x00FF,   // 0000 0000 1111 1111 O
	0x44C7,   // 0100 0100 1100 0111 P
	0x08FF,   // 0000 1000 1111 1111 Q
	0x4CC7,   // 0100 1100 1100 0111 R
	0x44BB,   // 0100 0100 1011 1011 S
	0x1103,   // 0001 0001 0000 0011 T
	0x00FC,   // 0000 0000 1111 1100 U
	0x22C0,   // 0010 0010 1100 0000 V
	0x28CC,   // 0010 1000 1100 1100 W
	0xAA00,   // 1010 1010 0000 0000 X
	0x9200,   // 1001 0010 0000 0000 Y
	0x2233,   // 0010 0010 0011 0011 Z
	0x00E1,   // 0000 0000 1110 0001 [
	0x8800,   // 1000 1000 0000 0000 (backslash)
	0x001E,   // 0000 0000 0001 1110 ]
	0x2800,   // 0010 1000 0000 0000 ^
	0x0030,   // 0000 0000 0011 0000 _
	
	0x8000,   // 1000 0000 0000 0000 `	-> male pismena
	0x44CF,   // 0100 0100 1100 1111 A
	0x44F8,   // 0100 0100 1111 1000 b
	0x4470,   // 0100 0100 0111 0000 c
	0x447C,   // 0100 0100 0111 1100 d
	0x40F3,   // 0100 0000 1111 0011 E
	0x5502,   // 0101 0101 0000 0010 f
	0x04FB,   // 0000 0100 1111 1011 G
	0x44C8,   // 0100 0100 1100 1000 h
	0x5030,   // 0101 0000 0011 0000 i
	0x007C,   // 0000 0000 0111 1100 J
	0x4AC0,   // 0100 1010 1100 0000 K
	0x00F0,   // 0000 0000 1111 0000 L
	0x82CC,   // 1000 0010 1100 1100 M
	0x4448,   // 0100 0100 0100 1000 n
	0x4478,   // 0100 0100 0111 1000 o
	0x44C7,   // 0100 0100 1100 0111 P
	0x08FF,   // 0000 1000 1111 1111 Q
	0x4440,   // 0100 0100 0100 0000 r
	0x44BB,   // 0100 0100 1011 1011 S
	0x40F0,   // 0100 0000 1111 0000 t
	0x0078,   // 0000 0000 0111 1000 u
	0x22C0,   // 0010 0010 1100 0000 V
	0x2848,   // 0010 1000 0100 1000 W
	0xAA00,   // 1010 1010 0000 0000 X
	0x9200,   // 1001 0010 0000 0000 Y
	0x2233,   // 0010 0010 0011 0011 Z
	0x5112,   // 0101 0001 0001 0010 {
	0x1100,   // 0001 0001 0000 0000 |
	0x1521,   // 0001 0101 0010 0001 }
	0x0585,   // 0000 0101 1000 0101 ~
	0xFFFF,   // 1111 1111 1111 1111 dummy
};

///////////////////////////////////////////////////////////////////////////
int _step(int a)
{
 if (a>=1) return(1);
 if (a<=-1) return(-1);
 return(0);
}

/* nastavenie fontu
	size		- size (hight) in pixels
	width		- width in pixels
	space		- space between two characters
	bold		- pens width
*/
void f16s_init(char size, char width, char space, char bold)
{
 textFont.size = size;
 textFont.width = width;
 textFont.space = space;
 textFont.bold = bold;
// textFont.gap = textFont.bold/6;
}

// vykresli jeden znak na displej pre zadanu poziciu (x,y)
// vrati hrubku znaku
int _char16seg(char mych, int x, int y)
{
 int mySegment;
 unsigned long ulSegment;
 int i, boldstep;
 //int sgap;
 int x1, x2, y1, y2;
 int x1b, x2b, y1b, y2b;
 int  dx, dy;
 int Wpol, Spol;
 int flag1=0, flag2=0, flag3=0;

 if (mych < 0)   mych=127;			//unknown character (dummy)
 if (mych > 127) mych=127;			//unknown character (dummy)
 if (mych < 32) return(0);				//control character (invisible)
 if (textFont.bold == 0) return(0);		//invisible pen width
 if (textFont.size < 5) return(0);		//invisible character size
 if (textFont.width < 3) return(0);		//invisible character width


 ulSegment = text16segmentCharset[mych-32];	//character to multi-segment
 if (ulSegment < 0xFFFF)		//zobrazuje sa 16-segmentovka
 {
	 mySegment = ulSegment;
	#if 1
	 if ((mySegment & 0x0003) == 0x0003) { flag1=1; mySegment &= (~0x0002); }		//segment 0 aj 1 - kresli sa segmentom 0
	 if ((mySegment & 0x0030) == 0x0030) { flag2=1; mySegment &= (~0x0010); }		//segment 4 aj 5 - kresli sa segmentom 5
	 if ((mySegment & 0x4400) == 0x4400) { flag3=1; mySegment &= (~0x0400); }		//segment A aj E - kresli sa segmentom E
	#endif

	 Wpol = (textFont.width-textFont.bold+1)/2;
	 Spol = textFont.size/2;

	 for (i = 0; i<16; i++)
		{
		 if (mySegment & 0x0001)	// segment svieti
			{
			 dx = text16segmentVectors[i][2]-text16segmentVectors[i][0];
			 dy = text16segmentVectors[i][3]-text16segmentVectors[i][1];

			 x1 = x + text16segmentVectors[i][0] * Wpol+1;
			 y1 = y + text16segmentVectors[i][1] * Spol;
			 x2 = x + text16segmentVectors[i][2] * Wpol+1;
			 y2 = y + text16segmentVectors[i][3] * Spol;

		#if 1
			 if (((flag1==1)&(i==0x0)) |							//spojenie segmentov 0-1, 5-6, A-E
				 ((flag2==1)&(i==0x5)) |
				 ((flag3==1)&(i==0xE)) )
				{
				 x2 += text16segmentVectors[i][2] * Wpol;
				// y2 += text16segmentVectors[i][3] * Spol;			//netreba - vodorovne segmenty
				}
		#endif

		#if 0
			 sgap = textFont.gap;
			 if ((dx != 0) & (dy != 0)) sgap*=2;
			 x1 += sgap * dx;
			 x2 -= sgap * dx;	//zmensovanie segmentu == medzera medzi segmentami
			 y1 += sgap * dy;
			 y2 -= sgap * dy;
		#endif

			 x1b = x1; x2b = x2;
			 y1b = y1; y2b = y2;

			 boldstep = textFont.bold;

//			 if ((dx != 0) && (dy != 0) && ((boldstep & 0x01)==0))
//				 boldstep++; // pri parnej sirke treba X segmenty vyplnit

			 if ((boldstep & 0x01)==0)	//test na parnost
				{
				 if ((dx != 0) && (dy != 0))
				 	 {
					 boldstep++;
				 	 }
				 else
				 	 {
					 x1b -= dy;		y1b += dx;
					 x2b -= dy;		y2b += dx;
				 	 }
				}

			 do
				{
				 if (boldstep & 0x01)	//test na neparnost
					{
					 GD_LineFast(x1, y1, x2, y2);
					 x1 += _step(dx + dy);		y1 += _step(dy - dx);
					 x2 -= _step(dx - dy);		y2 -= _step(dy + dx);
					 x1b += _step(dx - dy);		y1b += _step(dy + dx);
					 x2b -= _step(dx + dy);		y2b -= _step(dy - dx);
					 boldstep--;		//parne alebo 0
					}

				 if (boldstep != 0)		//parna hrubka
					{
					 GD_LineFast(x1, y1, x2, y2);
					 GD_LineFast(x1b, y1b, x2b, y2b);
					 x1 += _step(dx + dy);		y1 += _step(dy - dx);
					 x2 -= _step(dx - dy);		y2 -= _step(dy + dx);
					 x1b += _step(dx - dy);		y1b += _step(dy + dx);
					 x2b -= _step(dx + dy);		y2b -= _step(dy - dx);
					 boldstep--;
					 boldstep--;
					}
				}
			 while (boldstep != 0);

			}
		 mySegment>>=1;
		}
	 if (textFont.bold == 2)		//zaoblovanie (mazanie) rohov pre hrubku 2
	 {
		 disp_set_drawmode(1);		//mazanie
		 disp_setpixel(x,y);
		 disp_setpixel(x+textFont.width,y);
		 disp_setpixel(x,y+textFont.size+1);
		 disp_setpixel(x+textFont.width,y+textFont.size+1);
		 disp_set_drawmode(0);
	 }
	 return(textFont.width + 1);	//textFont.bold);
 }
 else
 {
	 mySegment = ulSegment>>16;			//zobrazuje sa ciara (5-segmentov)
	 x1 = x-textFont.bold/2+1;
	 x2 = x+textFont.bold/2+1;
	 boldstep = (textFont.size+textFont.bold-1);		//vyska jedneho segmentu
	 for (i = 0; i<5; i++)
		{
		 if (mySegment & 0x0001)
			{
			 y1 = y + (4-i)*boldstep/5;
			 y2 = y + (5-i)*boldstep/5;
			 disp_box(x1, y1, x2, y2);
			}
		 mySegment>>=1;
		}
	 return(textFont.bold+1);
 }
}


int text16seg(char* text, int x, int y)
{
 int i;
// int mx, my;

 if (textFont.size == 0)		//default hodnoty
	{
	 textFont.size = 28;
	 textFont.width = 13;
	 textFont.space = 2;
	 textFont.bold = 4;
	}
 
 
 i = 0;
 while (text[i] != '\0')
	{
//	 mx = (unsigned char)x;
//	 my = (unsigned char)y;
	 x += _char16seg(text[i], x+((textFont.bold-1)/2), y+((textFont.bold-1)/2));
	 x += textFont.space;
	 /*
	 switch(text[i])
		{
	 	 case	'.':
			{
			 my += textFont.size-(textFont.bold/2);
			 disp_box(mx, my, mx+textFont.bold-1, my+textFont.bold-1);
			 x += textFont.bold;
			 x += textFont.space;
			 break;
			}
		 case	':':
			{
			 my += (textFont.size/4);
			 disp_box(mx, my, mx+textFont.bold-1, my+textFont.bold-1);
			 my += (textFont.size/2);
			 disp_box(mx, my, mx+textFont.bold-1, my+textFont.bold-1);
			 x += textFont.bold;
			 x += textFont.space;
			 break;
			}
		 case	';':
			{
			 my += (textFont.size/2);
			 disp_box(mx, my, mx+textFont.bold-1, my+textFont.size/4-1);
			 my += (textFont.size/4);
			 disp_box(mx, my+textFont.size/8, mx+textFont.bold-1, my+textFont.size/4+textFont.bold);
			 x += textFont.bold;
			 x += textFont.space;
			 break;
			}
		 case	'!':
			{
			 disp_box(mx, my, mx+textFont.bold-1, my+textFont.size-(textFont.bold)-1);	// |
			 my += textFont.size-(textFont.bold/2);
			 disp_box(mx, my, mx+textFont.bold-1, my+textFont.bold-1);						// .
			 x += textFont.bold;
			 x += textFont.space;
			 break;
			}
		 default:	
			{
			 x += _char16seg(text[i], x+((textFont.bold-1)/2), y+((textFont.bold-1)/2));
			 //x += textFont.width;
			 //x += textFont.bold;
			 x += textFont.space;
			 break;
			}
		}
	*/
	 i++;
	}
 return(x);	//vrati aktualnu poziciu kurzora
}
