// dear imgui, v1.73
// (headers)

// See imgui.cpp file for documentation.
// Call and read ImGui::ShowDemoWindow() in imgui_demo.cpp for demo code.
// Newcomers, read 'Programmer guide' in imgui.cpp for notes on how to setup Dear ImGui in your codebase.
// Get latest version at https://github.com/ocornut/imgui

/*

Index of this file:
// Header mess
// Forward declarations and basic types
// ImGui API (Dear ImGui end-user API)
// Flags & Enumerations
// Memory allocations macros
// ImVector<>
// ImGuiStyle
// ImGuiIO
// Misc data structures (ImGuiInputTextCallbackData, ImGuiSizeCallbackData, ImGuiPayload)
// Obsolete functions
// Helpers (ImGuiOnceUponAFrame, ImGuiTextFilter, ImGuiTextBuffer, ImGuiStorage, ImGuiListClipper, ImColor)
// Draw List API (ImDrawCallback, ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListSplitter, ImDrawListFlags, ImDrawList, ImDrawData)
// Font API (ImFontConfig, ImFontGlyph, ImFontGlyphRangesBuilder, ImFontAtlasFlags, ImFontAtlas, ImFont)

*/

#pragma once

// Configuration file with compile-time options (edit imconfig.h or define IMGUI_USER_CONFIG to your own filename)
#ifdef IMGUI_USER_CONFIG
	#include IMGUI_USER_CONFIG
#endif
#if !defined(IMGUI_DISABLE_INCLUDE_IMCONFIG_H) || defined(IMGUI_INCLUDE_IMCONFIG_H)
	#include "imconfig.h"
#endif

//-----------------------------------------------------------------------------
// Header mess
//-----------------------------------------------------------------------------

#include <float.h>                  // FLT_MAX
#include <stdarg.h>                 // va_list
#include <stddef.h>                 // ptrdiff_t, NULL
#include <string.h>                 // memset, memmove, memcpy, strlen, strchr, strcpy, strcmp

// Version
// (Integer encoded as XYYZZ for use in #if preprocessor conditionals. Work in progress versions typically starts at XYY99 then bounce up to XYY00, XYY01 etc. when release tagging happens)
#define IMGUI_VERSION               "1.73"
#define IMGUI_VERSION_NUM           17300
#define IMGUI_CHECKVERSION()        ImGui::DebugCheckVersionAndDataLayout(IMGUI_VERSION, sizeof(ImGuiIO), sizeof(ImGuiStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert), sizeof(ImDrawIdx))

// Define attributes of all API symbols declarations (e.g. for DLL under Windows)
// IMGUI_API is used for core imgui functions, IMGUI_IMPL_API is used for the default bindings files (imgui_impl_xxx.h)
// Using dear imgui via a shared library is not recommended, because of function call overhead and because we don't guarantee backward nor forward ABI compatibility.
#ifndef IMGUI_API
	#define IMGUI_API
#endif
#ifndef IMGUI_IMPL_API
	#define IMGUI_IMPL_API              IMGUI_API
#endif

// Helper Macros
#ifndef IM_ASSERT
	#include <assert.h>
	#define IM_ASSERT(_EXPR)            assert(_EXPR)                               // You can override the default assert handler by editing imconfig.h
#endif
#if defined(__clang__) || defined(__GNUC__)
	#define IM_FMTARGS(FMT)             __attribute__((format(printf, FMT, FMT+1))) // Apply printf-style warnings to user functions.
	#define IM_FMTLIST(FMT)             __attribute__((format(printf, FMT, 0)))
#else
	#define IM_FMTARGS(FMT)
	#define IM_FMTLIST(FMT)
#endif
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))         // Size of a static C-style array. Don't use on pointers!
#define IM_UNUSED(_VAR)             ((void)_VAR)                                // Used to silence "unused variable warnings". Often useful as asserts may be stripped out from final builds.
#if (__cplusplus >= 201100)
	#define IM_OFFSETOF(_TYPE,_MEMBER)  offsetof(_TYPE, _MEMBER)                    // Offset of _MEMBER within _TYPE. Standardized as offsetof() in C++11
#else
	#define IM_OFFSETOF(_TYPE,_MEMBER)  ((size_t)&(((_TYPE*)0)->_MEMBER))           // Offset of _MEMBER within _TYPE. Old style macro.
#endif

// Warnings
#if defined(__clang__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wold-style-cast"
	#if __has_warning("-Wzero-as-null-pointer-constant")
		#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
	#endif
#elif defined(__GNUC__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpragmas"                  // warning: unknown option after '#pragma GCC diagnostic' kind
	#pragma GCC diagnostic ignored "-Wclass-memaccess"          // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

//-----------------------------------------------------------------------------
// Forward declarations and basic types
//-----------------------------------------------------------------------------

struct ImDrawChannel;               // Temporary storage to output draw commands out of order, used by ImDrawListSplitter and ImDrawList::ChannelsSplit()
struct ImDrawCmd;                   // A single draw command within a parent ImDrawList (generally maps to 1 GPU draw call, unless it is a callback)
struct ImDrawData;                  // All draw command lists required to render the frame + pos/size coordinates to use for the projection matrix.
struct ImDrawList;                  // A single draw command list (generally one per window, conceptually you may see this as a dynamic "mesh" builder)
struct ImDrawListSharedData;        // Data shared among multiple draw lists (typically owned by parent ImGui context, but you may create one yourself)
struct ImDrawListSplitter;          // Helper to split a draw list into different layers which can be drawn into out of order, then flattened back.
struct ImDrawVert;                  // A single vertex (pos + uv + col = 20 bytes by default. Override layout with IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT)
struct ImFont;                      // Runtime data for a single font within a parent ImFontAtlas
struct ImFontAtlas;                 // Runtime data for multiple fonts, bake multiple fonts into a single texture, TTF/OTF font loader
struct ImFontConfig;                // Configuration data when adding a font or merging fonts
struct ImFontGlyph;                 // A single font glyph (code point + coordinates within in ImFontAtlas + offset)
struct ImFontGlyphRangesBuilder;    // Helper to build glyph ranges from text/string data
struct ImColor;                     // Helper functions to create a color that can be converted to either u32 or float4 (*OBSOLETE* please avoid using)
struct ImGuiContext;                // Dear ImGui context (opaque structure, unless including imgui_internal.h)
struct ImGuiIO;                     // Main configuration and I/O between your application and ImGui
struct ImGuiInputTextCallbackData;  // Shared state of InputText() when using custom ImGuiInputTextCallback (rare/advanced use)
struct ImGuiListClipper;            // Helper to manually clip large list of items
struct ImGuiOnceUponAFrame;         // Helper for running a block of code not more than once a frame, used by IMGUI_ONCE_UPON_A_FRAME macro
struct ImGuiPayload;                // User data payload for drag and drop operations
struct ImGuiSizeCallbackData;       // Callback data when using SetNextWindowSizeConstraints() (rare/advanced use)
struct ImGuiStorage;                // Helper for key->value storage
struct ImGuiStyle;                  // Runtime data for styling/colors
struct ImGuiTextBuffer;             // Helper to hold and append into a text buffer (~string builder)
struct ImGuiTextFilter;             // Helper to parse and apply text filters (e.g. "aaaaa[,bbbb][,ccccc]")

// Typedefs and Enums/Flags (declared as int for compatibility with old C++, to allow using as flags and to not pollute the top of this file)
// Use your programming IDE "Go to definition" facility on the names of the center columns to find the actual flags/enum lists.
#ifndef ImTextureID
	typedef void* ImTextureID;          // User data to identify a texture (this is whatever to you want it to be! read the FAQ about ImTextureID in imgui.cpp)
#endif
typedef unsigned int ImGuiID;       // Unique ID used by widgets (typically hashed from a stack of string)
typedef unsigned short ImWchar;     // A single U16 character for keyboard input/display. We encode them as multi bytes UTF-8 when used in strings.
typedef int ImGuiCol;               // -> enum ImGuiCol_             // Enum: A color identifier for styling
typedef int ImGuiCond;              // -> enum ImGuiCond_            // Enum: A condition for many Set*() functions
typedef int ImGuiDataType;          // -> enum ImGuiDataType_        // Enum: A primary data type
typedef int ImGuiDir;               // -> enum ImGuiDir_             // Enum: A cardinal direction
typedef int ImGuiKey;               // -> enum ImGuiKey_             // Enum: A key identifier (ImGui-side enum)
typedef int ImGuiNavInput;          // -> enum ImGuiNavInput_        // Enum: An input identifier for navigation
typedef int ImGuiMouseCursor;       // -> enum ImGuiMouseCursor_     // Enum: A mouse cursor identifier
typedef int ImGuiStyleVar;          // -> enum ImGuiStyleVar_        // Enum: A variable identifier for styling
typedef int ImDrawCornerFlags;      // -> enum ImDrawCornerFlags_    // Flags: for ImDrawList::AddRect(), AddRectFilled() etc.
typedef int ImDrawListFlags;        // -> enum ImDrawListFlags_      // Flags: for ImDrawList
typedef int ImFontAtlasFlags;       // -> enum ImFontAtlasFlags_     // Flags: for ImFontAtlas
typedef int ImGuiBackendFlags;      // -> enum ImGuiBackendFlags_    // Flags: for io.BackendFlags
typedef int ImGuiColorEditFlags;    // -> enum ImGuiColorEditFlags_  // Flags: for ColorEdit4(), ColorPicker4() etc.
typedef int ImGuiConfigFlags;       // -> enum ImGuiConfigFlags_     // Flags: for io.ConfigFlags
typedef int ImGuiComboFlags;        // -> enum ImGuiComboFlags_      // Flags: for BeginCombo()
typedef int ImGuiDragDropFlags;     // -> enum ImGuiDragDropFlags_   // Flags: for BeginDragDropSource(), AcceptDragDropPayload()
typedef int ImGuiFocusedFlags;      // -> enum ImGuiFocusedFlags_    // Flags: for IsWindowFocused()
typedef int ImGuiHoveredFlags;      // -> enum ImGuiHoveredFlags_    // Flags: for IsItemHovered(), IsWindowHovered() etc.
typedef int ImGuiInputTextFlags;    // -> enum ImGuiInputTextFlags_  // Flags: for InputText(), InputTextMultiline()
typedef int ImGuiSelectableFlags;   // -> enum ImGuiSelectableFlags_ // Flags: for Selectable()
typedef int ImGuiTabBarFlags;       // -> enum ImGuiTabBarFlags_     // Flags: for BeginTabBar()
typedef int ImGuiTabItemFlags;      // -> enum ImGuiTabItemFlags_    // Flags: for BeginTabItem()
typedef int ImGuiTreeNodeFlags;     // -> enum ImGuiTreeNodeFlags_   // Flags: for TreeNode(), TreeNodeEx(), CollapsingHeader()
typedef int ImGuiWindowFlags;       // -> enum ImGuiWindowFlags_     // Flags: for Begin(), BeginChild()
typedef int (*ImGuiInputTextCallback)(ImGuiInputTextCallbackData *data);
typedef void (*ImGuiSizeCallback)(ImGuiSizeCallbackData* data);

// Scalar data types
typedef signed char         ImS8;   // 8-bit signed integer == char
typedef unsigned char       ImU8;   // 8-bit unsigned integer
typedef signed short        ImS16;  // 16-bit signed integer
typedef unsigned short      ImU16;  // 16-bit unsigned integer
typedef signed int          ImS32;  // 32-bit signed integer == int
typedef unsigned int        ImU32;  // 32-bit unsigned integer (often used to store packed colors)
#if defined(_MSC_VER) && !defined(__clang__)
	typedef signed   __int64    ImS64;  // 64-bit signed integer (pre and post C++11 with Visual Studio)
	typedef unsigned __int64    ImU64;  // 64-bit unsigned integer (pre and post C++11 with Visual Studio)
#elif (defined(__clang__) || defined(__GNUC__)) && (__cplusplus < 201100)
	#include <stdint.h>
	typedef int64_t             ImS64;  // 64-bit signed integer (pre C++11)
	typedef uint64_t            ImU64;  // 64-bit unsigned integer (pre C++11)
#else
	typedef signed   long long  ImS64;  // 64-bit signed integer (post C++11)
	typedef unsigned long long  ImU64;  // 64-bit unsigned integer (post C++11)
#endif

