
#pragma once
#ifndef ProfilerCPP
#define ProfilerCPP


namespace profiler {


	// Should be called at the beginning of every frame
	void
	InitProfiler(profiler_data* Data)
	{
		for (uint32 Index = 0; Index < Data->TimesCount; Index++) {
			Data->AccumulatedMethods[Index] = {};
		}
		Data->DrawCallCount = 0;
	}

	void ImguiBlockInfo(accum_method* Accum, graph_data* Graph, game_state* GameState, int id)
	{
		if (Accum->TotalCycles > 0) {
			ImGui::PushID(id);

			Graph->Type = graph_type::line;

			float m = (float)GameState->TotalCyclesUsedLastFrame;
			if (m > Graph->Max) { Graph->Max = m; }

			//GraphAddPoint(Graph, (float)Accum->TotalCycles / (float)GameState->TotalCyclesUsedLastFrame);
			GraphAddPoint(Graph, (float)Accum->TotalCycles);
			ImGuiGraph(Accum->FunctionName, Graph);

			real64 PlatformMicroSecond = ((real64)Accum->PerfCounterDuration / (real64)PlatformApi.PerformanceCounterFrequency) * 1000000.0f;
			string PlatformStr = Humanize((int64)PlatformMicroSecond) + " µs";

			real64 MicroSecondLastFrame = GameState->DeltaTimeMS * 1000.0f;
			int32 PercentageOfFrame = (int32)((PlatformMicroSecond / MicroSecondLastFrame) * 100.0f);
			string PercStr = string{PercentageOfFrame} + "%";

			ImGui::Columns(4);

			// Average processor cycles
			ImGui::Text(Humanize((int64)Graph->Average).CharArray);
			ImGui::NextColumn();

			// Windows perf counter / clock timings
			ImGui::Text(PlatformStr.CharArray);
			ImGui::NextColumn();

			// Percentage of frame
			ImGui::Text(PercStr.CharArray);
			ImGui::NextColumn();

			// Number of calls
			ImGui::Text(string{Accum->CallsCount} .CharArray);
			ImGui::NextColumn();

			ImGui::Columns(1);

			ImGui::PopID();
			ImGui::Separator();
		}
	}

