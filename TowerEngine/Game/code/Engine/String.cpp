#pragma once
#ifndef StringCPP
#define StringCPP

#include "Math.cpp"

#define MAX_STRING_SIZE 200

bool32 IsLineEnd(char c)
{
	return (c == '\n' || c == '\r' || c == GameNull);
}

bool IsWhitespace(char c)
{
	return (c == '\n' || c == '\r' || c == ' ' || c == '\t');
}

bool IsNumeric(char c)
{
	bool Result = (c >= '0') && (c <= '9');
	// TODO handle negative?
	return Result;
}

int32
DigitCount(int64 Input)
{
	int32 Count = 0;
	while (Input != 0) {
		Input = Input / 10;
		Count++;
	}

	return (Count);
}

void
IntToCharArray(int64 Input, char *Output, uint32 Offset)
{
	char *OutputPointer = Output + DigitCount(Input) - 1 + Offset;

	if (Input == 0) {
		*Output = '0';
	}

	int32 Count = 1;
	while (Input != 0) {
		if (Count % 4 == 0) {
			// *OutputPointer = ',';
			// OutputPointer--;
		} else {
			uint8 LastDigit = Input % 10;
			*OutputPointer = '0' + LastDigit;
			OutputPointer--;

			Input = Input / 10;
		}

		Count++;
	}
}

void
IntToCharArray(int64 Input, char *Output)
{
	IntToCharArray(Input, Output, 0);
}

int32
CharArrayLength(const char *String)
{
	int Count = 0;
	while (*String++) {
		Count++;
	}
	return (Count);
}


int32
CharArrayLength(char *String)
{
	int Count = 0;
	while (*String++) {
		Count++;
	}
	return (Count);
}

bool32 CharArraysEqual(char* A, char* B)
{
	int32 ALen = CharArrayLength(A);
	int32 BLen = CharArrayLength(B);

	if (ALen != BLen) { return false; }

	for (int32 x = 0; x < ALen; x++) {
		if (A[x] != B[x]) {
			return false;
		}
	}

	return true;
}

void
ConcatCharArrays(char *SourceA, char *SourceB, char *Destination)
{
	int32 SourceALength = CharArrayLength(SourceA);
	int32 SourceBLength = CharArrayLength(SourceB);

	for (int32 Index = 0; Index < SourceALength; Index++) {
		*Destination++ = *SourceA++;
	}

	for (int32 Index = 0; Index < SourceBLength; Index++) {
		*Destination++ = *SourceB++;
	}

	*Destination++ = 0;
}

struct string {

	char CharArray[MAX_STRING_SIZE] = {};

	char* Array()
	{
		return &CharArray[0];
	}

	string()
	{

	}

	string (char Input)
	{
		CharArray[0] = Input;
	}

	string(char *Input)
	{
		uint32 StringCount = CharArrayLength(Input);
		Assert(StringCount < MAX_STRING_SIZE);
		if (StringCount < MAX_STRING_SIZE) {
			for (uint32 Index = 0;
			        Index < StringCount;
			        Index++) {
				CharArray[Index] = Input[Index];
			}
			CharArray[StringCount] = '\0';
		}
	}

	string(const char *Input)
	{
		uint32 StringCount = CharArrayLength(Input);
		Assert(StringCount < MAX_STRING_SIZE);
		if (StringCount < MAX_STRING_SIZE) {
			for (uint32 Index = 0;
			        Index < StringCount;
			        Index++) {
				CharArray[Index] = Input[Index];
			}
			CharArray[StringCount] = '\0';
		}
	}

	// TODO can we compress these?
	string(int64 Input)
	{
		if (Input < 0) {
			CharArray[0] = '-';
			IntToCharArray(Input * -1, CharArray, 1);
		} else {
			IntToCharArray(Input, CharArray);
		}
	}

	string(int32 Input)
	{
		if (Input < 0) {
			CharArray[0] = '-';
			IntToCharArray(Input * -1, CharArray, 1);
		} else {
			IntToCharArray(Input, CharArray);
		}
	}


	string(uint32 Input)
	{
		if (Input < 0) {
			CharArray[0] = '-';
			IntToCharArray(Input * -1, CharArray, 1);
		} else {
			IntToCharArray(Input, CharArray);
		}
	}

