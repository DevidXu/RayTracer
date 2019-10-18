#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include "Helper.h"
using namespace std;

int main() {
	cout << "Ray Tracer. USC CSCI-653: High Performance Computing" << endl;
	srand((int)time(NULL));
	loadConfigInfo("./Config/sceneConfig", "./Config/modelConfig");
	constructKDTree();
	renderScene();
	system("pause");
}