#include "plugins_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define PLUGINS_SECTION "Plugins"

#define KV_COMMENT_PREFIX "/"
#define KV_STRIP_CHARS " \t\n"

#define KV_CONTROL_CHARS "\"{}"
#define KV_QUOTE_CHAR '"'
#define KV_SECTION_BEGIN_CHAR '{'
#define KV_SECTION_END_CHAR '}'

#define KV_CONTROL_CHARS_LEN sizeof(KV_CONTROL_CHARS) - 1
#define KV_STRIP_CHARS_LEN sizeof(KV_STRIP_CHARS) - 1
#define KV_COMMENT_PREFIX_LEN sizeof(KV_COMMENT_PREFIX) - 1

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------

typedef enum
{
	KV_SEEK_NONE = 0,
	KV_SEEK_KEY_BEGIN,
	KV_SEEK_KEY_END,
	KV_SEEK_VALUE_BEGIN,
	KV_SEEK_VALUE_END,
	KV_SEEK_SECTION_BEGIN,
	KV_SEEK_SECTION_END,
} KV_SEEK_STATE;

typedef enum
{
	KV_PARSE_OK = 0,
	KV_PARSE_FAILED
} KV_PARSE_RESULT;

//-----------------------------------------------------------------------------
// Error holders
//-----------------------------------------------------------------------------

static volatile const char *s_pszLastErrorMessage = "";
static volatile int s_iLastErrorLine = 0;

//-----------------------------------------------------------------------------
// Utilities
//-----------------------------------------------------------------------------

static bool KV_ContainsChars(char ch, const char *chars, size_t length)
{
	for (size_t i = 0; i < length; ++i)
	{
		if (chars[i] == ch)
			return true;
	}

	return false;
}

static bool KV_IsControlChar(char ch)
{
	return KV_ContainsChars(ch, KV_CONTROL_CHARS, KV_CONTROL_CHARS_LEN);
}

static bool KV_IsWhitespace(char ch)
{
	return ch == ' ' || ch == '\t';
}

static void KV_RemoveComment(char *str)
{
	while (*str && !KV_ContainsChars(*str, KV_COMMENT_PREFIX, KV_COMMENT_PREFIX_LEN))
		++str;

	if (*str)
		*str = '\0';
}

static char *KV_LStrip(char *str)
{
	while (*str && KV_ContainsChars(*str, KV_STRIP_CHARS, KV_STRIP_CHARS_LEN))
		++str;

	return str;
}

static void KV_RStrip(char *str)
{
	char *end = str + strlen(str) - 1;

	if (end < str)
		return;

	while (end >= str && KV_ContainsChars(*end, KV_STRIP_CHARS, KV_STRIP_CHARS_LEN))
	{
		*end = '\0';
		--end;
	}
}

static char *KV_Strip(char *str)
{
	char *result = KV_LStrip(str);
	KV_RStrip(result);

	return result;
}

