#pragma once
#ifndef panel_stack_h
#define panel_stack_h

void PanelStackPush(ui_panel* Panel, panel_stack* Stack, state_to_serialize* State);
void PanelStackPush(panel_id PanelID, panel_stack* Stack, state_to_serialize* State);
void PanelStackPop(panel_stack* Stack, state_to_serialize* State);
void PanelStackRemove(ui_panel* PanelRemoving, panel_stack* Stack, state_to_serialize *State);
void PanelStackRemove(panel_id PanelID, panel_stack* Stack, state_to_serialize *State);

#endif