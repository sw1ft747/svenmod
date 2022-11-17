#pragma warning(disable : 26812)

#include "keyvalues_custom_parser.h"
#include "hl_sdk/common/Platform.h"

namespace KeyValuesParser
{
	constexpr auto STRIP_CHARS = " \t\n";

	constexpr auto CONTROL_CHARS = "\"{}";
	constexpr auto QUOTE_CHAR = '"';
	constexpr auto SECTION_BEGIN_CHAR = '{';
	constexpr auto SECTION_END_CHAR = '}';

	constexpr auto CONTROL_CHARS_LEN = sizeof(CONTROL_CHARS) - 1;
	constexpr auto STRIP_CHARS_LEN = sizeof(STRIP_CHARS) - 1;

	std::vector<KeyValues *> vSectionsStack;

	const char *pszLastErrorMessage = "";
	int iLastErrorLine = 0;
	bool bUsesEscapeSequences = false;

	inline bool ContainsChars(char ch, const char *chars, size_t length)
	{
		for (size_t i = 0; i < length; ++i)
		{
			if (chars[i] == ch)
				return true;
		}

		return false;
	}

	inline bool IsControlChar(char ch)
	{
		return ContainsChars(ch, CONTROL_CHARS, CONTROL_CHARS_LEN);
	}

	inline bool IsWhitespace(char ch)
	{
		return ch == ' ' || ch == '\t';
	}

	inline void RemoveComment(char *str)
	{
		char *prev_char = NULL;
		char *comment = NULL;

		char *key_start = NULL;
		char *key_end = NULL;

		char *value_start = NULL;
		char *value_end = NULL;

		while (*str)
		{
			if (*str == '/')
			{
				if ( comment == NULL )
					comment = str;
			}
			else if (*str == '\"')
			{
				if (prev_char != NULL && *prev_char == '\\')
				{
					prev_char = str;
					str++;
					continue;
				}

				if (key_start == NULL)
				{
					key_start = str;
				}
				else if (key_end == NULL)
				{
					key_end = str;
				}
				else if (value_start == NULL)
				{
					value_start = str;
				}
				else if (value_end == NULL)
				{
					value_end = str;
					break;
				}
			}

			prev_char = str;
			str++;
		}

		if ( comment != NULL )
		{
			if (key_start && key_end)
			{
				if (key_start < comment && key_end > comment)
				{
					return;
				}

				if (value_start && value_end)
				{
					if (value_start < comment && value_end > comment)
					{
						return;
					}
				}
			}

			*comment = '\0';
		}
	}

	inline char *LStrip(char *str)
	{
		while (*str && ContainsChars(*str, STRIP_CHARS, STRIP_CHARS_LEN))
			++str;

		return str;
	}

	inline void RStrip(char *str)
	{
		char *end = str + strlen(str) - 1;

		if (end < str)
			return;

		while (end >= str && ContainsChars(*end, STRIP_CHARS, STRIP_CHARS_LEN))
		{
			*end = '\0';
			--end;
		}
	}

	inline char *Strip(char *str)
	{
		char *result = LStrip(str);
		RStrip(result);

		return result;
	}

