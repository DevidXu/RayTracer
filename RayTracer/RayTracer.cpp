#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Helper.h"
using namespace std;

int main() {
	cout << "Ray Tracer. USC CSCI-653: High Performance Computing" << endl;
	loadScene("./Config/sceneConfig", "./Config/modelConfig");
	constructKDTree();
	renderScene();
	system("pause");
}