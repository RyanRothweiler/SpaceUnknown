namespace save_data {

#define MEMBER_PARS_MAX 131072

	struct member;

	struct pair {
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
		} Data;
	};

	struct member {
		int32 PairsCount;
		pair Pairs[MEMBER_PARS_MAX];
	};
};