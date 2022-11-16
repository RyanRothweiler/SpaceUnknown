#pragma once

#ifndef LoggingCPP
#define LoggingCPP

struct console_entry {
	uint32 LineNum;
	char* FileName;

	char* Message;
	uint64 CycleNum;
};

global_variable console_entry GlobalConsoleLog[500];

global_variable uint64 GlobalFrameCounter = 0;
#define ConsoleLog(a) LogToConsole("", a, ID_LINE, ID_FILE)
#define ConsoleLogRenderer(a) LogToConsole("Renderer", a, ID_LINE, ID_FILE)

#ifndef LOG
	#define LOG(a) PlatformApi.Print(a)
#endif

void
LogToConsole(char* Tag, char* Message, int32 LineNumber, char* FileName)
{
	//PlatformApi.Print((Tag + " : " + Message).CharArray);
	PlatformApi.Print(Message);

	/*
	for (int Index = 0; Index < ArrayCount(GlobalConsoleLog) - 1; Index++) {
		GlobalConsoleLog[Index] = GlobalConsoleLog[Index + 1];
	}

	console_entry NewLog = {};
	NewLog.Message = Message;
	NewLog.FileName = FileName;
	NewLog.LineNum = LineNumber;
	NewLog.CycleNum = GlobalFrameCounter;
	GlobalConsoleLog[ArrayCount(GlobalConsoleLog) - 1] = NewLog;
	*/

	// TODO write this to some file somewhere
}


/*void
ImguiConsoleWindow()
{
ImGui::Begin("Log Console");
ImGui::BeginChild("Console", ImVec2(ImGui::GetWindowContentRegionWidth(), -1), true);

for (int32 Index = 0; Index < ArrayCount(GlobalConsoleLog); Index++) {
	console_entry* Entry = &GlobalConsoleLog[Index];
	if (!StringIsEmpty(Entry->Message)) {

		if (ImGui::TreeNode(Entry->Message.CharArray)) {
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), Entry->FileName.CharArray);
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), string{(int32)Entry->LineNum} .CharArray);
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.5f), string{(int32)Entry->CycleNum} .CharArray);
		}
	}
}

ImGui::EndChild();
ImGui::End();
}
*/

#endif