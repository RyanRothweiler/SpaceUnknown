struct info_window {
	bool Showing;

	b32 ShowingItem;
	item_id Item;
	ship_id Ship;
};

class InfoWindow {
	private:
		static info_window Windows[10];

	public:
		static void Show(item_id Item);	
		static void Show(ship_id Ship);	
		static void ImGuiRender();	
		static info_window* OpenWindow();	
};
