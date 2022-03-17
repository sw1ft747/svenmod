#ifndef CLIENTSTATE_H
#define CLIENTSTATE_H

#ifdef _WIN32
#pragma once
#endif

typedef enum
{
	CLS_NONE = 0,		// no state yet, the engine is initializing
	CLS_DISCONNECTED, 	// not talking to a server
	CLS_CHALLENGE,		// sending request packets to the server
	CLS_CONNECTED,		// netchan_t established, waiting for svc_serverdata
	CLS_LOADING,		// loading to the server and verifying resources
	CLS_ACTIVE,			// game views should be displayed
} client_state_t;

#endif // CLIENTSTATE_H