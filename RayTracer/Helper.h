#pragma once
#ifndef _HELPER_H__
#define _HELPER_H__

#include <fstream>
#include <string>
#include <assert.h>
#include "Singleton.h"

#define assert__(x) for ( ; !(x) ; assert(x) )

struct Vector3;

using namespace std;

void assertFileExist(string name);

Vector3 readVector3(string vec);

void loadConfigInfo(std::string sceneConfig, std::string modelConfig);

void constructKDTree();

string trim(string s);

#endif