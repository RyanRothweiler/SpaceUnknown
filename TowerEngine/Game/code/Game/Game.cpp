#pragma once

#ifndef GAME_CPP
#define GAME_CPP

#include "Game.h"

#include <chrono>
#include <ctime>

namespace game {

	string ChronoToString(std::chrono::seconds SecondsTotal)
	{
		int64 Hours = std::chrono::duration_cast<std::chrono::hours>(SecondsTotal).count();
		int64 Minutes = (int64)(std::chrono::duration_cast<std::chrono::minutes>(SecondsTotal).count() - (Hours * 60.0f));
		int64 Seconds = (int64)(SecondsTotal.count() - (Minutes * 60.0f) - (Hours * 60.0f * 60.0f));

		return string{Hours} + string{"h "} + string{Minutes} + string{"m "} + string{Seconds} + string{"s "};
	}

	void RegisterStepper(stepper* Stepper,
	                     step_func Method,
	                     void* SelfData,
	                     game::state* State)
	{
		Assert(State->SteppersCount < ArrayCount(State->Steppers));

		Stepper->Step = Method;
		Stepper->SelfData = SelfData;

		State->Steppers[State->SteppersCount++] = Stepper;
	}

	void TimeStep(void* SelfData, real64 Time, game::state* State)
	{
		game::universe_time* UT = (game::universe_time*)SelfData;
		UT->TimeMS += Time;
	}

	void StepUniverse(game::state* State, real64 TimeMS)
	{
		for (int i = 0; i < State->SteppersCount; i++) {
			stepper* Stepper = State->Steppers[i];
			Stepper->Step(Stepper->SelfData, TimeMS, State);
		}
	}

#include "Definitions.cpp"
#include "Asteroid.cpp"
#include "Item.cpp"
#include "Ship.cpp"

	void ClearShipSelection(game::state* State)
	{
		if (State->ShipSelected != GameNull && !State->ShipSelected->IsMoving) {
			State->ShipSelected->CurrentJourney = {};
		}
		State->ShipSelected = GameNull;
	}

	void Start(engine_state* EngineState)
	{
		game::state* State = &EngineState->GameState;
		CreateDefinitions();

		RegisterStepper(&State->UniverseTime.Stepper, &TimeStep, (void*)(&State->UniverseTime), State);

		// Load asteroid images
		Globals->AssetsList.AsteroidImages[0] = assets::GetImage("Asteroid1");
		Globals->AssetsList.AsteroidImages[1] = assets::GetImage("Asteroid2");
		Globals->AssetsList.AsteroidImages[2] = assets::GetImage("Asteroid3");

		ShipSetup(State, vector2{0, 0});
		AsteroidCreateCluster(vector2{0, 0}, 30.0f, State);
	}

	const real32 ZoomMin = 0.0f;
	const real32 ZoomMax = 1.0f;
	const real32 ZoomRealMin = 0.1f;
	const real32 ZoomRealMax = 20.0f;

	const real32 RenderLayerShip = -2;
	const real32 RenderLayerPlanet = -3;
	const real32 KeyboardPanSpeed = 0.75f;
	const real32 MousePanSpeed = 0.03f;

	const real32 MouseZoomSpeed = 0.03f;
	const real32 MouseZoomInvert = -1;