	string(uint64 Input)
	{
		if (Input < 0) {
			CharArray[0] = '-';
			IntToCharArray(Input * -1, CharArray, 1);
		} else {
			IntToCharArray(Input, CharArray);
		}
	}

	string (real64 Input)
	{
		uint32 PreDecimalCount = DigitCount((int32)Input);

		real64 MovedDecimal = 100 * Input;
		char Dummy[MAX_STRING_SIZE] = {};
		if (Input < 0) {
			PreDecimalCount++;
			IntToCharArray((int64)(-MovedDecimal), Dummy);
		} else {
			IntToCharArray((int64)MovedDecimal, Dummy);
		}

		uint32 ArrayIndex = 0;

		if (Input < 0.0f) {
			CharArray[0] = '-';
			ArrayIndex++;
		}

		for (uint32 Index = 0; Index < MAX_STRING_SIZE - 1; Index++) {
			if (ArrayIndex == PreDecimalCount) {
				CharArray[ArrayIndex] = '.';
				ArrayIndex++;
			}

			CharArray[ArrayIndex] = Dummy[Index];
			ArrayIndex++;
		}
	}

};

string Real64ToString(real64 Input, int32 DecimalsCount)
{
	string Ret = {};
	uint32 PreDecimalCount = DigitCount((int32)Input);

	real64 MovedDecimal = pow(10, DecimalsCount) * Input;
	char Dummy[MAX_STRING_SIZE] = {};
	if (Input < 0) {
		PreDecimalCount++;
		IntToCharArray((int64)(-MovedDecimal), Dummy);
	} else {
		IntToCharArray((int64)MovedDecimal, Dummy);
	}

	uint32 ArrayIndex = 0;

	if (Input < 0.0f) {
		Ret.CharArray[0] = '-';
		ArrayIndex++;
	}

	for (uint32 Index = 0; Index < MAX_STRING_SIZE - 1; Index++) {
		if (ArrayIndex == PreDecimalCount) {
			Ret.CharArray[ArrayIndex] = '.';
			ArrayIndex++;
		}

		Ret.CharArray[ArrayIndex] = Dummy[Index];
		ArrayIndex++;
	}

	return Ret;
}

const string EmptyString = string{};

string BuildString(char* Input, uint32 InputLength)
{
	string Output = {};
	if (InputLength < MAX_STRING_SIZE) {
		for (uint32 Index = 0; Index < InputLength; Index++) {
			Output.CharArray[Index] = Input[Index];
		}
		Output.CharArray[InputLength] = '\0';
	}

	return Output;
}

int32
StringLength(string String)
{
	return (CharArrayLength(String.CharArray));
}

bool
StringIsEmpty(string Str)
{
	return (StringLength(Str) <= 0);
}

string
IntToString(int64 Input)
{
	string NewString = Input;
	return (NewString);
}

string
operator + (string A, string B)
{
	string Output = {};
	ConcatCharArrays(A.CharArray, B.CharArray, Output.CharArray);
	return (Output);
}

bool
operator == (string A, string B)
{
	int32 ALength = StringLength(A);
	int32 BLength = StringLength(B);

	if (ALength == BLength) {
		for (int charIndex = 0;
		        charIndex < ALength;
		        charIndex++) {
			if (A.CharArray[charIndex] != B.CharArray[charIndex]) {
				return (false);
			}
		}

		return (true);
	}
	return (false);
}

bool
operator != (string A, string B)
{
	return (!(A == B));
}

void
ConcatIntChar(int64 IntInput, char *CharInput,
              char *CharOutput)
{
	char IntInputAsChar[256] = {};
	IntToCharArray(IntInput, IntInputAsChar);
	ConcatCharArrays(IntInputAsChar, CharInput, CharOutput);
}


string
CopyString(string OrigString)
{
	string FinalString = {};

	uint32 StringCount = StringLength(OrigString);
	for (uint32 Index = 0;
	        Index < StringCount;
	        Index++) {
		FinalString.CharArray[Index] = OrigString.CharArray[Index];
	}

	return (FinalString);
}

