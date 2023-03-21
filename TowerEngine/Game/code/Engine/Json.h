namespace json {

	const int32 Max_Pairs_Count = 100;
#define JSON_MAX_ARRAY 512

	struct json_data;

	struct json_pair {
		string Key;

		union {
			string Data;

			// NOTE this is a pointer only to avoid circlic structures
			json_data* Child[JSON_MAX_ARRAY];
		};
	};

	struct json_data {
		bool32 IsArray;

		int32 PairsCount;
		json_pair* Pairs;
	};

	enum class token_type {
		close_curly,
		open_curly,
		open_bracket,
		close_bracket,
		quote,
		comma,
		colon,
		identifier,
		end_of_file
	};

	struct tokenizer {
		char* Position;
		char* End;

		bool32 Valid()
		{
			return Position > End;
		}
	};
}