#pragma once
#ifndef ui_panel_h
#define ui_panel_h

#define UIPanelRenderParams state_to_serialize* State, window_info* WindowInfo
typedef void (*ui_panel_render_func)(UIPanelRenderParams);

#define UIPanelShowParams state_to_serialize* State, window_info* WindowInfo
typedef void (*ui_panel_show_func)(UIPanelShowParams);

struct ui_panel {
	ui_panel_render_func Render;
	ui_panel_show_func Show;

	// If true, then this renders over the previous panel and darkens it
	bool32 Overlay;
};

enum class panel_id {
	count
};

struct ui_panels;

#endif