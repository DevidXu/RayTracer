#include "Helper.h"
#include <assert.h>
#include <string>
#include <iostream>
#include <sstream>

#include "Constants.h"
#include "Model.h"

using namespace std;

void assertFileExist(string name) {
	const char *fileName = name.c_str();
	ifstream infile(fileName);
	string info = "Cannot find file: " + name;
	assert__(infile.good()) {
		cout << "ERROR: Cannot find file " + name << endl;
	}
}

Vector3 readVector3(string vec) {
	istringstream iss(vec);
	float x0 = 0.0, x1 = 0.0, x2 = 0.0;
	iss >> x0 >> x1 >> x2;
	return Vector3(x0, x1, x2);
}

/**
This function will read config data from single file to Constants Singleton class
Format Specification:
Height 20
Width 100
xStart 0
xEnd 100
yStart 0
yEnd 100
CameraPosition 0.0 0.0 0.0
CameraDirection 1.0 0.0 0.0

Model Config
OBJ
FileName
TextureName
cUBE 
...(8 points)
...(1 color)
SPHERE
...(1 center)
...(1 color)

**/
void loadScene(string sceneConfig, string modelConfig) {
	assertFileExist(sceneConfig);
	Logger::Instance()->info("Begin to load scene config and model config");
	// read config file
	ifstream file; file.open(sceneConfig);
	string line = "";
	while (getline(file, line)) {
		int spaceIdx = line.find(' ');
		string prefix = line.substr(0, spaceIdx);
		
		if (prefix == "Height") {
			int num = atoi(line.substr(spaceIdx + 1).c_str());
			Constants::Instance()->height = num;
		} else if (prefix == "Width") {
			int num = atoi(line.substr(spaceIdx + 1).c_str());
			Constants::Instance()->width = num;
		} else if (prefix == "xStart") {
			int num = atoi(line.substr(spaceIdx + 1).c_str());
			Constants::Instance()->xStart = num;
		} else if (prefix == "xEnd") {
			int num = atoi(line.substr(spaceIdx + 1).c_str());
			Constants::Instance()->xEnd = num;
		} else if (prefix == "yStart") {
			int num = atoi(line.substr(spaceIdx + 1).c_str());
			Constants::Instance()->yStart = num;
		} else if (prefix == "yEnd") {
			int num = atoi(line.substr(spaceIdx + 1).c_str());
			Constants::Instance()->yEnd = num;
		} else if (prefix == "CameraPosition") {
			Constants::Instance()->cameraPosition = readVector3(line.substr(spaceIdx + 1));
		} else if (prefix == "CameraDirection") {
			Constants::Instance()->cameraDirection = readVector3(line.substr(spaceIdx + 1));
		}
	}

	// read model information
	file.open(modelConfig);
	while (getline(file, line)) {
		shared_ptr<Model> model = make_shared<Model>(new Model());	// create a shared ptr to new model

		if (line == "OBJ") {
			getline(file, line); string fileName = line;
			getline(file, line); string textureName = line;
			shared_ptr<OBJShape> shape = make_shared<OBJShape>(new OBJShape());
			shape->loadOBJShape(fileName);
		}
		else if (line == "CUBE") {
			vector<Vector3> positions;
			for (int i = 0; i < 8; i++) {
				getline(file, line);
				positions.push_back(readVector3(line));
			}
			getline(file, line); Vector3 color = readVector3(line);
			shared_ptr<CubeShape> shape = make_shared<CubeShape>(new CubeShape());
			shape->loadCubeShape(positions, color);
		}
		else if (line == "SPHERE") {
			getline(file, line); Vector3 center = readVector3(line);
			getline(file, line); Vector3 color = readVector3(line);
			shared_ptr<SphereShape> shape = make_shared<SphereShape>(new SphereShape());
			shape->loadSphereShape(center, color);
		}

		Constants::Instance()->addModel(model);
		Logger::Instance()->info(line + " model has been loaded into scene");
	}
}


void constructKDTree() {
	shared_ptr<ModelArr> models = Constants::Instance()->getModels();
	shared_ptr<KDTree> kdTree = Constants::Instance()->getKDTree();
	for (auto & model : *models) {
		kdTree->addModelPatches(model);
	}
}

void renderScene() {

}