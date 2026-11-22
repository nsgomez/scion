/*
 *  Scion - an open-source implementation of the Maxis GZCOM/RZCOM framework
 *  Copyright (C) 2026  Nelson Gomez (nsgomez) <nelson@ngomez.me>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation, under
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, see <https://www.gnu.org/licenses/>.
 */

#include "cGZDBSDFRecord.h"

cGZDBSDFRecord::cGZDBSDFRecord(cIGZPersistDBSegment* segment) :
	cGZPersistDBSerialRecord(segment, true),
	mOpen(false),
	mMode(0)
{
}

cGZDBSDFRecord::~cGZDBSDFRecord()
{
	if (mFile.IsOpen() || mTempFile.IsOpen())
	{
		AbortInternal();
	}
}

bool cGZDBSDFRecord::QueryInterface(GZIID iid, void** outPtr)
{
	if (iid != GZIID_cIGZPersistDBSDFRecord)
	{
		return cGZPersistDBSerialRecord::QueryInterface(iid, outPtr);
	}

	*outPtr = static_cast<cIGZPersistDBSDFRecord*>(this);
	AddRef();
	return true;
}

uint32_t cGZDBSDFRecord::AddRef()
{
	return cGZPersistDBSerialRecord::AddRef();
}

uint32_t cGZDBSDFRecord::Release()
{
	return cGZPersistDBSerialRecord::Release();
}

uint32_t cGZDBSDFRecord::GetSize()
{
	if (mOpen)
	{
		return mFile.Length();
	}

	return -1;
}

bool cGZDBSDFRecord::SetSize(uint32_t size)
{
	return false;
}

bool cGZDBSDFRecord::GetFieldVoid(void* data, size_t length)
{
	if (!mOpen)
	{
		return false;
	}
	else if (mMode != GZFileAccessRead)
	{
		return false;
	}
	else
	{
		return mFile.Read(data, length);
	}
}

bool cGZDBSDFRecord::SetFieldVoid(void const* data, size_t length)
{
	if (!mOpen)
	{
		return false;
	}
	else if (mMode == GZFileAccessWrite || mMode == GZFileAccessReadWrite)
	{
		return mFile.Write(data, length);
	}
	else
	{
		return false;
	}
}

uint32_t cGZDBSDFRecord::GetPosition()
{
	if (!mOpen)
	{
		return -1;
	}

	return mFile.Position();
}

bool cGZDBSDFRecord::SeekAbsolute(uint32_t position)
{
	if (!mOpen || mMode != GZFileAccessRead)
	{
		return false;
	}

	return mFile.SeekToPosition(position) != -1;
}

bool cGZDBSDFRecord::SeekRelative(int32_t offset)
{
	if (!mOpen || mMode != GZFileAccessRead)
	{
		return false;
	}

	return mFile.SeekToRelativePosition(offset) != -1;
}

uint32_t cGZDBSDFRecord::Read(void* data, uint32_t byteCount)
{
	if (!mOpen)
	{
		return 0;
	}
	else if (mMode != GZFileAccessRead)
	{
		return 0;
	}
	else
	{
		uint32_t count = byteCount;
		mFile.ReadWithCount(data, count);
		return count;
	}
}

uint32_t cGZDBSDFRecord::Write(void const* data, uint32_t byteCount)
{
	if (!mOpen)
	{
		return 0;
	}
	else if (mMode == GZFileAccessRead)
	{
		return 0;
	}
	else
	{
		uint32_t count = byteCount;
		mFile.WriteWithCount(data, count);
		return count;
	}
}

bool cGZDBSDFRecord::Tell(uint32_t& position)
{
	if (!mOpen)
	{
		position = 0;
		return false;
	}

	position = mFile.Position();
	return true;
}

bool cGZDBSDFRecord::Seek(uint32_t offset, GZFileSeekMode method)
{
	if (!mOpen)
	{
		return false;
	}
	else if (offset > mFile.Length())
	{
		return false;
	}
	else
	{
		return mFile.Seek(offset, method) != -1;
	}
}

void cGZDBSDFRecord::Flush()
{
	if (mOpen && mMode != GZFileAccessRead)
	{
		mFile.Flush();
	}
}

bool cGZDBSDFRecord::GetPath(cIGZString** outPath)
{
	if (!mOpen)
	{
		return false;
	}

	cRZString* path = new cRZString();
	path->AddRef();
	mFile.GetPath(*path);

	*outPath = path;
	return true;
}

bool cGZDBSDFRecord::GetName(cIGZString** outName)
{
	if (!mOpen)
	{
		return false;
	}

	cRZString* name = new cRZString();
	name->AddRef();

	cRZString path;
	mFile.GetPath(path);
	cRZFile::GetPathName(path, *name, true);

	*outName = name;
	return true;
}

bool cGZDBSDFRecord::GetExtension(cIGZString** outExtension)
{
	if (!mOpen)
	{
		return false;
	}

	cRZString* extension = new cRZString();
	extension->AddRef();

	cRZString path;
	mFile.GetPath(path);
	// cRZFile::GetPathExtension(path, *extension); // TODO: GetPathExtension not implemented

	*outExtension = extension;
	return true;
}

bool cGZDBSDFRecord::Init(cGZPersistResourceKey const& key, cIGZString const& path, GZFileAccessFlags flags)
{
	mMode = flags;

	if (flags == GZFileAccessRead)
	{
		mAccessFlags = flags;
	}
	else if (flags == GZFileAccessWrite || flags == GZFileAccessReadWrite)
	{
		mAccessFlags = GZFileAccessWrite;
	}

	if (mOpen)
	{
		return false;
	}

	mFile.SetPath(path);
	mFile.SetBuffering(4096, 4096);

	if (mMode == GZFileAccessWrite)
	{
		cRZString tempPath;
		if (!cRZFile::MakeTempPathName(tempPath, NULL))
		{
			return false;
		}

		mTempFile.SetPath(tempPath);
		if (!mTempFile.Copy(mFile, true))
		{
			return false;
		}
	}

	bool opened = false;
	switch (mMode)
	{
	case GZFileAccessRead:
		opened = mFile.Open(GZFileAccessRead, GZFileOpenExistingOnly, GZFileShareRead);
		break;

	case GZFileAccessWrite:
		opened = mFile.Open(GZFileAccessWrite, GZFileOverwriteExistingOnly, GZFileShareRead);
		break;

	case GZFileAccessReadWrite:
		opened = mFile.Open(GZFileAccessWrite, GZFileCreateNewOnly, GZFileShareRead);
		break;
	}

	if (!opened)
	{
		AbortInternal();
		return false;
	}

	mKey = key;
	mOpen = true;
	return true;
}

bool cGZDBSDFRecord::CloseInternal()
{
	if (mOpen)
	{
		mOpen = false;

		if (mFile.IsOpen())
		{
			mFile.Close();
		}

		if (mMode == GZFileAccessWrite)
		{
			mTempFile.Remove();
			mMode = 0;
		}

		return true;
	}

	return false;
}

bool cGZDBSDFRecord::AbortInternal()
{
	bool result = false;
	if (mOpen)
	{
		mOpen = false;

		if (mFile.IsOpen())
		{
			mFile.Close();
		}

		if (mMode == GZFileAccessWrite)
		{
			result = mFile.Copy(mTempFile, true);
			mMode = 0;
		}
	}

	return result;
}