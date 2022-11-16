#ifndef UI_H
#define UI_H

namespace ui {
	enum class child_layout_type {
		none,

		// setting the children size to ChildLayoutSizePixels
		horizontal_align_right, horizontal_align_left,
		vertical_align_bottom, vertical_align_top,

		// --- maybe??
		// children pick what size they want to be
		//horizontal_child_percentage, vertical_child_percentage,
		// Only for child_percentage layouts. What percentage of the space does this child want
		//real64 ChildLayoutPercentag;
		// --- maybe??


		// children are sized so that they all fit
		horizontal_fit, vertical_fit,
	};

	struct container {
		rect FinalPositionAbsolute;
		rect AnchorsRelative;

		container* Children[100];
		int32 ChildrenCount;

		child_layout_type ChildLayoutType;
		real64 ChildLayoutGutter;
		vector2 ChildLayoutSizePixels;

		vector2 ScrollOffset;

		// Only for debugging visualization
		color DebugColor;
	};

	struct anim_state {
		real64 HoverAnimTime;
		real64 ClickAnimTime;
	};

}

#endif