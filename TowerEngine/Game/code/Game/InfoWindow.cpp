void InfoWindowShow(item_id Item, state* State) {
	for (int i = 0; i < ArrayCount(State->InfoWindows); i++) {
		if (!State->InfoWindows[i].Showing) {
			State->InfoWindows[i].Showing = true;
			State->InfoWindows[i].Item = Item;
			return;
		}
	}

	// no windows available, using first window
	State->InfoWindows[0].Showing = true;
	State->InfoWindows[0].Item = Item;
}
