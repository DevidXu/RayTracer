#pragma once
#ifndef _SINGLETON_H__
#define _SINGLETON_H__

#include <memory>
#include <assert.h>

using namespace std;

template<class T>
class Singleton {
public:
	static shared_ptr<T> Instance() {
		if (!m_Instance) m_Instance = make_shared<T>();
		assert(m_Instance != NULL);
		return m_Instance;
	}
	static shared_ptr<T> m_Instance;
protected:
	Singleton() {};
	~Singleton() {};
};

template<class T> shared_ptr<T> Singleton<T>::m_Instance = NULL;

#endif