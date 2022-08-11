#ifndef FONT16SEG_H
#define FONT16SEG_H

#ifndef __FONTPARAMS
#define __FONTPARAMS

struct FONT_PARAMS
	{
	char	type;			// type of characters
	char	size;			// size (hight) in pixels
	char	width;			// width in pixels
	char	space;			// space between two characters
	char	gap;			// gap between two segments
	char	bold;			// pens width
	};
#endif

extern void f16s_init(char size, char width, char space, char bold);
extern int f16s_text(char* text, int x, int y);

#endif  // FONT16SEG_H
