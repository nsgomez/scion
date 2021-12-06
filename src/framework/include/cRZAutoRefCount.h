#pragma once

template<class T>
class cRZAutoRefCount
{
public:
	cRZAutoRefCount();
	cRZAutoRefCount(T* obj);
	virtual ~cRZAutoRefCount();

	cRZAutoRefCount<T>& operator=(T* other);
	T* operator->() const;
	T& operator*() const;
	operator T*() const;

protected:
	T* obj;
};