// 2D vector (often used to store positions, sizes, etc.)
struct ImVec2 {
	float     x, y;
	ImVec2()  { x = y = 0.0f; }
	ImVec2(float _x, float _y) { x = _x; y = _y; }
	float  operator[] (size_t idx) const { IM_ASSERT(idx <= 1); return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
	float& operator[] (size_t idx)       { IM_ASSERT(idx <= 1); return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
#ifdef IM_VEC2_CLASS_EXTRA
	IM_VEC2_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec2.
#endif
};

// 4D vector (often used to store floating-point colors)
struct ImVec4 {
	float     x, y, z, w;
	ImVec4()  { x = y = z = w = 0.0f; }
	ImVec4(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
#ifdef IM_VEC4_CLASS_EXTRA
	IM_VEC4_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec4.
#endif
};

//-----------------------------------------------------------------------------
// ImGui: Dear ImGui end-user API
// (Inside a namespace so you can add extra functions in your own separate file. Please don't modify imgui.cpp/.h!)
//-----------------------------------------------------------------------------

namespace ImGui {
	// Context creation and access
	// Each context create its own ImFontAtlas by default. You may instance one yourself and pass it to CreateContext() to share a font atlas between imgui contexts.
	// All those functions are not reliant on the current context.
	IMGUI_API ImGuiContext* CreateContext(ImFontAtlas* shared_font_atlas = NULL);
	IMGUI_API void          DestroyContext(ImGuiContext* ctx = NULL);   // NULL = destroy current context
	IMGUI_API ImGuiContext* GetCurrentContext();
	IMGUI_API void          SetCurrentContext(ImGuiContext* ctx);
	IMGUI_API bool          DebugCheckVersionAndDataLayout(const char* version_str, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_drawvert, size_t sz_drawidx);

	// Main
	IMGUI_API ImGuiIO&      GetIO();                                    // access the IO structure (mouse/keyboard/gamepad inputs, time, various configuration options/flags)
	IMGUI_API ImGuiStyle&   GetStyle();                                 // access the Style structure (colors, sizes). Always use PushStyleCol(), PushStyleVar() to modify style mid-frame.
	IMGUI_API void          NewFrame();                                 // start a new Dear ImGui frame, you can submit any command from this point until Render()/EndFrame().
	IMGUI_API void          EndFrame();                                 // ends the Dear ImGui frame. automatically called by Render(), you likely don't need to call that yourself directly. If you don't need to render data (skipping rendering) you may call EndFrame() but you'll have wasted CPU already! If you don't need to render, better to not create any imgui windows and not call NewFrame() at all!
	IMGUI_API void          Render();                                   // ends the Dear ImGui frame, finalize the draw data. You can get call GetDrawData() to obtain it and run your rendering function. (Obsolete: this used to call io.RenderDrawListsFn(). Nowadays, we allow and prefer calling your render function yourself.)
	IMGUI_API ImDrawData*   GetDrawData();                              // valid after Render() and until the next call to NewFrame(). this is what you have to render.

	// Demo, Debug, Information
	IMGUI_API void          ShowDemoWindow(bool* p_open = NULL);        // create Demo window (previously called ShowTestWindow). demonstrate most ImGui features. call this to learn about the library! try to make it always available in your application!
	IMGUI_API void          ShowAboutWindow(bool* p_open = NULL);       // create About window. display Dear ImGui version, credits and build/system information.
	IMGUI_API void          ShowMetricsWindow(bool* p_open = NULL);     // create Metrics/Debug window. display Dear ImGui internals: draw commands (with individual draw calls and vertices), window list, basic internal state, etc.
	IMGUI_API void          ShowStyleEditor(ImGuiStyle* ref = NULL);    // add style editor block (not a window). you can pass in a reference ImGuiStyle structure to compare to, revert to and save to (else it uses the default style)
	IMGUI_API bool          ShowStyleSelector(const char* label);       // add style selector block (not a window), essentially a combo listing the default styles.
	IMGUI_API void          ShowFontSelector(const char* label);        // add font selector block (not a window), essentially a combo listing the loaded fonts.
	IMGUI_API void          ShowUserGuide();                            // add basic help/info block (not a window): how to manipulate ImGui as a end-user (mouse/keyboard controls).
	IMGUI_API const char*   GetVersion();                               // get the compiled version string e.g. "1.23" (essentially the compiled value for IMGUI_VERSION)

	// Styles
	IMGUI_API void          StyleColorsDark(ImGuiStyle* dst = NULL);    // new, recommended style (default)
	IMGUI_API void          StyleColorsClassic(ImGuiStyle* dst = NULL); // classic imgui style
	IMGUI_API void          StyleColorsLight(ImGuiStyle* dst = NULL);   // best used with borders and a custom, thicker font

	// Windows
	// - Begin() = push window to the stack and start appending to it. End() = pop window from the stack.
	// - You may append multiple times to the same window during the same frame.
	// - Passing 'bool* p_open != NULL' shows a window-closing widget in the upper-right corner of the window,
	//   which clicking will set the boolean to false when clicked.
	// - Begin() return false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting
	//   anything to the window. Always call a matching End() for each Begin() call, regardless of its return value!
	//   [this is due to legacy reason and is inconsistent with most other functions such as BeginMenu/EndMenu, BeginPopup/EndPopup, etc.
	//    where the EndXXX call should only be called if the corresponding BeginXXX function returned true.]
	// - Note that the bottom of window stack always contains a window called "Debug".
	IMGUI_API bool          Begin(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0);
	IMGUI_API void          End();

	// Child Windows
	// - Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window. Child windows can embed their own child.
	// - For each independent axis of 'size': ==0.0f: use remaining host window size / >0.0f: fixed size / <0.0f: use remaining window size minus abs(size) / Each axis can use a different mode, e.g. ImVec2(0,400).
	// - BeginChild() returns false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting anything to the window.
	//   Always call a matching EndChild() for each BeginChild() call, regardless of its return value [this is due to legacy reason and is inconsistent with most other functions such as BeginMenu/EndMenu, BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function returned true.]
	IMGUI_API bool          BeginChild(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0);
	IMGUI_API bool          BeginChild(ImGuiID id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0);
	IMGUI_API void          EndChild();

	// Windows Utilities
	// - "current window" = the window we are appending into while inside a Begin()/End() block. "next window" = next window we will Begin() into.
	IMGUI_API bool          IsWindowAppearing();
	IMGUI_API bool          IsWindowCollapsed();
	IMGUI_API bool          IsWindowFocused(ImGuiFocusedFlags flags = 0); // is current window focused? or its root/child, depending on flags. see flags for options.
	IMGUI_API bool          IsWindowHovered(ImGuiHoveredFlags flags = 0); // is current window hovered (and typically: not blocked by a popup/modal)? see flags for options. NB: If you are trying to check whether your mouse should be dispatched to imgui or to your app, you should use the 'io.WantCaptureMouse' boolean for that! Please read the FAQ!
	IMGUI_API ImDrawList*   GetWindowDrawList();                        // get draw list associated to the current window, to append your own drawing primitives
	IMGUI_API ImVec2        GetWindowPos();                             // get current window position in screen space (useful if you want to do your own drawing via the DrawList API)
	IMGUI_API ImVec2        GetWindowSize();                            // get current window size
	IMGUI_API float         GetWindowWidth();                           // get current window width (shortcut for GetWindowSize().x)
	IMGUI_API float         GetWindowHeight();                          // get current window height (shortcut for GetWindowSize().y)

	// Prefer using SetNextXXX functions (before Begin) rather that SetXXX functions (after Begin).
	IMGUI_API void          SetNextWindowPos(const ImVec2& pos, ImGuiCond cond = 0, const ImVec2& pivot = ImVec2(0, 0)); // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
	IMGUI_API void          SetNextWindowSize(const ImVec2& size, ImGuiCond cond = 0);                  // set next window size. set axis to 0.0f to force an auto-fit on this axis. call before Begin()
	IMGUI_API void          SetNextWindowSizeConstraints(const ImVec2& size_min, const ImVec2& size_max, ImGuiSizeCallback custom_callback = NULL, void* custom_callback_data = NULL); // set next window size limits. use -1,-1 on either X/Y axis to preserve the current size. Sizes will be rounded down. Use callback to apply non-trivial programmatic constraints.
	IMGUI_API void          SetNextWindowContentSize(const ImVec2& size);                               // set next window content size (~ scrollable client area, which enforce the range of scrollbars). Not including window decorations (title bar, menu bar, etc.) nor WindowPadding. set an axis to 0.0f to leave it automatic. call before Begin()
	IMGUI_API void          SetNextWindowCollapsed(bool collapsed, ImGuiCond cond = 0);                 // set next window collapsed state. call before Begin()
	IMGUI_API void          SetNextWindowFocus();                                                       // set next window to be focused / top-most. call before Begin()
	IMGUI_API void          SetNextWindowBgAlpha(float alpha);                                          // set next window background color alpha. helper to easily modify ImGuiCol_WindowBg/ChildBg/PopupBg. you may also use ImGuiWindowFlags_NoBackground.
	IMGUI_API void          SetWindowPos(const ImVec2& pos, ImGuiCond cond = 0);                        // (not recommended) set current window position - call within Begin()/End(). prefer using SetNextWindowPos(), as this may incur tearing and side-effects.
	IMGUI_API void          SetWindowSize(const ImVec2& size, ImGuiCond cond = 0);                      // (not recommended) set current window size - call within Begin()/End(). set to ImVec2(0,0) to force an auto-fit. prefer using SetNextWindowSize(), as this may incur tearing and minor side-effects.
	IMGUI_API void          SetWindowCollapsed(bool collapsed, ImGuiCond cond = 0);                     // (not recommended) set current window collapsed state. prefer using SetNextWindowCollapsed().
	IMGUI_API void          SetWindowFocus();                                                           // (not recommended) set current window to be focused / top-most. prefer using SetNextWindowFocus().
	IMGUI_API void          SetWindowFontScale(float scale);                                            // set font scale. Adjust IO.FontGlobalScale if you want to scale all windows. This is an old API! For correct scaling, prefer to reload font + rebuild ImFontAtlas + call style.ScaleAllSizes().
	IMGUI_API void          SetWindowPos(const char* name, const ImVec2& pos, ImGuiCond cond = 0);      // set named window position.
	IMGUI_API void          SetWindowSize(const char* name, const ImVec2& size, ImGuiCond cond = 0);    // set named window size. set axis to 0.0f to force an auto-fit on this axis.
	IMGUI_API void          SetWindowCollapsed(const char* name, bool collapsed, ImGuiCond cond = 0);   // set named window collapsed state
	IMGUI_API void          SetWindowFocus(const char* name);                                           // set named window to be focused / top-most. use NULL to remove focus.

	// Content region
	// - Those functions are bound to be redesigned soon (they are confusing, incomplete and return values in local window coordinates which increases confusion)
	IMGUI_API ImVec2        GetContentRegionMax();                                          // current content boundaries (typically window boundaries including scrolling, or current column boundaries), in windows coordinates
	IMGUI_API ImVec2        GetContentRegionAvail();                                        // == GetContentRegionMax() - GetCursorPos()
	IMGUI_API ImVec2        GetWindowContentRegionMin();                                    // content boundaries min (roughly (0,0)-Scroll), in window coordinates
	IMGUI_API ImVec2        GetWindowContentRegionMax();                                    // content boundaries max (roughly (0,0)+Size-Scroll) where Size can be override with SetNextWindowContentSize(), in window coordinates
	IMGUI_API float         GetWindowContentRegionWidth();                                  //

	// Windows Scrolling
	IMGUI_API float         GetScrollX();                                                   // get scrolling amount [0..GetScrollMaxX()]
	IMGUI_API float         GetScrollY();                                                   // get scrolling amount [0..GetScrollMaxY()]
	IMGUI_API float         GetScrollMaxX();                                                // get maximum scrolling amount ~~ ContentSize.X - WindowSize.X
	IMGUI_API float         GetScrollMaxY();                                                // get maximum scrolling amount ~~ ContentSize.Y - WindowSize.Y
	IMGUI_API void          SetScrollX(float scroll_x);                                     // set scrolling amount [0..GetScrollMaxX()]
	IMGUI_API void          SetScrollY(float scroll_y);                                     // set scrolling amount [0..GetScrollMaxY()]
	IMGUI_API void          SetScrollHereX(float center_x_ratio = 0.5f);                    // adjust scrolling amount to make current cursor position visible. center_x_ratio=0.0: left, 0.5: center, 1.0: right. When using to make a "default/current item" visible, consider using SetItemDefaultFocus() instead.
	IMGUI_API void          SetScrollHereY(float center_y_ratio = 0.5f);                    // adjust scrolling amount to make current cursor position visible. center_y_ratio=0.0: top, 0.5: center, 1.0: bottom. When using to make a "default/current item" visible, consider using SetItemDefaultFocus() instead.
	IMGUI_API void          SetScrollFromPosX(float local_x, float center_x_ratio = 0.5f);  // adjust scrolling amount to make given position visible. Generally GetCursorStartPos() + offset to compute a valid position.
	IMGUI_API void          SetScrollFromPosY(float local_y, float center_y_ratio = 0.5f);  // adjust scrolling amount to make given position visible. Generally GetCursorStartPos() + offset to compute a valid position.

	// Parameters stacks (shared)
	IMGUI_API void          PushFont(ImFont* font);                                         // use NULL as a shortcut to push default font
	IMGUI_API void          PopFont();
	IMGUI_API void          PushStyleColor(ImGuiCol idx, ImU32 col);
	IMGUI_API void          PushStyleColor(ImGuiCol idx, const ImVec4& col);
	IMGUI_API void          PopStyleColor(int count = 1);
	IMGUI_API void          PushStyleVar(ImGuiStyleVar idx, float val);
	IMGUI_API void          PushStyleVar(ImGuiStyleVar idx, const ImVec2& val);
	IMGUI_API void          PopStyleVar(int count = 1);
	IMGUI_API const ImVec4& GetStyleColorVec4(ImGuiCol idx);                                // retrieve style color as stored in ImGuiStyle structure. use to feed back into PushStyleColor(), otherwise use GetColorU32() to get style color with style alpha baked in.
	IMGUI_API ImFont*       GetFont();                                                      // get current font
	IMGUI_API float         GetFontSize();                                                  // get current font size (= height in pixels) of current font with current scale applied
	IMGUI_API ImVec2        GetFontTexUvWhitePixel();                                       // get UV coordinate for a while pixel, useful to draw custom shapes via the ImDrawList API
	IMGUI_API ImU32         GetColorU32(ImGuiCol idx, float alpha_mul = 1.0f);              // retrieve given style color with style alpha applied and optional extra alpha multiplier
	IMGUI_API ImU32         GetColorU32(const ImVec4& col);                                 // retrieve given color with style alpha applied
	IMGUI_API ImU32         GetColorU32(ImU32 col);                                         // retrieve given color with style alpha applied

	// Parameters stacks (current window)
	IMGUI_API void          PushItemWidth(float item_width);                                // set width of items for common large "item+label" widgets. >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -1.0f always align width to the right side). 0.0f = default to ~2/3 of windows width,
	IMGUI_API void          PopItemWidth();
	IMGUI_API void          SetNextItemWidth(float item_width);                             // set width of the _next_ common large "item+label" widget. >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -1.0f always align width to the right side)
	IMGUI_API float         CalcItemWidth();                                                // width of item given pushed settings and current cursor position. NOT necessarily the width of last item unlike most 'Item' functions.
	IMGUI_API void          PushTextWrapPos(float wrap_local_pos_x = 0.0f);                 // word-wrapping for Text*() commands. < 0.0f: no wrapping; 0.0f: wrap to end of window (or column); > 0.0f: wrap at 'wrap_pos_x' position in window local space
	IMGUI_API void          PopTextWrapPos();
	IMGUI_API void          PushAllowKeyboardFocus(bool allow_keyboard_focus);              // allow focusing using TAB/Shift-TAB, enabled by default but you can disable it for certain widgets
	IMGUI_API void          PopAllowKeyboardFocus();
	IMGUI_API void          PushButtonRepeat(bool repeat);                                  // in 'repeat' mode, Button*() functions return repeated true in a typematic manner (using io.KeyRepeatDelay/io.KeyRepeatRate setting). Note that you can call IsItemActive() after any Button() to tell if the button is held in the current frame.
	IMGUI_API void          PopButtonRepeat();

	// Cursor / Layout
	// - By "cursor" we mean the current output position.
	// - The typical widget behavior is to output themselves at the current cursor position, then move the cursor one line down.
	IMGUI_API void          Separator();                                                    // separator, generally horizontal. inside a menu bar or in horizontal layout mode, this becomes a vertical separator.
	IMGUI_API void          SameLine(float offset_from_start_x = 0.0f, float spacing = -1.0f); // call between widgets or groups to layout them horizontally. X position given in window coordinates.
	IMGUI_API void          NewLine();                                                      // undo a SameLine() or force a new line when in an horizontal-layout context.
	IMGUI_API void          Spacing();                                                      // add vertical spacing.
	IMGUI_API void          Dummy(const ImVec2& size);                                      // add a dummy item of given size. unlike InvisibleButton(), Dummy() won't take the mouse click or be navigable into.
	IMGUI_API void          Indent(float indent_w = 0.0f);                                  // move content position toward the right, by style.IndentSpacing or indent_w if != 0
	IMGUI_API void          Unindent(float indent_w = 0.0f);                                // move content position back to the left, by style.IndentSpacing or indent_w if != 0
	IMGUI_API void          BeginGroup();                                                   // lock horizontal starting position
	IMGUI_API void          EndGroup();                                                     // unlock horizontal starting position + capture the whole group bounding box into one "item" (so you can use IsItemHovered() or layout primitives such as SameLine() on whole group, etc.)
	IMGUI_API ImVec2        GetCursorPos();                                                 // cursor position in window coordinates (relative to window position)
	IMGUI_API float         GetCursorPosX();                                                //   (some functions are using window-relative coordinates, such as: GetCursorPos, GetCursorStartPos, GetContentRegionMax, GetWindowContentRegion* etc.
	IMGUI_API float         GetCursorPosY();                                                //    other functions such as GetCursorScreenPos or everything in ImDrawList::
	IMGUI_API void          SetCursorPos(const ImVec2& local_pos);                          //    are using the main, absolute coordinate system.
	IMGUI_API void          SetCursorPosX(float local_x);                                   //    GetWindowPos() + GetCursorPos() == GetCursorScreenPos() etc.)
	IMGUI_API void          SetCursorPosY(float local_y);                                   //
	IMGUI_API ImVec2        GetCursorStartPos();                                            // initial cursor position in window coordinates
	IMGUI_API ImVec2        GetCursorScreenPos();                                           // cursor position in absolute screen coordinates [0..io.DisplaySize] (useful to work with ImDrawList API)
	IMGUI_API void          SetCursorScreenPos(const ImVec2& pos);                          // cursor position in absolute screen coordinates [0..io.DisplaySize]
	IMGUI_API void          AlignTextToFramePadding();                                      // vertically align upcoming text baseline to FramePadding.y so that it will align properly to regularly framed items (call if you have text on a line before a framed item)
	IMGUI_API float         GetTextLineHeight();                                            // ~ FontSize
	IMGUI_API float         GetTextLineHeightWithSpacing();                                 // ~ FontSize + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of text)
	IMGUI_API float         GetFrameHeight();                                               // ~ FontSize + style.FramePadding.y * 2
	IMGUI_API float         GetFrameHeightWithSpacing();                                    // ~ FontSize + style.FramePadding.y * 2 + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of framed widgets)

	// ID stack/scopes
	// - Read the FAQ for more details about how ID are handled in dear imgui. If you are creating widgets in a loop you most
	//   likely want to push a unique identifier (e.g. object pointer, loop index) to uniquely differentiate them.
	// - The resulting ID are hashes of the entire stack.
	// - You can also use the "Label##foobar" syntax within widget label to distinguish them from each others.
	// - In this header file we use the "label"/"name" terminology to denote a string that will be displayed and used as an ID,
	//   whereas "str_id" denote a string that is only used as an ID and not normally displayed.
	IMGUI_API void          PushID(const char* str_id);                                     // push string into the ID stack (will hash string).
	IMGUI_API void          PushID(const char* str_id_begin, const char* str_id_end);       // push string into the ID stack (will hash string).
	IMGUI_API void          PushID(const void* ptr_id);                                     // push pointer into the ID stack (will hash pointer).
	IMGUI_API void          PushID(int int_id);                                             // push integer into the ID stack (will hash integer).
	IMGUI_API void          PopID();                                                        // pop from the ID stack.
	IMGUI_API ImGuiID       GetID(const char* str_id);                                      // calculate unique ID (hash of whole ID stack + given parameter). e.g. if you want to query into ImGuiStorage yourself
	IMGUI_API ImGuiID       GetID(const char* str_id_begin, const char* str_id_end);
	IMGUI_API ImGuiID       GetID(const void* ptr_id);

	// Widgets: Text
	IMGUI_API void          TextUnformatted(const char* text, const char* text_end = NULL);                // raw text without formatting. Roughly equivalent to Text("%s", text) but: A) doesn't require null terminated string if 'text_end' is specified, B) it's faster, no memory copy is done, no buffer size limits, recommended for long chunks of text.
	IMGUI_API void          Text(const char* fmt, ...)                                      IM_FMTARGS(1); // simple formatted text
	IMGUI_API void          TextV(const char* fmt, va_list args)                            IM_FMTLIST(1);
	IMGUI_API void          TextColored(const ImVec4& col, const char* fmt, ...)            IM_FMTARGS(2); // shortcut for PushStyleColor(ImGuiCol_Text, col); Text(fmt, ...); PopStyleColor();
	IMGUI_API void          TextColoredV(const ImVec4& col, const char* fmt, va_list args)  IM_FMTLIST(2);
	IMGUI_API void          TextDisabled(const char* fmt, ...)                              IM_FMTARGS(1); // shortcut for PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]); Text(fmt, ...); PopStyleColor();
	IMGUI_API void          TextDisabledV(const char* fmt, va_list args)                    IM_FMTLIST(1);
	IMGUI_API void          TextWrapped(const char* fmt, ...)                               IM_FMTARGS(1); // shortcut for PushTextWrapPos(0.0f); Text(fmt, ...); PopTextWrapPos();. Note that this won't work on an auto-resizing window if there's no other widgets to extend the window width, yoy may need to set a size using SetNextWindowSize().
	IMGUI_API void          TextWrappedV(const char* fmt, va_list args)                     IM_FMTLIST(1);
	IMGUI_API void          LabelText(const char* label, const char* fmt, ...)              IM_FMTARGS(2); // display text+label aligned the same way as value+label widgets
	IMGUI_API void          LabelTextV(const char* label, const char* fmt, va_list args)    IM_FMTLIST(2);
	IMGUI_API void          BulletText(const char* fmt, ...)                                IM_FMTARGS(1); // shortcut for Bullet()+Text()
	IMGUI_API void          BulletTextV(const char* fmt, va_list args)                      IM_FMTLIST(1);

	// Widgets: Main
	// - Most widgets return true when the value has been changed or when pressed/selected
	IMGUI_API bool          Button(const char* label, const ImVec2& size = ImVec2(0, 0));   // button
	IMGUI_API bool          SmallButton(const char* label);                                 // button with FramePadding=(0,0) to easily embed within text
	IMGUI_API bool          InvisibleButton(const char* str_id, const ImVec2& size);        // button behavior without the visuals, frequently useful to build custom behaviors using the public api (along with IsItemActive, IsItemHovered, etc.)
	IMGUI_API bool          ArrowButton(const char* str_id, ImGuiDir dir);                  // square button with an arrow shape
	IMGUI_API void          Image(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
	IMGUI_API bool          ImageButton(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0),  const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1)); // <0 frame_padding uses default frame padding settings. 0 for no padding
	IMGUI_API bool          Checkbox(const char* label, bool* v);
	IMGUI_API bool          CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value);
	IMGUI_API bool          RadioButton(const char* label, bool active);                    // use with e.g. if (RadioButton("one", my_value==1)) { my_value = 1; }
	IMGUI_API bool          RadioButton(const char* label, int* v, int v_button);           // shortcut to handle the above pattern when value is an integer
	IMGUI_API void          ProgressBar(float fraction, const ImVec2& size_arg = ImVec2(-1, 0), const char* overlay = NULL);
	IMGUI_API void          Bullet();                                                       // draw a small circle and keep the cursor on the same line. advance cursor x position by GetTreeNodeToLabelSpacing(), same distance that TreeNode() uses

	// Widgets: Combo Box
	// - The new BeginCombo()/EndCombo() api allows you to manage your contents and selection state however you want it, by creating e.g. Selectable() items.
	// - The old Combo() api are helpers over BeginCombo()/EndCombo() which are kept available for convenience purpose.
	IMGUI_API bool          BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags = 0);
	IMGUI_API void          EndCombo(); // only call EndCombo() if BeginCombo() returns true!
	IMGUI_API bool          Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
	IMGUI_API bool          Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);      // Separate items with \0 within a string, end item-list with \0\0. e.g. "One\0Two\0Three\0"
	IMGUI_API bool          Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1);

	// Widgets: Drags
	// - CTRL+Click on any drag box to turn them into an input box. Manually input values aren't clamped and can go off-bounds.
	// - For all the Float2/Float3/Float4/Int2/Int3/Int4 versions of every functions, note that a 'float v[X]' function argument is the same as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible. You can pass address of your first element out of a contiguous set, e.g. &myvector.x
	// - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
	// - Speed are per-pixel of mouse movement (v_speed=0.2f: mouse needs to move by 5 pixels to increase value by 1). For gamepad/keyboard navigation, minimum speed is Max(v_speed, minimum_step_at_given_precision).
	// - Use v_min < v_max to clamp edits to given limits. Note that CTRL+Click manual input can override those limits.
	// - Use v_min > v_max to lock edits.
	IMGUI_API bool          DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);     // If v_min >= v_max we have no bound
	IMGUI_API bool          DragFloat2(const char* label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
	IMGUI_API bool          DragFloat3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
	IMGUI_API bool          DragFloat4(const char* label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", float power = 1.0f);
	IMGUI_API bool          DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", const char* format_max = NULL, float power = 1.0f);
	IMGUI_API bool          DragInt(const char* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");                                       // If v_min >= v_max we have no bound
	IMGUI_API bool          DragInt2(const char* label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");
	IMGUI_API bool          DragInt3(const char* label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");
	IMGUI_API bool          DragInt4(const char* label, int v[4], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d");
	IMGUI_API bool          DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", const char* format_max = NULL);
	IMGUI_API bool          DragScalar(const char* label, ImGuiDataType data_type, void* v, float v_speed, const void* v_min = NULL, const void* v_max = NULL, const char* format = NULL, float power = 1.0f);
	IMGUI_API bool          DragScalarN(const char* label, ImGuiDataType data_type, void* v, int components, float v_speed, const void* v_min = NULL, const void* v_max = NULL, const char* format = NULL, float power = 1.0f);

	// Widgets: Sliders
	// - CTRL+Click on any slider to turn them into an input box. Manually input values aren't clamped and can go off-bounds.
	// - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
	IMGUI_API bool          SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display. Use power!=1.0 for power curve sliders
	IMGUI_API bool          SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
	IMGUI_API bool          SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
	IMGUI_API bool          SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
	IMGUI_API bool          SliderAngle(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f, const char* format = "%.0f deg");
	IMGUI_API bool          SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d");
	IMGUI_API bool          SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format = "%d");
	IMGUI_API bool          SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format = "%d");
	IMGUI_API bool          SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format = "%d");
	IMGUI_API bool          SliderScalar(const char* label, ImGuiDataType data_type, void* v, const void* v_min, const void* v_max, const char* format = NULL, float power = 1.0f);
	IMGUI_API bool          SliderScalarN(const char* label, ImGuiDataType data_type, void* v, int components, const void* v_min, const void* v_max, const char* format = NULL, float power = 1.0f);
	IMGUI_API bool          VSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* format = "%.3f", float power = 1.0f);
	IMGUI_API bool          VSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* format = "%d");
	IMGUI_API bool          VSliderScalar(const char* label, const ImVec2& size, ImGuiDataType data_type, void* v, const void* v_min, const void* v_max, const char* format = NULL, float power = 1.0f);

	// Widgets: Input with Keyboard
	// - If you want to use InputText() with a dynamic string type such as std::string or your own, see misc/cpp/imgui_stdlib.h
	// - Most of the ImGuiInputTextFlags flags are only useful for InputText() and not for InputFloatX, InputIntX, InputDouble etc.
	IMGUI_API bool          InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	IMGUI_API bool          InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	IMGUI_API bool          InputTextWithHint(const char* label, const char* hint, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
	IMGUI_API bool          InputFloat(const char* label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputFloat2(const char* label, float v[2], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputFloat3(const char* label, float v[3], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputFloat4(const char* label, float v[4], const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputInt(const char* label, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputInt2(const char* label, int v[2], ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputInt3(const char* label, int v[3], ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputInt4(const char* label, int v[4], ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputDouble(const char* label, double* v, double step = 0.0, double step_fast = 0.0, const char* format = "%.6f", ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputScalar(const char* label, ImGuiDataType data_type, void* v, const void* step = NULL, const void* step_fast = NULL, const char* format = NULL, ImGuiInputTextFlags flags = 0);
	IMGUI_API bool          InputScalarN(const char* label, ImGuiDataType data_type, void* v, int components, const void* step = NULL, const void* step_fast = NULL, const char* format = NULL, ImGuiInputTextFlags flags = 0);

	// Widgets: Color Editor/Picker (tip: the ColorEdit* functions have a little colored preview square that can be left-clicked to open a picker, and right-clicked to open an option menu.)
	// - Note that in C++ a 'float v[X]' function argument is the _same_ as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible.
	// - You can pass the address of a first float element out of a contiguous structure, e.g. &myvector.x
	IMGUI_API bool          ColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags = 0);
	IMGUI_API bool          ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0);
	IMGUI_API bool          ColorPicker3(const char* label, float col[3], ImGuiColorEditFlags flags = 0);
	IMGUI_API bool          ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags = 0, const float* ref_col = NULL);
	IMGUI_API bool          ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags = 0, ImVec2 size = ImVec2(0, 0)); // display a colored square/button, hover for details, return true when pressed.
	IMGUI_API void          SetColorEditOptions(ImGuiColorEditFlags flags);                     // initialize current options (generally on application startup) if you want to select a default format, picker type, etc. User will be able to change many settings, unless you pass the _NoOptions flag to your calls.

	// Widgets: Trees
	// - TreeNode functions return true when the node is open, in which case you need to also call TreePop() when you are finished displaying the tree node contents.
	IMGUI_API bool          TreeNode(const char* label);
	IMGUI_API bool          TreeNode(const char* str_id, const char* fmt, ...) IM_FMTARGS(2);   // helper variation to easily decorelate the id from the displayed string. Read the FAQ about why and how to use ID. to align arbitrary text at the same level as a TreeNode() you can use Bullet().
	IMGUI_API bool          TreeNode(const void* ptr_id, const char* fmt, ...) IM_FMTARGS(2);   // "
	IMGUI_API bool          TreeNodeV(const char* str_id, const char* fmt, va_list args) IM_FMTLIST(2);
	IMGUI_API bool          TreeNodeV(const void* ptr_id, const char* fmt, va_list args) IM_FMTLIST(2);
	IMGUI_API bool          TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags = 0);
	IMGUI_API bool          TreeNodeEx(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, ...) IM_FMTARGS(3);
	IMGUI_API bool          TreeNodeEx(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, ...) IM_FMTARGS(3);
	IMGUI_API bool          TreeNodeExV(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args) IM_FMTLIST(3);
	IMGUI_API bool          TreeNodeExV(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args) IM_FMTLIST(3);
	IMGUI_API void          TreePush(const char* str_id);                                       // ~ Indent()+PushId(). Already called by TreeNode() when returning true, but you can call TreePush/TreePop yourself if desired.
	IMGUI_API void          TreePush(const void* ptr_id = NULL);                                // "
	IMGUI_API void          TreePop();                                                          // ~ Unindent()+PopId()
	IMGUI_API float         GetTreeNodeToLabelSpacing();                                        // horizontal distance preceding label when using TreeNode*() or Bullet() == (g.FontSize + style.FramePadding.x*2) for a regular unframed TreeNode
	IMGUI_API bool          CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags = 0);  // if returning 'true' the header is open. doesn't indent nor push on ID stack. user doesn't have to call TreePop().
	IMGUI_API bool          CollapsingHeader(const char* label, bool* p_open, ImGuiTreeNodeFlags flags = 0); // when 'p_open' isn't NULL, display an additional small close button on upper right of the header
	IMGUI_API void          SetNextItemOpen(bool is_open, ImGuiCond cond = 0);                  // set next TreeNode/CollapsingHeader open state.

	// Widgets: Selectables
	// - A selectable highlights when hovered, and can display another color when selected.
	// - Neighbors selectable extend their highlight bounds in order to leave no gap between them.
	IMGUI_API bool          Selectable(const char* label, bool selected = false, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0)); // "bool selected" carry the selection state (read-only). Selectable() is clicked is returns true so you can modify your selection state. size.x==0.0: use remaining width, size.x>0.0: specify width. size.y==0.0: use label height, size.y>0.0: specify height
	IMGUI_API bool          Selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));      // "bool* p_selected" point to the selection state (read-write), as a convenient helper.

	// Widgets: List Boxes
	// - FIXME: To be consistent with all the newer API, ListBoxHeader/ListBoxFooter should in reality be called BeginListBox/EndListBox. Will rename them.
	IMGUI_API bool          ListBox(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items = -1);
	IMGUI_API bool          ListBox(const char* label, int* current_item, bool (*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1);
	IMGUI_API bool          ListBoxHeader(const char* label, const ImVec2& size = ImVec2(0, 0)); // use if you want to reimplement ListBox() will custom data or interactions. if the function return true, you can output elements then call ListBoxFooter() afterwards.
	IMGUI_API bool          ListBoxHeader(const char* label, int items_count, int height_in_items = -1); // "
	IMGUI_API void          ListBoxFooter();                                                    // terminate the scrolling region. only call ListBoxFooter() if ListBoxHeader() returned true!

	// Widgets: Data Plotting
	IMGUI_API void          PlotLines(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
	IMGUI_API void          PlotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0));
	IMGUI_API void          PlotHistogram(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
	IMGUI_API void          PlotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0));

	// Widgets: Value() Helpers.
	// - Those are merely shortcut to calling Text() with a format string. Output single value in "name: value" format (tip: freely declare more in your code to handle your types. you can add functions to the ImGui namespace)
	IMGUI_API void          Value(const char* prefix, bool b);
	IMGUI_API void          Value(const char* prefix, int v);
	IMGUI_API void          Value(const char* prefix, unsigned int v);
	IMGUI_API void          Value(const char* prefix, float v, const char* float_format = NULL);

	// Widgets: Menus
	IMGUI_API bool          BeginMainMenuBar();                                                 // create and append to a full screen menu-bar.
	IMGUI_API void          EndMainMenuBar();                                                   // only call EndMainMenuBar() if BeginMainMenuBar() returns true!
	IMGUI_API bool          BeginMenuBar();                                                     // append to menu-bar of current window (requires ImGuiWindowFlags_MenuBar flag set on parent window).
	IMGUI_API void          EndMenuBar();                                                       // only call EndMenuBar() if BeginMenuBar() returns true!
	IMGUI_API bool          BeginMenu(const char* label, bool enabled = true);                  // create a sub-menu entry. only call EndMenu() if this returns true!
	IMGUI_API void          EndMenu();                                                          // only call EndMenu() if BeginMenu() returns true!
	IMGUI_API bool          MenuItem(const char* label, const char* shortcut = NULL, bool selected = false, bool enabled = true);  // return true when activated. shortcuts are displayed for convenience but not processed by ImGui at the moment
	IMGUI_API bool          MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled = true);              // return true when activated + toggle (*p_selected) if p_selected != NULL

	// Tooltips
	IMGUI_API void          BeginTooltip();                                                     // begin/append a tooltip window. to create full-featured tooltip (with any kind of items).
	IMGUI_API void          EndTooltip();
	IMGUI_API void          SetTooltip(const char* fmt, ...) IM_FMTARGS(1);                     // set a text-only tooltip, typically use with ImGui::IsItemHovered(). override any previous call to SetTooltip().
	IMGUI_API void          SetTooltipV(const char* fmt, va_list args) IM_FMTLIST(1);

	// Popups, Modals
	// The properties of popups windows are:
	// - They block normal mouse hovering detection outside them. (*)
	// - Unless modal, they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
	// - Their visibility state (~bool) is held internally by imgui instead of being held by the programmer as we are used to with regular Begin() calls.
	//   User can manipulate the visibility state by calling OpenPopup().
	// (*) One can use IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) to bypass it and detect hovering even when normally blocked by a popup.
	// Those three properties are connected. The library needs to hold their visibility state because it can close popups at any time.
	IMGUI_API void          OpenPopup(const char* str_id);                                      // call to mark popup as open (don't call every frame!). popups are closed when user click outside, or if CloseCurrentPopup() is called within a BeginPopup()/EndPopup() block. By default, Selectable()/MenuItem() are calling CloseCurrentPopup(). Popup identifiers are relative to the current ID-stack (so OpenPopup and BeginPopup needs to be at the same level).
	IMGUI_API bool          BeginPopup(const char* str_id, ImGuiWindowFlags flags = 0);                                             // return true if the popup is open, and you can start outputting to it. only call EndPopup() if BeginPopup() returns true!
	IMGUI_API bool          BeginPopupContextItem(const char* str_id = NULL, int mouse_button = 1);                                 // helper to open and begin popup when clicked on last item. if you can pass a NULL str_id only if the previous item had an id. If you want to use that on a non-interactive item such as Text() you need to pass in an explicit ID here. read comments in .cpp!
	IMGUI_API bool          BeginPopupContextWindow(const char* str_id = NULL, int mouse_button = 1, bool also_over_items = true);  // helper to open and begin popup when clicked on current window.
	IMGUI_API bool          BeginPopupContextVoid(const char* str_id = NULL, int mouse_button = 1);                                 // helper to open and begin popup when clicked in void (where there are no imgui windows).
	IMGUI_API bool          BeginPopupModal(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0);                     // modal dialog (regular window with title bar, block interactions behind the modal window, can't close the modal window by clicking outside)
	IMGUI_API void          EndPopup();                                                                                             // only call EndPopup() if BeginPopupXXX() returns true!
	IMGUI_API bool          OpenPopupOnItemClick(const char* str_id = NULL, int mouse_button = 1);                                  // helper to open popup when clicked on last item (note: actually triggers on the mouse _released_ event to be consistent with popup behaviors). return true when just opened.
	IMGUI_API bool          IsPopupOpen(const char* str_id);                                    // return true if the popup is open at the current begin-ed level of the popup stack.
	IMGUI_API void          CloseCurrentPopup();                                                // close the popup we have begin-ed into. clicking on a MenuItem or Selectable automatically close the current popup.

	// Columns
	// - You can also use SameLine(pos_x) to mimic simplified columns.
	// - The columns API is work-in-progress and rather lacking (columns are arguably the worst part of dear imgui at the moment!)
	IMGUI_API void          Columns(int count = 1, const char* id = NULL, bool border = true);
	IMGUI_API void          NextColumn();                                                       // next column, defaults to current row or next row if the current row is finished
	IMGUI_API int           GetColumnIndex();                                                   // get current column index
	IMGUI_API float         GetColumnWidth(int column_index = -1);                              // get column width (in pixels). pass -1 to use current column
	IMGUI_API void          SetColumnWidth(int column_index, float width);                      // set column width (in pixels). pass -1 to use current column
	IMGUI_API float         GetColumnOffset(int column_index = -1);                             // get position of column line (in pixels, from the left side of the contents region). pass -1 to use current column, otherwise 0..GetColumnsCount() inclusive. column 0 is typically 0.0f
	IMGUI_API void          SetColumnOffset(int column_index, float offset_x);                  // set position of column line (in pixels, from the left side of the contents region). pass -1 to use current column
	IMGUI_API int           GetColumnsCount();

	// Tab Bars, Tabs
	// [BETA API] API may evolve!
	IMGUI_API bool          BeginTabBar(const char* str_id, ImGuiTabBarFlags flags = 0);        // create and append into a TabBar
	IMGUI_API void          EndTabBar();                                                        // only call EndTabBar() if BeginTabBar() returns true!
	IMGUI_API bool          BeginTabItem(const char* label, bool* p_open = NULL, ImGuiTabItemFlags flags = 0);// create a Tab. Returns true if the Tab is selected.
	IMGUI_API void          EndTabItem();                                                       // only call EndTabItem() if BeginTabItem() returns true!
	IMGUI_API void          SetTabItemClosed(const char* tab_or_docked_window_label);           // notify TabBar or Docking system of a closed tab/window ahead (useful to reduce visual flicker on reorderable tab bars). For tab-bar: call after BeginTabBar() and before Tab submissions. Otherwise call with a window name.

	// Logging/Capture
	// - All text output from the interface can be captured into tty/file/clipboard. By default, tree nodes are automatically opened during logging.
	IMGUI_API void          LogToTTY(int auto_open_depth = -1);                                 // start logging to tty (stdout)
	IMGUI_API void          LogToFile(int auto_open_depth = -1, const char* filename = NULL);   // start logging to file
	IMGUI_API void          LogToClipboard(int auto_open_depth = -1);                           // start logging to OS clipboard
	IMGUI_API void          LogFinish();                                                        // stop logging (close file, etc.)
	IMGUI_API void          LogButtons();                                                       // helper to display buttons for logging to tty/file/clipboard
	IMGUI_API void          LogText(const char* fmt, ...) IM_FMTARGS(1);                        // pass text data straight to log (without being displayed)

	// Drag and Drop
	// [BETA API] API may evolve!
	IMGUI_API bool          BeginDragDropSource(ImGuiDragDropFlags flags = 0);                                      // call when the current item is active. If this return true, you can call SetDragDropPayload() + EndDragDropSource()
	IMGUI_API bool          SetDragDropPayload(const char* type, const void* data, size_t sz, ImGuiCond cond = 0);  // type is a user defined string of maximum 32 characters. Strings starting with '_' are reserved for dear imgui internal types. Data is copied and held by imgui.
	IMGUI_API void          EndDragDropSource();                                                                    // only call EndDragDropSource() if BeginDragDropSource() returns true!
	IMGUI_API bool                  BeginDragDropTarget();                                                          // call after submitting an item that may receive a payload. If this returns true, you can call AcceptDragDropPayload() + EndDragDropTarget()
	IMGUI_API const ImGuiPayload*   AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags = 0);          // accept contents of a given type. If ImGuiDragDropFlags_AcceptBeforeDelivery is set you can peek into the payload before the mouse button is released.
	IMGUI_API void                  EndDragDropTarget();                                                            // only call EndDragDropTarget() if BeginDragDropTarget() returns true!
	IMGUI_API const ImGuiPayload*   GetDragDropPayload();                                                           // peek directly into the current payload from anywhere. may return NULL. use ImGuiPayload::IsDataType() to test for the payload type.

	// Clipping
	IMGUI_API void          PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect);
	IMGUI_API void          PopClipRect();

	// Focus, Activation
	// - Prefer using "SetItemDefaultFocus()" over "if (IsWindowAppearing()) SetScrollHereY()" when applicable to signify "this is the default item"
	IMGUI_API void          SetItemDefaultFocus();                                              // make last item the default focused item of a window.
	IMGUI_API void          SetKeyboardFocusHere(int offset = 0);                               // focus keyboard on the next widget. Use positive 'offset' to access sub components of a multiple component widget. Use -1 to access previous widget.

	// Item/Widgets Utilities
	// - Most of the functions are referring to the last/previous item we submitted.
	// - See Demo Window under "Widgets->Querying Status" for an interactive visualization of most of those functions.
	IMGUI_API bool          IsItemHovered(ImGuiHoveredFlags flags = 0);                         // is the last item hovered? (and usable, aka not blocked by a popup, etc.). See ImGuiHoveredFlags for more options.
	IMGUI_API bool          IsItemActive();                                                     // is the last item active? (e.g. button being held, text field being edited. This will continuously return true while holding mouse button on an item. Items that don't interact will always return false)
	IMGUI_API bool          IsItemFocused();                                                    // is the last item focused for keyboard/gamepad navigation?
	IMGUI_API bool          IsItemClicked(int mouse_button = 0);                                // is the last item clicked? (e.g. button/node just clicked on) == IsMouseClicked(mouse_button) && IsItemHovered()
	IMGUI_API bool          IsItemVisible();                                                    // is the last item visible? (items may be out of sight because of clipping/scrolling)
	IMGUI_API bool          IsItemEdited();                                                     // did the last item modify its underlying value this frame? or was pressed? This is generally the same as the "bool" return value of many widgets.
	IMGUI_API bool          IsItemActivated();                                                  // was the last item just made active (item was previously inactive).
	IMGUI_API bool          IsItemDeactivated();                                                // was the last item just made inactive (item was previously active). Useful for Undo/Redo patterns with widgets that requires continuous editing.
	IMGUI_API bool          IsItemDeactivatedAfterEdit();                                       // was the last item just made inactive and made a value change when it was active? (e.g. Slider/Drag moved). Useful for Undo/Redo patterns with widgets that requires continuous editing. Note that you may get false positives (some widgets such as Combo()/ListBox()/Selectable() will return true even when clicking an already selected item).
	IMGUI_API bool          IsAnyItemHovered();                                                 // is any item hovered?
	IMGUI_API bool          IsAnyItemActive();                                                  // is any item active?
	IMGUI_API bool          IsAnyItemFocused();                                                 // is any item focused?
	IMGUI_API ImVec2        GetItemRectMin();                                                   // get upper-left bounding rectangle of the last item (screen space)
	IMGUI_API ImVec2        GetItemRectMax();                                                   // get lower-right bounding rectangle of the last item (screen space)
	IMGUI_API ImVec2        GetItemRectSize();                                                  // get size of last item
	IMGUI_API void          SetItemAllowOverlap();                                              // allow last item to be overlapped by a subsequent item. sometimes useful with invisible buttons, selectables, etc. to catch unused area.

	// Miscellaneous Utilities
	IMGUI_API bool          IsRectVisible(const ImVec2& size);                                  // test if rectangle (of given size, starting from cursor position) is visible / not clipped.
	IMGUI_API bool          IsRectVisible(const ImVec2& rect_min, const ImVec2& rect_max);      // test if rectangle (in screen space) is visible / not clipped. to perform coarse clipping on user's side.
	IMGUI_API double        GetTime();                                                          // get global imgui time. incremented by io.DeltaTime every frame.
	IMGUI_API int           GetFrameCount();                                                    // get global imgui frame count. incremented by 1 every frame.
	IMGUI_API ImDrawList*   GetBackgroundDrawList();                                            // this draw list will be the first rendering one. Useful to quickly draw shapes/text behind dear imgui contents.
	IMGUI_API ImDrawList*   GetForegroundDrawList();                                            // this draw list will be the last rendered one. Useful to quickly draw shapes/text over dear imgui contents.
	IMGUI_API ImDrawListSharedData* GetDrawListSharedData();                                    // you may use this when creating your own ImDrawList instances.
	IMGUI_API const char*   GetStyleColorName(ImGuiCol idx);                                    // get a string corresponding to the enum value (for display, saving, etc.).
	IMGUI_API void          SetStateStorage(ImGuiStorage* storage);                             // replace current window storage with our own (if you want to manipulate it yourself, typically clear subsection of it)
	IMGUI_API ImGuiStorage* GetStateStorage();
	IMGUI_API ImVec2        CalcTextSize(const char* text, const char* text_end = NULL, bool hide_text_after_double_hash = false, float wrap_width = -1.0f);
	IMGUI_API void          CalcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end);    // calculate coarse clipping for large list of evenly sized items. Prefer using the ImGuiListClipper higher-level helper if you can.
	IMGUI_API bool          BeginChildFrame(ImGuiID id, const ImVec2& size, ImGuiWindowFlags flags = 0); // helper to create a child window / scrolling region that looks like a normal widget frame
	IMGUI_API void          EndChildFrame();                                                    // always call EndChildFrame() regardless of BeginChildFrame() return values (which indicates a collapsed/clipped window)

	// Color Utilities
	IMGUI_API ImVec4        ColorConvertU32ToFloat4(ImU32 in);
	IMGUI_API ImU32         ColorConvertFloat4ToU32(const ImVec4& in);
	IMGUI_API void          ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v);
	IMGUI_API void          ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b);

	// Inputs Utilities
	IMGUI_API int           GetKeyIndex(ImGuiKey imgui_key);                                    // map ImGuiKey_* values into user's key index. == io.KeyMap[key]
	IMGUI_API bool          IsKeyDown(int user_key_index);                                      // is key being held. == io.KeysDown[user_key_index]. note that imgui doesn't know the semantic of each entry of io.KeysDown[]. Use your own indices/enums according to how your backend/engine stored them into io.KeysDown[]!
	IMGUI_API bool          IsKeyPressed(int user_key_index, bool repeat = true);               // was key pressed (went from !Down to Down). if repeat=true, uses io.KeyRepeatDelay / KeyRepeatRate
	IMGUI_API bool          IsKeyReleased(int user_key_index);                                  // was key released (went from Down to !Down)..
	IMGUI_API int           GetKeyPressedAmount(int key_index, float repeat_delay, float rate); // uses provided repeat rate/delay. return a count, most often 0 or 1 but might be >1 if RepeatRate is small enough that DeltaTime > RepeatRate
	IMGUI_API bool          IsMouseDown(int button);                                            // is mouse button held (0=left, 1=right, 2=middle)
	IMGUI_API bool          IsAnyMouseDown();                                                   // is any mouse button held
	IMGUI_API bool          IsMouseClicked(int button, bool repeat = false);                    // did mouse button clicked (went from !Down to Down) (0=left, 1=right, 2=middle)
	IMGUI_API bool          IsMouseDoubleClicked(int button);                                   // did mouse button double-clicked. a double-click returns false in IsMouseClicked(). uses io.MouseDoubleClickTime.
	IMGUI_API bool          IsMouseReleased(int button);                                        // did mouse button released (went from Down to !Down)
	IMGUI_API bool          IsMouseDragging(int button = 0, float lock_threshold = -1.0f);      // is mouse dragging. if lock_threshold < -1.0f uses io.MouseDraggingThreshold
	IMGUI_API bool          IsMouseHoveringRect(const ImVec2& r_min, const ImVec2& r_max, bool clip = true);  // is mouse hovering given bounding rect (in screen space). clipped by current clipping settings, but disregarding of other consideration of focus/window ordering/popup-block.
	IMGUI_API bool          IsMousePosValid(const ImVec2* mouse_pos = NULL);                    // by convention we use (-FLT_MAX,-FLT_MAX) to denote that there is no mouse
	IMGUI_API ImVec2        GetMousePos();                                                      // shortcut to ImGui::GetIO().MousePos provided by user, to be consistent with other calls
	IMGUI_API ImVec2        GetMousePosOnOpeningCurrentPopup();                                 // retrieve backup of mouse position at the time of opening popup we have BeginPopup() into
	IMGUI_API ImVec2        GetMouseDragDelta(int button = 0, float lock_threshold = -1.0f);    // return the delta from the initial clicking position while the mouse button is pressed or was just released. This is locked and return 0.0f until the mouse moves past a distance threshold at least once. If lock_threshold < -1.0f uses io.MouseDraggingThreshold.
	IMGUI_API void          ResetMouseDragDelta(int button = 0);                                //
	IMGUI_API ImGuiMouseCursor GetMouseCursor();                                                // get desired cursor type, reset in ImGui::NewFrame(), this is updated during the frame. valid before Render(). If you use software rendering by setting io.MouseDrawCursor ImGui will render those for you
	IMGUI_API void          SetMouseCursor(ImGuiMouseCursor type);                              // set desired cursor type
	IMGUI_API void          CaptureKeyboardFromApp(bool want_capture_keyboard_value = true);    // attention: misleading name! manually override io.WantCaptureKeyboard flag next frame (said flag is entirely left for your application to handle). e.g. force capture keyboard when your widget is being hovered. This is equivalent to setting "io.WantCaptureKeyboard = want_capture_keyboard_value"; after the next NewFrame() call.
	IMGUI_API void          CaptureMouseFromApp(bool want_capture_mouse_value = true);          // attention: misleading name! manually override io.WantCaptureMouse flag next frame (said flag is entirely left for your application to handle). This is equivalent to setting "io.WantCaptureMouse = want_capture_mouse_value;" after the next NewFrame() call.

	// Clipboard Utilities (also see the LogToClipboard() function to capture or output text data to the clipboard)
	IMGUI_API const char*   GetClipboardText();
	IMGUI_API void          SetClipboardText(const char* text);

	// Settings/.Ini Utilities
	// - The disk functions are automatically called if io.IniFilename != NULL (default is "imgui.ini").
	// - Set io.IniFilename to NULL to load/save manually. Read io.WantSaveIniSettings description about handling .ini saving manually.
	IMGUI_API void          LoadIniSettingsFromDisk(const char* ini_filename);                  // call after CreateContext() and before the first call to NewFrame(). NewFrame() automatically calls LoadIniSettingsFromDisk(io.IniFilename).
	IMGUI_API void          LoadIniSettingsFromMemory(const char* ini_data, size_t ini_size = 0); // call after CreateContext() and before the first call to NewFrame() to provide .ini data from your own data source.
	IMGUI_API void          SaveIniSettingsToDisk(const char* ini_filename);                    // this is automatically called (if io.IniFilename is not empty) a few seconds after any modification that should be reflected in the .ini file (and also by DestroyContext).
	IMGUI_API const char*   SaveIniSettingsToMemory(size_t* out_ini_size = NULL);               // return a zero-terminated string with the .ini data which you can save by your own mean. call when io.WantSaveIniSettings is set, then save data by your own mean and clear io.WantSaveIniSettings.

	// Memory Allocators
	// - All those functions are not reliant on the current context.
	// - If you reload the contents of imgui.cpp at runtime, you may need to call SetCurrentContext() + SetAllocatorFunctions() again because we use global storage for those.
	IMGUI_API void          SetAllocatorFunctions(void* (*alloc_func)(size_t sz, void* user_data), void (*free_func)(void* ptr, void* user_data), void* user_data = NULL);
	IMGUI_API void*         MemAlloc(size_t size);
	IMGUI_API void          MemFree(void* ptr);

} // namespace ImGui

