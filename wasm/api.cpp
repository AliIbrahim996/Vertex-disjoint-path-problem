//! @copyright 2022-2026 All rights reserved.
//! @author Ali Ibrahim
//!
//! The whole interface between the browser and the C++ core: two functions that exchange text.

#include <string>

#include <emscripten/emscripten.h>

#include "algolab/Algorithms.h"

namespace
{
	std::string buffer;
}

extern "C"
{
	//! JSON list of the available algorithms.
	EMSCRIPTEN_KEEPALIVE const char* algolab_catalog()
	{
		buffer = algolab::catalogJson();
		return buffer.c_str();
	}

	//! Run a simulation described in the line-based request format and return its trace as JSON.
	EMSCRIPTEN_KEEPALIVE const char* algolab_run(const char* request)
	{
		buffer = algolab::runFromText(request ? request : "");
		return buffer.c_str();
	}
}
