#include <stdio.h>

#include <IConfigManager.h>

#include <sys.h>
#include <dbg.h>
#include <baselogic.h>

#include <data_struct/hash.h>

#include <hl_sdk/common/Platform.h>

//-----------------------------------------------------------------------------
// Macro defs
//-----------------------------------------------------------------------------

#define INI_BUFFER_LENGTH 256
#define INI_COMMENT_PREFIX ";#"
#define INI_SECTION_PREFIX '['
#define INI_SECTION_POSTFIX ']'
#define INI_PARAMETER_DELIMITER "="
#define INI_STRIP_CHARS (" \t\n")
#define INI_STRIP_CHARS_LEN (sizeof(INI_STRIP_CHARS) - 1)
#define INI_COMMENT_PREFIX_LEN (sizeof(INI_COMMENT_PREFIX) - 1)

typedef enum
{
	CFG_NONE = 0,
	CFG_EXPORTING,
	CFG_IMPORTING
} CFG_STATE;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class CConfigManager : public IConfigManager
{
public:
	CConfigManager();
	virtual						~CConfigManager();

	//-----------------------------------------------------------------------------
	// Utility
	//-----------------------------------------------------------------------------

	virtual int					GetHashTableSize();
	virtual bool				ReinitializeHashTable(int iBuckets);

	virtual void				SetConversionRadix( int radix );
	virtual void				ResetRadix( void );

	virtual int					GetLastState( void );
	virtual const char			*GetLastErrorMessage( void );
	virtual int					GetLastErrorLine( void );

	//-----------------------------------------------------------------------------
	// Import
	//-----------------------------------------------------------------------------

	virtual bool				BeginImport( const char *pszFilePath );
	virtual void				EndImport( void );

	virtual bool				BeginSectionImport( const char *pszSectionName );
	virtual void				EndSectionImport( void );

	virtual bool				ImportParam( const char *pszPropertyName, void *value, int fieldtype );

	virtual bool				ImportParam( const char *pszPropertyName, const char *&value );
	virtual bool				ImportParam( const char *pszPropertyName, short &value );
	virtual bool				ImportParam( const char *pszPropertyName, int &value );
	virtual bool				ImportParam( const char *pszPropertyName, int64 &value );
	virtual bool				ImportParam( const char *pszPropertyName, uint16 &value );
	virtual bool				ImportParam( const char *pszPropertyName, uint32 &value );
	virtual bool				ImportParam( const char *pszPropertyName, uint64 &value );
	virtual bool				ImportParam( const char *pszPropertyName, float &value );
	virtual bool				ImportParam( const char *pszPropertyName, double &value );
	virtual bool				ImportParam( const char *pszPropertyName, char &value );
	virtual bool				ImportParam( const char *pszPropertyName, unsigned char &value );
	virtual bool				ImportParam( const char *pszPropertyName, bool &value );

	//-----------------------------------------------------------------------------
	// Export
	//-----------------------------------------------------------------------------

	virtual bool				BeginExport( const char *pszFilePath );
	virtual void				EndExport( void );

	virtual bool				BeginSectionExport( const char *pszSectionName );
	virtual void				EndSectionExport( void );

	virtual void				ExportParam( const char *pszPropertyName, void *value, int fieldtype );

	virtual void				ExportParam( const char *pszPropertyName, const char *value );
	virtual void				ExportParam( const char *pszPropertyName, short value );
	virtual void				ExportParam( const char *pszPropertyName, int value );
	virtual void				ExportParam( const char *pszPropertyName, int64 value );
	virtual void				ExportParam( const char *pszPropertyName, uint16 value );
	virtual void				ExportParam( const char *pszPropertyName, uint32 value );
	virtual void				ExportParam( const char *pszPropertyName, uint64 value );
	virtual void				ExportParam( const char *pszPropertyName, float value );
	virtual void				ExportParam( const char *pszPropertyName, double value );
	virtual void				ExportParam( const char *pszPropertyName, char value );
	virtual void				ExportParam( const char *pszPropertyName, unsigned char value );
	virtual void				ExportParam( const char *pszPropertyName, bool value );

private:
	typedef struct ConfigPair_s
	{
		const char *section;
		const char *property;
		const char *value;
	} ConfigPair_t;

	typedef const char *section_cstring_t;

	class CLookupFunctor
	{
	public:
		CLookupFunctor() {}

		// The compare function for config pairs
		bool operator()(const ConfigPair_t &a, const ConfigPair_t &b) const
		{
			return !stricmp(a.section, b.section) && !stricmp(a.property, b.property);
		}

		// The hash function for config pairs
		unsigned int operator()(const ConfigPair_t &item) const
		{
			return HashStringCaseless(item.property);
		}

		// The compare function for sections
		bool operator()(const section_cstring_t &a, const section_cstring_t &b) const
		{
			return !stricmp(a, b);
		}

		// The hash function for sections
		unsigned int operator()(const section_cstring_t &item) const
		{
			return HashStringCaseless(item);
		}
	};

private:
	void ClearConfigTable();
	void ClearSectionsTable();

	const char *FindValueFromConfig(const char *pszSection, const char *pszProperty);
	bool ParseFile(const char *pszFilePath);

	bool ParseUtil_ContainsChars(char ch, const char *chars, size_t length);
	char *ParseUtil_LStrip(char *str);
	void ParseUtil_RStrip(char *str);
	char *ParseUtil_Strip(char *str);
	void ParseUtil_RemoveComment(char *str);

private:
	CLookupFunctor m_Functor;

	CHash<ConfigPair_t, CLookupFunctor &, CLookupFunctor &> m_ConfigTable;
	CHash<const char *, CLookupFunctor &, CLookupFunctor &> m_SectionsTable;

	bool m_bSectionsDuped;
	bool m_bSectionsFilled;

	FILE *m_hConfigFile;
	const char *m_pszCurrentSection;
	int m_iRadix;

	int m_State;
	int m_ParseState;
	int m_iLastErrorLine;
	const char *m_pszLastErrorMessage;
};

