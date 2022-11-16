
namespace ability {

	color GetSlotColor(bool32 Button, ability_type Type)
	{
		color OutlineColor = COLOR_RED;

		switch (Type) {
			//case ability_type::passive: 	OutlineColor = COLOR_TEAL; break;
			case ability_type::active: 		OutlineColor = COLOR_RED; break;
			case ability_type::support: 	OutlineColor = COLOR_TEAL; break;
			default: OutlineColor = COLOR_RED;
		};

		if (!Button) {
			//OutlineColor.A = 0.5;
			float G = 0.5f;
			OutlineColor = color{G * OutlineColor.R, G * OutlineColor.G, G * OutlineColor.B, 1.0f};
		}

		return OutlineColor;
	}

	bool32 RenderSupportSlot(vector2 TopLeft, vector2 BottomRight, real64 Thickness, bool32 Button)
	{
		rect Hitbox = {};
		Hitbox.TopLeft = TopLeft;
		Hitbox.BottomRight = BottomRight;

		RenderRect(Hitbox, COLOR_WHITE, 0, Globals->UIRenderer);

		real64 HoverThickness = 0;
		if (Button) {
			HoverThickness = ui::ButtonHoverOutline(Hitbox);
			if (RectContains(Hitbox, Globals->Input->TouchPosition[0]) &&
			        Globals->UIUserInput->TouchStatus[0].OnUp &&
			        Globals->UIUserInput->Touches[0].Gesture == touch_gesture::tap
			   ) {
				return true;
			}
		}

		real32 MiterX = 30.0f;
		real32 MiterY = 15.0f;
		real32 Down = 15.0f;

		render_line Line = {};
		Line.PointsCount = 6;
		Line.Points = (vector2*)ArenaAllocate(GlobalTransMem, sizeof(vector2) * Line.PointsCount);
		Line.Points[0] = vector2{TopLeft.X, TopLeft.Y + Down};
		Line.Points[1] = vector2{TopLeft.X + MiterX, TopLeft.Y - MiterY + Down};
		Line.Points[2] = vector2{BottomRight.X - MiterX, TopLeft.Y - MiterY + Down};
		Line.Points[3] = vector2{BottomRight.X, TopLeft.Y + Down};
		Line.Points[4] = vector2{BottomRight.X, BottomRight.Y};
		Line.Points[5] = vector2{TopLeft.X, BottomRight.Y};

		RenderLine(Line, (real32)(Thickness + HoverThickness),
		           GetSlotColor(Button, ability_type::support),
		           Globals->UIRenderer, true);

		return false;
	}

	/*
	bool32 RenderPassiveAbility(vector2 TopLeft, vector2 BottomRight, real64 Thickness, bool32 Button)
	{
		rect Hitbox = {};
		Hitbox.TopLeft = TopLeft;
		Hitbox.BottomRight = BottomRight;

		RenderRect(Hitbox, COLOR_WHITE, 0, Globals->UIRenderer);


		real64 HoverThickness = 0;
		if (Button) {
			HoverThickness = ui::ButtonHoverOutline(Hitbox);
			if (RectContains(Hitbox, Globals->Input->TouchPosition[0]) && Globals->Input->TouchStatus[0].OnDown) {
				return true;
			}
		}

		real32 Miter = 20.0f;

		render_line Line = {};
		Line.PointsCount = 8;
		Line.Points = (vector2*)ArenaAllocate(GlobalTransMem, sizeof(vector2) * Line.PointsCount);
		Line.Points[0] = vector2{TopLeft.X + Miter, TopLeft.Y};
		Line.Points[1] = vector2{BottomRight.X - Miter, TopLeft.Y};
		Line.Points[2] = vector2{BottomRight.X, TopLeft.Y + Miter};
		Line.Points[3] = vector2{BottomRight.X, BottomRight.Y - Miter};
		Line.Points[4] = vector2{BottomRight.X - Miter, BottomRight.Y};
		Line.Points[5] = vector2{TopLeft.X + Miter, BottomRight.Y};
		Line.Points[6] = vector2{TopLeft.X, BottomRight.Y - Miter};
		Line.Points[7] = vector2{TopLeft.X, TopLeft.Y + Miter};

		RenderLine(Line, (real32)(Thickness + HoverThickness),
		           GetAbilitySlotColor(Button, ability_type::passive),
		           Globals->UIRenderer, true);

		return false;
	}
	*/

