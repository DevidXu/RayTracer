#pragma once
#ifndef _HELPER_H__
#define _HELPER_H__

#include <fstream>
#include <string>
#include <assert.h>
#include "Vector.h"
#include "Logger.h"

#define assert__(x) for ( ; !(x) ; assert(x) )

using namespace std;

template<class T>
class Singleton {
public:
	static shared_ptr<T> Instance() {
		if (!m_Instance) m_Instance = shared_ptr<T>(new T());
		assert(m_Instance != NULL);
		return m_Instance;
	}
	static shared_ptr<T> m_Instance;
protected:
	Singleton() {};
	~Singleton() {};
};

template<class T> shared_ptr<T> Singleton<T>::m_Instance = NULL;

void assertFileExist(string name);

Vector3 readVector3(string vec);

void loadScene(std::string sceneConfig, std::string modelConfig);

void constructKDTree();

void renderScene();

#endif