//-----------------------------------------------------------------------------
// Flags & Enumerations
//-----------------------------------------------------------------------------

// Flags for ImGui::Begin()
enum ImGuiWindowFlags_ {
	ImGuiWindowFlags_None                   = 0,
	ImGuiWindowFlags_NoTitleBar             = 1 << 0,   // Disable title-bar
	ImGuiWindowFlags_NoResize               = 1 << 1,   // Disable user resizing with the lower-right grip
	ImGuiWindowFlags_NoMove                 = 1 << 2,   // Disable user moving the window
	ImGuiWindowFlags_NoScrollbar            = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
	ImGuiWindowFlags_NoScrollWithMouse      = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
	ImGuiWindowFlags_NoCollapse             = 1 << 5,   // Disable user collapsing window by double-clicking on it
	ImGuiWindowFlags_AlwaysAutoResize       = 1 << 6,   // Resize every window to its content every frame
	ImGuiWindowFlags_NoBackground           = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
	ImGuiWindowFlags_NoSavedSettings        = 1 << 8,   // Never load/save settings in .ini file
	ImGuiWindowFlags_NoMouseInputs          = 1 << 9,   // Disable catching mouse, hovering test with pass through.
	ImGuiWindowFlags_MenuBar                = 1 << 10,  // Has a menu-bar
	ImGuiWindowFlags_HorizontalScrollbar    = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
	ImGuiWindowFlags_NoFocusOnAppearing     = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
	ImGuiWindowFlags_NoBringToFrontOnFocus  = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
	ImGuiWindowFlags_AlwaysVerticalScrollbar = 1 << 14, // Always show vertical scrollbar (even if ContentSize.y < Size.y)
	ImGuiWindowFlags_AlwaysHorizontalScrollbar = 1 << 15, // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
	ImGuiWindowFlags_AlwaysUseWindowPadding = 1 << 16,  // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
	ImGuiWindowFlags_NoNavInputs            = 1 << 18,  // No gamepad/keyboard navigation within the window
	ImGuiWindowFlags_NoNavFocus             = 1 << 19,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
	ImGuiWindowFlags_UnsavedDocument        = 1 << 20,  // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. When used in a tab/docking context, tab is selected on closure and closure is deferred by one frame to allow code to cancel the closure (with a confirmation popup, etc.) without flicker.
	ImGuiWindowFlags_NoNav                  = ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus,
	ImGuiWindowFlags_NoDecoration           = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse,
	ImGuiWindowFlags_NoInputs               = ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus,

	// [Internal]
	ImGuiWindowFlags_NavFlattened           = 1 << 23,  // [BETA] Allow gamepad/keyboard navigation to cross over parent border to this child (only use on child that have no scrolling!)
	ImGuiWindowFlags_ChildWindow            = 1 << 24,  // Don't use! For internal use by BeginChild()
	ImGuiWindowFlags_Tooltip                = 1 << 25,  // Don't use! For internal use by BeginTooltip()
	ImGuiWindowFlags_Popup                  = 1 << 26,  // Don't use! For internal use by BeginPopup()
	ImGuiWindowFlags_Modal                  = 1 << 27,  // Don't use! For internal use by BeginPopupModal()
	ImGuiWindowFlags_ChildMenu              = 1 << 28   // Don't use! For internal use by BeginMenu()

	        // [Obsolete]
	        //ImGuiWindowFlags_ShowBorders          = 1 << 7,   // --> Set style.FrameBorderSize=1.0f / style.WindowBorderSize=1.0f to enable borders around windows and items
	        //ImGuiWindowFlags_ResizeFromAnySide    = 1 << 17,  // --> Set io.ConfigWindowsResizeFromEdges and make sure mouse cursors are supported by back-end (io.BackendFlags & ImGuiBackendFlags_HasMouseCursors)
};

