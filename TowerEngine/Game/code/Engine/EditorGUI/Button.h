#ifndef BUTTON_H
#define BUTTON_H

struct ui_button
{
	input_state InputState;
	bool32 IsMouseOver;

	loaded_image *ButtonImage;

	rect Rect;
	gl_square_outline GLOutline;

	string ButtonText;
};

struct button_bucket
{
	uint32 Count;
	ui_button Bucket[100];
};

#endif