void
ClearString(string *String)
{
	for (int CharIndex = 0;
	        CharIndex < MAX_STRING_SIZE;
	        CharIndex++) {
		String->CharArray[CharIndex] = 0x00000000;
	}
}

struct split_info {
	string *Strings;
	uint32 StringsCount;
};

split_info
SplitString(string StringSplitting, char SplitChar, bool32 KeepSplitChar, bool32 RemoveWhiteSpace, memory_arena *Memory)
{
	split_info SplitInfo = {};

	uint32 WordLength = StringLength(StringSplitting);

	uint32 SplitStartIndex = 0;
	uint32 SplitCheckIndex = 0;

	uint32 CurrentArraySize = 50;
	void *Array = ArenaAllocate(Memory, sizeof(string) * CurrentArraySize);
	string *SplitStrings = (string *)Array;
	uint32 CurrentStringsIndex = 0;

	SplitInfo.StringsCount = 0;
	SplitInfo.Strings = SplitStrings;

	while (SplitCheckIndex < WordLength) {
		if (StringSplitting.CharArray[SplitCheckIndex] == SplitChar) {
			ClearString(&SplitStrings[CurrentStringsIndex]);
			char *CharArray = SplitStrings[CurrentStringsIndex].CharArray;

			uint32 StringFillingIndex = 0;
			for (uint32 Index = SplitStartIndex; Index < SplitCheckIndex; Index++) {

				bool32 ValidChar = true;
				char NewChar = StringSplitting.CharArray[Index];
				if (RemoveWhiteSpace) {
					ValidChar = (NewChar != '\r') && (NewChar != '\t');
				}

				if (ValidChar) {
					SplitStrings[CurrentStringsIndex].CharArray[StringFillingIndex] = NewChar;
					StringFillingIndex++;
				}
			}

			CurrentStringsIndex++;

			// TODO make a system which doubles the size when we want an array larger thatn the one delclared here.
			// do such a thing here instead of asserting this
			Assert(CurrentStringsIndex < CurrentArraySize);

			SplitInfo.StringsCount++;

			SplitStartIndex = SplitCheckIndex;
			if (!KeepSplitChar) {
				SplitStartIndex++;
			}

			SplitCheckIndex++;
		} else {
			SplitCheckIndex++;
		}
	}

	// add the last string
	SplitStrings[CurrentStringsIndex] = string{};
	char *CharArray = SplitStrings[CurrentStringsIndex].CharArray;

	uint32 StringFillingIndex = 0;
	for (uint32 Index = SplitStartIndex; Index < SplitCheckIndex; Index++) {

		bool32 ValidChar = true;
		char NewChar = StringSplitting.CharArray[Index];
		if (RemoveWhiteSpace) {
			ValidChar = (NewChar != '\r') && (NewChar != '\t');
		}

		if (ValidChar) {
			SplitStrings[CurrentStringsIndex].CharArray[StringFillingIndex] = NewChar;
			StringFillingIndex++;
		}
	}
	CharArray[SplitCheckIndex + 1] = '\0';

	CurrentStringsIndex++;
	SplitInfo.StringsCount++;

	return (SplitInfo);
}

bool32 CharIsNumber(char Char)
{
	// NOTE this handles negatives too. So a '-' is assumed part of the number
	// Should this also count '.' as a number for floats? It does in Crossbow.cpp, if this comes up again then maybe add '.' here
	return Char == '-' || Char == '1' || Char == '2' || Char == '3' || Char == '4' || Char == '5' || Char == '6' || Char == '7' || Char == '8' || Char == '9' || Char == '0';
}

bool32 StringStartsWith(string Source, string Starting)
{
	int32 SourceLen = StringLength(Source);
	int32 StartingLen = StringLength(Starting);

	if (SourceLen == 0 || StartingLen == 0) { return false; }
	if (StartingLen > SourceLen) { return false; }

	for (int x = 0; x < StartingLen; x++) {
		if (Source.CharArray[x] != Starting.CharArray[x])  {
			return false;
		}
	}

	return true;
}