	inline bool __fastcall IsPair(char *buffer, const char **ppszKey, const char **ppszValue, bool *bSection, bool *bHasEscapeSeq, char **ppEnd)
	{
		const char *pszKey = NULL;
		const char *pszValue = NULL;

		*ppszKey = pszKey;
		*ppszValue = pszValue;

		*bSection = false;

		if (bHasEscapeSeq)
		{
			*bHasEscapeSeq = false;
		}
		
		if (ppEnd)
		{
			*ppEnd = NULL;
		}

		SeekState seek_state = SEEK_KEY_BEGIN;

		// Key

		while (*buffer)
		{
			if (IsControlChar(*buffer))
			{
				if (*buffer == QUOTE_CHAR)
				{
					buffer++;
					pszKey = buffer;

					seek_state = SEEK_KEY_END;
					break;
				}
				else if (*buffer == SECTION_BEGIN_CHAR || *buffer == SECTION_END_CHAR)
				{
					pszLastErrorMessage = "Expected begin of key, not section begin/end";
					break;
				}
			}
			else if (!IsWhitespace(*buffer))
			{
				pszLastErrorMessage = "Unrecognized symbol, expected begin of key";
				break;
			}

			buffer++;
		}

		if (seek_state != SEEK_KEY_END)
		{
			return false;
		}

		while (*buffer)
		{
			if (IsControlChar(*buffer))
			{
				if (*buffer == QUOTE_CHAR)
				{
					*buffer = 0;
					buffer++;

					seek_state = SEEK_VALUE_BEGIN;
					break;
				}
				else if (*buffer == SECTION_BEGIN_CHAR || *buffer == SECTION_END_CHAR)
				{
					pszLastErrorMessage = "Expected end of key, not section begin/end";
					break;
				}
			}

			buffer++;
		}

		if (seek_state != SEEK_VALUE_BEGIN)
		{
			pszLastErrorMessage = "End of key not found, reached end of line";
			return false;
		}

		char *pszCheck = buffer;
		bool bPair = false;

		while (*pszCheck)
		{
			if (IsControlChar(*pszCheck))
			{
				if (*pszCheck == QUOTE_CHAR)
				{
					bPair = true;
				}
				else
				{
					pszLastErrorMessage = "Declare begin of section on new line";
					return false;
				}
			}
			else if (!IsWhitespace(*pszCheck) && !bPair)
			{
				pszLastErrorMessage = "Unrecognized symbol, expected begin of value";
				return false;
			}

			pszCheck++;
		}

		if (!bPair)
		{
			*bSection = true;
			*ppszKey = pszKey;
			return false;
		}

		// Value

		while (*buffer)
		{
			if (IsControlChar(*buffer))
			{
				if (*buffer == QUOTE_CHAR)
				{
					buffer++;
					pszValue = buffer;

					seek_state = SEEK_VALUE_END;
					break;
				}
				else if (*buffer == SECTION_BEGIN_CHAR || *buffer == SECTION_END_CHAR)
				{
					pszLastErrorMessage = "Expected begin of value, not section begin/end";
					break;
				}
			}
			else if (!IsWhitespace(*buffer))
			{
				pszLastErrorMessage = "Unrecognized symbol, expected begin of value";
				break;
			}

			buffer++;
		}

		if (seek_state != SEEK_VALUE_END)
		{
			return false;
		}

		bool has_escape_seq = false;
		char *prev_char = NULL;

		while (*buffer)
		{
			if (IsControlChar(*buffer))
			{
				if (*buffer == QUOTE_CHAR)
				{
					if (prev_char != NULL && *prev_char == '\\')
					{
						prev_char = buffer;
						buffer++;
						continue;
					}

					*buffer = 0;
					buffer++;

					seek_state = SEEK_NONE;
					break;
				}
				else if (*buffer == SECTION_BEGIN_CHAR || *buffer == SECTION_END_CHAR)
				{
					pszLastErrorMessage = "Expected begin of key, not section begin/end";
					break;
				}
			}
			else if (*buffer == '\\')
			{
				has_escape_seq = true;
			}

			prev_char = buffer;
			buffer++;
		}

		if (ppEnd)
		{
			*ppEnd = buffer;
		}

		if (seek_state != SEEK_NONE)
		{
			pszLastErrorMessage = "End of value not found, reached end of line";
			return false;
		}

		*ppszKey = pszKey;
		*ppszValue = pszValue;

		if ( bHasEscapeSeq )
		{
			*bHasEscapeSeq = true;
		}
		
		return true;
	}

