#ifndef ICONFIGMANAGER_H
#define ICONFIGMANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "fieldtypes.h"
#include "steamtypes.h"

typedef enum
{
	CFG_PARSE_OK = 0,
	CFG_MISSING_FILE,
	CFG_ERROR_SECTION_START,
	CFG_ERROR_SECTION_END,
	CFG_ERROR_SECTION_EMPTY,
	CFG_ERROR_EMPTY_KEY,
	CFG_ERROR_EMPTY_VALUE
} CFG_PARSE_STATE;

//-----------------------------------------------------------------------------
// Purpose: interface to import/export INI files
// Import/export supports ONLY fundamental types (i.e.: int, float, bool, etc.)
//-----------------------------------------------------------------------------

abstract_class IConfigManager
{
public:
	virtual						~IConfigManager() {}

	//-----------------------------------------------------------------------------
	// Utility
	//-----------------------------------------------------------------------------

	// By default hash table have 64 buckets. Won't be changed if you call the function during import/export
	virtual int					GetHashTableSize( void ) = 0;
	virtual bool				ReinitializeHashTable( int iBuckets ) = 0;

	// It's 10 by default, when you start import or export
	virtual void				SetConversionRadix( int radix ) = 0;

	// Reset radix to 10
	virtual void				ResetRadix( void ) = 0;

	virtual int					GetLastState( void ) = 0;
	virtual const char			*GetLastErrorMessage( void ) = 0;
	virtual int					GetLastErrorLine( void ) = 0;

	//-----------------------------------------------------------------------------
	// Import
	//-----------------------------------------------------------------------------

	// Returns 'true' if a file was opened and parsed successfully
	virtual bool				BeginImport( const char *pszFilePath ) = 0;

	virtual void				EndImport( void ) = 0;

	// Before importing parameters, declare a section you're going to import from
	// Returns 'true' if a section exists
	virtual bool				BeginSectionImport( const char *pszSectionName ) = 0;

	// Call this function when you've finished importing parameters from a specified section
	virtual void				EndSectionImport( void ) = 0;

	// Advanced way to import any variable: ConfigManager()->ImportParam( "MyProperty", &myValue, DeduceValueFieldType(myValue) );
	virtual bool				ImportParam( const char *pszPropertyName, void *value, int fieldtype ) = 0;

	// CString will be duped, don't forget to call 'free()' if it's no longer needed 
	virtual bool				ImportParam( const char *pszPropertyName, const char *&value ) = 0;
	virtual bool				ImportParam( const char *pszPropertyName, short &value ) = 0;
	virtual bool				ImportParam( const char *pszPropertyName, int &value ) = 0;
	virtual bool				ImportParam( const char *pszPropertyName, int64 &value ) = 0;
	virtual bool				ImportParam( const char *pszPropertyName, uint16 &value ) = 0;
	virtual bool				ImportParam( const char *pszPropertyName, uint32 &value ) = 0;
	virtual bool				ImportParam( const char *pszPropertyName, uint64 &value ) = 0;
	virtual bool				ImportParam( const char *pszPropertyName, float &value ) = 0;
	virtual bool				ImportParam( const char *pszPropertyName, double &value ) = 0;
	virtual bool				ImportParam( const char *pszPropertyName, char &value ) = 0;
	virtual bool				ImportParam( const char *pszPropertyName, unsigned char &value ) = 0;
	virtual bool				ImportParam( const char *pszPropertyName, bool &value ) = 0;

	// ToDo: add Vector, Colors?

	//-----------------------------------------------------------------------------
	// Export
	//-----------------------------------------------------------------------------

	// Returns 'true' if a file was opened/created
	virtual bool				BeginExport( const char *pszFilePath ) = 0;

	virtual void				EndExport( void ) = 0;

	// Returns 'true' if a section is valid
	virtual bool				BeginSectionExport( const char *pszSectionName ) = 0;
	virtual void				EndSectionExport( void ) = 0;

	virtual void				ExportParam( const char *pszPropertyName, void *value, int fieldtype ) = 0;

	virtual void				ExportParam( const char *pszPropertyName, const char *value ) = 0;
	virtual void				ExportParam( const char *pszPropertyName, short value ) = 0;
	virtual void				ExportParam( const char *pszPropertyName, int value ) = 0;
	virtual void				ExportParam( const char *pszPropertyName, int64 value ) = 0;
	virtual void				ExportParam( const char *pszPropertyName, uint16 value ) = 0;
	virtual void				ExportParam( const char *pszPropertyName, uint32 value ) = 0;
	virtual void				ExportParam( const char *pszPropertyName, uint64 value ) = 0;
	virtual void				ExportParam( const char *pszPropertyName, float value ) = 0;
	virtual void				ExportParam( const char *pszPropertyName, double value ) = 0;
	virtual void				ExportParam( const char *pszPropertyName, char value ) = 0;
	virtual void				ExportParam( const char *pszPropertyName, unsigned char value ) = 0;
	virtual void				ExportParam( const char *pszPropertyName, bool value ) = 0;
};

extern IConfigManager *g_pConfigManager;
PLATFORM_INTERFACE IConfigManager *ConfigManager();

#define CONFIG_MANAGER_INTERFACE_VERSION "ConfigManager001"

#endif // ICONFIGMANAGER_H
