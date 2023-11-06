namespace save_data {

	#define MEMBER_PAIRS_MAX 131'072

	struct pair {
		i8 Used;
		int64 Key;

		meta_member_type Type;

		union {
			uint8 ui8;
			uint16 ui16;
			uint32 ui32;

			int8 i8;
			int16 i16;
			int32 i32;
			int64 i64;

			real32 r32;
			real64 r64;

			bool32 b32;
		} Data;
	};

	struct member_array {
		pair Array[20];
	};

	struct member_info {
		i32 PairsCount;
		b32 Linear;
	};

	// !!! Adding anything here will break theSaveData::Write
	struct member {
		member_info Info;
		member_array Pairs[MEMBER_PAIRS_MAX];
	};

};