// Flags for ImGui::InputText()
enum ImGuiInputTextFlags_ {
	ImGuiInputTextFlags_None                = 0,
	ImGuiInputTextFlags_CharsDecimal        = 1 << 0,   // Allow 0123456789.+-*/
	ImGuiInputTextFlags_CharsHexadecimal    = 1 << 1,   // Allow 0123456789ABCDEFabcdef
	ImGuiInputTextFlags_CharsUppercase      = 1 << 2,   // Turn a..z into A..Z
	ImGuiInputTextFlags_CharsNoBlank        = 1 << 3,   // Filter out spaces, tabs
	ImGuiInputTextFlags_AutoSelectAll       = 1 << 4,   // Select entire text when first taking mouse focus
	ImGuiInputTextFlags_EnterReturnsTrue    = 1 << 5,   // Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider looking at the IsItemDeactivatedAfterEdit() function.
	ImGuiInputTextFlags_CallbackCompletion  = 1 << 6,   // Callback on pressing TAB (for completion handling)
	ImGuiInputTextFlags_CallbackHistory     = 1 << 7,   // Callback on pressing Up/Down arrows (for history handling)
	ImGuiInputTextFlags_CallbackAlways      = 1 << 8,   // Callback on each iteration. User code may query cursor position, modify text buffer.
	ImGuiInputTextFlags_CallbackCharFilter  = 1 << 9,   // Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
	ImGuiInputTextFlags_AllowTabInput       = 1 << 10,  // Pressing TAB input a '\t' character into the text field
	ImGuiInputTextFlags_CtrlEnterForNewLine = 1 << 11,  // In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus with Ctrl+Enter, add line with Enter).
	ImGuiInputTextFlags_NoHorizontalScroll  = 1 << 12,  // Disable following the cursor horizontally
	ImGuiInputTextFlags_AlwaysInsertMode    = 1 << 13,  // Insert mode
	ImGuiInputTextFlags_ReadOnly            = 1 << 14,  // Read-only mode
	ImGuiInputTextFlags_Password            = 1 << 15,  // Password mode, display all characters as '*'
	ImGuiInputTextFlags_NoUndoRedo          = 1 << 16,  // Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().
	ImGuiInputTextFlags_CharsScientific     = 1 << 17,  // Allow 0123456789.+-*/eE (Scientific notation input)
	ImGuiInputTextFlags_CallbackResize      = 1 << 18,  // Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow. Notify when the string wants to be resized (for string types which hold a cache of their Size). You will be provided a new BufSize in the callback and NEED to honor it. (see misc/cpp/imgui_stdlib.h for an example of using this)
	// [Internal]
	ImGuiInputTextFlags_Multiline           = 1 << 20,  // For internal use by InputTextMultiline()
	ImGuiInputTextFlags_NoMarkEdited        = 1 << 21   // For internal use by functions using InputText() before reformatting data
};

// Flags for ImGui::TreeNodeEx(), ImGui::CollapsingHeader*()
enum ImGuiTreeNodeFlags_ {
	ImGuiTreeNodeFlags_None                 = 0,
	ImGuiTreeNodeFlags_Selected             = 1 << 0,   // Draw as selected
	ImGuiTreeNodeFlags_Framed               = 1 << 1,   // Full colored frame (e.g. for CollapsingHeader)
	ImGuiTreeNodeFlags_AllowItemOverlap     = 1 << 2,   // Hit testing to allow subsequent widgets to overlap this one
	ImGuiTreeNodeFlags_NoTreePushOnOpen     = 1 << 3,   // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
	ImGuiTreeNodeFlags_NoAutoOpenOnLog      = 1 << 4,   // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
	ImGuiTreeNodeFlags_DefaultOpen          = 1 << 5,   // Default node to be open
	ImGuiTreeNodeFlags_OpenOnDoubleClick    = 1 << 6,   // Need double-click to open node
	ImGuiTreeNodeFlags_OpenOnArrow          = 1 << 7,   // Only open when clicking on the arrow part. If ImGuiTreeNodeFlags_OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
	ImGuiTreeNodeFlags_Leaf                 = 1 << 8,   // No collapsing, no arrow (use as a convenience for leaf nodes).
	ImGuiTreeNodeFlags_Bullet               = 1 << 9,   // Display a bullet instead of arrow
	ImGuiTreeNodeFlags_FramePadding         = 1 << 10,  // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
	ImGuiTreeNodeFlags_SpanAvailWidth       = 1 << 11,  // Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line. In the future we may refactor the hit system to be front-to-back, allowing natural overlaps and then this can become the default.
	ImGuiTreeNodeFlags_SpanFullWidth        = 1 << 12,  // Extend hit box to the left-most and right-most edges (bypass the indented area).
	ImGuiTreeNodeFlags_NavLeftJumpsBackHere = 1 << 13,  // (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)
	//ImGuiTreeNodeFlags_NoScrollOnOpen     = 1 << 14,  // FIXME: TODO: Disable automatic scroll on TreePop() if node got just open and contents is not visible
	ImGuiTreeNodeFlags_CollapsingHeader     = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog

	        // Obsolete names (will be removed)
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
	, ImGuiTreeNodeFlags_AllowOverlapMode = ImGuiTreeNodeFlags_AllowItemOverlap // [renamed in 1.53]
#endif
};

// Flags for ImGui::Selectable()
enum ImGuiSelectableFlags_ {
	ImGuiSelectableFlags_None               = 0,
	ImGuiSelectableFlags_DontClosePopups    = 1 << 0,   // Clicking this don't close parent popup window
	ImGuiSelectableFlags_SpanAllColumns     = 1 << 1,   // Selectable frame can span all columns (text will still fit in current column)
	ImGuiSelectableFlags_AllowDoubleClick   = 1 << 2,   // Generate press events on double clicks too
	ImGuiSelectableFlags_Disabled           = 1 << 3,   // Cannot be selected, display grayed out text
	ImGuiSelectableFlags_AllowItemOverlap   = 1 << 4    // (WIP) Hit testing to allow subsequent widgets to overlap this one
};

// Flags for ImGui::BeginCombo()
enum ImGuiComboFlags_ {
	ImGuiComboFlags_None                    = 0,
	ImGuiComboFlags_PopupAlignLeft          = 1 << 0,   // Align the popup toward the left by default
	ImGuiComboFlags_HeightSmall             = 1 << 1,   // Max ~4 items visible. Tip: If you want your combo popup to be a specific size you can use SetNextWindowSizeConstraints() prior to calling BeginCombo()
	ImGuiComboFlags_HeightRegular           = 1 << 2,   // Max ~8 items visible (default)
	ImGuiComboFlags_HeightLarge             = 1 << 3,   // Max ~20 items visible
	ImGuiComboFlags_HeightLargest           = 1 << 4,   // As many fitting items as possible
	ImGuiComboFlags_NoArrowButton           = 1 << 5,   // Display on the preview box without the square arrow button
	ImGuiComboFlags_NoPreview               = 1 << 6,   // Display only a square arrow button
	ImGuiComboFlags_HeightMask_             = ImGuiComboFlags_HeightSmall | ImGuiComboFlags_HeightRegular | ImGuiComboFlags_HeightLarge | ImGuiComboFlags_HeightLargest
};

// Flags for ImGui::BeginTabBar()
enum ImGuiTabBarFlags_ {
	ImGuiTabBarFlags_None                           = 0,
	ImGuiTabBarFlags_Reorderable                    = 1 << 0,   // Allow manually dragging tabs to re-order them + New tabs are appended at the end of list
	ImGuiTabBarFlags_AutoSelectNewTabs              = 1 << 1,   // Automatically select new tabs when they appear
	ImGuiTabBarFlags_TabListPopupButton             = 1 << 2,   // Disable buttons to open the tab list popup
	ImGuiTabBarFlags_NoCloseWithMiddleMouseButton   = 1 << 3,   // Disable behavior of closing tabs (that are submitted with p_open != NULL) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
	ImGuiTabBarFlags_NoTabListScrollingButtons      = 1 << 4,   // Disable scrolling buttons (apply when fitting policy is ImGuiTabBarFlags_FittingPolicyScroll)
	ImGuiTabBarFlags_NoTooltip                      = 1 << 5,   // Disable tooltips when hovering a tab
	ImGuiTabBarFlags_FittingPolicyResizeDown        = 1 << 6,   // Resize tabs when they don't fit
	ImGuiTabBarFlags_FittingPolicyScroll            = 1 << 7,   // Add scroll buttons when tabs don't fit
	ImGuiTabBarFlags_FittingPolicyMask_             = ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_FittingPolicyScroll,
	ImGuiTabBarFlags_FittingPolicyDefault_          = ImGuiTabBarFlags_FittingPolicyResizeDown
};

// Flags for ImGui::BeginTabItem()
enum ImGuiTabItemFlags_ {
	ImGuiTabItemFlags_None                          = 0,
	ImGuiTabItemFlags_UnsavedDocument               = 1 << 0,   // Append '*' to title without affecting the ID, as a convenience to avoid using the ### operator. Also: tab is selected on closure and closure is deferred by one frame to allow code to undo it without flicker.
	ImGuiTabItemFlags_SetSelected                   = 1 << 1,   // Trigger flag to programmatically make the tab selected when calling BeginTabItem()
	ImGuiTabItemFlags_NoCloseWithMiddleMouseButton  = 1 << 2,   // Disable behavior of closing tabs (that are submitted with p_open != NULL) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
	ImGuiTabItemFlags_NoPushId                      = 1 << 3    // Don't call PushID(tab->ID)/PopID() on BeginTabItem()/EndTabItem()
};

// Flags for ImGui::IsWindowFocused()
enum ImGuiFocusedFlags_ {
	ImGuiFocusedFlags_None                          = 0,
	ImGuiFocusedFlags_ChildWindows                  = 1 << 0,   // IsWindowFocused(): Return true if any children of the window is focused
	ImGuiFocusedFlags_RootWindow                    = 1 << 1,   // IsWindowFocused(): Test from root window (top most parent of the current hierarchy)
	ImGuiFocusedFlags_AnyWindow                     = 1 << 2,   // IsWindowFocused(): Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use ImGui::GetIO().WantCaptureMouse instead.
	ImGuiFocusedFlags_RootAndChildWindows           = ImGuiFocusedFlags_RootWindow | ImGuiFocusedFlags_ChildWindows
};

// Flags for ImGui::IsItemHovered(), ImGui::IsWindowHovered()
// Note: if you are trying to check whether your mouse should be dispatched to imgui or to your app, you should use the 'io.WantCaptureMouse' boolean for that. Please read the FAQ!
// Note: windows with the ImGuiWindowFlags_NoInputs flag are ignored by IsWindowHovered() calls.
enum ImGuiHoveredFlags_ {
	ImGuiHoveredFlags_None                          = 0,        // Return true if directly over the item/window, not obstructed by another window, not obstructed by an active popup or modal blocking inputs under them.
	ImGuiHoveredFlags_ChildWindows                  = 1 << 0,   // IsWindowHovered() only: Return true if any children of the window is hovered
	ImGuiHoveredFlags_RootWindow                    = 1 << 1,   // IsWindowHovered() only: Test from root window (top most parent of the current hierarchy)
	ImGuiHoveredFlags_AnyWindow                     = 1 << 2,   // IsWindowHovered() only: Return true if any window is hovered
	ImGuiHoveredFlags_AllowWhenBlockedByPopup       = 1 << 3,   // Return true even if a popup window is normally blocking access to this item/window
	//ImGuiHoveredFlags_AllowWhenBlockedByModal     = 1 << 4,   // Return true even if a modal popup window is normally blocking access to this item/window. FIXME-TODO: Unavailable yet.
	ImGuiHoveredFlags_AllowWhenBlockedByActiveItem  = 1 << 5,   // Return true even if an active item is blocking access to this item/window. Useful for Drag and Drop patterns.
	ImGuiHoveredFlags_AllowWhenOverlapped           = 1 << 6,   // Return true even if the position is obstructed or overlapped by another window
	ImGuiHoveredFlags_AllowWhenDisabled             = 1 << 7,   // Return true even if the item is disabled
	ImGuiHoveredFlags_RectOnly                      = ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AllowWhenOverlapped,
	ImGuiHoveredFlags_RootAndChildWindows           = ImGuiHoveredFlags_RootWindow | ImGuiHoveredFlags_ChildWindows
};

// Flags for ImGui::BeginDragDropSource(), ImGui::AcceptDragDropPayload()
enum ImGuiDragDropFlags_ {
	ImGuiDragDropFlags_None                         = 0,
	// BeginDragDropSource() flags
	ImGuiDragDropFlags_SourceNoPreviewTooltip       = 1 << 0,   // By default, a successful call to BeginDragDropSource opens a tooltip so you can display a preview or description of the source contents. This flag disable this behavior.
	ImGuiDragDropFlags_SourceNoDisableHover         = 1 << 1,   // By default, when dragging we clear data so that IsItemHovered() will return false, to avoid subsequent user code submitting tooltips. This flag disable this behavior so you can still call IsItemHovered() on the source item.
	ImGuiDragDropFlags_SourceNoHoldToOpenOthers     = 1 << 2,   // Disable the behavior that allows to open tree nodes and collapsing header by holding over them while dragging a source item.
	ImGuiDragDropFlags_SourceAllowNullID            = 1 << 3,   // Allow items such as Text(), Image() that have no unique identifier to be used as drag source, by manufacturing a temporary identifier based on their window-relative position. This is extremely unusual within the dear imgui ecosystem and so we made it explicit.
	ImGuiDragDropFlags_SourceExtern                 = 1 << 4,   // External source (from outside of dear imgui), won't attempt to read current item/window info. Will always return true. Only one Extern source can be active simultaneously.
	ImGuiDragDropFlags_SourceAutoExpirePayload      = 1 << 5,   // Automatically expire the payload if the source cease to be submitted (otherwise payloads are persisting while being dragged)
	// AcceptDragDropPayload() flags
	ImGuiDragDropFlags_AcceptBeforeDelivery         = 1 << 10,  // AcceptDragDropPayload() will returns true even before the mouse button is released. You can then call IsDelivery() to test if the payload needs to be delivered.
	ImGuiDragDropFlags_AcceptNoDrawDefaultRect      = 1 << 11,  // Do not draw the default highlight rectangle when hovering over target.
	ImGuiDragDropFlags_AcceptNoPreviewTooltip       = 1 << 12,  // Request hiding the BeginDragDropSource tooltip from the BeginDragDropTarget site.
	ImGuiDragDropFlags_AcceptPeekOnly               = ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect  // For peeking ahead and inspecting the payload before delivery.
};

// Standard Drag and Drop payload types. You can define you own payload types using short strings. Types starting with '_' are defined by Dear ImGui.
#define IMGUI_PAYLOAD_TYPE_COLOR_3F     "_COL3F"    // float[3]: Standard type for colors, without alpha. User code may use this type.
#define IMGUI_PAYLOAD_TYPE_COLOR_4F     "_COL4F"    // float[4]: Standard type for colors. User code may use this type.

// A primary data type
enum ImGuiDataType_ {
	ImGuiDataType_S8,       // signed char / char (with sensible compilers)
	ImGuiDataType_U8,       // unsigned char
	ImGuiDataType_S16,      // short
	ImGuiDataType_U16,      // unsigned short
	ImGuiDataType_S32,      // int
	ImGuiDataType_U32,      // unsigned int
	ImGuiDataType_S64,      // long long / __int64
	ImGuiDataType_U64,      // unsigned long long / unsigned __int64
	ImGuiDataType_Float,    // float
	ImGuiDataType_Double,   // double
	ImGuiDataType_COUNT
};

// A cardinal direction
enum ImGuiDir_ {
	ImGuiDir_None    = -1,
	ImGuiDir_Left    = 0,
	ImGuiDir_Right   = 1,
	ImGuiDir_Up      = 2,
	ImGuiDir_Down    = 3,
	ImGuiDir_COUNT
};

// User fill ImGuiIO.KeyMap[] array with indices into the ImGuiIO.KeysDown[512] array
enum ImGuiKey_ {
	ImGuiKey_Tab,
	ImGuiKey_LeftArrow,
	ImGuiKey_RightArrow,
	ImGuiKey_UpArrow,
	ImGuiKey_DownArrow,
	ImGuiKey_PageUp,
	ImGuiKey_PageDown,
	ImGuiKey_Home,
	ImGuiKey_End,
	ImGuiKey_Insert,
	ImGuiKey_Delete,
	ImGuiKey_Backspace,
	ImGuiKey_Space,
	ImGuiKey_Enter,
	ImGuiKey_Escape,
	ImGuiKey_KeyPadEnter,
	ImGuiKey_A,         // for text edit CTRL+A: select all
	ImGuiKey_C,         // for text edit CTRL+C: copy
	ImGuiKey_V,         // for text edit CTRL+V: paste
	ImGuiKey_X,         // for text edit CTRL+X: cut
	ImGuiKey_Y,         // for text edit CTRL+Y: redo
	ImGuiKey_Z,         // for text edit CTRL+Z: undo
	ImGuiKey_COUNT
};

// Gamepad/Keyboard directional navigation
// Keyboard: Set io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard to enable. NewFrame() will automatically fill io.NavInputs[] based on your io.KeysDown[] + io.KeyMap[] arrays.
// Gamepad:  Set io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad to enable. Back-end: set ImGuiBackendFlags_HasGamepad and fill the io.NavInputs[] fields before calling NewFrame(). Note that io.NavInputs[] is cleared by EndFrame().
// Read instructions in imgui.cpp for more details. Download PNG/PSD at http://goo.gl/9LgVZW.
enum ImGuiNavInput_ {
	// Gamepad Mapping
	ImGuiNavInput_Activate,      // activate / open / toggle / tweak value       // e.g. Cross  (PS4), A (Xbox), A (Switch), Space (Keyboard)
	ImGuiNavInput_Cancel,        // cancel / close / exit                        // e.g. Circle (PS4), B (Xbox), B (Switch), Escape (Keyboard)
	ImGuiNavInput_Input,         // text input / on-screen keyboard              // e.g. Triang.(PS4), Y (Xbox), X (Switch), Return (Keyboard)
	ImGuiNavInput_Menu,          // tap: toggle menu / hold: focus, move, resize // e.g. Square (PS4), X (Xbox), Y (Switch), Alt (Keyboard)
	ImGuiNavInput_DpadLeft,      // move / tweak / resize window (w/ PadMenu)    // e.g. D-pad Left/Right/Up/Down (Gamepads), Arrow keys (Keyboard)
	ImGuiNavInput_DpadRight,     //
	ImGuiNavInput_DpadUp,        //
	ImGuiNavInput_DpadDown,      //
	ImGuiNavInput_LStickLeft,    // scroll / move window (w/ PadMenu)            // e.g. Left Analog Stick Left/Right/Up/Down
	ImGuiNavInput_LStickRight,   //
	ImGuiNavInput_LStickUp,      //
	ImGuiNavInput_LStickDown,    //
	ImGuiNavInput_FocusPrev,     // next window (w/ PadMenu)                     // e.g. L1 or L2 (PS4), LB or LT (Xbox), L or ZL (Switch)
	ImGuiNavInput_FocusNext,     // prev window (w/ PadMenu)                     // e.g. R1 or R2 (PS4), RB or RT (Xbox), R or ZL (Switch)
	ImGuiNavInput_TweakSlow,     // slower tweaks                                // e.g. L1 or L2 (PS4), LB or LT (Xbox), L or ZL (Switch)
	ImGuiNavInput_TweakFast,     // faster tweaks                                // e.g. R1 or R2 (PS4), RB or RT (Xbox), R or ZL (Switch)

	// [Internal] Don't use directly! This is used internally to differentiate keyboard from gamepad inputs for behaviors that require to differentiate them.
	// Keyboard behavior that have no corresponding gamepad mapping (e.g. CTRL+TAB) will be directly reading from io.KeysDown[] instead of io.NavInputs[].
	ImGuiNavInput_KeyMenu_,      // toggle menu                                  // = io.KeyAlt
	ImGuiNavInput_KeyTab_,       // tab                                          // = Tab key
	ImGuiNavInput_KeyLeft_,      // move left                                    // = Arrow keys
	ImGuiNavInput_KeyRight_,     // move right
	ImGuiNavInput_KeyUp_,        // move up
	ImGuiNavInput_KeyDown_,      // move down
	ImGuiNavInput_COUNT,
	ImGuiNavInput_InternalStart_ = ImGuiNavInput_KeyMenu_
};

// Configuration flags stored in io.ConfigFlags. Set by user/application.
enum ImGuiConfigFlags_ {
	ImGuiConfigFlags_None                   = 0,
	ImGuiConfigFlags_NavEnableKeyboard      = 1 << 0,   // Master keyboard navigation enable flag. NewFrame() will automatically fill io.NavInputs[] based on io.KeysDown[].
	ImGuiConfigFlags_NavEnableGamepad       = 1 << 1,   // Master gamepad navigation enable flag. This is mostly to instruct your imgui back-end to fill io.NavInputs[]. Back-end also needs to set ImGuiBackendFlags_HasGamepad.
	ImGuiConfigFlags_NavEnableSetMousePos   = 1 << 2,   // Instruct navigation to move the mouse cursor. May be useful on TV/console systems where moving a virtual mouse is awkward. Will update io.MousePos and set io.WantSetMousePos=true. If enabled you MUST honor io.WantSetMousePos requests in your binding, otherwise ImGui will react as if the mouse is jumping around back and forth.
	ImGuiConfigFlags_NavNoCaptureKeyboard   = 1 << 3,   // Instruct navigation to not set the io.WantCaptureKeyboard flag when io.NavActive is set.
	ImGuiConfigFlags_NoMouse                = 1 << 4,   // Instruct imgui to clear mouse position/buttons in NewFrame(). This allows ignoring the mouse information set by the back-end.
	ImGuiConfigFlags_NoMouseCursorChange    = 1 << 5,   // Instruct back-end to not alter mouse cursor shape and visibility. Use if the back-end cursor changes are interfering with yours and you don't want to use SetMouseCursor() to change mouse cursor. You may want to honor requests from imgui by reading GetMouseCursor() yourself instead.

	// User storage (to allow your back-end/engine to communicate to code that may be shared between multiple projects. Those flags are not used by core Dear ImGui)
	ImGuiConfigFlags_IsSRGB                 = 1 << 20,  // Application is SRGB-aware.
	ImGuiConfigFlags_IsTouchScreen          = 1 << 21   // Application is using a touch screen instead of a mouse.
};

// Back-end capabilities flags stored in io.BackendFlags. Set by imgui_impl_xxx or custom back-end.
enum ImGuiBackendFlags_ {
	ImGuiBackendFlags_None                  = 0,
	ImGuiBackendFlags_HasGamepad            = 1 << 0,   // Back-end Platform supports gamepad and currently has one connected.
	ImGuiBackendFlags_HasMouseCursors       = 1 << 1,   // Back-end Platform supports honoring GetMouseCursor() value to change the OS cursor shape.
	ImGuiBackendFlags_HasSetMousePos        = 1 << 2,   // Back-end Platform supports io.WantSetMousePos requests to reposition the OS mouse position (only used if ImGuiConfigFlags_NavEnableSetMousePos is set).
	ImGuiBackendFlags_RendererHasVtxOffset  = 1 << 3    // Back-end Renderer supports ImDrawCmd::VtxOffset. This enables output of large meshes (64K+ vertices) while still using 16-bits indices.
};