	bool32 RenderActiveSlot(vector2 TopLeft, vector2 BottomRight, real64 Thickness, bool32 Button)
	{
		rect Hitbox = {};
		Hitbox.TopLeft = TopLeft;
		Hitbox.BottomRight = BottomRight;

		RenderRect(Hitbox, COLOR_WHITE, 0, Globals->UIRenderer);

		real32 TriHeight = 25.0f;

		real64 HoverThickness = 0;
		if (Button) {
			HoverThickness = ui::ButtonHoverOutline(Hitbox);
			if (RectContains(Hitbox, Globals->Input->TouchPosition[0]) &&
			        Globals->UIUserInput->TouchStatus[0].OnUp &&
			        Globals->UIUserInput->Touches[0].Gesture == touch_gesture::tap
			   ) {
				return true;
			}
		}

		render_line Line = {};
		Line.PointsCount = 6;
		Line.Points = (vector2*)ArenaAllocate(GlobalTransMem, sizeof(vector2) * Line.PointsCount);
		Line.Points[0] = TopLeft;
		Line.Points[1] = vector2{BottomRight.X, TopLeft.Y};
		Line.Points[2] = vector2{BottomRight.X + (TriHeight * 0.5f), TopLeft.Y + (TriHeight * 0.5f)};
		Line.Points[3] = BottomRight;
		Line.Points[4] = vector2{TopLeft.X, BottomRight.Y};
		Line.Points[5] = vector2{TopLeft.X - (TriHeight * 0.5f), TopLeft.Y + (TriHeight * 0.5f)};

		RenderLine(Line, (real32)(Thickness + HoverThickness),
		           GetSlotColor(Button, ability_type::active),
		           Globals->UIRenderer, true);

		return false;
	}

