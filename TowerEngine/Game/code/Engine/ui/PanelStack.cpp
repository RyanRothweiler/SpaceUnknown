#pragma once
#ifndef panel_stack_cpp
#define panel_stack_cpp

void PanelStackPush(ui_panel* Panel, panel_stack* Stack, state_to_serialize* State)
{
	if (Stack->NextIndex >= 1) {
		//Stack->Panels[Stack->NextIndex - 1]->Hide(State, Globals->Window);
	}

	Stack->Panels[Stack->NextIndex] = Panel;
	if (Stack->Panels[Stack->NextIndex]->Show != GameNull) { Stack->Panels[Stack->NextIndex]->Show(State, Globals->Window); }
	Stack->NextIndex++;

	Assert(Stack->NextIndex < ArrayCount(Stack->Panels));
}

void PanelStackPush(panel_id PanelID, panel_stack* Stack, state_to_serialize* State)
{
	PanelStackPush(Globals->UIPanels->Get(PanelID), Stack, State);
}

void PanelStackPop(panel_stack* Stack, state_to_serialize* State)
{
	if (Stack->NextIndex >= 1) {
		//Stack->Panels[Stack->NextIndex - 1]->Hide(State, Globals->Window);
		Stack->NextIndex--;
		//if (Stack->Panels[Stack->NextIndex - 1]->Show != GameNull) { Stack->Panels[Stack->NextIndex - 1]->Show(State, Globals->Window); }
	}
}

void PanelStackRemove(ui_panel* PanelRemoving, panel_stack* Stack, state_to_serialize *State)
{
	for (uint32 i = 0; i < Stack->NextIndex; i++) {
		ui_panel* Panel = Stack->Panels[i];

		if (Panel == PanelRemoving) {
			//Panel->Hide(State, Globals->Window);
			//void RemoveSlideArray(void* ArrayStart, uint32 ArrayCount, uint32 IndexSize, uint32 IndexRemoving)

			RemoveSlideArray((void*)&Stack->Panels[0], Stack->NextIndex, sizeof(Stack->Panels[i]), i);
			Stack->NextIndex--;
		}
	}

	if (Stack->Panels[Stack->NextIndex - 1]->Show != GameNull) { Stack->Panels[Stack->NextIndex - 1]->Show(State, Globals->Window); }
}

void PanelStackRemove(panel_id PanelID, panel_stack* Stack, state_to_serialize *State)
{
	PanelStackRemove(Globals->UIPanels->Get(PanelID), Stack, State);
}

#endif