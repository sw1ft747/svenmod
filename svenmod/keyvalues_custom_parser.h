#ifndef KEYVALUES_CUSTOM_PARSER_H
#define KEYVALUES_CUSTOM_PARSER_H

#ifdef _WIN32
#pragma once
#endif

#include <vector>
#include <string>

namespace KeyValuesParser
{
	enum SeekState
	{
		SEEK_NONE = 0,
		SEEK_KEY_BEGIN,
		SEEK_KEY_END,
		SEEK_VALUE_BEGIN,
		SEEK_VALUE_END,
		SEEK_SECTION_BEGIN,
		SEEK_SECTION_END,
	};

	enum ParseResult
	{
		PARSE_OK = 0,
		PARSE_FAILED
	};

	struct KeyValues
	{
		inline KeyValues() : bSection(false), sKey(), sValue(), vKeyValues()
		{
		}
		
		inline KeyValues(const char *pszSectionName) : bSection(true), sKey(pszSectionName), sValue(), vKeyValues()
		{
		}
		
		inline KeyValues(const char *pszKey, const char *pszValue) : bSection(false), sKey(pszKey), sValue(pszValue), vKeyValues()
		{
		}
		
		inline KeyValues(std::string &sSectionName) : bSection(true), sKey(sSectionName), sValue(), vKeyValues()
		{
		}
		
		inline KeyValues(std::string &sKey, std::string &sValue) : bSection(false), sKey(sKey), sValue(sValue), vKeyValues()
		{
		}
		
		inline ~KeyValues()
		{
			Clear();

			sKey.clear();
			sValue.clear();

			vKeyValues.clear();
		}

		inline bool IsSection() { return bSection; }

		inline void AddItem(KeyValues *kv)
		{
			if ( bSection )
				vKeyValues.push_back( kv );
		}
		
		inline void AddItem(KeyValues &kv)
		{
			if ( bSection )
				vKeyValues.push_back( &kv );
		}
		
		inline std::string &Key() { return sKey; }
		
		inline std::string &Value() { return sValue; }
		
		inline std::vector<KeyValues *> &GetList() { return vKeyValues; }

		inline void Clear()
		{
			if ( bSection )
			{
				for (size_t i = 0; i < vKeyValues.size(); i++)
				{
					delete vKeyValues[i];
				}
			}
		}

		bool bSection;

		std::string sKey;
		std::string sValue;

		std::vector<KeyValues *> vKeyValues;
	};

	KeyValues *LoadFromFile(const char *pszFilename, int *result_code);

	const char *GetLastErrorMessage();
	int GetLastErrorLine();

	void UsesEscapeSequences(bool state);
}

#endif // KEYVALUES_H