bool32 StringEndsWith(string Source, string Ending)
{
	int32 SourceLen = StringLength(Source);
	int32 EndLen = StringLength(Ending);

	if (SourceLen == 0 || EndLen == 0) { return false; }
	if (EndLen > SourceLen) { return false; }

	for (int x = 0; x < EndLen; x++) {
		char S = Source.CharArray[SourceLen - 1 - x];
		char En = Ending.CharArray[EndLen - 1 - x];
		if (S != En) {
			return false;
		}
	}

	return true;
}

int64
StringToInt64(string String)
{
	uint32 MaxLength = CharArrayLength("18446744073709551615");
	uint32 StringCount = ClampValue(0, MaxLength - 1, CharArrayLength(String.CharArray));

	int64 FinalNumber = 0;
	bool32 IsNegative = false;

	// Handle leading zeroes such as 0012343
	int32 NumStart = 0;
	for (uint32 CharIndex = 0; CharIndex < StringCount; CharIndex++) {
		if (String.CharArray[CharIndex] != '0') {
			NumStart = CharIndex;
			break;
		}
	}

	for (uint32 CharIndex = NumStart; CharIndex < StringCount; CharIndex++) {
		FinalNumber = FinalNumber * 10;

		char NextCharacter = String.CharArray[CharIndex];
		switch (NextCharacter) {
			case ('-'): IsNegative = true; break;
			case ('0'): FinalNumber += 0; break;
			case ('1'): FinalNumber += 1; break;
			case ('2'): FinalNumber += 2; break;
			case ('3'): FinalNumber += 3; break;
			case ('4'): FinalNumber += 4; break;
			case ('5'): FinalNumber += 5; break;
			case ('6'): FinalNumber += 6; break;
			case ('7'): FinalNumber += 7; break;
			case ('8'): FinalNumber += 8; break;
			case ('9'): FinalNumber += 9; break;

			// Ignore white space
			case ('\n'): { } break;
			case ('\t'): { } break;
			case ('\r'): { } break;

			default: {
				// String is not entirely a number
				Assert(0);
			}
			break;
		}
	}

	if (IsNegative) {
		FinalNumber = FinalNumber * -1;
	}

	return (FinalNumber);
}

int32
StringToInt32(string String)
{
	uint32 MaxLength = CharArrayLength("2147483647");
	uint32 StringCount = ClampValue(0, MaxLength - 1, CharArrayLength(String.CharArray));

	int32 FinalNumber = 0;
	bool32 IsNegative = false;

	// Handle leading zeroes such as 0012343
	int32 NumStart = 0;
	for (uint32 CharIndex = 0; CharIndex < StringCount; CharIndex++) {
		if (String.CharArray[CharIndex] != '0') {
			NumStart = CharIndex;
			break;
		}
	}

	for (uint32 CharIndex = NumStart; CharIndex < StringCount; CharIndex++) {
		FinalNumber = FinalNumber * 10;

		char NextCharacter = String.CharArray[CharIndex];
		switch (NextCharacter) {
			case ('-'): IsNegative = true; break;
			case ('0'): FinalNumber += 0; break;
			case ('1'): FinalNumber += 1; break;
			case ('2'): FinalNumber += 2; break;
			case ('3'): FinalNumber += 3; break;
			case ('4'): FinalNumber += 4; break;
			case ('5'): FinalNumber += 5; break;
			case ('6'): FinalNumber += 6; break;
			case ('7'): FinalNumber += 7; break;
			case ('8'): FinalNumber += 8; break;
			case ('9'): FinalNumber += 9; break;

			// Ignore white space
			case ('\n'): { } break;
			case ('\t'): { } break;
			case ('\r'): { } break;

			default: {
				// String is not entirely a number
				Assert(0);
			}
			break;
		}
	}

	if (IsNegative) {
		FinalNumber = FinalNumber * -1;
	}

	return (FinalNumber);
}

