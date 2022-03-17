#ifndef ICLIENTHOOKS_H
#define ICLIENTHOOKS_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "hook_codes.h"

#include "hl_sdk/engine/APIProxy.h"

//-----------------------------------------------------------------------------
// Purpose: handle client's dll functions
// 
// Copy already implemented class to don't bother with it
// 
// Note: if function returns value, the comment section will include
// the following: "Return value"
//-----------------------------------------------------------------------------

abstract_class IClientHooks
{
public:
	//-----------------------------------------------------------------------------
	// Called when the game initializes, client entered the game and whenever the
	// vid_mode is changed so the HUD can reinitialize itself
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_VidInit( void ) = 0;

	//-----------------------------------------------------------------------------
	// Called every screen frame to redraw the HUD
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_Redraw( float time, int intermission ) = 0;

	//-----------------------------------------------------------------------------
	// Called every time shared client dll/engine data gets changed, and
	// gives the cdll a chance to modify the data
	// 
	// Returns 1 if anything has been changed, 0 otherwise
	// 
	// Return value: @changed
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_UpdateClientData( int *changed, client_data_t *pcldata, float flTime ) = 0;

	//-----------------------------------------------------------------------------
	// Called when client dll process player's movement (CL_CreateMove already called atm)
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_PlayerMove( playermove_t *ppmove, int server ) = 0;

	//-----------------------------------------------------------------------------
	// Called when mouse should be enabled
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT IN_ActivateMouse( void ) = 0;

	//-----------------------------------------------------------------------------
	// Called when mouse should be disabled
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT IN_DeactivateMouse( void ) = 0;

	//-----------------------------------------------------------------------------
	// Called when engine must handle mouse button event
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT IN_MouseEvent( int mstate ) = 0;

	//-----------------------------------------------------------------------------
	// Called when mouse accumulations and old states need to be reset
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT IN_ClearStates( void ) = 0;

	//-----------------------------------------------------------------------------
	// Called when mouse need to accumulate
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT IN_Accumulate( void ) = 0;

	//-----------------------------------------------------------------------------
	// Called when create and send the intended movement message to the server
	// 
	// If (active == 1) then we are:
	// 1) not playing back demos (where our commands are ignored)
	// 2) we have finished signing on to server
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT CL_CreateMove( float frametime, usercmd_t *cmd, int active ) = 0;

	//-----------------------------------------------------------------------------
	// Returns 1 if the client in third person, otherwise 0
	// 
	// Return value: @thirdperson
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HOOK_RETURN_VALUE CL_IsThirdPerson( int *thirdperson ) = 0;

	//-----------------------------------------------------------------------------
	// Allows the engine to get a kbutton_t directly (so it can check +mlook state, etc)
	// for saving out to .cfg files
	// 
	// Return value: @button
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HOOK_RETURN_VALUE KB_Find( kbutton_t **button, const char *name ) = 0;

	//-----------------------------------------------------------------------------
	// Called on Host_Frame to process camera movement
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT CAM_Think( void ) = 0;

	//-----------------------------------------------------------------------------
	// Called before render the view to set the render settings
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT V_CalcRefdef( ref_params_t *pparams ) = 0;

	//-----------------------------------------------------------------------------
	// Called to add an entity in visible list to render it
	// Return 0 to filter entity from visible list for rendering
	// 
	// @type: ET_NORMAL, ET_PLAYER, ET_BEAM, ET_TEMPENTITY, ET_FRAGMENTED
	// 
	// Return value: @visible
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_AddEntity( int *visible, int type, cl_entity_t *ent, const char *modelname ) = 0;

	//-----------------------------------------------------------------------------
	// Gives us a chance to add additional entities to the render this frame
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_CreateEntities( void ) = 0;

	//-----------------------------------------------------------------------------
	// Non-transparent triangles
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_DrawNormalTriangles( void ) = 0;

	//-----------------------------------------------------------------------------
	// Render any triangles with transparent rendermode needs here
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_DrawTransparentTriangles( void ) = 0;

	//-----------------------------------------------------------------------------
	// The entity's studio model description indicated an event was fired
	// during this frame, handle the event by it's tag ( e.g., muzzleflash, sound )
	// 
	// See enum StudioAnimEvent
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_StudioEvent( const mstudioevent_t *studio_event, const cl_entity_t *entity ) = 0;

	//-----------------------------------------------------------------------------
	// Client calls this during prediction, after it has moved the player and
	// updated any info changed into 'to->time' is the current client clock
	// based on prediction cmd is the command that caused the movement,
	// etc runfuncs is 1 if this is the first time we've predicted this command.
	// If so, sounds and effects should play, otherwise, they should be ignored
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_PostRunCmd( local_state_t *from, local_state_t *to, usercmd_t *cmd, int runfuncs, double time, unsigned int random_seed ) = 0;

	//-----------------------------------------------------------------------------
	// The server sends us our origin with extra precision as part of the clientdata structure,
	// not during the normal playerstate update in entity_state_t.
	// In order for these overrides to eventually get to the appropriate playerstate structure,
	// we need to copy them into the state structure at this point.
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_TxferLocalOverrides( entity_state_t *state, const clientdata_t *client ) = 0;

	//-----------------------------------------------------------------------------
	// We have received entity_state_t for this player over the network.
	// We need to copy appropriate fields to the playerstate structure
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_ProcessPlayerState( entity_state_t *dst, const entity_state_t *src ) = 0;

	//-----------------------------------------------------------------------------
	// Because we can predict an arbitrary number of frames before the server responds with an update, we need to be able to copy client side prediction data in
	// from the state that the server ack'd receiving, which can be anywhere along the predicted frame path ( i.e., we could predict 20 frames into the future and the server ack's
	// up through 10 of those frames, so we need to copy persistent client-side only state from the 10th predicted frame to the slot the server update is occupying.
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_TxferPredictionData( entity_state_t *ps, const entity_state_t *pps, clientdata_t *pcd, const clientdata_t *ppcd, weapon_data_t *wd, const weapon_data_t *pwd ) = 0;

	//-----------------------------------------------------------------------------
	// Engine wants us to parse some data from the demo stream
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT Demo_ReadBuffer( int size, unsigned const char *buffer ) = 0;

	//-----------------------------------------------------------------------------
	// Return 1 if the packet is valid.
	// Set response_buffer_size if you want to send a response packet.
	// Incoming, it holds the max size of the response_buffer, so you must zero it out if you choose not to respond.
	// 
	// By default, it doesn't respond, zeros @response_buffer_size and returns 0.
	// 
	// Return value: @valid_packet (0 or 1)
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_ConnectionlessPacket( int *valid_packet, netadr_t *net_from, const char *args, const char *response_buffer, int *response_buffer_size ) = 0;

	//-----------------------------------------------------------------------------
	// Engine calls this to enumerate player collision hulls, for prediction.
	// Return 0 if the hullnumber doesn't exist.
	// 
	// Return value: @hullnumber_exist (0 or 1)
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_GetHullBounds( int *hullnumber_exist, int hullnumber, float *mins, float *maxs ) = 0;

	//-----------------------------------------------------------------------------
	// Called by engine every frame that client .dll is loaded (on Host_Frame)
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_Frame( double time ) = 0;

	//-----------------------------------------------------------------------------
	// Return 1 to allow engine to process the key, otherwise, act on it as needed
	// If you don't want to process key event for gViewPort, then don't even call the function
	// 
	// Return value: @process_key (0 or 1)
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_Key_Event( int *process_key, int down, int keynum, const char *pszCurrentBinding ) = 0;

	//-----------------------------------------------------------------------------
	// Simulation and cleanup of temporary entities
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_TempEntUpdate( double frametime, double client_time, double cl_gravity, TEMPENTITY **ppTempEntFree, TEMPENTITY **ppTempEntActive, int (*Callback_AddVisibleEntity)(cl_entity_t *pEntity), void (*Callback_TempEntPlaySound)(TEMPENTITY *pTemp, float damp) ) = 0;
	
	//-----------------------------------------------------------------------------
	// If you specify negative numbers for beam start and end point entities, then
	// the engine will call back into this function requesting a pointer to a cl_entity_t object
	// that describes the entity to attach the beam onto.
	// 
	// Return value: @ent
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HOOK_RETURN_VALUE HUD_GetUserEntity( cl_entity_t **ent, int index ) = 0;

	//-----------------------------------------------------------------------------
	// Called when a player starts or stops talking
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_VoiceStatus( int entindex, qboolean bTalking ) = 0;

	//-----------------------------------------------------------------------------
	// Called when a director event message was received
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_DirectorMessage( unsigned char command, unsigned int firstObject, unsigned int secondObject, unsigned int flags ) = 0;

	//-----------------------------------------------------------------------------
	// Sets the location of the input for chat text
	//-----------------------------------------------------------------------------

	virtual HOOK_RESULT HUD_ChatInputPosition( int *x, int *y ) = 0;
};

#endif // ICLIENTHOOKS_H
