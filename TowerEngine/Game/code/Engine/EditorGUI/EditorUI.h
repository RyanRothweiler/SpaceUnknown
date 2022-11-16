
// NOTE high means higher coordinates. low means 0
enum ui_anchor_type {
	UIAnchor_High,
	UIAnchor_Middle,
	UIAnchor_Low,
};

struct ui_anchor {
	ui_anchor_type VerticalAnchor;

	ui_anchor_type HorizontalAnchor;

	vector2 Offset;

	// window_info WindowInfo;

	bool32 VerticalStretch;
	bool32 HorizontalStretch;

	vector2 StretchOffset;

	// NOTE the streatching / anchoring overrides the scaling
	vector2 Scale;
};

enum LayoutType {
	Layout_VerticalFill,

	Layout_HorizontalFit,
};

struct live_graph {
	string Name;
	bool32 Claimed;

	// These are the graph points
	int32 Points[100];

	// This is the raw data.
	real64 RawData[100];
	real64 DataMin, DataMax;

	bool32 Showing;

	// If the graph is collecting new data
	bool32 Paused;

	real64 HistoricalLargest;
};

struct editor_window {
	rect Rect;
	real64 NextVertPos;
	real64 NextHorPos;

	vector2 PrevMouseDragPos;
	string Title;

	LayoutType LayoutState;
	real64 HorLayoutVerAdvance;

	// When the layout state is changed, for the first frame, a layout pass is done, where the number of members is counted.
	// that count is then used to draw the members on the next frame.
	bool32 LayoutPass;
	int32 HorizontalMembersCount[100];
	int32 HorizontalLayoutsCount;
	// in the drawing pass, this is incremented each element which is drawn
	int32 HorizontalElementIndex;

	bool32 Collapsed;
	// NOTE this is used to hold the state of the nested window before it's parent window collapsed the nested window
	bool32 WasOpen;

	bool32 IsDragging;
	bool32 IsResizing;

	bool32 IsNested;

	int32 HotItemID;
	int32 SliderDragging;

	bool32 ShowingScrollbar;
	real64 ScrollPos;
	real64 ScrollOffset;
	real64 TotalContentHeight;
	real64 MouseScrollWeight;
};