real64
StringToReal64(string Str)
{
	// Assume scientific notation is basically zero
	int len = StringLength(Str);
	for (int x = 0; x < len; x++) {
		if (Str.CharArray[x] == 'e') {
			return 0;
		}
	}

	int32 NumBeforeDec = 0;
	int32 NumAfterDec = 0;
	int32 ZeroesAfterDecimal = 0;

	int32 CountUntilDec = 0;

	bool32 FoundDec = false;
	for (int x = 0; x < len; x++) {
		if (Str.CharArray[x] == '.') {
			FoundDec = true;
			break;
		}
		CountUntilDec++;
	}

	string StrBeforDec = BuildString(&Str.CharArray[0], CountUntilDec);
	char* DecStart = &Str.CharArray[0] + CountUntilDec + 1; // + 1 to skip over the decimal itself
	string StrAfterDec = BuildString(DecStart, len - CountUntilDec);


	if (!FoundDec) {
		NumBeforeDec = StringToInt32(Str);
	} else {
		NumBeforeDec = StringToInt32(StrBeforDec);
		NumAfterDec = StringToInt32(StrAfterDec);

		for (int x = 0; x < StringLength(StrAfterDec); x++) {
			if (StrAfterDec.CharArray[x] == '0') {
				ZeroesAfterDecimal++;
			} else {
				break;
			}
		}
	}

	int32 Neg = 1;
	if (Str.CharArray[0] == '-') {
		Neg = -1;
		if (NumBeforeDec < 0) {
			NumBeforeDec = NumBeforeDec * -1;
		}
	}

	int32 AfterDecimalCount = DigitCount(NumAfterDec) + ZeroesAfterDecimal;
	int64 PowNum = Pow(10, AfterDecimalCount);
	int64 FullNum = (int64)((NumBeforeDec * PowNum) + NumAfterDec);
	real64 ReturnVal = (real64)FullNum / (real64)Pow(10, AfterDecimalCount);
	ReturnVal = ReturnVal * Neg;
	return (ReturnVal);
}

/*
// NOTE Includes the decimal
char AlphaChars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.'};
char LineEndChars[] = {'\r', '\n'};

struct charwrap {
	char* Char;
};

string PullUntil(charwrap* Curr, char* ReturnOnThese, int32 ReturnOnCount)
{
	char* Start = Curr->Char;
	int32 Count = 0;

	while (true) {

		for (int x = 0; x < ReturnOnCount; x++) {
			if (Curr->Char[0] == ReturnOnThese[x]) {
				return BuildString(Start, Count);
			}
		}

		Count++;
		Curr++;
	}
}
*/

// Output a float with separators
string Humanize(int64 Input)
{
	int32 DigCount = DigitCount(Input);
	int32 SeparatorCount = (int32)((real32)DigCount / 3.1f); // This 3.1 is kinda a hack. Probably something smarter to do here but I'm dumb
	string Raw = string{Input};

	if (DigCount <= 3) {
		return Raw;
	}

	string Final = {};
	int c = 0;
	int n = (DigCount + SeparatorCount) - 1; // -1 because the final digit needs to be at 0 spot
	for (int i = DigCount - 1; i >= 0; i--) {
		Final.CharArray[n--] = Raw.CharArray[i];
		c++;

		if (c == 3) {
			Final.CharArray[n--] = ',';
			c = 0;
		}
	}

	return Final;
}

// dumb manual way but whatever
void Uppercase(string* Input)
{
	int Len = StringLength(*Input);
	for (int i = 0; i < Len; i ++) {
		switch (Input->CharArray[i]) {
			case 'a': Input->CharArray[i] = 'A'; break;
			case 'b': Input->CharArray[i] = 'B'; break;
			case 'c': Input->CharArray[i] = 'C'; break;
			case 'd': Input->CharArray[i] = 'D'; break;
			case 'e': Input->CharArray[i] = 'E'; break;
			case 'f': Input->CharArray[i] = 'F'; break;
			case 'g': Input->CharArray[i] = 'G'; break;
			case 'h': Input->CharArray[i] = 'H'; break;
			case 'i': Input->CharArray[i] = 'I'; break;
			case 'j': Input->CharArray[i] = 'J'; break;
			case 'k': Input->CharArray[i] = 'K'; break;
			case 'l': Input->CharArray[i] = 'L'; break;
			case 'm': Input->CharArray[i] = 'M'; break;
			case 'n': Input->CharArray[i] = 'N'; break;
			case 'o': Input->CharArray[i] = 'O'; break;
			case 'p': Input->CharArray[i] = 'P'; break;
			case 'q': Input->CharArray[i] = 'Q'; break;
			case 'r': Input->CharArray[i] = 'R'; break;
			case 's': Input->CharArray[i] = 'S'; break;
			case 't': Input->CharArray[i] = 'T'; break;
			case 'u': Input->CharArray[i] = 'U'; break;
			case 'v': Input->CharArray[i] = 'V'; break;
			case 'w': Input->CharArray[i] = 'W'; break;
			case 'x': Input->CharArray[i] = 'X'; break;
			case 'y': Input->CharArray[i] = 'Y'; break;
			case 'z': Input->CharArray[i] = 'Z'; break;
		}
	}
}

