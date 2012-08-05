#!/usr/bin/env python

import math
from gimpfu import *

def generate_bitmap_font(timg, tdrawable, font, font_size, first_char, glyph_width, glyph_height, antialias):
	char_begin = int(first_char)
	char_end = 256
	num_chars = char_end - char_begin
	width = int(glyph_width) * 16
	num_lines = num_chars / 16
	if num_chars % 16 > 0:
		num_lines += 1
	height = int(glyph_height) * num_lines

	image = gimp.Image(width, height, RGB)
	image.disable_undo()

	gimp.set_foreground(255, 255, 255)
	gimp.set_background(0, 0, 0)

	glyphs_layer = gimp.Layer(image, "Glyphs", width, height, RGBA_IMAGE, 100, NORMAL_MODE)
	image.add_layer(glyphs_layer, 0)

	disp = gimp.Display(image)

	for i in range(char_begin, char_end):
		if i == 127 or i == 129 or (i >= 141 and i <= 144) or i == 157:
			string = u"\u25A1"
		else:
			string = chr(i).decode('cp1252')
		offset = i - char_begin

		x_pos =  (offset % 16) * int(glyph_width)
		y_pos =  (offset / 16) * int(glyph_height)

		text_layer = pdb.gimp_text_fontname(image, None, x_pos, y_pos, string, -1, antialias, font_size, PIXELS, font)

		gimp.progress_update(float(offset) / float(num_chars))

	image.merge_visible_layers(CLIP_TO_IMAGE)
	image.enable_undo()

register(
	"generate_bitmap_font",
	"Generate bitmap font",
	"Generate bitmap font",
	"Eduardo Bart",
	"Eduardo Bart",
	"2011",
	"<Image>/File/Create/_Generate Bitmap Font",
	"",
	[
		(PF_FONT, "Font", "Font", "Sans"),
		(PF_SPINNER, "Font_Size", "Font Size", 10, (1, 128, 1)),
		(PF_SPINNER, "First_Char", "First Char", 32, (0, 256, 1)),
		(PF_SPINNER, "Glyph_Width", "Glyph Width", 16, (2, 64, 2)),
		(PF_SPINNER, "Glyph_Height", "Glyph Height", 16, (2, 64, 2)),
		(PF_BOOL, "Antialias", "Antialias", True)
	],
	[],
	generate_bitmap_font)

main()
