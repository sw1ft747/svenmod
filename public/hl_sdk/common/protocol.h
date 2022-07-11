#ifndef SDK_PROTOCOL_H
#define SDK_PROTOCOL_H

#ifdef _WIN32
#pragma once
#endif

#define PROTOCOL_VERSION		48

// Server-to-client network messages
#define SVC_BAD					0 // immediately crash client when received
#define SVC_NOP					1 // does nothing
#define SVC_DISCONNECT			2 // disconnects a player with given reason
#define SVC_EVENT				3 // an event, defined by the game library, has recently occurred on the server
#define SVC_VERSION				4 // disconnects the client and sends a message to the console if client's protocol doesn't match the server's protocol version
#define SVC_SETVIEW				5 // attaches a player's view to an entity
#define SVC_SOUND				6 // plays a sound file on the client
#define SVC_TIME				7 // notifies clients about the current server time
#define SVC_PRINT				8 // sends a message to the client's console
#define SVC_STUFFTEXT			9 // executes command on player
#define SVC_SETANGLE			10 // update immediately the client's view angles
#define SVC_SERVERINFO			11 // contains information about the server
#define SVC_LIGHTSTYLE			12 // setup light animation tables. 'a' is total darkness, 'z' is maxbright
#define SVC_UPDATEUSERINFO		13 // contains information about a particular client
#define SVC_DELTADESCRIPTION	14 // synchronizes client delta descriptions with server ones
#define SVC_CLIENTDATA			15 // contains information about the client state at the time of last server frame
#define SVC_STOPSOUND			16 // stops an ambient sound
#define SVC_PINGS				17 // contains ping and loss values for a number of players
#define SVC_PARTICLE			18 // shows a particle effect
#define SVC_WEAPONANIMEX		19
#define SVC_SPAWNSTATIC			20 // marks an entity as "static", so that it can be freed from server memory
#define SVC_EVENT_RELIABLE		21 // similar to SVC_EVENT, but no queuing takes place, and the message can only hold one event.
#define SVC_SPAWNBASELINE		22 // creates a baseline for future referencing
#define SVC_TEMPENTITY			23 // creates a temp entity.
#define SVC_SETPAUSE			24 // puts client to a pause
#define SVC_SIGNONNUM			25 // called just after client_putinserver. Signals the client that the server has marked it as "active"
#define SVC_CENTERPRINT			26 // sends a centered message
#define SVC_KILLEDMONSTER		27
#define SVC_EVENTLIST			28
#define SVC_SPAWNSTATICSOUND	29 // start playback of a sound, loaded into the static portion of the channel array
#define SVC_INTERMISSION		30 // shows the intermission camera view
#define SVC_FINALE				31 // shows the intermission camera view, and writes-out text passed in first parameter
#define SVC_CDTRACK				32 // plays a Half-Life music
#define SVC_RESTORE				33 // maintains a global transition table for the saved game
#define SVC_CUTSCENE			34 // shows the intermission camera view, and writes-out text passed in first parameter
#define SVC_WEAPONANIM			35 // plays a weapon sequence
#define SVC_DECALNAME			36 // allows to set, into the client's decals array and at specific position index (0->511), a decal name
#define SVC_ROOMTYPE			37 // sets client room_type cvar to provided value
#define SVC_ADDANGLE			38 // add an angle on the yaw axis of the current client's view angle
#define SVC_NEWUSERMSG			39 // registers a new user message on the client
#define SVC_PACKETENTITIES		40 // contains information about the entity states, like origin, angles and such
#define SVC_DELTAPACKETENTITIES	41 // contains information about the entity states, like origin, angles and such
#define SVC_CHOKE				42 // notify the client that some outgoing datagrams were not transmitted due to exceeding bandwidth rate limits
#define SVC_RESOURCELIST		43 // contains all the resources provided by the server for clients to download. Consistency info can also be included
#define SVC_NEWMOVEVARS			44 // updates client's movevars
#define SVC_RESOURCEREQUEST		45 // allows the client to send its own resource list (CLC_RESOURCELIST)
#define SVC_CUSTOMIZATION		46 // notifies the client that a new customization is avaliable for download
#define SVC_CROSSHAIRANGLE		47 // adjusts the weapon's crosshair angle
#define SVC_SOUNDFADE			48 // updates client side sound fade
#define SVC_FILETXFERFAILED		49 // sends a message to the client's console telling what file has failed to be transfered
#define SVC_HLTV				50 // tells client about current spectator mode
#define SVC_DIRECTOR			51
#define SVC_VOICEINIT			52 // sends sv_voicecodec and sv_voicequality cvars to client
#define SVC_VOICEDATA			53 // contains compressed voice data
#define SVC_SENDEXTRAINFO		54 // sends some extra information regarding the server
#define SVC_TIMESCALE			55
#define SVC_RESOURCELOCATION	56 // sends sv_downloadurl to client
#define SVC_SENDCVARVALUE		57 // request a cvar value from a connected client
#define SVC_SENDCVARVALUE2		58 // request a cvar value from a connected client
#define SVC_LASTMSG				58 // start user messages at this point

// Client-to-server network messages
#define CLC_BAD					0
#define CLC_NOP					1
#define CLC_MOVE				2
#define CLC_STRINGCMD			3
#define CLC_DELTA				4
#define CLC_RESOURCELIST		5
#define CLC_TMOVE				6
#define CLC_FILECONSISTENCY		7
#define CLC_VOICEDATA			8 // CL_AddVoiceToDatagram
#define CLC_HLTV				9
#define CLC_REQUESTCVARVALUE	10 // CL_Send_CvarValue
#define CLC_REQUESTCVARVALUE2	11 // CL_Send_CvarValue2