// Enumeration for PushStyleColor() / PopStyleColor()
enum ImGuiCol_ {
	ImGuiCol_Text,
	ImGuiCol_TextDisabled,
	ImGuiCol_WindowBg,              // Background of normal windows
	ImGuiCol_ChildBg,               // Background of child windows
	ImGuiCol_PopupBg,               // Background of popups, menus, tooltips windows
	ImGuiCol_Border,
	ImGuiCol_BorderShadow,
	ImGuiCol_FrameBg,               // Background of checkbox, radio button, plot, slider, text input
	ImGuiCol_FrameBgHovered,
	ImGuiCol_FrameBgActive,
	ImGuiCol_TitleBg,
	ImGuiCol_TitleBgActive,
	ImGuiCol_TitleBgCollapsed,
	ImGuiCol_MenuBarBg,
	ImGuiCol_ScrollbarBg,
	ImGuiCol_ScrollbarGrab,
	ImGuiCol_ScrollbarGrabHovered,
	ImGuiCol_ScrollbarGrabActive,
	ImGuiCol_CheckMark,
	ImGuiCol_SliderGrab,
	ImGuiCol_SliderGrabActive,
	ImGuiCol_Button,
	ImGuiCol_ButtonHovered,
	ImGuiCol_ButtonActive,
	ImGuiCol_Header,                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
	ImGuiCol_HeaderHovered,
	ImGuiCol_HeaderActive,
	ImGuiCol_Separator,
	ImGuiCol_SeparatorHovered,
	ImGuiCol_SeparatorActive,
	ImGuiCol_ResizeGrip,
	ImGuiCol_ResizeGripHovered,
	ImGuiCol_ResizeGripActive,
	ImGuiCol_Tab,
	ImGuiCol_TabHovered,
	ImGuiCol_TabActive,
	ImGuiCol_TabUnfocused,
	ImGuiCol_TabUnfocusedActive,
	ImGuiCol_PlotLines,
	ImGuiCol_PlotLinesHovered,
	ImGuiCol_PlotHistogram,
	ImGuiCol_PlotHistogramHovered,
	ImGuiCol_TextSelectedBg,
	ImGuiCol_DragDropTarget,
	ImGuiCol_NavHighlight,          // Gamepad/keyboard: current highlighted item
	ImGuiCol_NavWindowingHighlight, // Highlight window when using CTRL+TAB
	ImGuiCol_NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
	ImGuiCol_ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
	ImGuiCol_COUNT

	// Obsolete names (will be removed)
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
	, ImGuiCol_ModalWindowDarkening = ImGuiCol_ModalWindowDimBg                      // [renamed in 1.63]
	, ImGuiCol_ChildWindowBg = ImGuiCol_ChildBg                                      // [renamed in 1.53]
	                           //ImGuiCol_CloseButton, ImGuiCol_CloseButtonActive, ImGuiCol_CloseButtonHovered, // [unused since 1.60+] the close button now uses regular button colors.
	                           //ImGuiCol_ComboBg,                                                              // [unused since 1.53+] ComboBg has been merged with PopupBg, so a redirect isn't accurate.
#endif
};

// Enumeration for PushStyleVar() / PopStyleVar() to temporarily modify the ImGuiStyle structure.
// NB: the enum only refers to fields of ImGuiStyle which makes sense to be pushed/popped inside UI code. During initialization, feel free to just poke into ImGuiStyle directly.
// NB: if changing this enum, you need to update the associated internal table GStyleVarInfo[] accordingly. This is where we link enum values to members offset/type.
enum ImGuiStyleVar_ {
	// Enum name --------------------- // Member in ImGuiStyle structure (see ImGuiStyle for descriptions)
	ImGuiStyleVar_Alpha,               // float     Alpha
	ImGuiStyleVar_WindowPadding,       // ImVec2    WindowPadding
	ImGuiStyleVar_WindowRounding,      // float     WindowRounding
	ImGuiStyleVar_WindowBorderSize,    // float     WindowBorderSize
	ImGuiStyleVar_WindowMinSize,       // ImVec2    WindowMinSize
	ImGuiStyleVar_WindowTitleAlign,    // ImVec2    WindowTitleAlign
	ImGuiStyleVar_ChildRounding,       // float     ChildRounding
	ImGuiStyleVar_ChildBorderSize,     // float     ChildBorderSize
	ImGuiStyleVar_PopupRounding,       // float     PopupRounding
	ImGuiStyleVar_PopupBorderSize,     // float     PopupBorderSize
	ImGuiStyleVar_FramePadding,        // ImVec2    FramePadding
	ImGuiStyleVar_FrameRounding,       // float     FrameRounding
	ImGuiStyleVar_FrameBorderSize,     // float     FrameBorderSize
	ImGuiStyleVar_ItemSpacing,         // ImVec2    ItemSpacing
	ImGuiStyleVar_ItemInnerSpacing,    // ImVec2    ItemInnerSpacing
	ImGuiStyleVar_IndentSpacing,       // float     IndentSpacing
	ImGuiStyleVar_ScrollbarSize,       // float     ScrollbarSize
	ImGuiStyleVar_ScrollbarRounding,   // float     ScrollbarRounding
	ImGuiStyleVar_GrabMinSize,         // float     GrabMinSize
	ImGuiStyleVar_GrabRounding,        // float     GrabRounding
	ImGuiStyleVar_TabRounding,         // float     TabRounding
	ImGuiStyleVar_ButtonTextAlign,     // ImVec2    ButtonTextAlign
	ImGuiStyleVar_SelectableTextAlign, // ImVec2    SelectableTextAlign
	ImGuiStyleVar_COUNT

	// Obsolete names (will be removed)
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
	, ImGuiStyleVar_Count_ = ImGuiStyleVar_COUNT                        // [renamed in 1.60]
	, ImGuiStyleVar_ChildWindowRounding = ImGuiStyleVar_ChildRounding   // [renamed in 1.53]
#endif
};

// Flags for ColorEdit3() / ColorEdit4() / ColorPicker3() / ColorPicker4() / ColorButton()
enum ImGuiColorEditFlags_ {
	ImGuiColorEditFlags_None            = 0,
	ImGuiColorEditFlags_NoAlpha         = 1 << 1,   //              // ColorEdit, ColorPicker, ColorButton: ignore Alpha component (will only read 3 components from the input pointer).
	ImGuiColorEditFlags_NoPicker        = 1 << 2,   //              // ColorEdit: disable picker when clicking on colored square.
	ImGuiColorEditFlags_NoOptions       = 1 << 3,   //              // ColorEdit: disable toggling options menu when right-clicking on inputs/small preview.
	ImGuiColorEditFlags_NoSmallPreview  = 1 << 4,   //              // ColorEdit, ColorPicker: disable colored square preview next to the inputs. (e.g. to show only the inputs)
	ImGuiColorEditFlags_NoInputs        = 1 << 5,   //              // ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to show only the small preview colored square).
	ImGuiColorEditFlags_NoTooltip       = 1 << 6,   //              // ColorEdit, ColorPicker, ColorButton: disable tooltip when hovering the preview.
	ImGuiColorEditFlags_NoLabel         = 1 << 7,   //              // ColorEdit, ColorPicker: disable display of inline text label (the label is still forwarded to the tooltip and picker).
	ImGuiColorEditFlags_NoSidePreview   = 1 << 8,   //              // ColorPicker: disable bigger color preview on right side of the picker, use small colored square preview instead.
	ImGuiColorEditFlags_NoDragDrop      = 1 << 9,   //              // ColorEdit: disable drag and drop target. ColorButton: disable drag and drop source.

	// User Options (right-click on widget to change some of them).
	ImGuiColorEditFlags_AlphaBar        = 1 << 16,  //              // ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.
	ImGuiColorEditFlags_AlphaPreview    = 1 << 17,  //              // ColorEdit, ColorPicker, ColorButton: display preview as a transparent color over a checkerboard, instead of opaque.
	ImGuiColorEditFlags_AlphaPreviewHalf = 1 << 18, //              // ColorEdit, ColorPicker, ColorButton: display half opaque / half checkerboard, instead of opaque.
	ImGuiColorEditFlags_HDR             = 1 << 19,  //              // (WIP) ColorEdit: Currently only disable 0.0f..1.0f limits in RGBA edition (note: you probably want to use ImGuiColorEditFlags_Float flag as well).
	ImGuiColorEditFlags_DisplayRGB      = 1 << 20,  // [Display]    // ColorEdit: override _display_ type among RGB/HSV/Hex. ColorPicker: select any combination using one or more of RGB/HSV/Hex.
	ImGuiColorEditFlags_DisplayHSV      = 1 << 21,  // [Display]    // "
	ImGuiColorEditFlags_DisplayHex      = 1 << 22,  // [Display]    // "
	ImGuiColorEditFlags_Uint8           = 1 << 23,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0..255.
	ImGuiColorEditFlags_Float           = 1 << 24,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0.0f..1.0f floats instead of 0..255 integers. No round-trip of value via integers.
	ImGuiColorEditFlags_PickerHueBar    = 1 << 25,  // [Picker]     // ColorPicker: bar for Hue, rectangle for Sat/Value.
	ImGuiColorEditFlags_PickerHueWheel  = 1 << 26,  // [Picker]     // ColorPicker: wheel for Hue, triangle for Sat/Value.
	ImGuiColorEditFlags_InputRGB        = 1 << 27,  // [Input]      // ColorEdit, ColorPicker: input and output data in RGB format.
	ImGuiColorEditFlags_InputHSV        = 1 << 28,  // [Input]      // ColorEdit, ColorPicker: input and output data in HSV format.

	// Defaults Options. You can set application defaults using SetColorEditOptions(). The intent is that you probably don't want to
	// override them in most of your calls. Let the user choose via the option menu and/or call SetColorEditOptions() once during startup.
	ImGuiColorEditFlags__OptionsDefault = ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar,

	// [Internal] Masks
	ImGuiColorEditFlags__DisplayMask    = ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_DisplayHex,
	ImGuiColorEditFlags__DataTypeMask   = ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_Float,
	ImGuiColorEditFlags__PickerMask     = ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_PickerHueBar,
	ImGuiColorEditFlags__InputMask      = ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_InputHSV

	                                      // Obsolete names (will be removed)
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
	, ImGuiColorEditFlags_RGB = ImGuiColorEditFlags_DisplayRGB, ImGuiColorEditFlags_HSV = ImGuiColorEditFlags_DisplayHSV, ImGuiColorEditFlags_HEX = ImGuiColorEditFlags_DisplayHex  // [renamed in 1.69]
#endif
};

// Enumeration for GetMouseCursor()
// User code may request binding to display given cursor by calling SetMouseCursor(), which is why we have some cursors that are marked unused here
enum ImGuiMouseCursor_ {
	ImGuiMouseCursor_None = -1,
	ImGuiMouseCursor_Arrow = 0,
	ImGuiMouseCursor_TextInput,         // When hovering over InputText, etc.
	ImGuiMouseCursor_ResizeAll,         // (Unused by Dear ImGui functions)
	ImGuiMouseCursor_ResizeNS,          // When hovering over an horizontal border
	ImGuiMouseCursor_ResizeEW,          // When hovering over a vertical border or a column
	ImGuiMouseCursor_ResizeNESW,        // When hovering over the bottom-left corner of a window
	ImGuiMouseCursor_ResizeNWSE,        // When hovering over the bottom-right corner of a window
	ImGuiMouseCursor_Hand,              // (Unused by Dear ImGui functions. Use for e.g. hyperlinks)
	ImGuiMouseCursor_COUNT

	// Obsolete names (will be removed)
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
	, ImGuiMouseCursor_Count_ = ImGuiMouseCursor_COUNT      // [renamed in 1.60]
#endif
};

// Enumateration for ImGui::SetWindow***(), SetNextWindow***(), SetNextItem***() functions
// Represent a condition.
// Important: Treat as a regular enum! Do NOT combine multiple values using binary operators! All the functions above treat 0 as a shortcut to ImGuiCond_Always.
enum ImGuiCond_ {
	ImGuiCond_Always        = 1 << 0,   // Set the variable
	ImGuiCond_Once          = 1 << 1,   // Set the variable once per runtime session (only the first call with succeed)
	ImGuiCond_FirstUseEver  = 1 << 2,   // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
	ImGuiCond_Appearing     = 1 << 3    // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
};

//-----------------------------------------------------------------------------
// Helpers: Memory allocations macros
// IM_MALLOC(), IM_FREE(), IM_NEW(), IM_PLACEMENT_NEW(), IM_DELETE()
// We call C++ constructor on own allocated memory via the placement "new(ptr) Type()" syntax.
// Defining a custom placement new() with a dummy parameter allows us to bypass including <new> which on some platforms complains when user has disabled exceptions.
//-----------------------------------------------------------------------------

struct ImNewDummy {};
inline void* operator new(size_t, ImNewDummy, void* ptr) { return ptr; }
inline void  operator delete(void*, ImNewDummy, void*)   {} // This is only required so we can use the symmetrical new()
#define IM_ALLOC(_SIZE)                     ImGui::MemAlloc(_SIZE)
#define IM_FREE(_PTR)                       ImGui::MemFree(_PTR)
#define IM_PLACEMENT_NEW(_PTR)              new(ImNewDummy(), _PTR)
#define IM_NEW(_TYPE)                       new(ImNewDummy(), ImGui::MemAlloc(sizeof(_TYPE))) _TYPE
template<typename T> void IM_DELETE(T* p)   { if (p) { p->~T(); ImGui::MemFree(p); } }

//-----------------------------------------------------------------------------
// Helper: ImVector<>
// Lightweight std::vector<>-like class to avoid dragging dependencies (also, some implementations of STL with debug enabled are absurdly slow, we bypass it so our code runs fast in debug).
// You generally do NOT need to care or use this ever. But we need to make it available in imgui.h because some of our data structures are relying on it.
// Important: clear() frees memory, resize(0) keep the allocated buffer. We use resize(0) a lot to intentionally recycle allocated buffers across frames and amortize our costs.
// Important: our implementation does NOT call C++ constructors/destructors, we treat everything as raw data! This is intentional but be extra mindful of that,
// do NOT use this class as a std::vector replacement in your own code! Many of the structures used by dear imgui can be safely initialized by a zero-memset.
//-----------------------------------------------------------------------------

template<typename T>
struct ImVector {
	int                 Size;
	int                 Capacity;
	T*                  Data;

	// Provide standard typedefs but we don't use them ourselves.
	typedef T                   value_type;
	typedef value_type*         iterator;
	typedef const value_type*   const_iterator;

	// Constructors, destructor
	inline ImVector()                                       { Size = Capacity = 0; Data = NULL; }
	inline ImVector(const ImVector<T>& src)                 { Size = Capacity = 0; Data = NULL; operator=(src); }
	inline ImVector<T>& operator=(const ImVector<T>& src)   { clear(); resize(src.Size); memcpy(Data, src.Data, (size_t)Size * sizeof(T)); return *this; }
	inline ~ImVector()                                      { if (Data) IM_FREE(Data); }

	inline bool         empty() const                       { return Size == 0; }
	inline int          size() const                        { return Size; }
	inline int          size_in_bytes() const               { return Size * (int)sizeof(T); }
	inline int          capacity() const                    { return Capacity; }
	inline T&           operator[](int i)                   { IM_ASSERT(i < Size); return Data[i]; }
	inline const T&     operator[](int i) const             { IM_ASSERT(i < Size); return Data[i]; }

	inline void         clear()                             { if (Data) { Size = Capacity = 0; IM_FREE(Data); Data = NULL; } }
	inline T*           begin()                             { return Data; }
	inline const T*     begin() const                       { return Data; }
	inline T*           end()                               { return Data + Size; }
	inline const T*     end() const                         { return Data + Size; }
	inline T&           front()                             { IM_ASSERT(Size > 0); return Data[0]; }
	inline const T&     front() const                       { IM_ASSERT(Size > 0); return Data[0]; }
	inline T&           back()                              { IM_ASSERT(Size > 0); return Data[Size - 1]; }
	inline const T&     back() const                        { IM_ASSERT(Size > 0); return Data[Size - 1]; }
	inline void         swap(ImVector<T>& rhs)              { int rhs_size = rhs.Size; rhs.Size = Size; Size = rhs_size; int rhs_cap = rhs.Capacity; rhs.Capacity = Capacity; Capacity = rhs_cap; T* rhs_data = rhs.Data; rhs.Data = Data; Data = rhs_data; }

	inline int          _grow_capacity(int sz) const        { int new_capacity = Capacity ? (Capacity + Capacity / 2) : 8; return new_capacity > sz ? new_capacity : sz; }
	inline void         resize(int new_size)                { if (new_size > Capacity) reserve(_grow_capacity(new_size)); Size = new_size; }
	inline void         resize(int new_size, const T& v)    { if (new_size > Capacity) reserve(_grow_capacity(new_size)); if (new_size > Size) for (int n = Size; n < new_size; n++) memcpy(&Data[n], &v, sizeof(v)); Size = new_size; }
	inline void         reserve(int new_capacity)           { if (new_capacity <= Capacity) return; T* new_data = (T*)IM_ALLOC((size_t)new_capacity * sizeof(T)); if (Data) { memcpy(new_data, Data, (size_t)Size * sizeof(T)); IM_FREE(Data); } Data = new_data; Capacity = new_capacity; }

	// NB: It is illegal to call push_back/push_front/insert with a reference pointing inside the ImVector data itself! e.g. v.push_back(v[10]) is forbidden.
	inline void         push_back(const T& v)               { if (Size == Capacity) reserve(_grow_capacity(Size + 1)); memcpy(&Data[Size], &v, sizeof(v)); Size++; }
	inline void         pop_back()                          { IM_ASSERT(Size > 0); Size--; }
	inline void         push_front(const T& v)              { if (Size == 0) push_back(v); else insert(Data, v); }
	inline T*           erase(const T* it)                  { IM_ASSERT(it >= Data && it < Data + Size); const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + 1, ((size_t)Size - (size_t)off - 1) * sizeof(T)); Size--; return Data + off; }
	inline T*           erase(const T* it, const T* it_last) { IM_ASSERT(it >= Data && it < Data + Size && it_last > it && it_last <= Data + Size); const ptrdiff_t count = it_last - it; const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + count, ((size_t)Size - (size_t)off - count) * sizeof(T)); Size -= (int)count; return Data + off; }
	inline T*           erase_unsorted(const T* it)         { IM_ASSERT(it >= Data && it < Data + Size);  const ptrdiff_t off = it - Data; if (it < Data + Size - 1) memcpy(Data + off, Data + Size - 1, sizeof(T)); Size--; return Data + off; }
	inline T*           insert(const T* it, const T& v)     { IM_ASSERT(it >= Data && it <= Data + Size); const ptrdiff_t off = it - Data; if (Size == Capacity) reserve(_grow_capacity(Size + 1)); if (off < (int)Size) memmove(Data + off + 1, Data + off, ((size_t)Size - (size_t)off) * sizeof(T)); memcpy(&Data[off], &v, sizeof(v)); Size++; return Data + off; }
	inline bool         contains(const T& v) const          { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data++ == v) return true; return false; }
		inline T*           find(const T& v)                    { T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data == v) break; else ++data; return data; }
		inline const T*     find(const T& v) const              { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data == v) break; else ++data; return data; }
	inline bool         find_erase(const T& v)              { const T* it = find(v); if (it < Data + Size) { erase(it); return true; } return false; }
	inline bool         find_erase_unsorted(const T& v)     { const T* it = find(v); if (it < Data + Size) { erase_unsorted(it); return true; } return false; }
	inline int          index_from_ptr(const T* it) const   { IM_ASSERT(it >= Data && it <= Data + Size); const ptrdiff_t off = it - Data; return (int)off; }
};

//-----------------------------------------------------------------------------
// ImGuiStyle
// You may modify the ImGui::GetStyle() main instance during initialization and before NewFrame().
// During the frame, use ImGui::PushStyleVar(ImGuiStyleVar_XXXX)/PopStyleVar() to alter the main style values,
// and ImGui::PushStyleColor(ImGuiCol_XXX)/PopStyleColor() for colors.
//-----------------------------------------------------------------------------

struct ImGuiStyle {
	float       Alpha;                      // Global alpha applies to everything in Dear ImGui.
	ImVec2      WindowPadding;              // Padding within a window.
	float       WindowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows.
	float       WindowBorderSize;           // Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
	ImVec2      WindowMinSize;              // Minimum window size. This is a global setting. If you want to constraint individual windows, use SetNextWindowSizeConstraints().
	ImVec2      WindowTitleAlign;           // Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
	ImGuiDir    WindowMenuButtonPosition;   // Side of the collapsing/docking button in the title bar (None/Left/Right). Defaults to ImGuiDir_Left.
	float       ChildRounding;              // Radius of child window corners rounding. Set to 0.0f to have rectangular windows.
	float       ChildBorderSize;            // Thickness of border around child windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
	float       PopupRounding;              // Radius of popup window corners rounding. (Note that tooltip windows use WindowRounding)
	float       PopupBorderSize;            // Thickness of border around popup/tooltip windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
	ImVec2      FramePadding;               // Padding within a framed rectangle (used by most widgets).
	float       FrameRounding;              // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
	float       FrameBorderSize;            // Thickness of border around frames. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
	ImVec2      ItemSpacing;                // Horizontal and vertical spacing between widgets/lines.
	ImVec2      ItemInnerSpacing;           // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
	ImVec2      TouchExtraPadding;          // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
	float       IndentSpacing;              // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
	float       ColumnsMinSpacing;          // Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1).
	float       ScrollbarSize;              // Width of the vertical scrollbar, Height of the horizontal scrollbar.
	float       ScrollbarRounding;          // Radius of grab corners for scrollbar.
	float       GrabMinSize;                // Minimum width/height of a grab box for slider/scrollbar.
	float       GrabRounding;               // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
	float       TabRounding;                // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
	float       TabBorderSize;              // Thickness of border around tabs.
	ImGuiDir    ColorButtonPosition;        // Side of the color button in the ColorEdit4 widget (left/right). Defaults to ImGuiDir_Right.
	ImVec2      ButtonTextAlign;            // Alignment of button text when button is larger than text. Defaults to (0.5f, 0.5f) (centered).
	ImVec2      SelectableTextAlign;        // Alignment of selectable text when selectable is larger than text. Defaults to (0.0f, 0.0f) (top-left aligned).
	ImVec2      DisplayWindowPadding;       // Window position are clamped to be visible within the display area by at least this amount. Only applies to regular windows.
	ImVec2      DisplaySafeAreaPadding;     // If you cannot see the edges of your screen (e.g. on a TV) increase the safe area padding. Apply to popups/tooltips as well regular windows. NB: Prefer configuring your TV sets correctly!
	float       MouseCursorScale;           // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
	bool        AntiAliasedLines;           // Enable anti-aliasing on lines/borders. Disable if you are really tight on CPU/GPU.
	bool        AntiAliasedFill;            // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
	float       CurveTessellationTol;       // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
	ImVec4      Colors[ImGuiCol_COUNT];

	IMGUI_API ImGuiStyle();
	IMGUI_API void ScaleAllSizes(float scale_factor);
};

//-----------------------------------------------------------------------------
// ImGuiIO
// Communicate most settings and inputs/outputs to Dear ImGui using this structure.
// Access via ImGui::GetIO(). Read 'Programmer guide' section in .cpp file for general usage.
//-----------------------------------------------------------------------------

struct ImGuiIO {
	//------------------------------------------------------------------
	// Configuration (fill once)                // Default value
	//------------------------------------------------------------------

	ImGuiConfigFlags   ConfigFlags;             // = 0              // See ImGuiConfigFlags_ enum. Set by user/application. Gamepad/keyboard navigation options, etc.
	ImGuiBackendFlags  BackendFlags;            // = 0              // See ImGuiBackendFlags_ enum. Set by back-end (imgui_impl_xxx files or custom back-end) to communicate features supported by the back-end.
	ImVec2      DisplaySize;                    // <unset>          // Main display size, in pixels.
	float       DeltaTime;                      // = 1.0f/60.0f     // Time elapsed since last frame, in seconds.
	float       IniSavingRate;                  // = 5.0f           // Minimum time between saving positions/sizes to .ini file, in seconds.
	const char* IniFilename;                    // = "imgui.ini"    // Path to .ini file. Set NULL to disable automatic .ini loading/saving, if e.g. you want to manually load/save from memory.
	const char* LogFilename;                    // = "imgui_log.txt"// Path to .log file (default parameter to ImGui::LogToFile when no file is specified).
	float       MouseDoubleClickTime;           // = 0.30f          // Time for a double-click, in seconds.
	float       MouseDoubleClickMaxDist;        // = 6.0f           // Distance threshold to stay in to validate a double-click, in pixels.
	float       MouseDragThreshold;             // = 6.0f           // Distance threshold before considering we are dragging.
	int         KeyMap[ImGuiKey_COUNT];         // <unset>          // Map of indices into the KeysDown[512] entries array which represent your "native" keyboard state.
	float       KeyRepeatDelay;                 // = 0.250f         // When holding a key/button, time before it starts repeating, in seconds (for buttons in Repeat mode, etc.).
	float       KeyRepeatRate;                  // = 0.050f         // When holding a key/button, rate at which it repeats, in seconds.
	void*       UserData;                       // = NULL           // Store your own data for retrieval by callbacks.