	void ImGuiProfiler(game_state* GameState, game_memory* Memory,
	                   profiler_data* GameTimes,
	                   profiler_data* RendererTimes
	                  )
	{

		ImGui::SetNextWindowPos(ImVec2(0, 20));
		ImGui::SetNextWindowSize(ImVec2(Globals->Window->Width * 0.5f, (real32)Globals->Window->Height));
		//ImGui::SetNextWindowSize(ImVec2(100, 100));

		ImGui::Begin("Profiler", &Globals->EditorData.ProfilerWindowOpen, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar);

		// fps
		{
			static graph_data Graph = {};
			Graph.Type = graph_type::line;
			Graph.Max = 70;

			string Str = "FPS " + string{GameState->PrevFrameFPS};

			GraphAddPoint(&Graph, (float)GameState->PrevFrameFPS);
			ImGuiGraph(Str, &Graph);
		}

		// draw call acount
		{
			static graph_data Graph = {};
			Graph.Type = graph_type::line;

			string Str = "Draw Calls " + string{RendererTimes->DrawCallCount};

			GraphAddPoint(&Graph, (float)RendererTimes->DrawCallCount);
			ImGuiGraph(Str, &Graph);
		}

		if (ImGui::CollapsingHeader("Memory")) {
			// permanent memory
			{
				int64 SizeTotalMB = BytesToMegabytes(Memory->PermanentMemory.Size);
				int64 SizeUsedBytes = Memory->PermanentMemory.Head - (uint8*)Memory->PermanentMemory.Memory;
				int64 SizeUsedMB = BytesToMegabytes(SizeUsedBytes);
				string Str = string{SizeUsedBytes} + " / " + string{Memory->PermanentMemory.Size};
				real64 progress = (real64)SizeUsedMB / (real64)SizeTotalMB;
				ImGui::ProgressBar((float)progress, ImVec2(0.f, 0.f), Str.CharArray); ImGui::SameLine(); ImGui::Text("Permanent Memory");
			}

			// transient memory
			{
				static graph_data Graph = {};
				Graph.Type = graph_type::line;

				int64 SizeTotalMB = BytesToMegabytes(Memory->TransientMemory.Size);
				int64 SizeUsedBytes = Memory->TransientMemory.Head - (uint8*)Memory->TransientMemory.Memory;
				int64 SizeUsedMB = BytesToMegabytes(SizeUsedBytes);

				GraphAddPoint(&Graph, (float)((real64)SizeUsedMB / (real64)SizeTotalMB));
				ImGuiGraph("Transient Memory", &Graph);
			}

			ImGui::Dummy(ImVec2(0.0f, 20.0f));
		}


		// Logic Cycles
		{
			// static graph_data Graph = {};
			// Graph.Type = graph_type::line;
			// GraphAddPoint(&Graph, (float)GameState->LogicCycles);
			// ImGuiGraph("Logic Cycles", &Graph);
			ImGui::PushID("Logic");
			ImGui::Text("Logic");

			static graph_data Graph = {};
			GraphAddPoint(&Graph, (float)GameState->LogicCycles);

			ImGui::Columns(4);

			// Average processor cycles
			ImGui::Text(Humanize((int64)Graph.Average).CharArray);
			ImGui::NextColumn();

			// Windows perf counter / clock timings
			//ImGui::Text(PlatformStr.CharArray);
			ImGui::NextColumn();

			// Percentage of frame
			//ImGui::Text(PercStr.CharArray);
			ImGui::NextColumn();

			// Number of calls
			//ImGui::Text(string{Accum->CallsCount} .CharArray);
			ImGui::NextColumn();

			ImGui::Columns(1);

			ImGui::PopID();
			ImGui::Separator();
		}

		// Rendering Cycles
		{
			// static graph_data Graph = {};
			// Graph.Type = graph_type::line;
			// GraphAddPoint(&Graph, (float)GameState->LogicCycles);
			// ImGuiGraph("Logic Cycles", &Graph);
			ImGui::PushID("Rendering");
			ImGui::Text("Rendering");

			static graph_data Graph = {};
			GraphAddPoint(&Graph, (float)GameState->CyclesPlatformRendering);

			ImGui::Columns(4);

			// Average processor cycles
			ImGui::Text(Humanize((int64)Graph.Average).CharArray);
			ImGui::NextColumn();

			// Windows perf counter / clock timings
			//ImGui::Text(PlatformStr.CharArray);
			ImGui::NextColumn();

			// Percentage of frame
			//ImGui::Text(PercStr.CharArray);
			ImGui::NextColumn();

			// Number of calls
			//ImGui::Text(string{Accum->CallsCount} .CharArray);
			ImGui::NextColumn();

			ImGui::Columns(1);

			ImGui::PopID();
			ImGui::Separator();
		}
		// verts
		/*
		{
			uint64 VertsCount = 0;
			list_link* CurrentLink = GameState->GameRenderer.RenderCommands->TopLink;
			for (uint32 i = 0; i < GameState->GameRenderer.RenderCommands->LinkCount; i++) {

				render_command* Command = (render_command*)CurrentLink->Data;
				VertsCount += Command->BufferCapacity * 3;

				CurrentLink = CurrentLink->NextLink;
			}

			ImGui::Text(("Verts " + string{VertsCount}).CharArray);
		}
		*/

		int i = 0;
		for (uint32 index = 0; index < GameTimes->TimesCount; index++) {
			ImguiBlockInfo(&GameTimes->AccumulatedMethods[index], &GameTimes->AccumulatedMethodsGraphs[index], GameState, i++);
		}
		for (uint32 index = 0; index < RendererTimes->TimesCount; index++) {
			ImguiBlockInfo(&RendererTimes->AccumulatedMethods[index], &RendererTimes->AccumulatedMethodsGraphs[index], GameState, i++);
		}

		ImGui::End();
	}
}

#endif