static bool KV_IsPair(char *buffer, const char **ppszKey, const char **ppszValue, bool *bKeyOnly)
{
	const char *pszKey = NULL;
	const char *pszValue = NULL;

	*ppszKey = pszKey;
	*ppszValue = pszValue;

	*bKeyOnly = false;

	KV_SEEK_STATE seek_state = KV_SEEK_KEY_BEGIN;

	// Key

	while (*buffer)
	{
		if (KV_IsControlChar(*buffer))
		{
			if (*buffer == KV_QUOTE_CHAR)
			{
				buffer++;
				pszKey = buffer;

				seek_state = KV_SEEK_KEY_END;
				break;
			}
			else if (*buffer == KV_SECTION_BEGIN_CHAR || *buffer == KV_SECTION_END_CHAR)
			{
				s_pszLastErrorMessage = "Expected begin of key, not section begin/end";
				break;
			}
		}
		else if (!KV_IsWhitespace(*buffer))
		{
			s_pszLastErrorMessage = "Unrecognized symbol, expected begin of key";
			break;
		}

		buffer++;
	}

	if (seek_state != KV_SEEK_KEY_END)
	{
		return false;
	}

	while (*buffer)
	{
		if (KV_IsControlChar(*buffer))
		{
			if (*buffer == KV_QUOTE_CHAR)
			{
				*buffer = 0;
				buffer++;

				seek_state = KV_SEEK_VALUE_BEGIN;
				break;
			}
			else if (*buffer == KV_SECTION_BEGIN_CHAR || *buffer == KV_SECTION_END_CHAR)
			{
				s_pszLastErrorMessage = "Expected end of key, not section begin/end";
				break;
			}
		}

		buffer++;
	}

	if (seek_state != KV_SEEK_VALUE_BEGIN)
	{
		s_pszLastErrorMessage = "End of key not found, reached end of line";
		return false;
	}

	char *pszCheck = buffer;
	bool bPair = false;

	while (*pszCheck)
	{
		if (KV_IsControlChar(*pszCheck))
		{
			if (*pszCheck == KV_QUOTE_CHAR)
			{
				bPair = true;
			}
			else
			{
				s_pszLastErrorMessage = "Declare begin of section on new line";
				return false;
			}
		}
		else if (!KV_IsWhitespace(*pszCheck) && !bPair)
		{
			s_pszLastErrorMessage = "Unrecognized symbol, expected begin of value";
			return false;
		}

		pszCheck++;
	}

	if (!bPair)
	{
		*bKeyOnly = true;
		*ppszKey = pszKey;
		return false;
	}

	// Value

	while (*buffer)
	{
		if (KV_IsControlChar(*buffer))
		{
			if (*buffer == KV_QUOTE_CHAR)
			{
				buffer++;
				pszValue = buffer;

				seek_state = KV_SEEK_VALUE_END;
				break;
			}
			else if (*buffer == KV_SECTION_BEGIN_CHAR || *buffer == KV_SECTION_END_CHAR)
			{
				s_pszLastErrorMessage = "Expected begin of value, not section begin/end";
				break;
			}
		}
		else if (!KV_IsWhitespace(*buffer))
		{
			s_pszLastErrorMessage = "Unrecognized symbol, expected begin of value";
			break;
		}

		buffer++;
	}

	if (seek_state != KV_SEEK_VALUE_END)
	{
		return false;
	}

	while (*buffer)
	{
		if (KV_IsControlChar(*buffer))
		{
			if (*buffer == KV_QUOTE_CHAR)
			{
				*buffer = 0;
				buffer++;

				seek_state = KV_SEEK_NONE;
				break;
			}
			else if (*buffer == KV_SECTION_BEGIN_CHAR || *buffer == KV_SECTION_END_CHAR)
			{
				s_pszLastErrorMessage = "Expected begin of key, not section begin/end";
				break;
			}
		}

		buffer++;
	}

	if (seek_state != KV_SEEK_NONE)
	{
		s_pszLastErrorMessage = "End of value not found, reached end of line";
		return false;
	}

	*ppszKey = pszKey;
	*ppszValue = pszValue;

	return true;
}

//-----------------------------------------------------------------------------
// CPluginsLoader
//-----------------------------------------------------------------------------

// ToDo: use Valve's parser of key values lol