	ImFontAtlas*Fonts;                          // <auto>           // Font atlas: load, rasterize and pack one or more fonts into a single texture.
	float       FontGlobalScale;                // = 1.0f           // Global scale all fonts
	bool        FontAllowUserScaling;           // = false          // Allow user scaling text of individual window with CTRL+Wheel.
	ImFont*     FontDefault;                    // = NULL           // Font to use on NewFrame(). Use NULL to uses Fonts->Fonts[0].
	ImVec2      DisplayFramebufferScale;        // = (1, 1)         // For retina display or other situations where window coordinates are different from framebuffer coordinates. This generally ends up in ImDrawData::FramebufferScale.

	// Miscellaneous options
	bool        MouseDrawCursor;                // = false          // Request ImGui to draw a mouse cursor for you (if you are on a platform without a mouse cursor). Cannot be easily renamed to 'io.ConfigXXX' because this is frequently used by back-end implementations.
	bool        ConfigMacOSXBehaviors;          // = defined(__APPLE__) // OS X style: Text editing cursor movement using Alt instead of Ctrl, Shortcuts using Cmd/Super instead of Ctrl, Line/Text Start and End using Cmd+Arrows instead of Home/End, Double click selects by word instead of selecting whole text, Multi-selection in lists uses Cmd/Super instead of Ctrl (was called io.OptMacOSXBehaviors prior to 1.63)
	bool        ConfigInputTextCursorBlink;     // = true           // Set to false to disable blinking cursor, for users who consider it distracting. (was called: io.OptCursorBlink prior to 1.63)
	bool        ConfigWindowsResizeFromEdges;   // = true           // Enable resizing of windows from their edges and from the lower-left corner. This requires (io.BackendFlags & ImGuiBackendFlags_HasMouseCursors) because it needs mouse cursor feedback. (This used to be a per-window ImGuiWindowFlags_ResizeFromAnySide flag)
	bool        ConfigWindowsMoveFromTitleBarOnly; // = false       // [BETA] Set to true to only allow moving windows when clicked+dragged from the title bar. Windows without a title bar are not affected.
	float       ConfigWindowsMemoryCompactTimer;// = 60.0f          // [BETA] Compact window memory usage when unused. Set to -1.0f to disable.

	//------------------------------------------------------------------
	// Platform Functions
	// (the imgui_impl_xxxx back-end files are setting those up for you)
	//------------------------------------------------------------------

	// Optional: Platform/Renderer back-end name (informational only! will be displayed in About Window) + User data for back-end/wrappers to store their own stuff.
	const char* BackendPlatformName;            // = NULL
	const char* BackendRendererName;            // = NULL
	void*       BackendPlatformUserData;        // = NULL
	void*       BackendRendererUserData;        // = NULL
	void*       BackendLanguageUserData;        // = NULL

	// Optional: Access OS clipboard
	// (default to use native Win32 clipboard on Windows, otherwise uses a private clipboard. Override to access OS clipboard on other architectures)
	const char* (*GetClipboardTextFn)(void* user_data);
	void        (*SetClipboardTextFn)(void* user_data, const char* text);
	void*       ClipboardUserData;

	// Optional: Notify OS Input Method Editor of the screen position of your cursor for text input position (e.g. when using Japanese/Chinese IME on Windows)
	// (default to use native imm32 api on Windows)
	void        (*ImeSetInputScreenPosFn)(int x, int y);
	void*       ImeWindowHandle;                // = NULL           // (Windows) Set this to your HWND to get automatic IME cursor positioning.

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
	// [OBSOLETE since 1.60+] Rendering function, will be automatically called in Render(). Please call your rendering function yourself now!
	// You can obtain the ImDrawData* by calling ImGui::GetDrawData() after Render(). See example applications if you are unsure of how to implement this.
	void        (*RenderDrawListsFn)(ImDrawData* data);
#else
	// This is only here to keep ImGuiIO the same size/layout, so that IMGUI_DISABLE_OBSOLETE_FUNCTIONS can exceptionally be used outside of imconfig.h.
	void*       RenderDrawListsFnUnused;
#endif

	//------------------------------------------------------------------
	// Input - Fill before calling NewFrame()
	//------------------------------------------------------------------

	ImVec2      MousePos;                       // Mouse position, in pixels. Set to ImVec2(-FLT_MAX,-FLT_MAX) if mouse is unavailable (on another screen, etc.)
	bool        MouseDown[5];                   // Mouse buttons: 0=left, 1=right, 2=middle + extras. ImGui itself mostly only uses left button (BeginPopupContext** are using right button). Others buttons allows us to track if the mouse is being used by your application + available to user as a convenience via IsMouse** API.
	float       MouseWheel;                     // Mouse wheel Vertical: 1 unit scrolls about 5 lines text.
	float       MouseWheelH;                    // Mouse wheel Horizontal. Most users don't have a mouse with an horizontal wheel, may not be filled by all back-ends.
	bool        KeyCtrl;                        // Keyboard modifier pressed: Control
	bool        KeyShift;                       // Keyboard modifier pressed: Shift
	bool        KeyAlt;                         // Keyboard modifier pressed: Alt
	bool        KeySuper;                       // Keyboard modifier pressed: Cmd/Super/Windows
	bool        KeysDown[512];                  // Keyboard keys that are pressed (ideally left in the "native" order your engine has access to keyboard keys, so you can use your own defines/enums for keys).
	float       NavInputs[ImGuiNavInput_COUNT]; // Gamepad inputs. Cleared back to zero by EndFrame(). Keyboard keys will be auto-mapped and be written here by NewFrame().

	// Functions
	IMGUI_API void  AddInputCharacter(unsigned int c);          // Queue new character input
	IMGUI_API void  AddInputCharactersUTF8(const char* str);    // Queue new characters input from an UTF-8 string
	IMGUI_API void  ClearInputCharacters();                     // Clear the text input buffer manually

	//------------------------------------------------------------------
	// Output - Retrieve after calling NewFrame()
	//------------------------------------------------------------------

	bool        WantCaptureMouse;               // When io.WantCaptureMouse is true, imgui will use the mouse inputs, do not dispatch them to your main game/application (in both cases, always pass on mouse inputs to imgui). (e.g. unclicked mouse is hovering over an imgui window, widget is active, mouse was clicked over an imgui window, etc.).
	bool        WantCaptureKeyboard;            // When io.WantCaptureKeyboard is true, imgui will use the keyboard inputs, do not dispatch them to your main game/application (in both cases, always pass keyboard inputs to imgui). (e.g. InputText active, or an imgui window is focused and navigation is enabled, etc.).
	bool        WantTextInput;                  // Mobile/console: when io.WantTextInput is true, you may display an on-screen keyboard. This is set by ImGui when it wants textual keyboard input to happen (e.g. when a InputText widget is active).
	bool        WantSetMousePos;                // MousePos has been altered, back-end should reposition mouse on next frame. Set only when ImGuiConfigFlags_NavEnableSetMousePos flag is enabled.
	bool        WantSaveIniSettings;            // When manual .ini load/save is active (io.IniFilename == NULL), this will be set to notify your application that you can call SaveIniSettingsToMemory() and save yourself. IMPORTANT: You need to clear io.WantSaveIniSettings yourself.
	bool        NavActive;                      // Directional navigation is currently allowed (will handle ImGuiKey_NavXXX events) = a window is focused and it doesn't use the ImGuiWindowFlags_NoNavInputs flag.
	bool        NavVisible;                     // Directional navigation is visible and allowed (will handle ImGuiKey_NavXXX events).
	float       Framerate;                      // Application framerate estimation, in frame per second. Solely for convenience. Rolling average estimation based on IO.DeltaTime over 120 frames
	int         MetricsRenderVertices;          // Vertices output during last call to Render()
	int         MetricsRenderIndices;           // Indices output during last call to Render() = number of triangles * 3
	int         MetricsRenderWindows;           // Number of visible windows
	int         MetricsActiveWindows;           // Number of active windows
	int         MetricsActiveAllocations;       // Number of active allocations, updated by MemAlloc/MemFree based on current context. May be off if you have multiple imgui contexts.
	ImVec2      MouseDelta;                     // Mouse delta. Note that this is zero if either current or previous position are invalid (-FLT_MAX,-FLT_MAX), so a disappearing/reappearing mouse won't have a huge delta.

	//------------------------------------------------------------------
	// [Internal] ImGui will maintain those fields. Forward compatibility not guaranteed!
	//------------------------------------------------------------------

	ImVec2      MousePosPrev;                   // Previous mouse position (note that MouseDelta is not necessary == MousePos-MousePosPrev, in case either position is invalid)
	ImVec2      MouseClickedPos[5];             // Position at time of clicking
	double      MouseClickedTime[5];            // Time of last click (used to figure out double-click)
	bool        MouseClicked[5];                // Mouse button went from !Down to Down
	bool        MouseDoubleClicked[5];          // Has mouse button been double-clicked?
	bool        MouseReleased[5];               // Mouse button went from Down to !Down
	bool        MouseDownOwned[5];              // Track if button was clicked inside a dear imgui window. We don't request mouse capture from the application if click started outside ImGui bounds.
	bool        MouseDownWasDoubleClick[5];     // Track if button down was a double-click
	float       MouseDownDuration[5];           // Duration the mouse button has been down (0.0f == just clicked)
	float       MouseDownDurationPrev[5];       // Previous time the mouse button has been down
	ImVec2      MouseDragMaxDistanceAbs[5];     // Maximum distance, absolute, on each axis, of how much mouse has traveled from the clicking point
	float       MouseDragMaxDistanceSqr[5];     // Squared maximum distance of how much mouse has traveled from the clicking point
	float       KeysDownDuration[512];          // Duration the keyboard key has been down (0.0f == just pressed)
	float       KeysDownDurationPrev[512];      // Previous duration the key has been down
	float       NavInputsDownDuration[ImGuiNavInput_COUNT];
	float       NavInputsDownDurationPrev[ImGuiNavInput_COUNT];
	ImVector<ImWchar> InputQueueCharacters;     // Queue of _characters_ input (obtained by platform back-end). Fill using AddInputCharacter() helper.

	IMGUI_API   ImGuiIO();
};

//-----------------------------------------------------------------------------
// Misc data structures
//-----------------------------------------------------------------------------

// Shared state of InputText(), passed as an argument to your callback when a ImGuiInputTextFlags_Callback* flag is used.
// The callback function should return 0 by default.
// Callbacks (follow a flag name and see comments in ImGuiInputTextFlags_ declarations for more details)
// - ImGuiInputTextFlags_CallbackCompletion:  Callback on pressing TAB
// - ImGuiInputTextFlags_CallbackHistory:     Callback on pressing Up/Down arrows
// - ImGuiInputTextFlags_CallbackAlways:      Callback on each iteration
// - ImGuiInputTextFlags_CallbackCharFilter:  Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
// - ImGuiInputTextFlags_CallbackResize:      Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow.
struct ImGuiInputTextCallbackData {
	ImGuiInputTextFlags EventFlag;      // One ImGuiInputTextFlags_Callback*    // Read-only
	ImGuiInputTextFlags Flags;          // What user passed to InputText()      // Read-only
	void*               UserData;       // What user passed to InputText()      // Read-only

	// Arguments for the different callback events
	// - To modify the text buffer in a callback, prefer using the InsertChars() / DeleteChars() function. InsertChars() will take care of calling the resize callback if necessary.
	// - If you know your edits are not going to resize the underlying buffer allocation, you may modify the contents of 'Buf[]' directly. You need to update 'BufTextLen' accordingly (0 <= BufTextLen < BufSize) and set 'BufDirty'' to true so InputText can update its internal state.
	ImWchar             EventChar;      // Character input                      // Read-write   // [CharFilter] Replace character with another one, or set to zero to drop. return 1 is equivalent to setting EventChar=0;
	ImGuiKey            EventKey;       // Key pressed (Up/Down/TAB)            // Read-only    // [Completion,History]
	char*               Buf;            // Text buffer                          // Read-write   // [Resize] Can replace pointer / [Completion,History,Always] Only write to pointed data, don't replace the actual pointer!
	int                 BufTextLen;     // Text length (in bytes)               // Read-write   // [Resize,Completion,History,Always] Exclude zero-terminator storage. In C land: == strlen(some_text), in C++ land: string.length()
	int                 BufSize;        // Buffer size (in bytes) = capacity+1  // Read-only    // [Resize,Completion,History,Always] Include zero-terminator storage. In C land == ARRAYSIZE(my_char_array), in C++ land: string.capacity()+1
	bool                BufDirty;       // Set if you modify Buf/BufTextLen!    // Write        // [Completion,History,Always]
	int                 CursorPos;      //                                      // Read-write   // [Completion,History,Always]
	int                 SelectionStart; //                                      // Read-write   // [Completion,History,Always] == to SelectionEnd when no selection)
	int                 SelectionEnd;   //                                      // Read-write   // [Completion,History,Always]

	// Helper functions for text manipulation.
	// Use those function to benefit from the CallbackResize behaviors. Calling those function reset the selection.
	IMGUI_API ImGuiInputTextCallbackData();
	IMGUI_API void      DeleteChars(int pos, int bytes_count);
	IMGUI_API void      InsertChars(int pos, const char* text, const char* text_end = NULL);
	bool                HasSelection() const { return SelectionStart != SelectionEnd; }
};

// Resizing callback data to apply custom constraint. As enabled by SetNextWindowSizeConstraints(). Callback is called during the next Begin().
// NB: For basic min/max size constraint on each axis you don't need to use the callback! The SetNextWindowSizeConstraints() parameters are enough.
struct ImGuiSizeCallbackData {
	void*   UserData;       // Read-only.   What user passed to SetNextWindowSizeConstraints()
	ImVec2  Pos;            // Read-only.   Window position, for reference.
	ImVec2  CurrentSize;    // Read-only.   Current window size.
	ImVec2  DesiredSize;    // Read-write.  Desired size, based on user's mouse position. Write to this field to restrain resizing.
};

// Data payload for Drag and Drop operations: AcceptDragDropPayload(), GetDragDropPayload()
struct ImGuiPayload {
	// Members
	void*           Data;               // Data (copied and owned by dear imgui)
	int             DataSize;           // Data size

	// [Internal]
	ImGuiID         SourceId;           // Source item id
	ImGuiID         SourceParentId;     // Source parent id (if available)
	int             DataFrameCount;     // Data timestamp
	char            DataType[32 + 1];   // Data type tag (short user-supplied string, 32 characters max)
	bool            Preview;            // Set when AcceptDragDropPayload() was called and mouse has been hovering the target item (nb: handle overlapping drag targets)
	bool            Delivery;           // Set when AcceptDragDropPayload() was called and mouse button is released over the target item.

	ImGuiPayload()  { Clear(); }
	void Clear()    { SourceId = SourceParentId = 0; Data = NULL; DataSize = 0; memset(DataType, 0, sizeof(DataType)); DataFrameCount = -1; Preview = Delivery = false; }
	bool IsDataType(const char* type) const { return DataFrameCount != -1 && strcmp(type, DataType) == 0; }
	bool IsPreview() const                  { return Preview; }
	bool IsDelivery() const                 { return Delivery; }
};

//-----------------------------------------------------------------------------
// Obsolete functions (Will be removed! Read 'API BREAKING CHANGES' section in imgui.cpp for details)
// Please keep your copy of dear imgui up to date! Occasionally set '#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS' in imconfig.h to stay ahead.
//-----------------------------------------------------------------------------

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
namespace ImGui {
	// OBSOLETED in 1.72 (from July 2019)
	static inline void  TreeAdvanceToLabelPos()               { SetCursorPosX(GetCursorPosX() + GetTreeNodeToLabelSpacing()); }
	// OBSOLETED in 1.71 (from June 2019)
	static inline void  SetNextTreeNodeOpen(bool open, ImGuiCond cond = 0) { SetNextItemOpen(open, cond); }
	// OBSOLETED in 1.70 (from May 2019)
	static inline float GetContentRegionAvailWidth()          { return GetContentRegionAvail().x; }
	// OBSOLETED in 1.69 (from Mar 2019)
	static inline ImDrawList* GetOverlayDrawList()            { return GetForegroundDrawList(); }
	// OBSOLETED in 1.66 (from Sep 2018)
	static inline void  SetScrollHere(float center_ratio = 0.5f) { SetScrollHereY(center_ratio); }
	// OBSOLETED in 1.63 (between Aug 2018 and Sept 2018)
	static inline bool  IsItemDeactivatedAfterChange()        { return IsItemDeactivatedAfterEdit(); }
	// OBSOLETED in 1.61 (between Apr 2018 and Aug 2018)
	IMGUI_API bool      InputFloat(const char* label, float* v, float step, float step_fast, int decimal_precision, ImGuiInputTextFlags flags = 0); // Use the 'const char* format' version instead of 'decimal_precision'!
	IMGUI_API bool      InputFloat2(const char* label, float v[2], int decimal_precision, ImGuiInputTextFlags flags = 0);
	IMGUI_API bool      InputFloat3(const char* label, float v[3], int decimal_precision, ImGuiInputTextFlags flags = 0);
	IMGUI_API bool      InputFloat4(const char* label, float v[4], int decimal_precision, ImGuiInputTextFlags flags = 0);
	// OBSOLETED in 1.60 (between Dec 2017 and Apr 2018)
	static inline bool  IsAnyWindowFocused()                  { return IsWindowFocused(ImGuiFocusedFlags_AnyWindow); }
	static inline bool  IsAnyWindowHovered()                  { return IsWindowHovered(ImGuiHoveredFlags_AnyWindow); }
	static inline ImVec2 CalcItemRectClosestPoint(const ImVec2& pos, bool on_edge = false, float outward = 0.f) { IM_UNUSED(on_edge); IM_UNUSED(outward); IM_ASSERT(0); return pos; }
	// OBSOLETED in 1.53 (between Oct 2017 and Dec 2017)
	static inline void  ShowTestWindow()                      { return ShowDemoWindow(); }
	static inline bool  IsRootWindowFocused()                 { return IsWindowFocused(ImGuiFocusedFlags_RootWindow); }
	static inline bool  IsRootWindowOrAnyChildFocused()       { return IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows); }
	static inline void  SetNextWindowContentWidth(float w)    { SetNextWindowContentSize(ImVec2(w, 0.0f)); }
	static inline float GetItemsLineHeightWithSpacing()       { return GetFrameHeightWithSpacing(); }
	// OBSOLETED in 1.52 (between Aug 2017 and Oct 2017)
	IMGUI_API bool      Begin(const char* name, bool* p_open, const ImVec2& size_on_first_use, float bg_alpha_override = -1.0f, ImGuiWindowFlags flags = 0); // Use SetNextWindowSize(size, ImGuiCond_FirstUseEver) + SetNextWindowBgAlpha() instead.
	static inline bool  IsRootWindowOrAnyChildHovered()       { return IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows); }
	static inline void  AlignFirstTextHeightToWidgets()       { AlignTextToFramePadding(); }
	static inline void  SetNextWindowPosCenter(ImGuiCond c = 0) { ImGuiIO& io = GetIO(); SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), c, ImVec2(0.5f, 0.5f)); }
}
typedef ImGuiInputTextCallback      ImGuiTextEditCallback;    // OBSOLETED in 1.63 (from Aug 2018): made the names consistent
typedef ImGuiInputTextCallbackData  ImGuiTextEditCallbackData;
#endif

//-----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

// Helper: Execute a block of code at maximum once a frame. Convenient if you want to quickly create an UI within deep-nested code that runs multiple times every frame.
// Usage: static ImGuiOnceUponAFrame oaf; if (oaf) ImGui::Text("This will be called only once per frame");
struct ImGuiOnceUponAFrame {
	ImGuiOnceUponAFrame() { RefFrame = -1; }
	mutable int RefFrame;
	operator bool() const { int current_frame = ImGui::GetFrameCount(); if (RefFrame == current_frame) return false; RefFrame = current_frame; return true; }
};

// Helper: Parse and apply text filters. In format "aaaaa[,bbbb][,ccccc]"
struct ImGuiTextFilter {
	IMGUI_API           ImGuiTextFilter(const char* default_filter = "");
	IMGUI_API bool      Draw(const char* label = "Filter (inc,-exc)", float width = 0.0f);  // Helper calling InputText+Build
	IMGUI_API bool      PassFilter(const char* text, const char* text_end = NULL) const;
	IMGUI_API void      Build();
	void                Clear()          { InputBuf[0] = 0; Build(); }
	bool                IsActive() const { return !Filters.empty(); }

	// [Internal]
	struct ImGuiTextRange {
		const char*     b;
		const char*     e;

		ImGuiTextRange()                                { b = e = NULL; }
		ImGuiTextRange(const char* _b, const char* _e)  { b = _b; e = _e; }
		bool            empty() const                   { return b == e; }
		IMGUI_API void  split(char separator, ImVector<ImGuiTextRange>* out) const;
	};
	char                    InputBuf[256];
	ImVector<ImGuiTextRange>Filters;
	int                     CountGrep;
};

// Helper: Growable text buffer for logging/accumulating text
// (this could be called 'ImGuiTextBuilder' / 'ImGuiStringBuilder')
struct ImGuiTextBuffer {
	ImVector<char>      Buf;
	IMGUI_API static char EmptyString[1];

	ImGuiTextBuffer()   { }
	inline char         operator[](int i)       { IM_ASSERT(Buf.Data != NULL); return Buf.Data[i]; }
	const char*         begin() const           { return Buf.Data ? &Buf.front() : EmptyString; }
	const char*         end() const             { return Buf.Data ? &Buf.back() : EmptyString; }   // Buf is zero-terminated, so end() will point on the zero-terminator
	int                 size() const            { return Buf.Size ? Buf.Size - 1 : 0; }
	bool                empty()                 { return Buf.Size <= 1; }
	void                clear()                 { Buf.clear(); }
	void                reserve(int capacity)   { Buf.reserve(capacity); }
	const char*         c_str() const           { return Buf.Data ? Buf.Data : EmptyString; }
	IMGUI_API void      append(const char* str, const char* str_end = NULL);
	IMGUI_API void      appendf(const char* fmt, ...) IM_FMTARGS(2);
	IMGUI_API void      appendfv(const char* fmt, va_list args) IM_FMTLIST(2);
};

// Helper: Key->Value storage
// Typically you don't have to worry about this since a storage is held within each Window.
// We use it to e.g. store collapse state for a tree (Int 0/1)
// This is optimized for efficient lookup (dichotomy into a contiguous buffer) and rare insertion (typically tied to user interactions aka max once a frame)
// You can use it as custom user storage for temporary values. Declare your own storage if, for example:
// - You want to manipulate the open/close state of a particular sub-tree in your interface (tree node uses Int 0/1 to store their state).
// - You want to store custom debug data easily without adding or editing structures in your code (probably not efficient, but convenient)
// Types are NOT stored, so it is up to you to make sure your Key don't collide with different types.
struct ImGuiStorage {
	// [Internal]
	struct ImGuiStoragePair {
		ImGuiID key;
		union { int val_i; float val_f; void* val_p; };
		ImGuiStoragePair(ImGuiID _key, int _val_i)      { key = _key; val_i = _val_i; }
		ImGuiStoragePair(ImGuiID _key, float _val_f)    { key = _key; val_f = _val_f; }
		ImGuiStoragePair(ImGuiID _key, void* _val_p)    { key = _key; val_p = _val_p; }
	};

