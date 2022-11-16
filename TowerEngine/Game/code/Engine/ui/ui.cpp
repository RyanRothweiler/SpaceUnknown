/*
	UI will be immediate mode for now, but everything will sit together in one place... here

	- BeginLayoutGroup (right / left alignment option)
	- if (Button)
	- Scaling for platforms


	NOTES
	- Anchors are relative to the screen (0.5 is half the screen)
	- Draws are abosolute relative to the anchor (TopLeft.XY of 0 is going to be exactlty where the anchor is)
	???? ^^^^^ ????


*/

namespace ui {

	const real32 ButtonStandardHeight = 32.0f;

	static anim_state AnimStates[100] = {};
	static int NextAnimState = 0;

	vector2 MiddleCenterText(rect Rect)
	{
		vector2 Pos = RectLeftEdgeMiddle(Rect);
		Pos.Y += RectHeight(Rect) * 0.5f;
		Pos.X += 4;
		return Pos;
	}

	real64 ButtonHoverOutline(rect Hitbox)
	{
		real32 HoverOutlineSize = 3.0f;
		real32 AnimSpeed = 0.5f;

		anim_state* AnimState = &AnimStates[NextAnimState++];
		Assert(NextAnimState < ArrayCount(AnimStates));
		// TODO check if this is a different anim state than last time

		float HoverAnimTarget = 0.0f;
		float ClickAnimTarget = 0.0f;

		if (RectContains(Hitbox, Globals->UIUserInput->TouchPosition[0])) {
			HoverAnimTarget = 1.0f;
			if (Globals->UIUserInput->TouchStatus[0].IsDown) {
				ClickAnimTarget = 1.0f;
			}
		}

		AnimState->HoverAnimTime = Lerp(AnimState->HoverAnimTime, HoverAnimTarget, AnimSpeed);
		AnimState->ClickAnimTime = Lerp(AnimState->ClickAnimTime, ClickAnimTarget, AnimSpeed);

		return 1.0f + (AnimState->HoverAnimTime * HoverOutlineSize) + (AnimState->ClickAnimTime * HoverOutlineSize);
	}

	bool DrawButton(rect Rect, string Content, color Color, font_style* Style, color BackgroundColor)
	{
		real64 OutlineSize = 1;
		Rect.TopLeft.X += OutlineSize;
		Rect.TopLeft.Y += OutlineSize;
		Rect.BottomRight.X -= OutlineSize;
		Rect.BottomRight.Y -= OutlineSize;

		rect OutlineRect = RectOutline(Rect, OutlineSize);
		real64 OutlineAnim = ButtonHoverOutline(OutlineRect);

		RenderRect(Rect, BackgroundColor, 0, Globals->UIRenderer);
		RenderRectangleOutline(Rect, OutlineAnim, Color, 0, Globals->UIRenderer);

		vector2 TextPos = MiddleCenterText(Rect) - vector2{0, 4};
		FontRenderString(Style->Font, Content,
		                 TextPos,
		                 Style->SizePoints,
		                 COLOR_WHITE, 0, Globals->UIRenderer);


		if (Globals->UIUserInput->TouchStatus[0].OnUp &&
		        Globals->UIUserInput->Touches[0].Gesture == touch_gesture::tap &&
		        RectContains(OutlineRect, Globals->UIUserInput->Touches[0].Position)
		   ) {
			return true;
		}
		return false;
	}

	void VisualizeContainers(container* Container)
	{
		RenderRect(Container->FinalPositionAbsolute, Container->DebugColor, 0, Globals->DebugUIRenderer);
		for (int x = 0; x < Container->ChildrenCount; x++) {
			VisualizeContainers(Container->Children[x]);
		}
	}