// Client move cmd bits
#define	CM_ANGLE1				( 1 << 0 )
#define	CM_ANGLE3				( 1 << 1 )
#define	CM_FORWARD				( 1 << 2 )
#define	CM_SIDE					( 1 << 3 )
#define	CM_UP					( 1 << 4 )
#define	CM_BUTTONS				( 1 << 5 )
#define	CM_IMPULSE				( 1 << 6 )
#define	CM_ANGLE2				( 1 << 7 )

/*

// Server-to-client network messages
#define SVC_BAD					0 // immediately crash client when received
#define SVC_NOP					1 // does nothing
#define SVC_DISCONNECT			2 // disconnects a player with given reason
#define SVC_EVENT				3 // an event, defined by the game library, has recently occurred on the server
#define SVC_VERSION				4 // disconnects the client and sends a message to the console if client's protocol doesn't match the server's protocol version
#define SVC_SETVIEW				5 // attaches a player's view to an entity
#define SVC_SOUND				6 // plays a sound file on the client
#define SVC_TIME				7 // notifies clients about the current server time
#define SVC_PRINT				8 // sends a message to the client's console
#define SVC_STUFFTEXT			9 // executes command on player
#define SVC_SETANGLE			10 // update immediately the client's view angles
#define SVC_SERVERINFO			11 // contains information about the server
#define SVC_LIGHTSTYLE			12 // setup light animation tables. 'a' is total darkness, 'z' is maxbright
#define SVC_UPDATEUSERINFO		13 // contains information about a particular client
#define SVC_DELTADESCRIPTION	14 // synchronizes client delta descriptions with server ones
#define SVC_CLIENTDATA			15 // contains information about the client state at the time of last server frame
#define SVC_STOPSOUND			16 // stops an ambient sound
#define SVC_PINGS				17 // contains ping and loss values for a number of players
#define SVC_PARTICLE			18 // shows a particle effect
#define SVC_DAMAGE				19
#define SVC_SPAWNSTATIC			20 // marks an entity as "static", so that it can be freed from server memory
#define SVC_EVENT_RELIABLE		21 // similar to SVC_EVENT, but no queuing takes place, and the message can only hold one event.
#define SVC_SPAWNBASELINE		22 // creates a baseline for future referencing
#define SVC_TEMPENTITY			23 // creates a temp entity.
#define SVC_SETPAUSE			24 // puts client to a pause
#define SVC_SIGNONNUM			25 // called just after client_putinserver. Signals the client that the server has marked it as "active"
#define SVC_CENTERPRINT			26 // sends a centered message
#define SVC_KILLEDMONSTER		27
#define SVC_FOUNDSECRET			28
#define SVC_SPAWNSTATICSOUND	29 // start playback of a sound, loaded into the static portion of the channel array
#define SVC_INTERMISSION		30 // shows the intermission camera view
#define SVC_FINALE				31 // shows the intermission camera view, and writes-out text passed in first parameter
#define SVC_CDTRACK				32 // plays a Half-Life music
#define SVC_RESTORE				33 // maintains a global transition table for the saved game
#define SVC_CUTSCENE			34 // shows the intermission camera view, and writes-out text passed in first parameter
#define SVC_WEAPONANIM			35 // plays a weapon sequence
#define SVC_DECALNAME			36 // allows to set, into the client's decals array and at specific position index (0->511), a decal name
#define SVC_ROOMTYPE			37 // sets client room_type cvar to provided value
#define SVC_ADDANGLE			38 // add an angle on the yaw axis of the current client's view angle
#define SVC_NEWUSERMSG			39 // registers a new user message on the client
#define SVC_PACKETENTITIES		40 // contains information about the entity states, like origin, angles and such
#define SVC_DELTAPACKETENTITIES	41 // contains information about the entity states, like origin, angles and such
#define SVC_CHOKE				42 // notify the client that some outgoing datagrams were not transmitted due to exceeding bandwidth rate limits
#define SVC_RESOURCELIST		43 // contains all the resources provided by the server for clients to download. Consistency info can also be included
#define SVC_NEWMOVEVARS			44 // updates client's movevars
#define SVC_RESOURCEREQUEST		45 // allows the client to send its own resource list (CLC_RESOURCELIST)
#define SVC_CUSTOMIZATION		46 // notifies the client that a new customization is avaliable for download
#define SVC_CROSSHAIRANGLE		47 // adjusts the weapon's crosshair angle
#define SVC_SOUNDFADE			48 // updates client side sound fade
#define SVC_FILETXFERFAILED		49 // sends a message to the client's console telling what file has failed to be transfered
#define SVC_HLTV				50 // tells client about current spectator mode
#define SVC_DIRECTOR			51
#define SVC_VOICEINIT			52 // sends sv_voicecodec and sv_voicequality cvars to client
#define SVC_VOICEDATA			53 // contains compressed voice data
#define SVC_SENDEXTRAINFO		54 // sends some extra information regarding the server
#define SVC_TIMESCALE			55
#define SVC_RESOURCELOCATION	56 // sends sv_downloadurl to client
#define SVC_SENDCVARVALUE		57 // request a cvar value from a connected client
#define SVC_SENDCVARVALUE2		58 // request a cvar value from a connected client
#define SVC_LASTMSG				64 // start user messages at this point

*/

#endif // SDK_PROTOCOL_H