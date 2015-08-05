/*

Copyright (c) 1999, 2000 Samuel A. Falvo II

This software is provided 'as-is', without any implied or express warranty.
In no event shall the authors be held liable for damages arising from the
use this software.

Permission is granted for anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software in a
   product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

3. This notice may not be removed or altered from any source
   distribution.

*/

#ifndef MAX_REGKEY_LEN
#warning Compiler did not receive a -DMAX_REGKEY_LEN option.
#warning Defaulting to 128.
#define MAX_REGKEY_LEN		128
#endif

#ifndef MAX_PATH_LEN
#warning Compiler did not receive a -DMAX_PATH_LEN option.
#warning Defaulting to 128.
#define MAX_PATH_LEN		128
#endif

#ifdef REGPATH
#define STR_REGISTRYHOME	REGPATH
#else
#warning Compiler did not receive a -DREGPATH=\\"$$REGPATH\\" option.
#warning Using /andromeda-1/ as default.
#define STR_REGISTRYHOME	"/andromeda"
#endif

#ifdef REGINPROCSERVERS
#define STR_INPROCSERVERS	REGINPROCSERVERS
#else
#warning Compiler did not receive a -DREGINPROCSERVERS=\\"$$REGINPROCSERVERS\\"
#warning option.  Using /InprocServers/ as default.
#define STR_INPROCSERVERS	"/InprocServers/"
#endif

#ifdef REGINPROCHANDLERS
#define STR_INPROCHANDLERS	REGINPROCHANDLERS
#else
#warning Compiler did not receive a -DREGINPROCHANDLERS=\\"$$REGINPROCHANDLERS\\"
#warning option.  Using /InprocHandlers/ as default.
#define STR_INPROCHANDLERS	"/InprocHandlers/"
#endif

#ifdef REGTREATAS
#define STR_TREATAS		REGTREATAS
#else
#warning Compiler did not receive a -DREGTREATAS=\\"$$REGTREATAS\\"
#warning option.  Using /TreatAS/ as default.
#define STR_TREATAS		"/TreatAs/"
#endif

#ifdef REGGUIDTEMPLATE
#define STR_GUIDSTRING_TEMPLATE	REGGUIDTEMPLATE
#else
#warning Compiler did not receive a -DREGGUIDTEMPLATE=\\"...\\" option.
#if defined( __ELATE__ ) && defined( __GNUC__ )
#warning Using the default "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"
#define STR_GUIDSTRING_TEMPLATE	"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"
#elif defined( __LINUX__ ) && defined( __GNUC__ )
#warning Using the default "{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"
#define STR_GUIDSTRING_TEMPLATE	"{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"
#endif
#endif

#ifdef DLLINITFUNC
#define WSTR_DLLINIT		DLLINITFUNC
#else
#warning Compiler did not receive a -DDLLINITFUNC=L\\"...\\" option.
#warning Using the default of L"__init_com_".
#define WSTR_DLLINIT		L"__init_com_"
#endif

#ifdef DLLEXPUNGEFUNC
#define WSTR_DLLEXPUNGE		DLLEXPUNGEFUNC
#else
#warning Compiler did not receive a -DDLLEXPUNGEFUNC=L\\"...\\" option.
#warning Using the default of L"__expunge_com_".
#define WSTR_DLLEXPUNGE		L"__expunge_com_"
#endif

#ifdef DLLSYMLEN
#define MAX_SYMBOL_LEN		DLLSYMLEN
#else
#warning Compiler did not receive a -DDLLSYMLEN=n option.
#warning Defaulting to 32.
#define MAX_SYMBOL_LEN		32
#endif
