/*	======================================================================

	2023-01-12
        added is utf8 function as I consider strictly using UTF8 over
        wchar_t


	======================================================================	*/

#ifndef STRINGENC_H
#define STRINGENC_H

#include <iostream>
#include <cstdint>		// needed for types such as uint8_t, uint16_t, uint32_t
#include <string>
#include <windows.h>
#include "stringext.h"

/**
 * Checks if the string contains bit characterizations of UTF8
 * @param string as const char*
 * @return true if string is UTF8
 */
bool is_utf8 (const char* str);

/**
 * Checks if UTF8 encoding is valid and not UTF16
 * @param string as const char*
 * @return true if UTF8 Encoding is Valid
 */
bool is_valid_utf8 (const char* str);

/**
 * Converts Japanese String Encoding (Shift-JIS) into Unicode
 * @param string (std::string)
 * @return SJIS as UTF16
 */
std::wstring shift_jis_to_utf_16(const std::string& str);

/**
 * Converts UTF8 to Japanese Encoding (Shift-JIS)
 * @param string (std::string)
 * @return UTF8 as SJIS
 */
std::string UTF8toSjis2(std::string srcUTF8);

/**
 * Converts Japanese Encoding (Shift-JIS) to UTF8 (ALT)
 * @param string (std::string)
 * @return SJIS as UTF8 (ALT)
 */
std::string SjistoUTF82(std::string srcSjis);

/**
 * Converts Japanese Encoding (Shift-JIS) to UTF8
 * @param string (std::string)
 * @return SJIS as UTF8
 */
std::string sjis2utf8(const std::string& sjis);

/**
 * Converts Unicode to Japanese Encoding (Shift-JIS)
 * @param string (std::string)
 * @return UTF16 as SJIS
 */
std::string utf_16_to_shift_jis(const std::wstring& str);

/**
 * Converts Unicode Char. to Japanese Encoding (Shift-JIS)
 * @param string (std::string)
 * @return UTF16 as SJIS (Single WORD)
 */
std::string char_utf_16_to_shift_jis(const char* str);

/**
 * Converts Unicode Char. to UTF8
 * @param string (std::string)
 * @return UTF16 as UTF8 (Single DWORD)
 */
std::string unicode_2_utf8 (uint32_t x);

/**
 * Converts Unicode to UTF8
 * @param string (std::string)
 * @return UTF16 as UTF8
 */
std::string unicode_to_utf8 (std::wstring str);

/**
 * Converts UTF8 to Unicode
 * @param string (std::string)
 * @return UTF8 as UTF16
 */
std::wstring utf8_2_unicode (std::string &str);

#endif // STRINGENC_H
