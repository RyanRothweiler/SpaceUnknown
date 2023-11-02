struct info_window {
	bool Showing;
	item_id Item;
};

class InfoWindow {
	private:
		static info_window Windows[10];

	public:
		static void Show(item_id Item);	
		static void ImGuiRender();	
};
