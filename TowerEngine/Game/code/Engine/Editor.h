namespace editor {

	enum line_draw_step { first, second};

	struct data {
		bool EditorMode;

		bool EntityLibraryOpen;
		bool MaterialEditorOpen;
		bool ProfilerWindowOpen;
		bool TransformWindowOpen;

		bool DebugLights;

		bool DisplayBoundingRadius;
		bool DisplayBoundingCube;

		bool RenderCameraObjects;
		bool DisplayConnections;
		bool DebugPath;
		bool DebugRoomGraph;
		bool ShowGizmos;

		bool GodMode;

		transform* TransformSelected;
		entity* EntitySelected;

		bool EditingTile;
		entity* TileEntityEditing;
		entity* TileEntityNavEditing;

		asset_material* MaterialEditing;

		bool32 ShowingTileSet;
		bool32 TileSetSceneMade;

		scene TileSetScene;
		scene TileScene;
		scene EditorScene;

		entity* EntityPreviewing;
		scene EntityPreviewScene;

		entity* PreviewEntity;
		int32 PreviewRotations;

		int32 BattleTestIndex;
		bool32 RunningAllTests;
		bool Invincible;

		// Line draw tool
		line_draw_step LineToolStep;
		bool LineTool;
		bool LineToolErase;
		bool LineToolPrev;
		line NewLine;
	};

};