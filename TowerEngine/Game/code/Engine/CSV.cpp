#pragma once
#ifndef csv_cpp

namespace csv {

	struct entry {
		char* Data;
		int32 Length;
	};

	struct line {
		int32 EntriesCount;
		entry* Entries;
	};

	struct csv {
		int32 LinesCount;
		line* Lines;
	};

	csv Load(string Path, memory_arena* Memory)
	{
		csv Ret = {};

		read_file_result Result = PlatformApi.ReadFile(Path.CharArray, Memory);

		if (Result.ContentsSize > 0) {
			char* Start = (char*)Result.Contents;
			char* End = (char*)Result.Contents + Result.ContentsSize;

			// Get lines count
			{
				char* Next = Start;
				Ret.LinesCount = 0;
				while (Next < End) {
					if (*Next == '\n') {
						Ret.LinesCount++;
					}
					Next++;
				}
			}

			// allocate lines
			Ret.Lines = (line*)ArenaAllocate(Memory, sizeof(line) * Ret.LinesCount);

			// Get line data
			{
				int l = 0;
				char* Current = Start;

				while (Current < End) {
					char* CurrentLine = Current;

					line* Line = &Ret.Lines[l];
					l++;
					Line->EntriesCount = 0;


					// Count entries
					{
						char* Ent = CurrentLine;
						while (*Ent != '\n') {
							if (*Ent == ',') {
								Line->EntriesCount++;
							}
							Ent++;
						}

						// +1 for the last entry
						Line->EntriesCount++;
					}

					// allocate entries
					Line->Entries = (entry*)ArenaAllocate(Memory, sizeof(entry) * Line->EntriesCount);

					// Get entries
					{
						char* Ent = CurrentLine;
						int C = 0;

						entry* Entry = &Line->Entries[C];
						Entry->Data = Ent;

						int32 Length = 0;
						while (*Ent != '\n') {

							if (*Ent == ',' || *Ent == '\r') {

								// End this entry
								Entry->Length = Length;

								C++;
								Ent++;
								Length = 0;

								// start next entry
								Entry = &Line->Entries[C];
								Entry->Data = Ent;
							} else {
								Length++;
								Ent++;
							}
						}

						// Skip over line end
						Ent++;

						// Move current pointer
						Current = Ent;
					}
				}
			}

		}

		return Ret;
	};

	string GetString(csv* CSV, int64 LineIndex, int64 EntryIndex)
	{
		if (CSV->LinesCount > LineIndex) {
			line* Line = &CSV->Lines[LineIndex];
			if (Line->EntriesCount > EntryIndex) {
				entry* Entry = &Line->Entries[EntryIndex];
				return BuildString(Entry->Data, Entry->Length);
			}
		}

		return {};
	}

	real64 GetReal64(csv* CSV, int64 LineIndex, int64 EntryIndex)
	{
		string String = GetString(CSV, LineIndex, EntryIndex);
		return StringToReal64(String);
	}

	int64 GetInt64(csv* CSV, int64 LineIndex, int64 EntryIndex)
	{
		real64 Val = GetReal64(CSV, LineIndex, EntryIndex);
		return (int64)Val;
	}
}

#endif