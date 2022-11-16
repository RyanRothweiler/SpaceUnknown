#pragma once

#ifndef HashH
#define HashH

namespace hash {

	struct table {
		list_head* Table;
		uint32 TableSize;

		// NOTE this forces all indecies to be the same size for convenience. Isn't necessarily required though.
		uint32 DataSize;
	};
}

#endif