	ImVector<ImGuiStoragePair>      Data;

	// - Get***() functions find pair, never add/allocate. Pairs are sorted so a query is O(log N)
	// - Set***() functions find pair, insertion on demand if missing.
	// - Sorted insertion is costly, paid once. A typical frame shouldn't need to insert any new pair.
	void                Clear() { Data.clear(); }
	IMGUI_API int       GetInt(ImGuiID key, int default_val = 0) const;
	IMGUI_API void      SetInt(ImGuiID key, int val);
	IMGUI_API bool      GetBool(ImGuiID key, bool default_val = false) const;
	IMGUI_API void      SetBool(ImGuiID key, bool val);
	IMGUI_API float     GetFloat(ImGuiID key, float default_val = 0.0f) const;
	IMGUI_API void      SetFloat(ImGuiID key, float val);
	IMGUI_API void*     GetVoidPtr(ImGuiID key) const; // default_val is NULL
	IMGUI_API void      SetVoidPtr(ImGuiID key, void* val);

	// - Get***Ref() functions finds pair, insert on demand if missing, return pointer. Useful if you intend to do Get+Set.
	// - References are only valid until a new value is added to the storage. Calling a Set***() function or a Get***Ref() function invalidates the pointer.
	// - A typical use case where this is convenient for quick hacking (e.g. add storage during a live Edit&Continue session if you can't modify existing struct)
	//      float* pvar = ImGui::GetFloatRef(key); ImGui::SliderFloat("var", pvar, 0, 100.0f); some_var += *pvar;
	IMGUI_API int*      GetIntRef(ImGuiID key, int default_val = 0);
	IMGUI_API bool*     GetBoolRef(ImGuiID key, bool default_val = false);
	IMGUI_API float*    GetFloatRef(ImGuiID key, float default_val = 0.0f);
	IMGUI_API void**    GetVoidPtrRef(ImGuiID key, void* default_val = NULL);

	// Use on your own storage if you know only integer are being stored (open/close all tree nodes)
	IMGUI_API void      SetAllInt(int val);

	// For quicker full rebuild of a storage (instead of an incremental one), you may add all your contents and then sort once.
	IMGUI_API void      BuildSortByKey();
};

// Helper: Manually clip large list of items.
// If you are submitting lots of evenly spaced items and you have a random access to the list, you can perform coarse clipping based on visibility to save yourself from processing those items at all.
// The clipper calculates the range of visible items and advance the cursor to compensate for the non-visible items we have skipped.
// ImGui already clip items based on their bounds but it needs to measure text size to do so. Coarse clipping before submission makes this cost and your own data fetching/submission cost null.
// Usage:
//     ImGuiListClipper clipper(1000);  // we have 1000 elements, evenly spaced.
//     while (clipper.Step())
//         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
//             ImGui::Text("line number %d", i);
// - Step 0: the clipper let you process the first element, regardless of it being visible or not, so we can measure the element height (step skipped if we passed a known height as second arg to constructor).
// - Step 1: the clipper infer height from first element, calculate the actual range of elements to display, and position the cursor before the first element.
// - (Step 2: dummy step only required if an explicit items_height was passed to constructor or Begin() and user call Step(). Does nothing and switch to Step 3.)
// - Step 3: the clipper validate that we have reached the expected Y position (corresponding to element DisplayEnd), advance the cursor to the end of the list and then returns 'false' to end the loop.
struct ImGuiListClipper {
	float   StartPosY;
	float   ItemsHeight;
	int     ItemsCount, StepNo, DisplayStart, DisplayEnd;

	// items_count:  Use -1 to ignore (you can call Begin later). Use INT_MAX if you don't know how many items you have (in which case the cursor won't be advanced in the final step).
	// items_height: Use -1.0f to be calculated automatically on first step. Otherwise pass in the distance between your items, typically GetTextLineHeightWithSpacing() or GetFrameHeightWithSpacing().
	// If you don't specify an items_height, you NEED to call Step(). If you specify items_height you may call the old Begin()/End() api directly, but prefer calling Step().
	ImGuiListClipper(int items_count = -1, float items_height = -1.0f)  { Begin(items_count, items_height); } // NB: Begin() initialize every fields (as we allow user to call Begin/End multiple times on a same instance if they want).
	~ImGuiListClipper()                                                 { IM_ASSERT(ItemsCount == -1); }      // Assert if user forgot to call End() or Step() until false.

	IMGUI_API bool Step();                                              // Call until it returns false. The DisplayStart/DisplayEnd fields will be set and you can process/draw those items.
	IMGUI_API void Begin(int items_count, float items_height = -1.0f);  // Automatically called by constructor if you passed 'items_count' or by Step() in Step 1.
	IMGUI_API void End();                                               // Automatically called on the last call of Step() that returns false.
};

// Helpers macros to generate 32-bits encoded colors
#ifdef IMGUI_USE_BGRA_PACKED_COLOR
	#define IM_COL32_R_SHIFT    16
	#define IM_COL32_G_SHIFT    8
	#define IM_COL32_B_SHIFT    0
	#define IM_COL32_A_SHIFT    24
	#define IM_COL32_A_MASK     0xFF000000
#else
	#define IM_COL32_R_SHIFT    0
	#define IM_COL32_G_SHIFT    8
	#define IM_COL32_B_SHIFT    16
	#define IM_COL32_A_SHIFT    24
	#define IM_COL32_A_MASK     0xFF000000
#endif
#define IM_COL32(R,G,B,A)    (((ImU32)(A)<<IM_COL32_A_SHIFT) | ((ImU32)(B)<<IM_COL32_B_SHIFT) | ((ImU32)(G)<<IM_COL32_G_SHIFT) | ((ImU32)(R)<<IM_COL32_R_SHIFT))
#define IM_COL32_WHITE       IM_COL32(255,255,255,255)  // Opaque white = 0xFFFFFFFF
#define IM_COL32_BLACK       IM_COL32(0,0,0,255)        // Opaque black
#define IM_COL32_BLACK_TRANS IM_COL32(0,0,0,0)          // Transparent black = 0x00000000

// Helper: ImColor() implicitly converts colors to either ImU32 (packed 4x1 byte) or ImVec4 (4x1 float)
// Prefer using IM_COL32() macros if you want a guaranteed compile-time ImU32 for usage with ImDrawList API.
// **Avoid storing ImColor! Store either u32 of ImVec4. This is not a full-featured color class. MAY OBSOLETE.
// **None of the ImGui API are using ImColor directly but you can use it as a convenience to pass colors in either ImU32 or ImVec4 formats. Explicitly cast to ImU32 or ImVec4 if needed.
struct ImColor {
	ImVec4              Value;

	ImColor()                                                       { Value.x = Value.y = Value.z = Value.w = 0.0f; }
	ImColor(int r, int g, int b, int a = 255)                       { float sc = 1.0f / 255.0f; Value.x = (float)r * sc; Value.y = (float)g * sc; Value.z = (float)b * sc; Value.w = (float)a * sc; }
	ImColor(ImU32 rgba)                                             { float sc = 1.0f / 255.0f; Value.x = (float)((rgba >> IM_COL32_R_SHIFT) & 0xFF) * sc; Value.y = (float)((rgba >> IM_COL32_G_SHIFT) & 0xFF) * sc; Value.z = (float)((rgba >> IM_COL32_B_SHIFT) & 0xFF) * sc; Value.w = (float)((rgba >> IM_COL32_A_SHIFT) & 0xFF) * sc; }
	ImColor(float r, float g, float b, float a = 1.0f)              { Value.x = r; Value.y = g; Value.z = b; Value.w = a; }
	ImColor(const ImVec4& col)                                      { Value = col; }
	inline operator ImU32() const                                   { return ImGui::ColorConvertFloat4ToU32(Value); }
	inline operator ImVec4() const                                  { return Value; }

	// FIXME-OBSOLETE: May need to obsolete/cleanup those helpers.
	inline void    SetHSV(float h, float s, float v, float a = 1.0f) { ImGui::ColorConvertHSVtoRGB(h, s, v, Value.x, Value.y, Value.z); Value.w = a; }
	static ImColor HSV(float h, float s, float v, float a = 1.0f)   { float r, g, b; ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b); return ImColor(r, g, b, a); }
};

//-----------------------------------------------------------------------------
// Draw List API (ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListSplitter, ImDrawListFlags, ImDrawList, ImDrawData)
// Hold a series of drawing commands. The user provides a renderer for ImDrawData which essentially contains an array of ImDrawList.
//-----------------------------------------------------------------------------

// Draw callbacks for advanced uses.
// NB: You most likely do NOT need to use draw callbacks just to create your own widget or customized UI rendering,
// you can poke into the draw list for that! Draw callback may be useful for example to:
//  A) Change your GPU render state,
//  B) render a complex 3D scene inside a UI element without an intermediate texture/render target, etc.
// The expected behavior from your rendering function is 'if (cmd.UserCallback != NULL) { cmd.UserCallback(parent_list, cmd); } else { RenderTriangles() }'
// If you want to override the signature of ImDrawCallback, you can simply use e.g. '#define ImDrawCallback MyDrawCallback' (in imconfig.h) + update rendering back-end accordingly.
#ifndef ImDrawCallback
	typedef void (*ImDrawCallback)(const ImDrawList* parent_list, const ImDrawCmd* cmd);
#endif

// Special Draw callback value to request renderer back-end to reset the graphics/render state.
// The renderer back-end needs to handle this special value, otherwise it will crash trying to call a function at this address.
// This is useful for example if you submitted callbacks which you know have altered the render state and you want it to be restored.
// It is not done by default because they are many perfectly useful way of altering render state for imgui contents (e.g. changing shader/blending settings before an Image call).
#define ImDrawCallback_ResetRenderState     (ImDrawCallback)(-1)

// Typically, 1 command = 1 GPU draw call (unless command is a callback)
// Pre 1.71 back-ends will typically ignore the VtxOffset/IdxOffset fields. When 'io.BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset'
// is enabled, those fields allow us to render meshes larger than 64K vertices while keeping 16-bits indices.
struct ImDrawCmd {
	unsigned int    ElemCount;              // Number of indices (multiple of 3) to be rendered as triangles. Vertices are stored in the callee ImDrawList's vtx_buffer[] array, indices in idx_buffer[].
	ImVec4          ClipRect;               // Clipping rectangle (x1, y1, x2, y2). Subtract ImDrawData->DisplayPos to get clipping rectangle in "viewport" coordinates
	ImTextureID     TextureId;              // User-provided texture ID. Set by user in ImfontAtlas::SetTexID() for fonts or passed to Image*() functions. Ignore if never using images or multiple fonts atlas.
	unsigned int    VtxOffset;              // Start offset in vertex buffer. Pre-1.71 or without ImGuiBackendFlags_RendererHasVtxOffset: always 0. With ImGuiBackendFlags_RendererHasVtxOffset: may be >0 to support meshes larger than 64K vertices with 16-bits indices.
	unsigned int    IdxOffset;              // Start offset in index buffer. Always equal to sum of ElemCount drawn so far.
	ImDrawCallback  UserCallback;           // If != NULL, call the function instead of rendering the vertices. clip_rect and texture_id will be set normally.
	void*           UserCallbackData;       // The draw callback code can access this.

	ImDrawCmd() { ElemCount = 0; TextureId = (ImTextureID)NULL; VtxOffset = IdxOffset = 0;  UserCallback = NULL; UserCallbackData = NULL; }
};

// Vertex index
// (to allow large meshes with 16-bits indices: set 'io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset' and handle ImDrawCmd::VtxOffset in the renderer back-end)
// (to use 32-bits indices: override with '#define ImDrawIdx unsigned int' in imconfig.h)
#ifndef ImDrawIdx
	typedef unsigned short ImDrawIdx;
#endif

// Vertex layout
#ifndef IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT
struct ImDrawVert {
	ImVec2  pos;
	ImVec2  uv;
	ImU32   col;
};
#else
// You can override the vertex format layout by defining IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT in imconfig.h
// The code expect ImVec2 pos (8 bytes), ImVec2 uv (8 bytes), ImU32 col (4 bytes), but you can re-order them or add other fields as needed to simplify integration in your engine.
// The type has to be described within the macro (you can either declare the struct or use a typedef). This is because ImVec2/ImU32 are likely not declared a the time you'd want to set your type up.
// NOTE: IMGUI DOESN'T CLEAR THE STRUCTURE AND DOESN'T CALL A CONSTRUCTOR SO ANY CUSTOM FIELD WILL BE UNINITIALIZED. IF YOU ADD EXTRA FIELDS (SUCH AS A 'Z' COORDINATES) YOU WILL NEED TO CLEAR THEM DURING RENDER OR TO IGNORE THEM.
IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT;
#endif

// For use by ImDrawListSplitter.
struct ImDrawChannel {
	ImVector<ImDrawCmd>         _CmdBuffer;
	ImVector<ImDrawIdx>         _IdxBuffer;
};

// Split/Merge functions are used to split the draw list into different layers which can be drawn into out of order.
// This is used by the Columns api, so items of each column can be batched together in a same draw call.
struct ImDrawListSplitter {
	int                         _Current;    // Current channel number (0)
	int                         _Count;      // Number of active channels (1+)
	ImVector<ImDrawChannel>     _Channels;   // Draw channels (not resized down so _Count might be < Channels.Size)

	inline ImDrawListSplitter()  { Clear(); }
	inline ~ImDrawListSplitter() { ClearFreeMemory(); }
	inline void                 Clear() { _Current = 0; _Count = 1; } // Do not clear Channels[] so our allocations are reused next frame
	IMGUI_API void              ClearFreeMemory();
	IMGUI_API void              Split(ImDrawList* draw_list, int count);
	IMGUI_API void              Merge(ImDrawList* draw_list);
	IMGUI_API void              SetCurrentChannel(ImDrawList* draw_list, int channel_idx);
};

enum ImDrawCornerFlags_ {
	ImDrawCornerFlags_None      = 0,
	ImDrawCornerFlags_TopLeft   = 1 << 0, // 0x1
	ImDrawCornerFlags_TopRight  = 1 << 1, // 0x2
	ImDrawCornerFlags_BotLeft   = 1 << 2, // 0x4
	ImDrawCornerFlags_BotRight  = 1 << 3, // 0x8
	ImDrawCornerFlags_Top       = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight,   // 0x3
	ImDrawCornerFlags_Bot       = ImDrawCornerFlags_BotLeft | ImDrawCornerFlags_BotRight,   // 0xC
	ImDrawCornerFlags_Left      = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotLeft,    // 0x5
	ImDrawCornerFlags_Right     = ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight,  // 0xA
	ImDrawCornerFlags_All       = 0xF     // In your function calls you may use ~0 (= all bits sets) instead of ImDrawCornerFlags_All, as a convenience
};

enum ImDrawListFlags_ {
	ImDrawListFlags_None             = 0,
	ImDrawListFlags_AntiAliasedLines = 1 << 0,  // Lines are anti-aliased (*2 the number of triangles for 1.0f wide line, otherwise *3 the number of triangles)
	ImDrawListFlags_AntiAliasedFill  = 1 << 1,  // Filled shapes have anti-aliased edges (*2 the number of vertices)
	ImDrawListFlags_AllowVtxOffset   = 1 << 2   // Can emit 'VtxOffset > 0' to allow large meshes. Set when 'ImGuiBackendFlags_RendererHasVtxOffset' is enabled.
};

// Draw command list
// This is the low-level list of polygons that ImGui:: functions are filling. At the end of the frame,
// all command lists are passed to your ImGuiIO::RenderDrawListFn function for rendering.
// Each dear imgui window contains its own ImDrawList. You can use ImGui::GetWindowDrawList() to
// access the current window draw list and draw custom primitives.
// You can interleave normal ImGui:: calls and adding primitives to the current draw list.
// All positions are generally in pixel coordinates (top-left at (0,0), bottom-right at io.DisplaySize), but you are totally free to apply whatever transformation matrix to want to the data (if you apply such transformation you'll want to apply it to ClipRect as well)
// Important: Primitives are always added to the list and not culled (culling is done at higher-level by ImGui:: functions), if you use this API a lot consider coarse culling your drawn objects.
struct ImDrawList {
	// This is what you have to render
	ImVector<ImDrawCmd>     CmdBuffer;          // Draw commands. Typically 1 command = 1 GPU draw call, unless the command is a callback.
	ImVector<ImDrawIdx>     IdxBuffer;          // Index buffer. Each command consume ImDrawCmd::ElemCount of those
	ImVector<ImDrawVert>    VtxBuffer;          // Vertex buffer.
	ImDrawListFlags         Flags;              // Flags, you may poke into these to adjust anti-aliasing settings per-primitive.

	// [Internal, used while building lists]
	const ImDrawListSharedData* _Data;          // Pointer to shared draw data (you can use ImGui::GetDrawListSharedData() to get the one from current ImGui context)
	const char*             _OwnerName;         // Pointer to owner window's name for debugging
	unsigned int            _VtxCurrentOffset;  // [Internal] Always 0 unless 'Flags & ImDrawListFlags_AllowVtxOffset'.
	unsigned int            _VtxCurrentIdx;     // [Internal] Generally == VtxBuffer.Size unless we are past 64K vertices, in which case this gets reset to 0.
	ImDrawVert*             _VtxWritePtr;       // [Internal] point within VtxBuffer.Data after each add command (to avoid using the ImVector<> operators too much)
	ImDrawIdx*              _IdxWritePtr;       // [Internal] point within IdxBuffer.Data after each add command (to avoid using the ImVector<> operators too much)
	ImVector<ImVec4>        _ClipRectStack;     // [Internal]
	ImVector<ImTextureID>   _TextureIdStack;    // [Internal]
	ImVector<ImVec2>        _Path;              // [Internal] current path building
	ImDrawListSplitter      _Splitter;          // [Internal] for channels api

	// If you want to create ImDrawList instances, pass them ImGui::GetDrawListSharedData() or create and use your own ImDrawListSharedData (so you can use ImDrawList without ImGui)
	ImDrawList(const ImDrawListSharedData* shared_data) { _Data = shared_data; _OwnerName = NULL; Clear(); }
	~ImDrawList() { ClearFreeMemory(); }
	IMGUI_API void  PushClipRect(ImVec2 clip_rect_min, ImVec2 clip_rect_max, bool intersect_with_current_clip_rect = false);  // Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level ImGui::PushClipRect() to affect logic (hit-testing and widget culling)
	IMGUI_API void  PushClipRectFullScreen();
	IMGUI_API void  PopClipRect();
	IMGUI_API void  PushTextureID(ImTextureID texture_id);
	IMGUI_API void  PopTextureID();
	inline ImVec2   GetClipRectMin() const { const ImVec4& cr = _ClipRectStack.back(); return ImVec2(cr.x, cr.y); }
	inline ImVec2   GetClipRectMax() const { const ImVec4& cr = _ClipRectStack.back(); return ImVec2(cr.z, cr.w); }

	// Primitives
	// - For rectangular primitives, "p_min" and "p_max" represent the upper-left and lower-right corners.
	IMGUI_API void  AddLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 1.0f);
	IMGUI_API void  AddRect(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawCornerFlags rounding_corners = ImDrawCornerFlags_All, float thickness = 1.0f);   // a: upper-left, b: lower-right (== upper-left + size), rounding_corners_flags: 4-bits corresponding to which corner to round
	IMGUI_API void  AddRectFilled(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawCornerFlags rounding_corners = ImDrawCornerFlags_All);                     // a: upper-left, b: lower-right (== upper-left + size)
	IMGUI_API void  AddRectFilledMultiColor(const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
	IMGUI_API void  AddQuad(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness = 1.0f);
	IMGUI_API void  AddQuadFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col);
	IMGUI_API void  AddTriangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness = 1.0f);
	IMGUI_API void  AddTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col);
	IMGUI_API void  AddCircle(const ImVec2& center, float radius, ImU32 col, int num_segments = 12, float thickness = 1.0f);
	IMGUI_API void  AddCircleFilled(const ImVec2& center, float radius, ImU32 col, int num_segments = 12);
	IMGUI_API void  AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL);
	IMGUI_API void  AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL, float wrap_width = 0.0f, const ImVec4* cpu_fine_clip_rect = NULL);
	IMGUI_API void  AddPolyline(const ImVec2* points, int num_points, ImU32 col, bool closed, float thickness);
	IMGUI_API void  AddConvexPolyFilled(const ImVec2* points, int num_points, ImU32 col); // Note: Anti-aliased filling requires points to be in clockwise order.
	IMGUI_API void  AddBezierCurve(const ImVec2& pos0, const ImVec2& cp0, const ImVec2& cp1, const ImVec2& pos1, ImU32 col, float thickness, int num_segments = 0);

	// Image primitives
	// - Read FAQ to understand what ImTextureID is.
	// - "p_min" and "p_max" represent the upper-left and lower-right corners of the rectangle.
	// - "uv_min" and "uv_max" represent the normalized texture coordinates to use for those corners. Using (0,0)->(1,1) texture coordinates will generally display the entire texture.
	IMGUI_API void  AddImage(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min = ImVec2(0, 0), const ImVec2& uv_max = ImVec2(1, 1), ImU32 col = IM_COL32_WHITE);
	IMGUI_API void  AddImageQuad(ImTextureID user_texture_id, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& uv1 = ImVec2(0, 0), const ImVec2& uv2 = ImVec2(1, 0), const ImVec2& uv3 = ImVec2(1, 1), const ImVec2& uv4 = ImVec2(0, 1), ImU32 col = IM_COL32_WHITE);
	IMGUI_API void  AddImageRounded(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, float rounding, ImDrawCornerFlags rounding_corners = ImDrawCornerFlags_All);

	// Stateful path API, add points then finish with PathFillConvex() or PathStroke()
	inline    void  PathClear()                                                 { _Path.Size = 0; }
	inline    void  PathLineTo(const ImVec2& pos)                               { _Path.push_back(pos); }
	inline    void  PathLineToMergeDuplicate(const ImVec2& pos)                 { if (_Path.Size == 0 || memcmp(&_Path.Data[_Path.Size - 1], &pos, 8) != 0) _Path.push_back(pos); }
	inline    void  PathFillConvex(ImU32 col)                                   { AddConvexPolyFilled(_Path.Data, _Path.Size, col); _Path.Size = 0; }  // Note: Anti-aliased filling requires points to be in clockwise order.
	inline    void  PathStroke(ImU32 col, bool closed, float thickness = 1.0f)  { AddPolyline(_Path.Data, _Path.Size, col, closed, thickness); _Path.Size = 0; }
	IMGUI_API void  PathArcTo(const ImVec2& center, float radius, float a_min, float a_max, int num_segments = 10);
	IMGUI_API void  PathArcToFast(const ImVec2& center, float radius, int a_min_of_12, int a_max_of_12);                                            // Use precomputed angles for a 12 steps circle
	IMGUI_API void  PathBezierCurveTo(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, int num_segments = 0);
	IMGUI_API void  PathRect(const ImVec2& rect_min, const ImVec2& rect_max, float rounding = 0.0f, ImDrawCornerFlags rounding_corners = ImDrawCornerFlags_All);

	// Advanced
	IMGUI_API void  AddCallback(ImDrawCallback callback, void* callback_data);  // Your rendering function must check for 'UserCallback' in ImDrawCmd and call the function instead of rendering triangles.
	IMGUI_API void  AddDrawCmd();                                               // This is useful if you need to forcefully create a new draw call (to allow for dependent rendering / blending). Otherwise primitives are merged into the same draw-call as much as possible
	IMGUI_API ImDrawList* CloneOutput() const;                                  // Create a clone of the CmdBuffer/IdxBuffer/VtxBuffer.

	// Advanced: Channels
	// - Use to split render into layers. By switching channels to can render out-of-order (e.g. submit foreground primitives before background primitives)
	// - Use to minimize draw calls (e.g. if going back-and-forth between multiple non-overlapping clipping rectangles, prefer to append into separate channels then merge at the end)
	inline void     ChannelsSplit(int count)    { _Splitter.Split(this, count); }
	inline void     ChannelsMerge()             { _Splitter.Merge(this); }
	inline void     ChannelsSetCurrent(int n)   { _Splitter.SetCurrentChannel(this, n); }

	// Internal helpers
	// NB: all primitives needs to be reserved via PrimReserve() beforehand!
	IMGUI_API void  Clear();
	IMGUI_API void  ClearFreeMemory();
	IMGUI_API void  PrimReserve(int idx_count, int vtx_count);
	IMGUI_API void  PrimRect(const ImVec2& a, const ImVec2& b, ImU32 col);      // Axis aligned rectangle (composed of two triangles)
	IMGUI_API void  PrimRectUV(const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col);
	IMGUI_API void  PrimQuadUV(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col);
	inline    void  PrimWriteVtx(const ImVec2& pos, const ImVec2& uv, ImU32 col) { _VtxWritePtr->pos = pos; _VtxWritePtr->uv = uv; _VtxWritePtr->col = col; _VtxWritePtr++; _VtxCurrentIdx++; }
	inline    void  PrimWriteIdx(ImDrawIdx idx)                                 { *_IdxWritePtr = idx; _IdxWritePtr++; }
	inline    void  PrimVtx(const ImVec2& pos, const ImVec2& uv, ImU32 col)     { PrimWriteIdx((ImDrawIdx)_VtxCurrentIdx); PrimWriteVtx(pos, uv, col); }
	IMGUI_API void  UpdateClipRect();
	IMGUI_API void  UpdateTextureID();
};