plugins_s *CPluginsLoader::LoadFromFile(const char *pszFileName, PLUGIN_LOAD_RESULT *pResult)
{
	static char szBuffer[512];
	FILE *file = fopen(pszFileName, "r");

	if (file)
	{
		int nLine = 0;
		bool bError = false;

		const char *pszSectionName = NULL;

		KV_SEEK_STATE seek_state = KV_SEEK_NONE;
		KV_PARSE_RESULT parse_result = KV_PARSE_OK;

		plugins_s *pLastPlugin = NULL;
		plugins_s *pPluginsBase = NULL;

		while (fgets(szBuffer, sizeof(szBuffer), file))
		{
			nLine++;
			bError = false;

			char *buffer = KV_LStrip((char *)szBuffer);
			KV_RemoveComment(buffer);
			KV_RStrip(buffer);

			if ( !*buffer )
				continue;

			if ( seek_state == KV_SEEK_NONE )
			{
				const char *pszKey = NULL;
				const char *pszValue = NULL;
				bool bPossibleSection = false;

				bool bPair = KV_IsPair( buffer, &pszKey, &pszValue, &bPossibleSection );

				if (!bPair && !bPossibleSection)
				{
					parse_result = KV_PARSE_FAILED;
					break;
				}

				if (!bPair && bPossibleSection)
				{
					if ( !strcmp(PLUGINS_SECTION, pszKey) )
					{
						seek_state = KV_SEEK_SECTION_BEGIN;
						continue;
					}
					else
					{
						s_pszLastErrorMessage = "Expected 'Plugins' section >:(";
						parse_result = KV_PARSE_FAILED;
						break;
					}
				}
			}
			else if (seek_state == KV_SEEK_SECTION_BEGIN)
			{
				while (*buffer)
				{
					if (KV_IsControlChar(*buffer))
					{
						if (*buffer == KV_SECTION_BEGIN_CHAR)
						{
							seek_state = KV_SEEK_SECTION_END;
							break;
						}
						else if (*buffer == KV_QUOTE_CHAR)
						{
							bError = true;
							s_pszLastErrorMessage = "Expected begin of section";
							break;
						}
						else if (*buffer == KV_SECTION_END_CHAR)
						{
							bError = true;
							s_pszLastErrorMessage = "Expected begin of section, not end";
							break;
						}
					}
					else if (!KV_IsWhitespace(*buffer))
					{
						bError = true;
						s_pszLastErrorMessage = "Unrecognized symbol, expected begin of value";
						break;
					}

					buffer++;
				}

				if (bError)
				{
					parse_result = KV_PARSE_FAILED;
					break;
				}
			}
			else if (seek_state == KV_SEEK_SECTION_END)
			{
				char *pszPossiblePair = buffer;

				while (*buffer)
				{
					if (KV_IsControlChar(*buffer))
					{
						if (*buffer == KV_SECTION_END_CHAR)
						{
							seek_state = KV_SEEK_NONE;
							break;
						}
						else if (*buffer == KV_SECTION_BEGIN_CHAR)
						{
							bError = true;
							s_pszLastErrorMessage = "Expected end of section, not begin";
							break;
						}
					}
					//else if (!KV_IsWhitespace(*buffer))
					//{
					//	bError = true;
					//	s_pszLastErrorMessage = "Unrecognized symbol, expected begin of value";
					//	break;
					//}

					buffer++;
				}

				if (bError)
				{
					parse_result = KV_PARSE_FAILED;
					break;
				}

				if (seek_state == KV_SEEK_NONE)
				{
					parse_result = KV_PARSE_OK;
					break;
				}
				else
				{
					const char *pszKey = NULL;
					const char *pszValue = NULL;

					bool bPossibleSection = false;

					bool bPair = KV_IsPair(pszPossiblePair, &pszKey, &pszValue, &bPossibleSection);

					if (!bPair && !bPossibleSection)
					{
						parse_result = KV_PARSE_FAILED;
						break;
					}

					if (bPair)
					{
						if ( !pPluginsBase )
						{
							pLastPlugin = pPluginsBase = (plugins_s *)calloc(1, sizeof(plugins_s));

							pPluginsBase->name = strdup(pszKey);
							pPluginsBase->load = atoi(pszValue);
						}
						else
						{
							plugins_s *pPlugin = (plugins_s *)calloc(1, sizeof(plugins_s));

							pPlugin->name = strdup(pszKey);
							pPlugin->load = atoi(pszValue);

							pLastPlugin->next = pPlugin;
							pLastPlugin = pPlugin;
						}
					}
				}
			}
		}

		if (parse_result == KV_PARSE_FAILED)
		{
			if (pResult)
			{
				FreePlugins(pPluginsBase);

				pPluginsBase = NULL;
				s_iLastErrorLine = nLine;

				*pResult = LOAD_RESULT_FAILED;
			}
		}
		else if (pResult)
		{
			*pResult = LOAD_RESULT_OK;
		}

		fclose(file);

		return pPluginsBase;
	}
	else if (pResult)
	{
		*pResult = LOAD_RESULT_MISSING_FILE;
	}

	return NULL;
}

void CPluginsLoader::FreePlugins(plugins_s *pPluginsBase)
{
	while (pPluginsBase)
	{
		plugins_s *pPrevPlugin = pPluginsBase;

		pPluginsBase = pPluginsBase->next;

		free((void *)pPrevPlugin->name);
		free((void *)pPrevPlugin);
	}
}

int CPluginsLoader::GetLastErrorLine()
{
	return s_iLastErrorLine;
}

const char *CPluginsLoader::GetLastErrorMessage()
{
	return (const char *)s_pszLastErrorMessage;
}

CPluginsLoader g_PluginsLoader;