uint64 StringHash(string Input)
{
	unsigned long hash = 5381;
	int c;

	int32 Len = StringLength(Input);
	for (int i = 0; i < Len; i++) {
		c = Input.Array()[i];
		//while (c = *str++) {

		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
}

#if UNIT_TESTING
void
StringUnitTests(memory_arena* Memory)
{
	string First = {};
	string Second = {};
	First = "ABC124!+-`,./";
	Second = "ABC124!+-`,./";
	Assert(StringLength(First) == 13);
	Assert(First == First);
	Assert(First == Second);
	Assert(First == "ABC124!+-`,./");

	First = "Donald Trump";
	Second = "President";
	Assert(First != Second);

	Assert(StringLength(First) == 12);

	ClearString(&First);
	int length = StringLength(First);
	Assert(StringLength(First) == 0);

	// test string to real
	{
		{
			real64 OrigVal = 1234;
			string ValAsString = string{OrigVal};
			real64 ConvertedVal = StringToReal64(ValAsString);
			Assert(OrigVal == ConvertedVal);
		}
		{
			real64 OrigVal = 123.4;
			string ValAsString = string{OrigVal};
			real64 ConvertedVal = StringToReal64(ValAsString);
			Assert(OrigVal == ConvertedVal);
		}
		{
			real64 OrigVal = 12.34;
			string ValAsString = string{OrigVal};
			real64 ConvertedVal = StringToReal64(ValAsString);
			Assert(OrigVal == ConvertedVal);
		}
		{
			real64 OrigVal = 0.12;
			string ValAsString = string{OrigVal};
			real64 ConvertedVal = StringToReal64(ValAsString);
			Assert(OrigVal == ConvertedVal);
		}

		/*
		{
			real64 OrigVal = 1234123890.57;
			string ValAsString = string{OrigVal};
			real64 ConvertedVal = StringToReal64(ValAsString);
			Assert(OrigVal == ConvertedVal);
		}
		*/
	}

	// test split string
	{
		{
			string Str = "abc.efg";
			split_info Split = SplitString(Str, '.', false, true, Memory);
			Assert(Split.StringsCount == 2);
			Assert(Split.Strings[0] == "abc");
			Assert(Split.Strings[1] == "efg");
		}
		{
			string Str = "abc|efg";
			split_info Split = SplitString(Str, '|', false, true, Memory);
			Assert(Split.StringsCount == 2);
			Assert(Split.Strings[0] == "abc");
			Assert(Split.Strings[1] == "efg");
		}
		{
			string Str = "abcefg";
			split_info Split = SplitString(Str, 'e', false, true, Memory);
			Assert(Split.StringsCount == 2);
			Assert(Split.Strings[0] == "abc");
			Assert(Split.Strings[1] == "fg");
		}
		{
			string Str = "abc|ef|g";
			split_info Split = SplitString(Str, '|', false, true, Memory);
			Assert(Split.StringsCount == 3);
			Assert(Split.Strings[0] == "abc");
			Assert(Split.Strings[1] == "ef");
			Assert(Split.Strings[2] == "g");
		}
	}

	// string to int32
	{
		{
			string Str = "-1";
			int32 Val = StringToInt32(Str);
			Assert(Val == -1);
		}
		{
			string Str = "1234512";
			int32 Val = StringToInt32(Str);
			Assert(Val == 1234512);
		}
	}

	/* TODO need unit tests for
		- CopyString
		- ConcatIntChar
		- DigitCount
		- IntToCharArray (two methods_)
		*/
}
#endif


#endif