	bool32 RenderBar(ability::instance* AbilityInstance, vector2 TopLeft, vector2 BottomRight)
	{
		rect Container = rect{TopLeft, BottomRight};
		real64 Thickness = 1.0f;

		real32 GreyColor = 1.0f;
		if (AbilityInstance != GameNull) {

			real32 Width = (real32)RectWidth(Container);
			real32 Height = (real32)RectHeight(Container);

			// This is a copy from the ui::ResolveLayout. Maybe put this into a method??
			rect Description = {};
			Description.TopLeft = vector2{
				Container.TopLeft.X + (0.25f * Width),
				Container.TopLeft.Y + (0.1f * Height)
			};
			Description.BottomRight = vector2{
				Container.TopLeft.X + (1.0f * Width),
				Container.TopLeft.Y + (1.0f * Height)
			};

			if (AbilityInstance->Definition->Type != ability_type::innate) {
				switch (AbilityInstance->Definition->Type) {
					case ability_type::active: {

						// bar / outline
						if (ability::RenderActiveSlot(TopLeft, BottomRight, Thickness, true)) {
							return true;
						}

					} break;

					/*
					case ability_type::passive: {
						if (RenderPassiveAbility(TopLeft, BottomRight, 3.0f, true)) {
							return true;
						}
					} break;
					*/

					case ability_type::support: {
						if (ability::RenderSupportSlot(TopLeft, BottomRight, Thickness, true)) {
							return true;
						}
					} break;

					default: {
						// Unknown ability type
						Assert(0);
					} break;
				}

			} else {
				RenderRect(Container, color{GreyColor, GreyColor, GreyColor, 1.0f}, 0, Globals->UIRenderer);
				FontRenderString(&Globals->FontStyleH3, "innate ability",
				                 Container.TopLeft + vector2{10, 65},
				                 color{0, 0, 0, 0.7f}, 0, Globals->UIRenderer);
			}


			real64 NameXOffst = 8;

			// user / active abilities
			if (AbilityInstance->Definition->Type != ability_type::innate) {
				NameXOffst = Height;

				// icon
				RenderTextureAll(TopLeft + vector2{Height * 0.5f, Height * 0.5f},
				                 vector2{Height, Height},
				                 ability::GetSlotColor(true, AbilityInstance->Definition->Type),
				                 AbilityInstance->Definition->Icon.GLID, 0, Globals->UIRenderer);

				// quality
				string Qual = "QUALITY " + string{AbilityInstance->Quality};
				FontRenderString(&Globals->FontStyleH3, Qual,
				                 BottomRight + vector2{ -160, -15},
				                 color{0, 0, 0, 0.7f},
				                 0, Globals->UIRenderer);

				ProgressBar(TopLeft + vector2{Height, Height - 15}, 7, Width - Height,
				            (real64)AbilityInstance->Quality / MAX_QUALITY,
				            "", 0.3f, COLOR_BLACK,
				            color{0, 0, 0, 0.5f}, color{0, 0, 0, 0.85f}
				           );
			}

			// name
			FontRenderString(&Globals->FontStyleH2, AbilityInstance->Definition->Name,
			                 Container.TopLeft + vector2{NameXOffst, 40},
			                 COLOR_BLACK, 0, Globals->UIRenderer);

			// innate button
			if (AbilityInstance->Definition->Type == ability_type::innate) {
				return ui::DrawButton(Container, "", COLOR_GREY, &Globals->FontStyleButton, {});
			}

		} else {
			RenderRect(Container, color{GreyColor, GreyColor, GreyColor, 1.0f}, 0, Globals->UIRenderer);
		}


		return false;
	}

	ability::instance* Create(string ID, float Quality)
	{
		ability::instance* NewInstance = assets::AllocateAbilityInstance(ID, GlobalPermMem);

		// Quality
		{
			// set quality
			if (Quality == -1) {
				real64 Rand = RandomQuality();
				NewInstance->Quality = (int64)(Rand * MAX_QUALITY);
			} else {
				NewInstance->Quality = (int64)(Quality * MAX_QUALITY);
			}

			// Resovle the quality values
			for (int v = 0; v < NewInstance->Definition->QualityValuesCount; v++) {
				ability::quality_value* QV = &NewInstance->Definition->QualityValues[v];

				NewInstance->QualityValuesResolved[v].ID = QV->ID;
				NewInstance->QualityValuesResolved[v].Value = (int64)Lerp(QV->Range.Min, QV->Range.Max, (real64)NewInstance->Quality / MAX_QUALITY);
			}

			// Update description
			{
				string SourceDesc = NewInstance->Definition->Description;

				int64 DestI = 0;
				int64 SourceLen = StringLength(SourceDesc);
				for (int SourceI = 0; SourceI < SourceLen; SourceI++) {
					if (SourceDesc.CharArray[SourceI] == '{') {

						// Move over
						SourceI++;

						int32 IDLen = 0;
						while (SourceDesc.CharArray[SourceI + IDLen] != '}') { IDLen++; }
						string VarID = BuildString(&SourceDesc.CharArray[SourceI], IDLen);

						string ValResolved = NewInstance->GetQualityValueDisplay(VarID.CharArray);
						for (int i = 0; i < StringLength(ValResolved); i++) {
							NewInstance->DescriptionResolved.CharArray[DestI++] = ValResolved.CharArray[i];
						}

						SourceI += StringLength(VarID);
					} else {
						NewInstance->DescriptionResolved.CharArray[DestI++] = SourceDesc.CharArray[SourceI];
					}
				}
			}
		}

		return NewInstance;
	}
}