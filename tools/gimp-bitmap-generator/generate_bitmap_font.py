#!/usr/bin/env python

import gimpcolor
import math
import time
from gimpfu import *

# handy funcs
# pdb.gimp_message(outputfolder)

def generate_otc_bitmap_font(image, drawable, font, font_size, border, antialias, outputfolder):
	if not outputfolder:
		pdb.gimp_message('You need to select correct output folder.')
		return

	# automated glyphs size getting
	temp_image = gimp.Image(300, 300, RGB)
	test_layer = pdb.gimp_text_fontname(temp_image, None, 0, 0, chr(137).decode('cp1252'), border, antialias, font_size, PIXELS, font)
	glyph_width = test_layer.width + (int(border) * 2)
	temp_image.remove_layer(test_layer)

	test_layer = pdb.gimp_text_fontname(temp_image, None, 0, 0, chr(138).decode('cp1252'), border, antialias, font_size, PIXELS, font)
	glyph_height = test_layer.height + (int(border) * 2)
	temp_image.remove_layer(test_layer)
	gimp.pdb.gimp_image_delete(temp_image)
	#pdb.gimp_message(str(int(round(glyph_width))) + ' ' + str(int(round(glyph_height))))

	# actual making glyph
	char_begin = 32 # space is always first
	char_end = 256
	num_chars = char_end - char_begin
	width = int(glyph_width) * 16
	num_lines = num_chars / 16
	if num_chars % 16 > 0:
		num_lines += 1
	height = int(glyph_height) * num_lines

	if border > 0:
		width += (int(border) * 2)
		height += (int(border) * 2)

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
		x_pos = (offset % 16) * int(glyph_width)
		y_pos = ((offset / 16) * int(glyph_height))

		if border > 0:
			border_layer = gimp.Layer(image, "border" + str(i), width, height, RGBA_IMAGE, 100, NORMAL_MODE)
			image.add_layer(border_layer, 0)
		
		text_layer = pdb.gimp_text_fontname(image, None, x_pos, y_pos, string, border, antialias, font_size, PIXELS, font)

		if border > 0:
			pdb.gimp_image_select_color(image, CHANNEL_OP_REPLACE, text_layer, gimpcolor.RGB(255,255,255))
			pdb.gimp_selection_grow(image, border)
			pdb.gimp_bucket_fill(border_layer, 1, 0, 100, 1.0, True, 0, 0)
			#pdb.gimp_edit_fill(text_layer, BACKGROUND_FILL)
			#pdb.gimp_drawable_edit_fill(text_layer, BACKGROUND_FILL)
			#add_text_outline(image, text_layer)
		gimp.progress_update(float(offset) / float(num_chars))

	image.merge_visible_layers(CLIP_TO_IMAGE)
	image.enable_undo()

	# save everything
	fontText = str(font) + '-' + str(int(round(font_size))) + 'px'
	if antialias:
		fontText = fontText + '-antialiased'
	if border:
		fontText = fontText + '-bordered'

	f = open(outputfolder + '/' + fontText + '.otfont', "w")
	f.write('Font\n  name: ' + fontText + '\n  texture: ' + fontText + '\n  height: ' + str(int(round(glyph_height))) + '\n  glyph-size: ' + str(int(round(glyph_width))) + ' ' + str(int(round(glyph_height))) + '\n  space-width: 4\n')
	f.close()
	pdb.gimp_file_save(image, image.layers[0], outputfolder + '/' + fontText + '.png', '?')

register(
	"generate_otc_bitmap_font",
	"Generate OTC font - Eduardo Bart & Qbazzz\n\nYou can leave input image and drawable empty.",
	"Generate OTC font",
	"Eduardo Bart & Qbazzz",
	"Eduardo Bart & Qbazzz",
	"2021",
	"<Image>/File/Create/_Generate OTC font",
	"",
	[
		(PF_FONT, "Font", "Font", "Arial"),
		(PF_SPINNER, "Font_Size", "Font size", 13, (1, 128, 1)),
		(PF_SPINNER, "Border_Size", "Border size", 0, (0, 256, 1)),
		(PF_BOOL, "Antialias", "Antialias", False),
		(PF_DIRNAME, "Output_Folder", "Output directory", "")
	],
	[],
	generate_otc_bitmap_font)

main()
