// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <nlohmann\json.hpp>
#include <iostream>
#define CONSTT //const

#define TEST_ARGV \
		int argc = 5; \
		const char *argv[] = { "program", "client", "127.0.0.1","8000", "Qizot", NULL }; \

#define FUNCTION_NAME //std::cout<<__FUNCTION__<<std::endl;
	


// TODO: reference additional headers your program requires here