//-----------------------------------------------------------------------------
// CConfigManager
//-----------------------------------------------------------------------------

CConfigManager::CConfigManager() : m_ConfigTable(63, m_Functor, m_Functor), m_SectionsTable(15, m_Functor, m_Functor)
{
	m_hConfigFile = NULL;
	m_pszCurrentSection = NULL;
	m_iRadix = 10;

	m_bSectionsDuped = false;
	m_bSectionsFilled = false;

	m_State = CFG_NONE;
	m_ParseState = CFG_PARSE_OK;
	m_iLastErrorLine = 0;
	m_pszLastErrorMessage = "";
}

CConfigManager::~CConfigManager()
{
	ClearConfigTable();
	ClearSectionsTable();

	m_ConfigTable.Purge();
	m_SectionsTable.Purge();
}

int CConfigManager::GetHashTableSize()
{
	return m_ConfigTable.Count();
}

bool CConfigManager::ReinitializeHashTable(int iBuckets)
{
	if (m_State == CFG_NONE)
	{
		Assert(iBuckets > 2);

		ClearConfigTable();
		ClearSectionsTable();

		m_ConfigTable.Resize(iBuckets);
		return true;
	}

	return false;
}

void CConfigManager::SetConversionRadix(int radix)
{
	Assert(radix == 2 || radix == 8 || radix == 10 || radix == 16);

	m_iRadix = radix;
}

void CConfigManager::ResetRadix(void)
{
	m_iRadix = 10;
}

int CConfigManager::GetLastState(void)
{
	return m_ParseState;
}

const char *CConfigManager::GetLastErrorMessage(void)
{
	return m_pszLastErrorMessage;
}