	container* AddChild(container* Parent)
	{
		container* Cont = (container*)ArenaAllocate(GlobalTransMem, sizeof(container));
		ClearMemory((uint8*)Cont, sizeof(container));

		Cont->AnchorsRelative.TopLeft = vector2{0, 0};
		Cont->AnchorsRelative.BottomRight = vector2{1, 1};
		Cont->DebugColor = color{0, 1, 0, 0.15f};

		Parent->Children[Parent->ChildrenCount++] = Cont;
		Assert(Parent->ChildrenCount < ArrayCount(Parent->Children));

		return Cont;
	}

	void ResolveLayout(container* Self, container* Parent, bool32 ParentDidLayout)
	{
		Assert(Self != Parent);

		bool32 ParentDoingLayout = false;

		// This allows this call to be used on top level containers
		if (Parent != GameNull) {

			if (!ParentDidLayout) {
				real32 Width = (real32)RectWidth(Parent->FinalPositionAbsolute);
				real32 Height = (real32)RectHeight(Parent->FinalPositionAbsolute);

				Self->FinalPositionAbsolute.TopLeft = vector2{
					Parent->FinalPositionAbsolute.TopLeft.X + (Self->AnchorsRelative.TopLeft.X * Width),
					Parent->FinalPositionAbsolute.TopLeft.Y + (Self->AnchorsRelative.TopLeft.Y * Height)
				} + Self->ScrollOffset;
				Self->FinalPositionAbsolute.BottomRight = vector2{
					Parent->FinalPositionAbsolute.TopLeft.X + (Self->AnchorsRelative.BottomRight.X * Width),
					Parent->FinalPositionAbsolute.TopLeft.Y + (Self->AnchorsRelative.BottomRight.Y * Height)
				} + Self->ScrollOffset;
			}

			// Parent will handle the layout of the children
			if (Self->ChildLayoutType != child_layout_type::none) {
				ParentDoingLayout = true;

				// Flow horizontally to the right
				if (Self->ChildLayoutType == child_layout_type::horizontal_align_right) {

					// Horizontal align buttons cannot fit horizontally
					Assert(Self->ChildLayoutSizePixels.X != -1);

					vector2 TopLeftCursor = vector2{Self->FinalPositionAbsolute.BottomRight.X, Self->FinalPositionAbsolute.TopLeft.Y};

					// Start at the back
					for (int x = Self->ChildrenCount - 1; x >= 0; x--) {

						TopLeftCursor.X -= Self->ChildLayoutSizePixels.X;
						container* Child = Self->Children[x];

						Child->FinalPositionAbsolute.TopLeft = TopLeftCursor;

						if (Self->ChildLayoutSizePixels.Y == -1) {
							Child->FinalPositionAbsolute.BottomRight.X = TopLeftCursor.X + Self->ChildLayoutSizePixels.X;
							Child->FinalPositionAbsolute.BottomRight.Y = Self->FinalPositionAbsolute.BottomRight.Y;
						} else {
							Child->FinalPositionAbsolute.BottomRight = TopLeftCursor + Self->ChildLayoutSizePixels;
						}

						TopLeftCursor.X -= Self->ChildLayoutGutter;
					}
				} else if (Self->ChildLayoutType == child_layout_type::vertical_align_bottom) {

					// Vertical align buttons cannot fit horzontally
					Assert(Self->ChildLayoutSizePixels.Y != -1);

					vector2 TopLeftCursor = vector2{Self->FinalPositionAbsolute.TopLeft.X,
					                                //Self->FinalPositionAbsolute.TopLeft.Y + ((Self->ChildrenCount - 1) * Self->ChildLayoutGutter) + (Self->ChildrenCount * Self->ChildLayoutSizePixels.Y),
					                                Self->FinalPositionAbsolute.BottomRight.Y,
					                               };

					// Start at the back
					for (int x = Self->ChildrenCount - 1; x >= 0; x--) {

						TopLeftCursor.Y -= Self->ChildLayoutSizePixels.Y;
						container* Child = Self->Children[x];

						Child->FinalPositionAbsolute.TopLeft = TopLeftCursor;

						if (Self->ChildLayoutSizePixels.X == -1) {
							Child->FinalPositionAbsolute.BottomRight.X = Self->FinalPositionAbsolute.BottomRight.X;
							Child->FinalPositionAbsolute.BottomRight.Y = TopLeftCursor.Y + Self->ChildLayoutSizePixels.Y;
						} else {
							Child->FinalPositionAbsolute.BottomRight = TopLeftCursor + Self->ChildLayoutSizePixels;
						}

						TopLeftCursor.Y -= Self->ChildLayoutGutter;
					}

				} else if (Self->ChildLayoutType == child_layout_type::vertical_align_top) {

					// Vertical align buttons cannot fit horzontally
					Assert(Self->ChildLayoutSizePixels.Y != -1);

					vector2 TopLeftCursor = vector2{Self->FinalPositionAbsolute.TopLeft.X,
					                                Self->FinalPositionAbsolute.TopLeft.Y + ((Self->ChildrenCount - 1) * Self->ChildLayoutGutter) + (Self->ChildrenCount * Self->ChildLayoutSizePixels.Y),
					                               };

					// Start at the back
					for (int x = Self->ChildrenCount - 1; x >= 0; x--) {

						TopLeftCursor.Y -= Self->ChildLayoutSizePixels.Y;
						container* Child = Self->Children[x];

						Child->FinalPositionAbsolute.TopLeft = TopLeftCursor;

						if (Self->ChildLayoutSizePixels.X == -1) {
							Child->FinalPositionAbsolute.BottomRight.X = Self->FinalPositionAbsolute.BottomRight.X;
							Child->FinalPositionAbsolute.BottomRight.Y = TopLeftCursor.Y + Self->ChildLayoutSizePixels.Y;
						} else {
							Child->FinalPositionAbsolute.BottomRight = TopLeftCursor + Self->ChildLayoutSizePixels;
						}

						TopLeftCursor.Y -= Self->ChildLayoutGutter;
					}


				} else if (Self->ChildLayoutType == child_layout_type::horizontal_fit) {
					real64 Width = RectWidth(Self->FinalPositionAbsolute);
					real64 SpaceForGutters = (Self->ChildrenCount - 1) * Self->ChildLayoutGutter;
					real64 ChildWidth = (Width - SpaceForGutters) / Self->ChildrenCount;

					real64 XCursor = Self->FinalPositionAbsolute.TopLeft.X;
					for (int i = 0; i < Self->ChildrenCount; i++) {
						rect R = {};

						R.TopLeft.X = XCursor;
						R.TopLeft.Y = Self->FinalPositionAbsolute.TopLeft.Y;

						XCursor += ChildWidth;

						R.BottomRight.X = XCursor;
						R.BottomRight.Y = Self->FinalPositionAbsolute.BottomRight.Y;

						XCursor += Self->ChildLayoutGutter;

						Self->Children[i]->FinalPositionAbsolute = R;
					}

				} else if (Self->ChildLayoutType == child_layout_type::vertical_fit) {
					real64 Height = RectHeight(Self->FinalPositionAbsolute);
					real64 SpaceForGutters = (Self->ChildrenCount - 1) * Self->ChildLayoutGutter;
					real64 ChildHeight = (Height - SpaceForGutters) / Self->ChildrenCount;

					real64 YCursor = Self->FinalPositionAbsolute.TopLeft.Y;
					for (int i = 0; i < Self->ChildrenCount; i++) {
						rect R = {};

						R.TopLeft.X = Self->FinalPositionAbsolute.TopLeft.X;
						R.TopLeft.Y = YCursor;

						YCursor += ChildHeight;

						R.BottomRight.X = Self->FinalPositionAbsolute.BottomRight.X;
						R.BottomRight.Y = YCursor;

						YCursor += Self->ChildLayoutGutter;

						Self->Children[i]->FinalPositionAbsolute = R;
					}

				} else {
					// That layout type not supported
					Assert(0);
				}
			}
		}

		for (int x = 0; x < Self->ChildrenCount; x++) {
			ResolveLayout(Self->Children[x], Self, ParentDoingLayout);
		}
	}
}