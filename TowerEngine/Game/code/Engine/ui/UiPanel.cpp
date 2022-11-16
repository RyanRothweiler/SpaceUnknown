#pragma once
#ifndef ui_panel_cpp
#define ui_panel_cpp

#include "UiPanel.h"
#include "PanelStack.h"

const float TouchScrollSensitivity = 1.5f;

void ScrollValue(real64* Value)
{
	// scrolling
	if (Globals->UIUserInput->Touches[0].Gesture == touch_gesture::drag) {
		*Value -= Globals->UIUserInput->Touches[0].FrameDelta.Y * TouchScrollSensitivity;
	}

	// For pc editor
	*Value += Globals->UIUserInput->MouseScrollDelta * 0.6f;
}

void HideAllPanels(state_to_serialize* State)
{
}

const vector2 MarginTopLeft = vector2{0.02f, 0.06f};
const vector2 MarginBottomRight = vector2{0.98f, 0.98f};
const vector2 MarginTabOffset = vector2{0, 0.1f};

// Keep for example
/*
namespace panel_mission_details {

	real64 Middle = 0.4f;

	void Render(state_to_serialize* State, window_info* WindowInfo)
	{
		ui::container ScreenContainer = {};
		ScreenContainer.DebugColor = color{1, 0, 0, 0.2f};
		ScreenContainer.FinalPositionAbsolute.TopLeft = vector2{0, 0};
		ScreenContainer.FinalPositionAbsolute.BottomRight = vector2{(real32)WindowInfo->Width, (real32)WindowInfo->Height};

		ui::container* Margin = AddChild(&ScreenContainer);
		Margin->AnchorsRelative.BottomRight = vector2{Middle, 1.0f};

		ui::container* LaunchButton = AddChild(Margin);
		LaunchButton->AnchorsRelative.TopLeft = vector2{0, 0.9f};
		LaunchButton->AnchorsRelative.BottomRight = vector2{1.0f, 1.0f};

		ResolveLayout(&ScreenContainer, GameNull, false);

		//ui::VisualizeContainers(&ScreenContainer);

		//bg
		RenderRect(Margin->FinalPositionAbsolute, color{0, 0, 0, 0.8f}, 0, Globals->UIRenderer);

		Uppercase(&State->MissionSelected->DisplayName);
		FontRenderString(&Globals->FontStyleH1, State->MissionSelected->DisplayName,
		                 Margin->FinalPositionAbsolute.TopLeft + vector2{20, 70},
		                 COLOR_WHITE, 0, Globals->UIRenderer);

		FontRenderString(&Globals->FontStyleH2, MissionTypeNames[(int)State->MissionSelected->Type],
		                 Margin->FinalPositionAbsolute.TopLeft + vector2{20, 130},
		                 COLOR_WHITE, 0, Globals->UIRenderer);

		FontRenderString(&Globals->FontStyleP,
		                 string{State->MissionSelected->RoundsCount} + " rounds",
		                 Margin->FinalPositionAbsolute.TopLeft + vector2{20, 200},
		                 COLOR_WHITE, 0, Globals->UIRenderer);

		FontRenderString(&Globals->FontStyleP, "Enemy Level",
		                 Margin->FinalPositionAbsolute.TopLeft + vector2{20, 270},
		                 COLOR_WHITE, 0, Globals->UIRenderer);
		FontRenderString(&Globals->FontStyleP, State->MissionSelected->EnemyLevel,
		                 Margin->FinalPositionAbsolute.TopLeft + vector2{20, 320},
		                 COLOR_WHITE, 0, Globals->UIRenderer);

		if (MissionAvailable(State->MissionSelected) &&
		        ui::DrawButton(LaunchButton->FinalPositionAbsolute, "LAUNCH", COLOR_WHITE, &Globals->FontStyleButton, {})
		   ) {
			HideAllPanels(State);
			GoToMission(State, State->MissionSelected);
		}
	}
};
*/

/*
This is only needed so that panels can be allocated on heap, so that pointers don't break on dll hot-reload
*/
struct ui_panels {

	ui_panel Panels[100];

	void Setup()
	{
		Assert(ArrayCount(Panels) > (int)panel_id::count);

		/*
		Panels[(int)panel_id::inventory].Render = &panel_inventory::Render;
		Panels[(int)panel_id::inventory].Show = &panel_inventory::Show;
		*/

	}

	ui_panel* Get(panel_id ID)
	{
		return (&Panels[(int)ID]);
	}
};

#include "PanelStack.cpp"

#endif