	void Loop(engine_state* EngineState, window_info* Window, game_input* Input)
	{
		game::state* State = &EngineState->GameState;
		game::editor_state* EditorState = &EngineState->EditorState;

		State->Zoom = (real32)Lerp(State->Zoom, State->ZoomTarget, 0.5f);
		float Curve = 3.5f;
		EngineState->GameCamera.OrthoZoom = (real32)LerpCurve(ZoomRealMin, ZoomRealMax, Curve, State->Zoom);
		real64 ZoomSpeedAdj = LerpCurve(4.0f, 200.0f, Curve, State->Zoom);

		// Editor
		{
			if (Input->FunctionKeys[1].OnDown) {
				EditorState->EditorMode = !EditorState->EditorMode;
			}
		}

		// Main window
		{
			static bool Open = true;
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2(Window->Width * 0.15f, -1));
			ImGui::Begin("Main", &Open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

			if (EditorState->EditorMode) {
				ImGui::Text("EDITOR MODE");

				// Mouse world position
				{
					vector3 MouseWorld = ScreenToWorld(Input->MousePos, vector3{0, 0, (EngineState->GameCamera.Far + EngineState->GameCamera.Near) * -0.5f}, vector3{0, 0, -1}, &EngineState->GameCamera);
					vector2 MouseWorldFlat = vector2{MouseWorld.X, MouseWorld.Y};
					ImGui::Text(string{MouseWorldFlat.X} .Array());
					ImGui::SameLine();
					ImGui::Text(string{MouseWorldFlat.Y} .Array());
					RenderCircle(MouseWorldFlat, vector2{1, 1},
					             COLOR_RED, -1, Globals->GameRenderer);
				}

				ImGui::Text("Time Boost");
				{
					ImGui::Checkbox("Pause", &EditorState->Paused);

					real64 SimFPS = 60.0f;
					real64 FrameLengthMS = (1.0f / SimFPS) * 1000.0f;
					if (ImGui::Button("1 minute")) {
						for (int i = 0; i < SimFPS * 60.0f; i++) { StepUniverse(State, FrameLengthMS); }
					}
					ImGui::SameLine();
					if (ImGui::Button("1 hour")) {
						for (int i = 0; i < SimFPS * 60.0f * 60.0f; i++) { StepUniverse(State, FrameLengthMS); }
					}
				}

				ImGui::Separator();

				// Ship simulate performance testing
				if (State->ShipSelected != GameNull) {
					if (ImGui::Button("Test Ship Simulation")) {
						ConsoleLog("Starting Test");

						ship* CurrentShip = State->ShipSelected;

						uint64 Accum = 0;
						int32 Runs = 10;

						CurrentShip->CurrentJourney.EdgeRatio = 0.25f;
						CurrentShip->CurrentJourney.StartPosition = CurrentShip->Position;
						CurrentShip->CurrentJourney.EndPosition = vector2{50, 50};

						real64 SimTimeTarget = 24.0f * 60.0f * 60.0f * 1000.0f;
						real64 TotalSimTime = 0;

						int i = 0;
						bool32 Running = true;
						//for (int i = 0; i < Runs; i++ ) {
						while (Running) {
							uint64 Start = PlatformApi.QueryPerformanceCounter();

							float SimFPS = 1.0f;
							float TimeStepMS = 1.0f / SimFPS;

							vector2 PosOrig = CurrentShip->Position;
							real64 FuelOrig = CurrentShip->FuelGallons;
							CurrentShip->Velocity = {};
							ShipMove(CurrentShip, CurrentShip->CurrentJourney);

							while (ShipSimulateMovement(CurrentShip, TimeStepMS) && Running) {
								TotalSimTime += TimeStepMS;
								if (TotalSimTime >= SimTimeTarget) {
									Running = false;
								}
							}

							if (Running) {
								CurrentShip->Position = PosOrig;
								CurrentShip->Velocity = {};
								CurrentShip->FuelGallons = FuelOrig;
								CurrentShip->IsMoving = false;

								uint64 End = PlatformApi.QueryPerformanceCounter();
								uint64 Count = End - Start;
								Accum += Count;

								real64 SimMinutes = MillisecondsToSeconds(TotalSimTime) / 60.0f;
								string Report = "Finished " + string{i + 1} + "/" + string{Runs} + " ->" + string{Count} + " SimMinutes->" + string{SimMinutes};
								ConsoleLog(Report.Array());
							}
						}

						real64 SimMinutes = MillisecondsToSeconds(TotalSimTime) / 60.0f;
						real64 Avg = (real64)Accum / (real64)Runs;
						real64 CyclePerMin = Avg / SimMinutes;
						string Report = "AVG " + string{Avg} + " ||  Total Real Time (m)" + string{SimMinutes} + " || Cycles per SimMin " + string{CyclePerMin};
						ConsoleLog(Report.Array());
					}
				}
			}

			ImGui::Text("Time");

			std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<double>> Time = {};
			Time += std::chrono::milliseconds((int)State->UniverseTime.TimeMS);
			std::chrono::seconds Diff = std::chrono::duration_cast<std::chrono::seconds>(Time.time_since_epoch());
			string Disp = ChronoToString(Diff);
			ImGui::Text(Disp.Array());

			ImGui::Separator();

			ImGui::SliderFloat("Zoom", &State->ZoomTarget, ZoomMin, ZoomMax, "%.3f");

			ImGui::End();
		}

		// camera controls
		{
			// Keyboard
			vector2 CamMoveDir = vector2{0, 0};
			if (Input->KeyboardInput['A'].IsDown || Input->KeyboardInput['a'].IsDown) { CamMoveDir.X = -1; }
			if (Input->KeyboardInput['D'].IsDown || Input->KeyboardInput['d'].IsDown) { CamMoveDir.X = 1; }
			if (Input->KeyboardInput['W'].IsDown || Input->KeyboardInput['w'].IsDown) { CamMoveDir.Y = -1; }
			if (Input->KeyboardInput['S'].IsDown || Input->KeyboardInput['s'].IsDown) { CamMoveDir.Y = 1; }
			CamMoveDir = Vector2Normalize(CamMoveDir);
			EngineState->GameCamera.Center.X += CamMoveDir.X * KeyboardPanSpeed * (ZoomSpeedAdj);
			EngineState->GameCamera.Center.Y += CamMoveDir.Y * KeyboardPanSpeed * (ZoomSpeedAdj);

			// Mouse
			static vector2 MouseStart;
			static vector2 CamStart;
			if (Input->MouseLeft.OnDown) {
				MouseStart = Input->MousePos;

				CamStart.X = EngineState->GameCamera.Center.X;
				CamStart.Y = EngineState->GameCamera.Center.Y;
			}
			if (Input->MouseLeft.IsDown) {
				vector2 Offset = MouseStart - Input->MousePos;
				vector2 P = CamStart + (Offset * MousePanSpeed * ZoomSpeedAdj);
				EngineState->GameCamera.Center.X = P.X;
				EngineState->GameCamera.Center.Y = P.Y;
			}

			State->ZoomTarget = (real32)ClampValue(ZoomMin, ZoomMax, State->ZoomTarget + (Input->MouseScrollDelta * MouseZoomSpeed * MouseZoomInvert));
		}

		// Ship
		{
			ship* CurrentShip = State->ShipSelected;

			vector3 MouseWorld = ScreenToWorld(Input->MousePos, vector3{0, 0, (EngineState->GameCamera.Far + EngineState->GameCamera.Near) * -0.5f}, vector3{0, 0, -1}, &EngineState->GameCamera);
			vector2 MouseWorldFlat = vector2{MouseWorld.X, MouseWorld.Y};

			RenderCircle(MouseWorldFlat, vector2{1, 1},
			             COLOR_RED, -1, Globals->GameRenderer);

			// Ship window
			if (!State->ShipInfoWindowShowing) { ClearShipSelection(State); }
			if (CurrentShip != GameNull) {

				// ship current movement line
				if (CurrentShip->IsMoving) {
					vector2 Points[2] = {};
					Points[0] = WorldToScreen(vector3{CurrentShip->CurrentJourney.EndPosition.X, CurrentShip->CurrentJourney.EndPosition.Y, 0}, &EngineState->GameCamera);
					Points[1] = WorldToScreen(vector3{CurrentShip->Position.X, CurrentShip->Position.Y, 0}, &EngineState->GameCamera);
					render_line Line = {};
					Line.Points = Points;
					Line.PointsCount = ArrayCount(Points);
					RenderLine(Line, 2.0f, color{56.0f / 255.0f, 255.0f / 255.0f, 248.0f / 255.0f, 0.5f}, &EngineState->UIRenderer, false);
				}

				ImGui::Begin("Ship Info", &State->ShipInfoWindowShowing);

				ImVec2 window_pos = ImGui::GetWindowPos();

				if (EditorState->EditorMode) {
					if (ImGui::TreeNode("Basic")) {
						// Posititon
						{
							ImGui::PushID("position");
							ImGui::Text("Position");
							ImGui::Columns(2);

							ImGui::Text("x");
							ImGui::SameLine();
							ImGui::Text(string{CurrentShip->Position.X} .Array());

							ImGui::NextColumn();

							ImGui::Text("y");
							ImGui::SameLine();
							ImGui::Text(string{CurrentShip->Position.Y} .Array());

							ImGui::Columns(1);
							ImGui::PopID();
						}
						ImGui::TreePop();
					}
					ImGui::Separator();
				}

				// Weight
				{
					string ShipWeightDisp = Humanize(CurrentShip->CurrentMassTotal);

					ImGui::Text("Ship Total Mass (t)");
					ImGui::SameLine();
					ImGui::Text(ShipWeightDisp.Array());
				}

				// Velocity
				{
					string V = Humanize((int64)(Vector2Length(CurrentShip->Velocity) * UnitToMeters * 1000.0f));
					ImGui::Text("Velocity (kph)");
					ImGui::SameLine();
					ImGui::Text(V.Array());
				}

				ImGui::Dummy(ImVec2(0, 10));

				// Fuel
				{
					string FuelDisp = "Fuel Tank (g) " + string{CurrentShip->FuelGallons} + "/" + string{CurrentShip->Definition.FuelTankGallons};
					ImGui::Text(FuelDisp.Array());
					float Progress = (float)(CurrentShip->FuelGallons / CurrentShip->Definition.FuelTankGallons);
					ImGui::ProgressBar(Progress);

				}

				ImGui::Dummy(ImVec2(0, 10));

				if (ImGui::CollapsingHeader("Modules")) {
					for (int i = 0; i < CurrentShip->ModulesCount; i++) {
						ship_module* Module = &CurrentShip->Modules[i];

						ImGui::Text(Module->Definition.DisplayName.Array());
						float Progress = (float)(Module->ActivationTimerMS / Module->Definition.ActivationTimeMS);
						ImGui::ProgressBar(Progress, ImVec2(-1.0f, 1.0f));
					}
				}

				// Cargo
				{
					int64 CargoWeight = (int64)CurrentShip->CurrentCargoMass;
					string CargoTitle = "Cargo (" + string{CargoWeight} + "/" + string{(int64)CurrentShip->Definition.CargoMassLimit} + ")(t)###CARGO";
					if (ImGui::CollapsingHeader(CargoTitle.Array())) {
						for (int i = 0; i < ArrayCount(CurrentShip->Cargo); i++) {
							item_instance* Item = &CurrentShip->Cargo[i];
							if (Item->Count > 0) {
								ImGui::Text(Item->Definition.DisplayName.Array());
								ImGui::SameLine();
								ImGui::Text("x");
								ImGui::SameLine();
								ImGui::Text(string{Item->Count} .Array());
							}
						}
					}
				}

				// Journey / movement stuff
				{
					static bool32 DoCalc = false;

					ImGui::Separator();
					ImGui::Dummy(ImVec2(0, 10));
					ImGui::Text("Issue Movement Command");

					if (
					    !CurrentShip->IsMoving &&
					    CurrentShip->CurrentJourney.EndPosition.X != 0 &&
					    CurrentShip->CurrentJourney.EndPosition.Y != 0
					) {


						// Line to taget destination
						{
							vector2 Points[2] = {};
							Points[0] = WorldToScreen(vector3{CurrentShip->CurrentJourney.EndPosition.X, CurrentShip->CurrentJourney.EndPosition.Y, 0}, &EngineState->GameCamera);
							Points[1] = WorldToScreen(vector3{CurrentShip->Position.X, CurrentShip->Position.Y, 0}, &EngineState->GameCamera);
							render_line Line = {};
							Line.Points = Points;
							Line.PointsCount = ArrayCount(Points);
							RenderLine(Line, 1.5f, color{0, 1, 0, 0.2f}, &EngineState->UIRenderer, false);
						}

						// Journey settings
						{
							static float DurationMS = 0.0f;
							static real64 FuelUsage = 0.0f;

							if (ImGui::SliderFloat("Fuel Usage", &CurrentShip->CurrentJourney.EdgeRatio, 0.1f, 1.0f, "%.2f")) {
								DoCalc = true;
							}

							if (DoCalc) {
								DoCalc = false;

								DurationMS = 0.0f;

								float SimFPS = 30.0f;
								float TimeStepMS = 1.0f / SimFPS;

								vector2 PosOrig = CurrentShip->Position;
								real64 FuelOrig = CurrentShip->FuelGallons;
								CurrentShip->Velocity = {};
								ShipMove(CurrentShip, CurrentShip->CurrentJourney);

								while (ShipSimulateMovement(CurrentShip, TimeStepMS)) {
									DurationMS += TimeStepMS;
								}

								FuelUsage = FuelOrig - CurrentShip->FuelGallons;

								CurrentShip->Position = PosOrig;
								CurrentShip->Velocity = {};
								CurrentShip->FuelGallons = FuelOrig;
								CurrentShip->IsMoving = false;
							}

							float DurationMinutes = DurationMS / 1000.0f / 60.0f;

							ImGui::Text("Journey Minutes");
							ImGui::SameLine();
							ImGui::Text(string{DurationMinutes} .Array());

							ImGui::Text("Fuel Usage");
							ImGui::SameLine();
							ImGui::Text(string{FuelUsage} .Array());

							if (FuelUsage < CurrentShip->FuelGallons) {
								if (ImGui::Button("Execute Movement", ImVec2(-1.0f, 0.0f))) {
									ShipMove(CurrentShip, CurrentShip->CurrentJourney);
								}
							} else {
								ImGui::TextColored(ImVec4(1, 0, 0, 1), "Not Enough Fuel");
							}
						}
					} else {
						ImGui::Text("Click world to set target destination");
					}


					// Render line
					{
						vector2 Points[2] = {};
						Points[0] = vector2{window_pos.x, window_pos.y};
						Points[1] = WorldToScreen(vector3{CurrentShip->Position.X, CurrentShip->Position.Y, 0}, &EngineState->GameCamera);
						render_line Line = {};
						Line.Points = Points;
						Line.PointsCount = ArrayCount(Points);
						RenderLine(Line, 1.5f, color{1, 1, 1, 0.2f}, &EngineState->UIRenderer, false);
					}

					if (Input->MouseLeft.OnUp && !CurrentShip->IsMoving && !Input->MouseMoved()) {
						DoCalc = true;

						CurrentShip->CurrentJourney.EdgeRatio = 0.1f;
						CurrentShip->CurrentJourney.StartPosition = CurrentShip->Position;
						CurrentShip->CurrentJourney.EndPosition = MouseWorldFlat;
					}
				}

				ImGui::End();
			}

			// Ship selecting
			if (Input->Escape.OnDown) { ClearShipSelection(State); }
			if (State->ShipSelected == GameNull && Input->MouseLeft.OnUp && !Input->MouseMoved() ) {

				for (int i = 0; i < ArrayCount(State->Ships); i++) {
					ship* Ship = &State->Ships[i];
					if (Ship->Using) {

						vector2 TopLeftWorld = Ship->Position - (Ship->Size * 0.5f);
						vector2 BottomRightWorld = Ship->Position + (Ship->Size * 0.5f);

						rect Bounds = {};
						Bounds.TopLeft = WorldToScreen(vector3{TopLeftWorld.X, TopLeftWorld.Y, 0}, &EngineState->GameCamera);
						Bounds.BottomRight = WorldToScreen(vector3{BottomRightWorld.X, BottomRightWorld.Y, 0}, &EngineState->GameCamera);

						if (RectContains(Bounds, Input->MousePos)) {
							State->ShipSelected = Ship;
							State->ShipInfoWindowShowing = true;
						}
					}
				}
			}
		}

		if (!EditorState->Paused) {
			StepUniverse(State, EngineState->DeltaTimeMS);
		}

		// Render planets
		RenderCircle(vector2{1200, 200}, vector2{2000, 2000},
		             COLOR_RED, RenderLayerPlanet, Globals->GameRenderer);

		// Render asteroids
		for (int i = 0; i < State->ClustersCount; i++) {
			asteroid_cluster* Clust = &State->Asteroids[i];
			for (int a = 0; a < ArrayCount(Clust->Asteroids); a++) {
				if (Clust->Asteroids[a].Using) {

					Clust->Asteroids[a].Rotation += Clust->Asteroids[a].RotationRate * EngineState->DeltaTimeMS * 0.0002f;

					m4y4 Model = m4y4Identity();
					Model = Rotate(Model, vector3{0, 0, Clust->Asteroids[a].Rotation});

					RenderTextureAll(
					    Clust->Asteroids[a].Position,
					    vector2{Clust->Asteroids[a].Size, Clust->Asteroids[a].Size},
					    Color255(79.0f, 60.0f, 48.0f, 1.0f),
					    Clust->Asteroids[a].Image->GLID,
					    RenderLayerPlanet, Model, Globals->GameRenderer);
				}
			}
		}

		// Render ships
		for (int i = 0; i < ArrayCount(State->Ships); i++) {
			ship* Ship = &State->Ships[i];
			if (Ship->Using) {

				m4y4 Model = m4y4Identity();
				Model = Rotate(Model, vector3{0, 0, Ship->Rotation});

				static loaded_image* ShipImage = assets::GetImage("Ship");
				RenderTextureAll(
				    Ship->Position,
				    Ship->Size,
				    COLOR_WHITE,
				    ShipImage->GLID, RenderLayerShip, Model, Globals->GameRenderer);
			}

			// Render ship module effects
			for (int m = 0; m < Ship->ModulesCount; m++) {
				ship_module* Module = &Ship->Modules[m];
				if (Module->Target != GameNull) {
					vector2 Points[2] = {};
					Points[0] = WorldToScreen(vector3{Ship->Position.X, Ship->Position.Y, 0}, &EngineState->GameCamera);
					Points[1] = WorldToScreen(vector3{Module->Target->Position.X, Module->Target->Position.Y, 0}, &EngineState->GameCamera);
					render_line Line = {};
					Line.Points = Points;
					Line.PointsCount = ArrayCount(Points);
					RenderLine(Line, 1.5f, color{1, 0, 0, 0.2f}, &EngineState->UIRenderer, false);
				}
			}
		}

	}
}

#endif