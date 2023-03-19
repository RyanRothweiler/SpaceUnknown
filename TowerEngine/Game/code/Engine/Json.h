namespace json {
	struct json_data;

	struct json_pair {
		string Key;

		union {
			string Data;
			json_data* Child;
		};

		//int32 ChildrenNodesCount;
	};

	struct json_data {
		int32 PairsCount;
		json_pair* Pairs;
	};

	enum class token_type {
		close_curly,
		open_curly,
		quote,
		comma,
		colon,
		identifier,
		end_of_file
	};

	struct tokenizer {
		char *Position;
	};

	const int32 Max_Pairs_Count = 100;
}