	KeyValues *LoadFromFile(const char *pszFilename, int *result_code)
	{
		FILE *file = fopen(pszFilename, "r");

		vSectionsStack.clear();

		iLastErrorLine = 0;
		*result_code = PARSE_OK;

		if (file)
		{
			char szBuffer[4096];

			KeyValues *keyvalues = NULL;
			KeyValues *keyvalues_head = NULL;

			SeekState seek_state = SEEK_NONE;
			ParseResult parse_result = PARSE_OK;

			while ( fgets(szBuffer, M_ARRAYSIZE(szBuffer), file) )
			{
				iLastErrorLine++;

				char *buffer = LStrip( (char *)szBuffer );
				RemoveComment( buffer );
				RStrip( buffer );

				if ( !*buffer )
					continue;

				if ( seek_state == SEEK_NONE )
				{
					bool bSection = false;

					const char *pszKey = NULL;
					const char *pszValue = NULL;

					bool bPair = IsPair( buffer, &pszKey, &pszValue, &bSection, NULL, NULL );

					if ( bPair || ( !bPair && !bSection ) )
					{
						if ( bPair )
						{
							pszLastErrorMessage = "Declaration of a keyvalue pair outside of section is not allowed";
						}

						parse_result = PARSE_FAILED;
						break;
					}

					if ( keyvalues_head != NULL )
					{
						pszLastErrorMessage = "Main section is already declared";
						parse_result = PARSE_FAILED;
						break;
					}

					if ( bSection )
					{
						seek_state = SEEK_SECTION_BEGIN;
						keyvalues_head = keyvalues = new KeyValues(pszKey);
					}
				}
				else if ( seek_state == SEEK_SECTION_BEGIN )
				{
					bool error = false;

					while (*buffer)
					{
						if ( IsControlChar(*buffer) )
						{
							if (*buffer == SECTION_BEGIN_CHAR)
							{
								seek_state = SEEK_SECTION_END;
								break;
							}
							else if (*buffer == QUOTE_CHAR)
							{
								pszLastErrorMessage = "Expected begin of section";
								error = true;
								break;
							}
							else if (*buffer == SECTION_END_CHAR)
							{
								pszLastErrorMessage = "Expected begin of section, not end";
								error = true;
								break;
							}
						}
						else if ( !IsWhitespace(*buffer) )
						{
							pszLastErrorMessage = "Unrecognized symbol, expected begin of value";
							error = true;
							break;
						}

						buffer++;
					}

					if (error)
					{
						parse_result = PARSE_FAILED;
						break;
					}
				}
				else if ( seek_state == SEEK_SECTION_END )
				{
					bool error = false;
					bool skip = false;

					char *pszPossiblePair = buffer;

					while (*buffer)
					{
						if ( IsControlChar(*buffer) )
						{
							if (*buffer == SECTION_END_CHAR)
							{
								if ( vSectionsStack.empty() )
								{
									seek_state = SEEK_NONE;
									skip = true;
								}
								else
								{
									keyvalues = vSectionsStack.back();
									vSectionsStack.pop_back();

									skip = true;
								}

								break;
							}
							else if (*buffer == SECTION_BEGIN_CHAR)
							{
								pszLastErrorMessage = "Expected end of section, not begin";
								error = true;
								break;
							}
						}

						buffer++;
					}

					if (error)
					{
						parse_result = PARSE_FAILED;
						break;
					}

					if (skip)
					{
						continue;
					}

					bool bSection = false;
					bool bHasEscapeSeq = false;

					char *pValueEnd = NULL;

					const char *pszKey = NULL;
					const char *pszValue = NULL;

					bool bPair = IsPair( pszPossiblePair, &pszKey, &pszValue, &bSection, &bHasEscapeSeq, &pValueEnd );

					if ( !bPair && !bSection )
					{
						parse_result = PARSE_FAILED;
						break;
					}

					if ( bSection )
					{
						KeyValues *prev = keyvalues;

						vSectionsStack.push_back(keyvalues);
						keyvalues = new KeyValues(pszKey);

						prev->AddItem( keyvalues );

						seek_state = SEEK_SECTION_BEGIN;
					}
					else // bPair
					{
						if ( bUsesEscapeSequences && bHasEscapeSeq )
						{
							int i = 0;

							char *value_buffer = (char *)pszValue;
							char *prev_char = NULL;

							char *value = (char *)malloc(pValueEnd - pszValue);

							while (*value_buffer)
							{
								if (*value_buffer == '\\')
								{
									prev_char = value_buffer;
									value_buffer++;
									continue;
								}

								if ( (prev_char != NULL && *prev_char == '\\') )
								{
									char esc_seq = *value_buffer;

									if (esc_seq == '\"')
									{
										value[i] = '\"';
										i++;
									}
									else if (esc_seq == 'n')
									{
										value[i] = '\n';
										i++;
									}
									else if (esc_seq == 't')
									{
										value[i] = '\t';
										i++;
									}
									else if (esc_seq == '\\')
									{
										value[i] = '\\';
										i++;
									}

									prev_char = value_buffer;
									value_buffer++;
									continue;
								}

								value[i] = *value_buffer;
								i++;

								prev_char = value_buffer;
								value_buffer++;
							}
							
							value[i] = 0;

							keyvalues->AddItem( new KeyValues(pszKey, value) );

							free( value );
						}
						else
						{
							keyvalues->AddItem( new KeyValues(pszKey, pszValue) );
						}
					}
				}
			}

			fclose( file );
			vSectionsStack.clear();

			if ( parse_result == PARSE_FAILED )
			{
				if ( keyvalues_head != NULL )
					delete keyvalues_head;

				*result_code = PARSE_FAILED;
				return NULL;
			}

			return keyvalues_head;
		}
		else
		{
			*result_code = PARSE_FAILED;
			pszLastErrorMessage = "Failed to open file";
		}

		return NULL;
	}

	const char *GetLastErrorMessage()
	{
		return pszLastErrorMessage;
	}

	int GetLastErrorLine()
	{
		return iLastErrorLine;
	}

	void UsesEscapeSequences(bool state)
	{
		bUsesEscapeSequences = state;
	}
}