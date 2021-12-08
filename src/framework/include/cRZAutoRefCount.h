#pragma once

template<class T>
class cRZAutoRefCount
{
public:
	cRZAutoRefCount() : obj(NULL) { }
	cRZAutoRefCount(T* other) : obj(other)
	{
		if (other)
		{
			other->AddRef();
		}
	}

	virtual ~cRZAutoRefCount()
	{
		if (obj)
		{
			obj->Release();
		}
	}

	cRZAutoRefCount<T>& operator=(T* other)
	{
		if (obj != other)
		{
			if (obj != NULL)
			{
				obj->Release();
			}

			obj = other;

			if (obj != NULL)
			{
				obj->AddRef();
			}
		}

		return *this;
	}

	T* operator->() const
	{
		return obj;
	}

	T& operator*() const
	{
		return *obj;
	}

	operator T*() const
	{
		return obj;
	}

protected:
	T* obj;
};