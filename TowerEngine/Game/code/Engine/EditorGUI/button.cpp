
// TODO make an edit mode to visually construct these positions, positions and rect
// TODO make this auto size to the Word

void
ProcessButtonState(input_state *ButtonProcessing, bool32 NewState)
{
	if (NewState)
	{
		if (ButtonProcessing->IsDown)
		{
			ButtonProcessing->OnDown = false;
		}
		else
		{
			if (!ButtonProcessing->Claimed)
			{
				ButtonProcessing->IsDown = true;
				ButtonProcessing->OnDown = true;
				ButtonProcessing->IsUp = false;
				ButtonProcessing->OnUp = false;
			}
		}
	}
	else
	{
		if (ButtonProcessing->IsUp)
		{
			ButtonProcessing->IsUp = true;
			ButtonProcessing->OnUp = false;
			ButtonProcessing->IsDown = false;
			ButtonProcessing->OnDown = false;
		}
		else
		{
			ButtonProcessing->IsUp = true;
			ButtonProcessing->OnUp = true;
			ButtonProcessing->IsDown = false;
			ButtonProcessing->OnDown = false;
		}
	}
}

// void
// RenderButtonOutline(ui_button *Button, list_head *RenderList)
// {
// 	PushRenderSquareOutline(RenderList, Button->GLOutline);
// }

void
RenderButtonTexture(ui_button *Button, loaded_image *Image, color Color, real64 RadiansAngle, render_layer* RenderLayer)
{
	gl_texture ButtonImage = {};
	ButtonImage.Image = Image;

	real64 ButtonWidth = Button->Rect.TopLeft.X - Button->Rect.BottomRight.X;
	real64 ButtonHeight = Button->Rect.TopLeft.Y - Button->Rect.BottomRight.Y;

	real64 FitScaleX = ButtonWidth / Image->Width;
	real64 FitScaleY = ButtonHeight / Image->Height;

	ButtonImage.Center = vector2{(Button->Rect.TopLeft.X + Button->Rect.BottomRight.X) / 2,
	                             (Button->Rect.TopLeft.Y + Button->Rect.BottomRight.Y) / 2};
	// ButtonImage.Scale = vector2{FitScaleX * Image->Width * 0.5f, FitScaleY * Image->Height * 0.5f};
	ButtonImage.Scale = vector2{ButtonWidth, ButtonHeight};
	ButtonImage.RadiansAngle = RadiansAngle;
	ButtonImage.Color = Color;

	PushRenderTexture(RenderLayer, &ButtonImage);

	// TODO render button with texture
	// PushRenderSquare(RenderList, MakeGLRectangle(GameState->ApplyShapeButton.Rect, Color), Memory);
}

void
RenderButtonTexture(ui_button *Button, loaded_image *Image, color Color, render_layer* RenderLayer)
{
	RenderButtonTexture(Button, Image, Color, PI, RenderLayer);
}

// NOTE this should only be called once per frame per button
void
UpdateButton(ui_button *Button, game_input *GameInput)
{
	//NOTE this is mouse input
	if (RectContains(Button->Rect, GameInput->MousePos))
	{
		Button->IsMouseOver = true;
	}
	else
	{
		Button->IsMouseOver = false;
	}

	if ((GameInput->MouseLeft.IsDown || GameInput->MouseRight.IsDown) &&
	    RectContains(Button->Rect, GameInput->MousePos))
	{
		ProcessButtonState(&Button->InputState, true);
	}
	else
	{
		ProcessButtonState(&Button->InputState, false);
	}

	// if (GameInput->TouchStatus[0].IsDown && RectContains(Button->Rect, GameInput->TouchPosition[0]))
	// {
	// 	ProcessButtonState(&Button->InputState, true);
	// }
	// else
	// {
	// 	ProcessButtonState(&Button->InputState, false);
	// }
}

void
MakeButton(ui_button *Button, vector2 TopLeft, vector2 BottomRight)
{
	Button->Rect.TopLeft = TopLeft;
	Button->Rect.BottomRight = BottomRight;
	Button->GLOutline = MakeSquareOutline(Button->Rect, COLOR_RED, 1.0f);
}

void
MakeButton(ui_button *Button, rect ButtonRect)
{
	Button->Rect = ButtonRect;
	Button->GLOutline = MakeSquareOutline(Button->Rect, COLOR_RED, 1.0f);
}

// NOTE this is just a convenience method for this specific type of button behaviour,
// for other effects we'll need to rewrite all this.
struct hover_info
{
	color IdleColor;
	color DownColor;
};

void
UpdateHoverButton(ui_button * TextButton, loaded_image * ButtonImage, real64 ScaleModifier, hover_info HoverInfo,
                  list_head * AlphabetBitmaps, render_layer* RenderLayer, game_input * GameInput)
{
	UpdateButton(TextButton, GameInput);

	color ButtonColor = HoverInfo.IdleColor;
	color TypeColor = COLOR_BLACK;
	if (TextButton->IsMouseOver)
	{
		// ButtonColor = color{0.8f, 0.8f, 0.8f, 1.0f};
	}
	if (TextButton->InputState.IsDown)
	{
		ButtonColor = HoverInfo.DownColor;
		TypeColor.A = ButtonColor.A;
	}
	RenderButtonTexture(TextButton, ButtonImage, ButtonColor, RenderLayer);
	FontRenderString(TextButton->ButtonText, vector2{TextButton->Rect.TopLeft.X + 4, TextButton->Rect.BottomRight.Y - 4},
	                 ScaleModifier, TypeColor, AlphabetBitmaps, RenderLayer);
}

void
UpdateHoverButton(ui_button * TextButton, loaded_image * ButtonImage, real64 ScaleModifier,
                  list_head * AlphabetBitmaps, render_layer* RenderLayer, game_input * GameInput)
{
	hover_info HoverInfo = {};
	HoverInfo.IdleColor = COLOR_WHITE;
	HoverInfo.DownColor = COLOR_GREY;
	UpdateHoverButton(TextButton, ButtonImage, ScaleModifier, HoverInfo, AlphabetBitmaps, RenderLayer, GameInput);
}

ui_button *
PullButtonFromBucket(button_bucket* Bucket)
{
	ui_button *ButtonReturning = &Bucket->Bucket[Bucket->Count];
	Bucket->Count++;
	return (ButtonReturning);
}