// All draw data to render a Dear ImGui frame
// (NB: the style and the naming convention here is a little inconsistent, we currently preserve them for backward compatibility purpose,
// as this is one of the oldest structure exposed by the library! Basically, ImDrawList == CmdList)
struct ImDrawData {
	bool            Valid;                  // Only valid after Render() is called and before the next NewFrame() is called.
	ImDrawList**    CmdLists;               // Array of ImDrawList* to render. The ImDrawList are owned by ImGuiContext and only pointed to from here.
	int             CmdListsCount;          // Number of ImDrawList* to render
	int             TotalIdxCount;          // For convenience, sum of all ImDrawList's IdxBuffer.Size
	int             TotalVtxCount;          // For convenience, sum of all ImDrawList's VtxBuffer.Size
	ImVec2          DisplayPos;             // Upper-left position of the viewport to render (== upper-left of the orthogonal projection matrix to use)
	ImVec2          DisplaySize;            // Size of the viewport to render (== io.DisplaySize for the main viewport) (DisplayPos + DisplaySize == lower-right of the orthogonal projection matrix to use)
	ImVec2          FramebufferScale;       // Amount of pixels for each unit of DisplaySize. Based on io.DisplayFramebufferScale. Generally (1,1) on normal display, (2,2) on OSX with Retina display.

	// Functions
	ImDrawData()    { Valid = false; Clear(); }
	~ImDrawData()   { Clear(); }
	void Clear()    { Valid = false; CmdLists = NULL; CmdListsCount = TotalVtxCount = TotalIdxCount = 0; DisplayPos = DisplaySize = FramebufferScale = ImVec2(0.f, 0.f); } // The ImDrawList are owned by ImGuiContext!
	IMGUI_API void  DeIndexAllBuffers();                    // Helper to convert all buffers from indexed to non-indexed, in case you cannot render indexed. Note: this is slow and most likely a waste of resources. Always prefer indexed rendering!
	IMGUI_API void  ScaleClipRects(const ImVec2& fb_scale); // Helper to scale the ClipRect field of each ImDrawCmd. Use if your final output buffer is at a different scale than Dear ImGui expects, or if there is a difference between your window resolution and framebuffer resolution.
};

//-----------------------------------------------------------------------------
// Font API (ImFontConfig, ImFontGlyph, ImFontAtlasFlags, ImFontAtlas, ImFontGlyphRangesBuilder, ImFont)
//-----------------------------------------------------------------------------

struct ImFontConfig {
	void*           FontData;               //          // TTF/OTF data
	int             FontDataSize;           //          // TTF/OTF data size
	bool            FontDataOwnedByAtlas;   // true     // TTF/OTF data ownership taken by the container ImFontAtlas (will delete memory itself).
	int             FontNo;                 // 0        // Index of font within TTF/OTF file
	float           SizePixels;             //          // Size in pixels for rasterizer (more or less maps to the resulting font height).
	int             OversampleH;            // 3        // Rasterize at higher quality for sub-pixel positioning. Read https://github.com/nothings/stb/blob/master/tests/oversample/README.md for details.
	int             OversampleV;            // 1        // Rasterize at higher quality for sub-pixel positioning. We don't use sub-pixel positions on the Y axis.
	bool            PixelSnapH;             // false    // Align every glyph to pixel boundary. Useful e.g. if you are merging a non-pixel aligned font with the default font. If enabled, you can set OversampleH/V to 1.
	ImVec2          GlyphExtraSpacing;      // 0, 0     // Extra spacing (in pixels) between glyphs. Only X axis is supported for now.
	ImVec2          GlyphOffset;            // 0, 0     // Offset all glyphs from this font input.
	const ImWchar*  GlyphRanges;            // NULL     // Pointer to a user-provided list of Unicode range (2 value per range, values are inclusive, zero-terminated list). THE ARRAY DATA NEEDS TO PERSIST AS LONG AS THE FONT IS ALIVE.
	float           GlyphMinAdvanceX;       // 0        // Minimum AdvanceX for glyphs, set Min to align font icons, set both Min/Max to enforce mono-space font
	float           GlyphMaxAdvanceX;       // FLT_MAX  // Maximum AdvanceX for glyphs
	bool            MergeMode;              // false    // Merge into previous ImFont, so you can combine multiple inputs font into one ImFont (e.g. ASCII font + icons + Japanese glyphs). You may want to use GlyphOffset.y when merge font of different heights.
	unsigned int    RasterizerFlags;        // 0x00     // Settings for custom font rasterizer (e.g. ImGuiFreeType). Leave as zero if you aren't using one.
	float           RasterizerMultiply;     // 1.0f     // Brighten (>1.0f) or darken (<1.0f) font output. Brightening small fonts may be a good workaround to make them more readable.
	ImWchar         EllipsisChar;           // -1       // Explicitly specify unicode codepoint of ellipsis character. When fonts are being merged first specified ellipsis will be used.

	// [Internal]
	char            Name[40];               // Name (strictly to ease debugging)
	ImFont*         DstFont;

	IMGUI_API ImFontConfig();
};

struct ImFontGlyph {
	ImWchar         Codepoint;          // 0x0000..0xFFFF
	float           AdvanceX;           // Distance to next character (= data from font + ImFontConfig::GlyphExtraSpacing.x baked in)
	float           X0, Y0, X1, Y1;     // Glyph corners
	float           U0, V0, U1, V1;     // Texture coordinates
};

// Helper to build glyph ranges from text/string data. Feed your application strings/characters to it then call BuildRanges().
// This is essentially a tightly packed of vector of 64k booleans = 8KB storage.
struct ImFontGlyphRangesBuilder {
	ImVector<ImU32> UsedChars;            // Store 1-bit per Unicode code point (0=unused, 1=used)

	ImFontGlyphRangesBuilder()          { Clear(); }
	inline void     Clear()             { int size_in_bytes = 0x10000 / 8; UsedChars.resize(size_in_bytes / (int)sizeof(ImU32)); memset(UsedChars.Data, 0, (size_t)size_in_bytes); }
	inline bool     GetBit(int n) const { int off = (n >> 5); ImU32 mask = 1u << (n & 31); return (UsedChars[off] & mask) != 0; }  // Get bit n in the array
	inline void     SetBit(int n)       { int off = (n >> 5); ImU32 mask = 1u << (n & 31); UsedChars[off] |= mask; }               // Set bit n in the array
	inline void     AddChar(ImWchar c)  { SetBit(c); }                          // Add character
	IMGUI_API void  AddText(const char* text, const char* text_end = NULL);     // Add string (each character of the UTF-8 string are added)
	IMGUI_API void  AddRanges(const ImWchar* ranges);                           // Add ranges, e.g. builder.AddRanges(ImFontAtlas::GetGlyphRangesDefault()) to force add all of ASCII/Latin+Ext
	IMGUI_API void  BuildRanges(ImVector<ImWchar>* out_ranges);                 // Output new ranges
};

// See ImFontAtlas::AddCustomRectXXX functions.
struct ImFontAtlasCustomRect {
	unsigned int    ID;             // Input    // User ID. Use <0x10000 to map into a font glyph, >=0x10000 for other/internal/custom texture data.
	unsigned short  Width, Height;  // Input    // Desired rectangle dimension
	unsigned short  X, Y;           // Output   // Packed position in Atlas
	float           GlyphAdvanceX;  // Input    // For custom font glyphs only (ID<0x10000): glyph xadvance
	ImVec2          GlyphOffset;    // Input    // For custom font glyphs only (ID<0x10000): glyph display offset
	ImFont*         Font;           // Input    // For custom font glyphs only (ID<0x10000): target font
	ImFontAtlasCustomRect()         { ID = 0xFFFFFFFF; Width = Height = 0; X = Y = 0xFFFF; GlyphAdvanceX = 0.0f; GlyphOffset = ImVec2(0, 0); Font = NULL; }
	bool IsPacked() const           { return X != 0xFFFF; }
};

enum ImFontAtlasFlags_ {
	ImFontAtlasFlags_None               = 0,
	ImFontAtlasFlags_NoPowerOfTwoHeight = 1 << 0,   // Don't round the height to next power of two
	ImFontAtlasFlags_NoMouseCursors     = 1 << 1    // Don't build software mouse cursors into the atlas
};

// Load and rasterize multiple TTF/OTF fonts into a same texture. The font atlas will build a single texture holding:
//  - One or more fonts.
//  - Custom graphics data needed to render the shapes needed by Dear ImGui.
//  - Mouse cursor shapes for software cursor rendering (unless setting 'Flags |= ImFontAtlasFlags_NoMouseCursors' in the font atlas).
// It is the user-code responsibility to setup/build the atlas, then upload the pixel data into a texture accessible by your graphics api.
//  - Optionally, call any of the AddFont*** functions. If you don't call any, the default font embedded in the code will be loaded for you.
//  - Call GetTexDataAsAlpha8() or GetTexDataAsRGBA32() to build and retrieve pixels data.
//  - Upload the pixels data into a texture within your graphics system (see imgui_impl_xxxx.cpp examples)
//  - Call SetTexID(my_tex_id); and pass the pointer/identifier to your texture in a format natural to your graphics API.
//    This value will be passed back to you during rendering to identify the texture. Read FAQ entry about ImTextureID for more details.
// Common pitfalls:
// - If you pass a 'glyph_ranges' array to AddFont*** functions, you need to make sure that your array persist up until the
//   atlas is build (when calling GetTexData*** or Build()). We only copy the pointer, not the data.
// - Important: By default, AddFontFromMemoryTTF() takes ownership of the data. Even though we are not writing to it, we will free the pointer on destruction.
//   You can set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed,
// - Even though many functions are suffixed with "TTF", OTF data is supported just as well.
// - This is an old API and it is currently awkward for those and and various other reasons! We will address them in the future!
struct ImFontAtlas {
	IMGUI_API ImFontAtlas();
	IMGUI_API ~ImFontAtlas();
	IMGUI_API ImFont*           AddFont(const ImFontConfig* font_cfg);
	IMGUI_API ImFont*           AddFontDefault(const ImFontConfig* font_cfg = NULL);
	IMGUI_API ImFont*           AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);
	IMGUI_API ImFont*           AddFontFromMemoryTTF(void* font_data, int font_size, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL); // Note: Transfer ownership of 'ttf_data' to ImFontAtlas! Will be deleted after destruction of the atlas. Set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed.
	IMGUI_API ImFont*           AddFontFromMemoryCompressedTTF(const void* compressed_font_data, int compressed_font_size, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL); // 'compressed_font_data' still owned by caller. Compress with binary_to_compressed_c.cpp.
	IMGUI_API ImFont*           AddFontFromMemoryCompressedBase85TTF(const char* compressed_font_data_base85, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);              // 'compressed_font_data_base85' still owned by caller. Compress with binary_to_compressed_c.cpp with -base85 parameter.
	IMGUI_API void              ClearInputData();           // Clear input data (all ImFontConfig structures including sizes, TTF data, glyph ranges, etc.) = all the data used to build the texture and fonts.
	IMGUI_API void              ClearTexData();             // Clear output texture data (CPU side). Saves RAM once the texture has been copied to graphics memory.
	IMGUI_API void              ClearFonts();               // Clear output font data (glyphs storage, UV coordinates).
	IMGUI_API void              Clear();                    // Clear all input and output.

	// Build atlas, retrieve pixel data.
	// User is in charge of copying the pixels into graphics memory (e.g. create a texture with your engine). Then store your texture handle with SetTexID().
	// The pitch is always = Width * BytesPerPixels (1 or 4)
	// Building in RGBA32 format is provided for convenience and compatibility, but note that unless you manually manipulate or copy color data into
	// the texture (e.g. when using the AddCustomRect*** api), then the RGB pixels emitted will always be white (~75% of memory/bandwidth waste.
	IMGUI_API bool              Build();                    // Build pixels data. This is called automatically for you by the GetTexData*** functions.
	IMGUI_API void              GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel = NULL);  // 1 byte per-pixel
	IMGUI_API void              GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel = NULL);  // 4 bytes-per-pixel
	bool                        IsBuilt()                   { return Fonts.Size > 0 && (TexPixelsAlpha8 != NULL || TexPixelsRGBA32 != NULL); }
	void                        SetTexID(ImTextureID id)    { TexID = id; }

	//-------------------------------------------
	// Glyph Ranges
	//-------------------------------------------

	// Helpers to retrieve list of common Unicode ranges (2 value per range, values are inclusive, zero-terminated list)
	// NB: Make sure that your string are UTF-8 and NOT in your local code page. In C++11, you can create UTF-8 string literal using the u8"Hello world" syntax. See FAQ for details.
	// NB: Consider using ImFontGlyphRangesBuilder to build glyph ranges from textual data.
	IMGUI_API const ImWchar*    GetGlyphRangesDefault();                // Basic Latin, Extended Latin
	IMGUI_API const ImWchar*    GetGlyphRangesKorean();                 // Default + Korean characters
	IMGUI_API const ImWchar*    GetGlyphRangesJapanese();               // Default + Hiragana, Katakana, Half-Width, Selection of 1946 Ideographs
	IMGUI_API const ImWchar*    GetGlyphRangesChineseFull();            // Default + Half-Width + Japanese Hiragana/Katakana + full set of about 21000 CJK Unified Ideographs
	IMGUI_API const ImWchar*    GetGlyphRangesChineseSimplifiedCommon();// Default + Half-Width + Japanese Hiragana/Katakana + set of 2500 CJK Unified Ideographs for common simplified Chinese
	IMGUI_API const ImWchar*    GetGlyphRangesCyrillic();               // Default + about 400 Cyrillic characters
	IMGUI_API const ImWchar*    GetGlyphRangesThai();                   // Default + Thai characters
	IMGUI_API const ImWchar*    GetGlyphRangesVietnamese();             // Default + Vietnamese characters

	//-------------------------------------------
	// [BETA] Custom Rectangles/Glyphs API
	//-------------------------------------------

	// You can request arbitrary rectangles to be packed into the atlas, for your own purposes.
	// After calling Build(), you can query the rectangle position and render your pixels.
	// You can also request your rectangles to be mapped as font glyph (given a font + Unicode point),
	// so you can render e.g. custom colorful icons and use them as regular glyphs.
	// Read misc/fonts/README.txt for more details about using colorful icons.
	IMGUI_API int               AddCustomRectRegular(unsigned int id, int width, int height);                                                                   // Id needs to be >= 0x10000. Id >= 0x80000000 are reserved for ImGui and ImDrawList
	IMGUI_API int               AddCustomRectFontGlyph(ImFont* font, ImWchar id, int width, int height, float advance_x, const ImVec2& offset = ImVec2(0, 0));  // Id needs to be < 0x10000 to register a rectangle to map into a specific font.
	const ImFontAtlasCustomRect*GetCustomRectByIndex(int index) const { if (index < 0) return NULL; return &CustomRects[index]; }

	// [Internal]
	IMGUI_API void              CalcCustomRectUV(const ImFontAtlasCustomRect* rect, ImVec2* out_uv_min, ImVec2* out_uv_max);
	IMGUI_API bool              GetMouseCursorTexData(ImGuiMouseCursor cursor, ImVec2* out_offset, ImVec2* out_size, ImVec2 out_uv_border[2], ImVec2 out_uv_fill[2]);

	//-------------------------------------------
	// Members
	//-------------------------------------------

	bool                        Locked;             // Marked as Locked by ImGui::NewFrame() so attempt to modify the atlas will assert.
	ImFontAtlasFlags            Flags;              // Build flags (see ImFontAtlasFlags_)
	ImTextureID                 TexID;              // User data to refer to the texture once it has been uploaded to user's graphic systems. It is passed back to you during rendering via the ImDrawCmd structure.
	int                         TexDesiredWidth;    // Texture width desired by user before Build(). Must be a power-of-two. If have many glyphs your graphics API have texture size restrictions you may want to increase texture width to decrease height.
	int                         TexGlyphPadding;    // Padding between glyphs within texture in pixels. Defaults to 1. If your rendering method doesn't rely on bilinear filtering you may set this to 0.

	// [Internal]
	// NB: Access texture data via GetTexData*() calls! Which will setup a default font for you.
	unsigned char*              TexPixelsAlpha8;    // 1 component per pixel, each component is unsigned 8-bit. Total size = TexWidth * TexHeight
	unsigned int*               TexPixelsRGBA32;    // 4 component per pixel, each component is unsigned 8-bit. Total size = TexWidth * TexHeight * 4
	int                         TexWidth;           // Texture width calculated during Build().
	int                         TexHeight;          // Texture height calculated during Build().
	ImVec2                      TexUvScale;         // = (1.0f/TexWidth, 1.0f/TexHeight)
	ImVec2                      TexUvWhitePixel;    // Texture coordinates to a white pixel
	ImVector<ImFont*>           Fonts;              // Hold all the fonts returned by AddFont*. Fonts[0] is the default font upon calling ImGui::NewFrame(), use ImGui::PushFont()/PopFont() to change the current font.
	ImVector<ImFontAtlasCustomRect> CustomRects;    // Rectangles for packing custom texture data into the atlas.
	ImVector<ImFontConfig>      ConfigData;         // Internal data
	int                         CustomRectIds[1];   // Identifiers of custom texture rectangle used by ImFontAtlas/ImDrawList

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
	typedef ImFontAtlasCustomRect    CustomRect;         // OBSOLETED in 1.72+
	typedef ImFontGlyphRangesBuilder GlyphRangesBuilder; // OBSOLETED in 1.67+
#endif
};

// Font runtime data and rendering
// ImFontAtlas automatically loads a default embedded font for you when you call GetTexDataAsAlpha8() or GetTexDataAsRGBA32().
struct ImFont {
	// Members: Hot ~20/24 bytes (for CalcTextSize)
	ImVector<float>             IndexAdvanceX;      // 12-16 // out //            // Sparse. Glyphs->AdvanceX in a directly indexable way (cache-friendly for CalcTextSize functions which only this this info, and are often bottleneck in large UI).
	float                       FallbackAdvanceX;   // 4     // out // = FallbackGlyph->AdvanceX
	float                       FontSize;           // 4     // in  //            // Height of characters/line, set during loading (don't change after loading)

	// Members: Hot ~36/48 bytes (for CalcTextSize + render loop)
	ImVector<ImWchar>           IndexLookup;        // 12-16 // out //            // Sparse. Index glyphs by Unicode code-point.
	ImVector<ImFontGlyph>       Glyphs;             // 12-16 // out //            // All glyphs.
	const ImFontGlyph*          FallbackGlyph;      // 4-8   // out // = FindGlyph(FontFallbackChar)
	ImVec2                      DisplayOffset;      // 8     // in  // = (0,0)    // Offset font rendering by xx pixels

	// Members: Cold ~32/40 bytes
	ImFontAtlas*                ContainerAtlas;     // 4-8   // out //            // What we has been loaded into
	const ImFontConfig*         ConfigData;         // 4-8   // in  //            // Pointer within ContainerAtlas->ConfigData
	short                       ConfigDataCount;    // 2     // in  // ~ 1        // Number of ImFontConfig involved in creating this font. Bigger than 1 when merging multiple font sources into one ImFont.
	ImWchar                     FallbackChar;       // 2     // in  // = '?'      // Replacement character if a glyph isn't found. Only set via SetFallbackChar()
	ImWchar                     EllipsisChar;       // 2     // out // = -1       // Character used for ellipsis rendering.
	float                       Scale;              // 4     // in  // = 1.f      // Base font scale, multiplied by the per-window font scale which you can adjust with SetWindowFontScale()
	float                       Ascent, Descent;    // 4+4   // out //            // Ascent: distance from top to bottom of e.g. 'A' [0..FontSize]
	int                         MetricsTotalSurface;// 4     // out //            // Total surface in pixels to get an idea of the font rasterization/texture cost (not exact, we approximate the cost of padding between glyphs)
	bool                        DirtyLookupTables;  // 1     // out //

	// Methods
	IMGUI_API ImFont();
	IMGUI_API ~ImFont();
	IMGUI_API const ImFontGlyph*FindGlyph(ImWchar c) const;
	IMGUI_API const ImFontGlyph*FindGlyphNoFallback(ImWchar c) const;
	float                       GetCharAdvance(ImWchar c) const     { return ((int)c < IndexAdvanceX.Size) ? IndexAdvanceX[(int)c] : FallbackAdvanceX; }
	bool                        IsLoaded() const                    { return ContainerAtlas != NULL; }
	const char*                 GetDebugName() const                { return ConfigData ? ConfigData->Name : "<unknown>"; }

	// 'max_width' stops rendering after a certain width (could be turned into a 2d size). FLT_MAX to disable.
	// 'wrap_width' enable automatic word-wrapping across multiple lines to fit into given width. 0.0f to disable.
	IMGUI_API ImVec2            CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end = NULL, const char** remaining = NULL) const; // utf8
	IMGUI_API const char*       CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const;
	IMGUI_API void              RenderChar(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, ImWchar c) const;
	IMGUI_API void              RenderText(ImDrawList* draw_list, float size, ImVec2 pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, float wrap_width = 0.0f, bool cpu_fine_clip = false) const;

	// [Internal] Don't use!
	IMGUI_API void              BuildLookupTable();
	IMGUI_API void              ClearOutputData();
	IMGUI_API void              GrowIndex(int new_size);
	IMGUI_API void              AddGlyph(ImWchar c, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, float advance_x);
	IMGUI_API void              AddRemapChar(ImWchar dst, ImWchar src, bool overwrite_dst = true); // Makes 'dst' character/glyph points to 'src' character/glyph. Currently needs to be called AFTER fonts have been built.
	IMGUI_API void              SetFallbackChar(ImWchar c);

#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
	typedef ImFontGlyph Glyph; // OBSOLETED in 1.52+
#endif
};

#if defined(__clang__)
	#pragma clang diagnostic pop
#elif defined(__GNUC__)
	#pragma GCC diagnostic pop
#endif

// Include imgui_user.h at the end of imgui.h (convenient for user to only explicitly include vanilla imgui.h)
#ifdef IMGUI_INCLUDE_IMGUI_USER_H
	#include "imgui_user.h"
#endif