int CConfigManager::GetLastErrorLine(void)
{
	return m_iLastErrorLine;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void CConfigManager::ClearConfigTable()
{
	for (int i = 0; i < m_ConfigTable.Count(); i++)
	{
		HashIterator_t it = m_ConfigTable.First(i);

		while (m_ConfigTable.IsValidIterator(it))
		{
			ConfigPair_t &pair = m_ConfigTable.At(i, it);

		#pragma warning(push)
		#pragma warning(disable : 6001)

			free( (void *)pair.property );

		#pragma warning(pop)

			it = m_ConfigTable.Next(i, it);
		}
	}

	m_ConfigTable.Clear();
}

void CConfigManager::ClearSectionsTable()
{
	if (m_bSectionsFilled)
	{
		if (m_bSectionsDuped)
		{
			for (int i = 0; i < m_SectionsTable.Count(); i++)
			{
				HashIterator_t it = m_SectionsTable.First(i);

				while (m_SectionsTable.IsValidIterator(it))
				{
					auto section = m_SectionsTable.At(i, it);

					free( (void *)section );

					it = m_SectionsTable.Next(i, it);
				}
			}
		}

		m_SectionsTable.Clear();
		m_bSectionsFilled = false;
	}
}

const char *CConfigManager::FindValueFromConfig(const char *pszSection, const char *pszProperty)
{
	ConfigPair_t find_pair =
	{
		pszSection,
		pszProperty,
		NULL
	};

	ConfigPair_t *pair = m_ConfigTable.Find(find_pair);

	if (pair)
	{
		return pair->value;
	}

	return NULL;
}

bool CConfigManager::ParseFile(const char *pszFilePath)
{
	FILE *file = fopen(pszFilePath, "r");

	if (file)
	{
		int line = 0;
		bool bParsingSection = true;

		static int s_iBufferSize = INI_BUFFER_LENGTH;
		static char *s_pszFileBuffer = NULL;

		if ( !s_pszFileBuffer )
			s_pszFileBuffer = (char *)malloc(s_iBufferSize);

		long int endpos;
		fseek(file, 0, SEEK_END);
		endpos = ftell(file);
		rewind(file);

		const char *pszSection = NULL;

		// Read line by line
		while (fgets(s_pszFileBuffer, s_iBufferSize, file))
		{
			size_t length = strlen(s_pszFileBuffer);

			// Increase buffer size
			while (s_pszFileBuffer[length - 1] != '\n' && ftell(file) != endpos)
			{
				s_iBufferSize *= 2;

				void *realloc_mem = realloc(s_pszFileBuffer, s_iBufferSize);

				if ( !realloc_mem )
				{
					fclose(file);
					return false;
				}

				s_pszFileBuffer = (char *)realloc_mem;
				fgets(s_pszFileBuffer + length, s_iBufferSize - length, file);

				length = strlen(s_pszFileBuffer);
			}

			++line;

			// Strip string from spaces and comments
			char *str = ParseUtil_LStrip(s_pszFileBuffer);
			ParseUtil_RemoveComment(str);
			ParseUtil_RStrip(str);

			// Nothing here, skip
			if ( !*str )
				continue;

			// Parsing section
			if (bParsingSection)
			{
			L_PARSE_SECTION:
				if (*str == INI_SECTION_PREFIX)
				{
					char *end = str + strlen(str) - 1;

					// Exclude prefix of section
					++str;

					if (*end == INI_SECTION_POSTFIX)
					{
						// Exclude postfix of section
						*end = '\0';

						// Strip
						str = ParseUtil_Strip(str);

						if (*str)
						{
							// Next try to parse parameters
							bParsingSection = false;

							const char **section_item = m_SectionsTable.Find(str);

							if (section_item)
							{
								// Already in the table
								pszSection = *section_item;
							}
							else
							{
								// Dupe and save name of section
								pszSection = strdup(str);
								m_SectionsTable.Insert(pszSection);
							}
						}
						else
						{
							m_pszLastErrorMessage = "section name is empty";
							m_ParseState = CFG_ERROR_SECTION_EMPTY;
							m_iLastErrorLine = line;
							return false;
						}
					}
					else
					{
						m_pszLastErrorMessage = "expected end-of-section identifier";
						m_ParseState = CFG_ERROR_SECTION_END;
						m_iLastErrorLine = line;
						return false;
					}
				}
				else
				{
					m_pszLastErrorMessage = "expected identifier of a section";
					m_ParseState = CFG_ERROR_SECTION_START;
					m_iLastErrorLine = line;
					return false;
				}
			}
			else
			{
				// Found prefix of section, start parsing a new one
				if (*str == INI_SECTION_PREFIX)
				{
					bParsingSection = true;
					goto L_PARSE_SECTION;
				}

				// Split parameter
				char *key = strtok(str, INI_PARAMETER_DELIMITER);

				if ( !key || !*key )
				{
					m_pszLastErrorMessage = "key of a parameter is empty";
					m_ParseState = CFG_ERROR_EMPTY_KEY;
					m_iLastErrorLine = line;
					return false;
				}

				char *value = strtok(NULL, INI_PARAMETER_DELIMITER);

				if ( !value || !*value )
				{
					m_pszLastErrorMessage = "value of a parameter is empty";
					m_ParseState = CFG_ERROR_EMPTY_VALUE;
					m_iLastErrorLine = line;
					return false;
				}

				// Strip
				ParseUtil_RStrip(key);
				value = ParseUtil_Strip(value);

				// Add entry
				const char *pszKey = (const char *)malloc(strlen(key) + strlen(value) + 2);

				if ( !pszKey )
				{
					Sys_Error("[SvenMod] CConfigManager::ParseFile: cannot allocate memory");
					continue;
				}

				const char *pszValue = pszKey + strlen(key) + 1;

				memcpy((void *)pszKey, key, strlen(key) + 1);
				memcpy((void *)pszValue, value, strlen(value) + 1);

				m_ConfigTable.Insert({ pszSection, pszKey, pszValue });
			}
		}

		fclose(file);

		m_ParseState = CFG_PARSE_OK;
		m_iLastErrorLine = -1;

		return true;
	}
	else
	{
		m_ParseState = CFG_MISSING_FILE;
		return false;
	}

	return false;
}

bool CConfigManager::ParseUtil_ContainsChars(char ch, const char *chars, size_t length)
{
	for (size_t i = 0; i < length; ++i)
	{
		if (chars[i] == ch)
			return true;
	}

	return false;
}

char *CConfigManager::ParseUtil_LStrip(char *str)
{
	while (*str && ParseUtil_ContainsChars(*str, INI_STRIP_CHARS, INI_STRIP_CHARS_LEN))
		++str;

	return str;
}

void CConfigManager::ParseUtil_RStrip(char *str)
{
	char *end = str + strlen(str) - 1;

	if (end < str)
		return;

	while (end >= str && ParseUtil_ContainsChars(*end, INI_STRIP_CHARS, INI_STRIP_CHARS_LEN))
	{
		*end = '\0';
		--end;
	}
}

char *CConfigManager::ParseUtil_Strip(char *str)
{
	char *result = ParseUtil_LStrip(str);
	ParseUtil_RStrip(result);
	return result;
}

void CConfigManager::ParseUtil_RemoveComment(char *str)
{
	while (*str && !ParseUtil_ContainsChars(*str, INI_COMMENT_PREFIX, INI_COMMENT_PREFIX_LEN))
		++str;

	if (*str)
		*str = '\0';
}

//-----------------------------------------------------------------------------
// CConfigManager Import
//-----------------------------------------------------------------------------

bool CConfigManager::BeginImport(const char *pszFilePath)
{
	if (m_State == CFG_NONE)
	{
		m_bSectionsDuped = true;
		m_bSectionsFilled = true;

		if ( !ParseFile(pszFilePath) )
		{
			if (GetLastState() == CFG_MISSING_FILE)
			{
				Warning("[SvenMod] Missing the config file \"%s\" to import\n", pszFilePath);
				LogMsg("[SvenMod] Missing the config file \"%s\" to import.\n", pszFilePath);
			}
			else
			{
				Warning("[SvenMod] Failed to export the config file \"%s\". Reason: %s (%d)\n", pszFilePath, GetLastErrorMessage(), GetLastErrorLine());
				LogMsg("[SvenMod] Failed to export the config file \"%s\". Reason: %s (%d).\n", pszFilePath, GetLastErrorMessage(), GetLastErrorLine());
			}

			ClearConfigTable();
			ClearSectionsTable();

			m_bSectionsDuped = false;

			return false;
		}

		m_pszCurrentSection = NULL;
		m_State = CFG_IMPORTING;

		return true;
	}

	return false;
}

void CConfigManager::EndImport(void)
{
	if (m_State == CFG_IMPORTING)
	{
		ClearConfigTable();
		ClearSectionsTable();

		m_bSectionsDuped = false;

		m_pszCurrentSection = NULL;
		m_State = CFG_NONE;
	}
}

bool CConfigManager::BeginSectionImport(const char *pszSectionName)
{
	if ( m_State != CFG_IMPORTING || m_pszCurrentSection || !m_SectionsTable.Find(pszSectionName) )
		return false;

	m_pszCurrentSection = pszSectionName;
	return true;
}

void CConfigManager::EndSectionImport(void)
{
	if ( m_State == CFG_IMPORTING && m_pszCurrentSection )
	{
		m_pszCurrentSection = NULL;
	}
}

bool CConfigManager::ImportParam(const char *pszPropertyName, void *value, int fieldtype)
{
	switch (fieldtype)
	{
	case FIELD_TYPE_INTEGER:
		return ImportParam(pszPropertyName, *(int *)value);

	case FIELD_TYPE_FLOAT:
		return ImportParam(pszPropertyName, *(float *)value);

	case FIELD_TYPE_CSTRING:
		return ImportParam(pszPropertyName, *(const char **)value);

	case FIELD_TYPE_BOOLEAN:
		return ImportParam(pszPropertyName, *(bool *)value);

	case FIELD_TYPE_DOUBLE:
		return ImportParam(pszPropertyName, *(double *)value);

	case FIELD_TYPE_BYTE:
		return ImportParam(pszPropertyName, *(unsigned char *)value);

	case FIELD_TYPE_CHARACTER:
		return ImportParam(pszPropertyName, *(char *)value);

	case FIELD_TYPE_SHORT:
		return ImportParam(pszPropertyName, *(short *)value);

	case FIELD_TYPE_INTEGER64:
		return ImportParam(pszPropertyName, *(int64 *)value);

	case FIELD_TYPE_UINT:
		return ImportParam(pszPropertyName, *(uint32 *)value);

	case FIELD_TYPE_UINT16:
		return ImportParam(pszPropertyName, *(uint16 *)value);

	case FIELD_TYPE_UINT64:
		return ImportParam(pszPropertyName, *(uint64 *)value);

	default:
		AssertMsg(false, "Unrecognized field type");
	}

	return false;
}

bool CConfigManager::ImportParam(const char *pszPropertyName, const char *&value)
{
	if (m_State == CFG_IMPORTING && m_pszCurrentSection)
	{
		const char *pszValue = FindValueFromConfig(m_pszCurrentSection, pszPropertyName);

		if (pszValue)
		{
			value = strdup(pszValue);
			return true;
		}
	}

	return false;
}

bool CConfigManager::ImportParam(const char *pszPropertyName, short &value)
{
	if (m_State == CFG_IMPORTING && m_pszCurrentSection)
	{
		const char *pszValue = FindValueFromConfig(m_pszCurrentSection, pszPropertyName);

		if (pszValue)
		{
			if (m_iRadix == 10)
			{
				value = (short)atoi(pszValue);
			}
			else if (m_iRadix == 16)
			{
				value = (short)strtol(pszValue, NULL, 16);
			}
			else if (m_iRadix == 8)
			{
				value = (short)strtol(pszValue, NULL, 8);
			}
			else if (m_iRadix == 2)
			{
				value = (short)strtol(pszValue, NULL, 2);
			}
			else
			{
				value = (short)strtol(pszValue, NULL, m_iRadix);
			}

			return true;
		}
	}

	return false;
}

bool CConfigManager::ImportParam(const char *pszPropertyName, int &value)
{
	if (m_State == CFG_IMPORTING && m_pszCurrentSection)
	{
		const char *pszValue = FindValueFromConfig(m_pszCurrentSection, pszPropertyName);

		if (pszValue)
		{
			if (m_iRadix == 10)
			{
				value = atoi(pszValue);
			}
			else if (m_iRadix == 16)
			{
				value = (int)strtol(pszValue, NULL, 16);
			}
			else if (m_iRadix == 8)
			{
				value = (int)strtol(pszValue, NULL, 8);
			}
			else if (m_iRadix == 2)
			{
				value = (int)strtol(pszValue, NULL, 2);
			}
			else
			{
				value = (int)strtol(pszValue, NULL, m_iRadix);
			}

			return true;
		}
	}

	return false;
}

bool CConfigManager::ImportParam(const char *pszPropertyName, int64 &value)
{
	if (m_State == CFG_IMPORTING && m_pszCurrentSection)
	{
		const char *pszValue = FindValueFromConfig(m_pszCurrentSection, pszPropertyName);

		if (pszValue)
		{
			if (m_iRadix == 10)
			{
				value = (int64)atoll(pszValue);
			}
			else if (m_iRadix == 16)
			{
				value = (int64)strtoll(pszValue, NULL, 16);
			}
			else if (m_iRadix == 8)
			{
				value = (int64)strtoll(pszValue, NULL, 8);
			}
			else if (m_iRadix == 2)
			{
				value = (int64)strtoll(pszValue, NULL, 2);
			}
			else
			{
				value = (int64)strtoll(pszValue, NULL, m_iRadix);
			}

			return true;
		}
	}

	return false;
}

bool CConfigManager::ImportParam(const char *pszPropertyName, uint16 &value)
{
	if (m_State == CFG_IMPORTING && m_pszCurrentSection)
	{
		const char *pszValue = FindValueFromConfig(m_pszCurrentSection, pszPropertyName);

		if (pszValue)
		{
			if (m_iRadix == 10)
			{
				value = (uint16)strtoul(pszValue, NULL, 10);
			}
			else if (m_iRadix == 16)
			{
				value = (uint16)strtoul(pszValue, NULL, 16);
			}
			else if (m_iRadix == 8)
			{
				value = (uint16)strtoul(pszValue, NULL, 8);
			}
			else if (m_iRadix == 2)
			{
				value = (uint16)strtoul(pszValue, NULL, 2);
			}
			else
			{
				value = (uint16)strtoul(pszValue, NULL, m_iRadix);
			}

			return true;
		}
	}

	return false;
}

bool CConfigManager::ImportParam(const char *pszPropertyName, uint32 &value)
{
	if (m_State == CFG_IMPORTING && m_pszCurrentSection)
	{
		const char *pszValue = FindValueFromConfig(m_pszCurrentSection, pszPropertyName);

		if (pszValue)
		{
			if (m_iRadix == 10)
			{
				value = (uint32)strtoul(pszValue, NULL, 10);
			}
			else if (m_iRadix == 16)
			{
				value = (uint32)strtoul(pszValue, NULL, 16);
			}
			else if (m_iRadix == 8)
			{
				value = (uint32)strtoul(pszValue, NULL, 8);
			}
			else if (m_iRadix == 2)
			{
				value = (uint32)strtoul(pszValue, NULL, 2);
			}
			else
			{
				value = (uint32)strtoul(pszValue, NULL, m_iRadix);
			}

			return true;
		}
	}

	return false;
}

bool CConfigManager::ImportParam(const char *pszPropertyName, uint64 &value)
{
	if (m_State == CFG_IMPORTING && m_pszCurrentSection)
	{
		const char *pszValue = FindValueFromConfig(m_pszCurrentSection, pszPropertyName);

		if (pszValue)
		{
			if (m_iRadix == 10)
			{
				value = (uint64)strtoull(pszValue, NULL, 10);
			}
			else if (m_iRadix == 16)
			{
				value = (uint64)strtoull(pszValue, NULL, 16);
			}
			else if (m_iRadix == 8)
			{
				value = (uint64)strtoull(pszValue, NULL, 8);
			}
			else if (m_iRadix == 2)
			{
				value = (uint64)strtoull(pszValue, NULL, 2);
			}
			else
			{
				value = (uint64)strtoull(pszValue, NULL, m_iRadix);
			}

			return true;
		}
	}

	return false;
}

bool CConfigManager::ImportParam(const char *pszPropertyName, float &value)
{
	if (m_State == CFG_IMPORTING && m_pszCurrentSection)
	{
		const char *pszValue = FindValueFromConfig(m_pszCurrentSection, pszPropertyName);

		if (pszValue)
		{
			value = static_cast<float>(atof(pszValue));
			return true;
		}
	}

	return false;
}

bool CConfigManager::ImportParam(const char *pszPropertyName, double &value)
{
	if (m_State == CFG_IMPORTING && m_pszCurrentSection)
	{
		const char *pszValue = FindValueFromConfig(m_pszCurrentSection, pszPropertyName);

		if (pszValue)
		{
			value = atof(pszValue);
			return true;
		}
	}

	return false;
}

bool CConfigManager::ImportParam(const char *pszPropertyName, char &value)
{
	if (m_State == CFG_IMPORTING && m_pszCurrentSection)
	{
		const char *pszValue = FindValueFromConfig(m_pszCurrentSection, pszPropertyName);

		if (pszValue)
		{
			value = (char)atoi(pszValue);
			return true;
		}
	}

	return false;
}

bool CConfigManager::ImportParam(const char *pszPropertyName, unsigned char &value)
{
	if (m_State == CFG_IMPORTING && m_pszCurrentSection)
	{
		const char *pszValue = FindValueFromConfig(m_pszCurrentSection, pszPropertyName);

		if (pszValue)
		{
			value = (unsigned char)strtoul(pszValue, NULL, 10);
			return true;
		}
	}

	return false;
}

bool CConfigManager::ImportParam(const char *pszPropertyName, bool &value)
{
	if (m_State == CFG_IMPORTING && m_pszCurrentSection)
	{
		const char *pszValue = FindValueFromConfig(m_pszCurrentSection, pszPropertyName);

		if (pszValue)
		{
			if ( !stricmp(pszValue, "true") )
			{
				value = true;
			}
			else if ( !stricmp(pszValue, "false") )
			{
				value = false;
			}
			else
			{
				value = static_cast<bool>(atoi(pszValue));
			}

			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// CConfigManager Export
//-----------------------------------------------------------------------------

bool CConfigManager::BeginExport(const char *pszFilePath)
{
	if (m_State == CFG_NONE)
	{
		m_hConfigFile = fopen(pszFilePath, "w");

		if ( !m_hConfigFile )
			return false;

		ClearSectionsTable();

		m_bSectionsDuped = false;

		m_pszCurrentSection = NULL;
		m_State = CFG_EXPORTING;

		return true;
	}

	return false;
}

void CConfigManager::EndExport(void)
{
	if (m_State == CFG_EXPORTING)
	{
		ClearSectionsTable();
		fclose(m_hConfigFile);

		m_bSectionsDuped = false;

		m_pszCurrentSection = NULL;
		m_hConfigFile = NULL;

		m_State = CFG_NONE;
	}
}

bool CConfigManager::BeginSectionExport(const char *pszSectionName)
{
	if ( m_State != CFG_EXPORTING || m_pszCurrentSection || m_SectionsTable.Find(pszSectionName) )
		return false;

	m_bSectionsFilled = true;

	m_SectionsTable.Insert(pszSectionName);
	m_pszCurrentSection = pszSectionName;

	fprintf(m_hConfigFile, "[%s]\n", pszSectionName);

	return true;
}

void CConfigManager::EndSectionExport(void)
{
	if ( m_State == CFG_EXPORTING && m_pszCurrentSection )
	{
		fprintf(m_hConfigFile, "\n");
		m_pszCurrentSection = NULL;
	}
}

void CConfigManager::ExportParam(const char *pszPropertyName, void *value, int fieldtype)
{
	switch (fieldtype)
	{
	case FIELD_TYPE_INTEGER:
		ExportParam(pszPropertyName, *(int *)value);
		break;

	case FIELD_TYPE_FLOAT:
		ExportParam(pszPropertyName, *(float *)value);
		break;

	case FIELD_TYPE_CSTRING:
		ExportParam(pszPropertyName, *(const char **)value);
		break;

	case FIELD_TYPE_BOOLEAN:
		ExportParam(pszPropertyName, *(bool *)value);
		break;

	case FIELD_TYPE_DOUBLE:
		ExportParam(pszPropertyName, *(double *)value);
		break;

	case FIELD_TYPE_BYTE:
		ExportParam(pszPropertyName, *(unsigned char *)value);
		break;

	case FIELD_TYPE_CHARACTER:
		ExportParam(pszPropertyName, *(char *)value);
		break;

	case FIELD_TYPE_SHORT:
		ExportParam(pszPropertyName, *(short *)value);
		break;

	case FIELD_TYPE_INTEGER64:
		ExportParam(pszPropertyName, *(int64 *)value);
		break;

	case FIELD_TYPE_UINT:
		ExportParam(pszPropertyName, *(uint32 *)value);
		break;

	case FIELD_TYPE_UINT16:
		ExportParam(pszPropertyName, *(uint16 *)value);
		break;

	case FIELD_TYPE_UINT64:
		ExportParam(pszPropertyName, *(uint64 *)value);
		break;

	default:
		AssertMsg(false, "Unrecognized field type");
	}
}

void CConfigManager::ExportParam(const char *pszPropertyName, const char *value)
{
	if (m_State == CFG_EXPORTING && m_pszCurrentSection)
	{
		fprintf(m_hConfigFile, "%s = %s\n", pszPropertyName, value);
	}
}

void CConfigManager::ExportParam(const char *pszPropertyName, short value)
{
	if (m_State == CFG_EXPORTING && m_pszCurrentSection)
	{
		if (m_iRadix == 10)
		{
			fprintf(m_hConfigFile, "%s = %hi\n", pszPropertyName, value);
		}
		else if (m_iRadix == 16)
		{
			fprintf(m_hConfigFile, "%s = 0x%hiX\n", pszPropertyName, value);
		}
		else if (m_iRadix == 8)
		{
			fprintf(m_hConfigFile, "%s = 0%hio\n", pszPropertyName, value);
		}
		else if (m_iRadix == 2)
		{
			fprintf(m_hConfigFile, "%s = %s%s%s%s\n", pszPropertyName,
					tetrades_table[(value >> 12) & 0x0F],
					tetrades_table[(value >> 8) & 0x0F],
					tetrades_table[(value >> 4) & 0x0F],
					tetrades_table[value & 0x0F]);
		}
		else
		{
			fprintf(m_hConfigFile, "%s = %hi\n", pszPropertyName, value);
		}
	}
}

void CConfigManager::ExportParam(const char *pszPropertyName, int value)
{
	if (m_State == CFG_EXPORTING && m_pszCurrentSection)
	{
		if (m_iRadix == 10)
		{
			fprintf(m_hConfigFile, "%s = %d\n", pszPropertyName, value);
		}
		else if (m_iRadix == 16)
		{
			fprintf(m_hConfigFile, "%s = 0x%X\n", pszPropertyName, value);
		}
		else if (m_iRadix == 8)
		{
			fprintf(m_hConfigFile, "%s = 0%o\n", pszPropertyName, value);
		}
		else if (m_iRadix == 2)
		{
			// XD
			fprintf(m_hConfigFile, "%s = %s%s%s%s%s%s%s%s\n", pszPropertyName,
					tetrades_table[(value >> 28) & 0x0F],
					tetrades_table[(value >> 24) & 0x0F],
					tetrades_table[(value >> 20) & 0x0F],
					tetrades_table[(value >> 16) & 0x0F],
					tetrades_table[(value >> 12) & 0x0F],
					tetrades_table[(value >> 8) & 0x0F],
					tetrades_table[(value >> 4) & 0x0F],
					tetrades_table[value & 0x0F]);
		}
		else
		{
			fprintf(m_hConfigFile, "%s = %d\n", pszPropertyName, value);
		}
	}
}

void CConfigManager::ExportParam(const char *pszPropertyName, int64 value)
{
	if (m_State == CFG_EXPORTING && m_pszCurrentSection)
	{
		if (m_iRadix == 10)
		{
			fprintf(m_hConfigFile, "%s = %lli\n", pszPropertyName, value);
		}
		else if (m_iRadix == 16)
		{
			fprintf(m_hConfigFile, "%s = 0x%llX\n", pszPropertyName, value);
		}
		else if (m_iRadix == 8)
		{
			fprintf(m_hConfigFile, "%s = 0%llo\n", pszPropertyName, value);
		}
		else if (m_iRadix == 2)
		{
			// lmao XD
			// is that actually needed??
			fprintf(m_hConfigFile, "%s = %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n", pszPropertyName,
					tetrades_table[(value >> 60) & 0x0F],
					tetrades_table[(value >> 56) & 0x0F],
					tetrades_table[(value >> 52) & 0x0F],
					tetrades_table[(value >> 48) & 0x0F],
					tetrades_table[(value >> 44) & 0x0F],
					tetrades_table[(value >> 40) & 0x0F],
					tetrades_table[(value >> 36) & 0x0F],
					tetrades_table[(value >> 32) & 0x0F],
					tetrades_table[(value >> 28) & 0x0F],
					tetrades_table[(value >> 24) & 0x0F],
					tetrades_table[(value >> 20) & 0x0F],
					tetrades_table[(value >> 16) & 0x0F],
					tetrades_table[(value >> 12) & 0x0F],
					tetrades_table[(value >> 8) & 0x0F],
					tetrades_table[(value >> 4) & 0x0F],
					tetrades_table[value & 0x0F]);
		}
		else
		{
			fprintf(m_hConfigFile, "%s = %lli\n", pszPropertyName, value);
		}
	}
}

void CConfigManager::ExportParam(const char *pszPropertyName, uint16 value)
{
	if (m_State == CFG_EXPORTING && m_pszCurrentSection)
	{
		if (m_iRadix == 10)
		{
			fprintf(m_hConfigFile, "%s = %hu\n", pszPropertyName, value);
		}
		else if (m_iRadix == 16)
		{
			fprintf(m_hConfigFile, "%s = 0x%huX\n", pszPropertyName, value);
		}
		else if (m_iRadix == 8)
		{
			fprintf(m_hConfigFile, "%s = 0%huo\n", pszPropertyName, value);
		}
		else if (m_iRadix == 2)
		{
			fprintf(m_hConfigFile, "%s = %s%s%s%s\n", pszPropertyName,
					tetrades_table[(value >> 12) & 0x0F],
					tetrades_table[(value >> 8) & 0x0F],
					tetrades_table[(value >> 4) & 0x0F],
					tetrades_table[value & 0x0F]);
		}
		else
		{
			fprintf(m_hConfigFile, "%s = %hu\n", pszPropertyName, value);
		}
	}
}

void CConfigManager::ExportParam(const char *pszPropertyName, uint32 value)
{
	if (m_State == CFG_EXPORTING && m_pszCurrentSection)
	{
		if (m_iRadix == 10)
		{
			fprintf(m_hConfigFile, "%s = %u\n", pszPropertyName, value);
		}
		else if (m_iRadix == 16)
		{
			fprintf(m_hConfigFile, "%s = 0x%X\n", pszPropertyName, value);
		}
		else if (m_iRadix == 8)
		{
			fprintf(m_hConfigFile, "%s = 0%o\n", pszPropertyName, value);
		}
		else if (m_iRadix == 2)
		{
			fprintf(m_hConfigFile, "%s = %s%s%s%s%s%s%s%s\n", pszPropertyName,
					tetrades_table[(value >> 28) & 0x0F],
					tetrades_table[(value >> 24) & 0x0F],
					tetrades_table[(value >> 20) & 0x0F],
					tetrades_table[(value >> 16) & 0x0F],
					tetrades_table[(value >> 12) & 0x0F],
					tetrades_table[(value >> 8) & 0x0F],
					tetrades_table[(value >> 4) & 0x0F],
					tetrades_table[value & 0x0F]);
		}
		else
		{
			fprintf(m_hConfigFile, "%s = %u\n", pszPropertyName, value);
		}
	}
}

void CConfigManager::ExportParam(const char *pszPropertyName, uint64 value)
{
	if (m_State == CFG_EXPORTING && m_pszCurrentSection)
	{
		if (m_iRadix == 10)
		{
			fprintf(m_hConfigFile, "%s = %llu\n", pszPropertyName, value);
		}
		else if (m_iRadix == 16)
		{
			fprintf(m_hConfigFile, "%s = 0x%llX\n", pszPropertyName, value);
		}
		else if (m_iRadix == 8)
		{
			fprintf(m_hConfigFile, "%s = 0%llo\n", pszPropertyName, value);
		}
		else if (m_iRadix == 2)
		{
			fprintf(m_hConfigFile, "%s = %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n", pszPropertyName,
					tetrades_table[(value >> 60) & 0x0F],
					tetrades_table[(value >> 56) & 0x0F],
					tetrades_table[(value >> 52) & 0x0F],
					tetrades_table[(value >> 48) & 0x0F],
					tetrades_table[(value >> 44) & 0x0F],
					tetrades_table[(value >> 40) & 0x0F],
					tetrades_table[(value >> 36) & 0x0F],
					tetrades_table[(value >> 32) & 0x0F],
					tetrades_table[(value >> 28) & 0x0F],
					tetrades_table[(value >> 24) & 0x0F],
					tetrades_table[(value >> 20) & 0x0F],
					tetrades_table[(value >> 16) & 0x0F],
					tetrades_table[(value >> 12) & 0x0F],
					tetrades_table[(value >> 8) & 0x0F],
					tetrades_table[(value >> 4) & 0x0F],
					tetrades_table[value & 0x0F]);
		}
		else
		{
			fprintf(m_hConfigFile, "%s = %llu\n", pszPropertyName, value);
		}
	}
}

void CConfigManager::ExportParam(const char *pszPropertyName, float value)
{
	if (m_State == CFG_EXPORTING && m_pszCurrentSection)
	{
		fprintf(m_hConfigFile, "%s = %.6f\n", pszPropertyName, value);
	}
}

void CConfigManager::ExportParam(const char *pszPropertyName, double value)
{
	if (m_State == CFG_EXPORTING && m_pszCurrentSection)
	{
		fprintf(m_hConfigFile, "%s = %lf\n", pszPropertyName, value);
	}
}

void CConfigManager::ExportParam(const char *pszPropertyName, char value)
{
	if (m_State == CFG_EXPORTING && m_pszCurrentSection)
	{
		fprintf(m_hConfigFile, "%s = %c\n", pszPropertyName, value);
	}
}

void CConfigManager::ExportParam(const char *pszPropertyName, unsigned char value)
{
	if (m_State == CFG_EXPORTING && m_pszCurrentSection)
	{
		fprintf(m_hConfigFile, "%s = %hhu\n", pszPropertyName, value);
	}
}

void CConfigManager::ExportParam(const char *pszPropertyName, bool value)
{
	if (m_State == CFG_EXPORTING && m_pszCurrentSection)
	{
		fprintf(m_hConfigFile, "%s = %s\n", pszPropertyName, (value ? "true" : "false"));
	}
}

//-----------------------------------------------------------------------------
// Export
//-----------------------------------------------------------------------------

CConfigManager g_ConfigManager;
IConfigManager *g_pConfigManager = (IConfigManager *)&g_ConfigManager;

IConfigManager *ConfigManager()
{
	return g_pConfigManager;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CConfigManager, IConfigManager, CONFIG_MANAGER_INTERFACE_VERSION, g_ConfigManager);
