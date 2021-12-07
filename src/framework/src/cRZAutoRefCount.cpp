#include <cstddef>
#include "cRZAutoRefCount.h"

template<class T>
cRZAutoRefCount<T>::cRZAutoRefCount()
{
	obj = NULL;
}

template<class T>
cRZAutoRefCount<T>::cRZAutoRefCount(T* obj)
{
	this->obj = obj;
	if (obj != NULL)
	{
		obj->AddRef();
	}
}

template<class T>
cRZAutoRefCount<T>::~cRZAutoRefCount()
{
	if (obj)
	{
		obj->Release();
	}
}

template<class T>
cRZAutoRefCount<T>& cRZAutoRefCount<T>::operator=(T* other)
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

template<class T>
T* cRZAutoRefCount<T>::operator->() const
{
	return obj;
}

template<class T>
T& cRZAutoRefCount<T>::operator*() const
{
	return *obj;
}

template<class T>
cRZAutoRefCount<T>::operator T*() const
{
	return obj;
}