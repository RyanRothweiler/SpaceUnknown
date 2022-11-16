#pragma once

#ifndef ImguiHelperCPP
#define ImguiHelperCPP

void
ImguiArrowSlider(string Title, int* Num, int32 Min, int32 Max)
{
	ImGui::Text(Title.CharArray);
	if (ImGui::ArrowButton(string{"##left" + Title} .CharArray, ImGuiDir_Left)) { *Num = *Num - 1; }
	ImGui::SameLine();
	ImGui::SliderInt(string{"##Slider" + Title} .CharArray, Num, Min, Max, "%i");
	ImGui::SameLine();
	if (ImGui::ArrowButton(string{"##right" + Title} .CharArray, ImGuiDir_Right)) { *Num = *Num + 1; }

	*Num = ClampValue(Min, Max, *Num);
}

void
GraphAddPoint(graph_data* Hist, float NewPoint)
{
	// Update max max max super max
	if (NewPoint > Hist->Max) { Hist->Max = NewPoint; }

	// Loop
	if (Hist->I == ArrayCount(Hist->Data)) {
		Hist->I = 0;
	}

	Hist->Sum -= Hist->Data[Hist->I];
	Hist->Sum += NewPoint;
	Hist->Average = Hist->Sum / ArrayCount(Hist->Data);

	Hist->Data[Hist->I] = NewPoint;
	Hist->I++;
}

void
ImGuiGraph(string Title, graph_data* Data)
{
	real32 Height = 35;
	if (Data->Type == graph_type::histogram) {
		ImGui::PlotHistogram(Title.CharArray, Data->Data, ArrayCount(Data->Data), Data->I, NULL, 0.0f, Data->Max, ImVec2(0, Height));
	} else if (Data->Type == graph_type::line) {
		ImGui::PlotLines(Title.CharArray, Data->Data, ArrayCount(Data->Data), Data->I, NULL, 0.0, Data->Max, ImVec2(0, Height));
	}
}

real64
ImGui_RealEntry(string Title, real64 Curr, int32 Size)
{
	string regStr = string{Curr};
	ImGui::InputText(Title.CharArray, regStr.CharArray, Size, ImGuiInputTextFlags_CharsDecimal);
	return (StringToReal64(regStr));
}

#endif