#pragma once
#include "cIGZUnknown.h"

static const GZREFIID GZIID_cIGZString = 0x089B7DC8;

class cIGZString : public cIGZUnknown
{
public:
	virtual void FromChar(char const* otherStr) = 0;
	virtual void FromChar(char const* otherStr, uint32_t length) = 0;

	virtual char const* ToChar() const = 0;
	virtual char const* Data() const = 0;
	virtual uint32_t Strlen() const = 0;

	virtual bool IsEqual(cIGZString const*, bool caseSensitive = true) const = 0;
	virtual bool IsEqual(cIGZString const&, bool caseSensitive = true) const = 0;
	virtual bool IsEqual(char const* other, uint32_t otherLen = -1, bool caseSensitive = true) const = 0;

	virtual int CompareTo(cIGZString const& other, bool caseSensitive = true) const = 0;
	virtual int CompareTo(char const* other, uint32_t otherLen = -1, bool caseSensitive = true) const = 0;

	virtual cIGZString& operator=(cIGZString const& other) = 0;
	virtual void Copy(cIGZString const& src) = 0;

	virtual void Resize(uint32_t newLength) = 0;

	virtual cIGZString& Append(char const* src, uint32_t srcLen) = 0;
	virtual cIGZString& Append(cIGZString const& src) = 0;

	virtual cIGZString& Insert(uint32_t position, char const* src, uint32_t srcLen) = 0;
	virtual cIGZString& Insert(uint32_t position, cIGZString const& src) = 0;

	virtual cIGZString& Replace(uint32_t position, char const* src, uint32_t srcLen) = 0;
	virtual cIGZString& Replace(uint32_t position, cIGZString const& src) = 0;

	virtual cIGZString& Erase(uint32_t position = 0, uint32_t length = -1) = 0;

	virtual uint32_t Find(char const* needle, uint32_t position = 0, bool caseSensitive = true) const = 0;
	virtual uint32_t Find(cIGZString const& needle, uint32_t position = 0, bool caseSensitive = true) const = 0;
	virtual uint32_t RFind(char const* needle, uint32_t position = 0, bool caseSensitive = true) const = 0;
	virtual uint32_t RFind(cIGZString const& needle, uint32_t position = 0, bool caseSensitive = true) const = 0;

	virtual void Sprintf(char const* format, ...) = 0;
};