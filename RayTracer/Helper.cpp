#include "Helper.h"
#include <assert.h>
#include <string>
#include <iostream>
#include <sstream>

#include "Constants.h"
#include "Model.h"
#include "Logger.h"
#include "KDTree.h"
#include "Raster.h"

using namespace std;

void assertFileExist(string name) {
	const char *fileName = name.c_str();
	ifstream infile(fileName);
	string info = "Cannot find file: " + name;
	assert__(infile.good()) {
		ERROR("ERROR: Cannot find file " + name);
	}
}

Vector3 readVector3(string vec) {
	assert__(vec.size() > 0) {
		ERROR("Try to decode Vector3 from empty line");
	}
	istringstream iss(vec);
	float x0 = 0.0, x1 = 0.0, x2 = 0.0;
	iss >> x0 >> x1 >> x2;
	return Vector3(x0, x1, x2);
}


void readMaterial(shared_ptr<Material> material, string line) {
	string front = line.substr(0, line.find(' '));
	assert__(front == "Material") {
		ERROR("Incorrect input format of material");
	}
	line = line.substr(line.find(' ') + 1);
	float emissive, specular, glass;
	stringstream ss(line);
	ss >> emissive >> specular >> glass;
	material->setEmissive(emissive);
	material->setSpecular(specular);
	material->setGlass(glass);
}

string trim(string s) {
	int begin = 0;
	while (begin < (int)s.size() && s[begin] == ' ') begin += 1;
	s = s.substr(begin);
	int end = s.size();
	while (end > 0 && (s[end-1] == ' ' || s[end-1] == '\n')) end -= 1;
	return s.substr(0, end);
}


/*
Scene Config Format Specification:
Height 20
Width 100
xStart 0
xEnd 100
yStart 0
yEnd 100
CameraPosition 0.0 0.0 0.0
CameraDirection 1.0 0.0 0.0
Small -2.0 -2.0 -2.0
Large 2.0 2.0 2.0
*/
void loadSceneConfig(string sceneConfig) {
	// read config file
	ifstream file; file.open(sceneConfig);
	string line = "";
	while (getline(file, line)) {
		line = trim(line);
		if (line.empty()) continue;

		int spaceIdx = line.find(' ');
		string prefix = line.substr(0, spaceIdx);

		if (prefix == "Height") {
			int num = atoi(line.substr(spaceIdx + 1).c_str());
			Raster::Instance()->height = num;
		}
		else if (prefix == "Width") {
			int num = atoi(line.substr(spaceIdx + 1).c_str());
			Raster::Instance()->width = num;
		}
		else if (prefix == "xStart") {
			int num = atoi(line.substr(spaceIdx + 1).c_str());
			Raster::Instance()->xStart = num;
		}
		else if (prefix == "xEnd") {
			int num = atoi(line.substr(spaceIdx + 1).c_str());
			Raster::Instance()->xEnd = num;
			assert__(num > Raster::Instance()->xStart) {
				INFO("Invalid range of xStart -- xEnd");
			}
		}
		else if (prefix == "yStart") {
			int num = atoi(line.substr(spaceIdx + 1).c_str());
			Raster::Instance()->yStart = num;
		}
		else if (prefix == "yEnd") {
			int num = atoi(line.substr(spaceIdx + 1).c_str());
			Raster::Instance()->yEnd = num;
			assert__(num > Raster::Instance()->yStart) {
				INFO("Invalid range of yStart -- yEnd");
			}
		}
		else if (prefix == "CameraPosition") {
			Raster::Instance()->cameraPosition = readVector3(line.substr(spaceIdx + 1));
		}
		else if (prefix == "CameraDirection") {
			Vector3 direction = readVector3(line.substr(spaceIdx + 1));
			direction = direction.normalize();
			Raster::Instance()->cameraDirection = direction;
		}
		else if (prefix == "angleHeight") {
			float num = float(atoi(line.substr(spaceIdx + 1).c_str()));
			num *= Constants::Instance()->PI / 180;
			Raster::Instance()->angleHeight = num;
		}
		else if (prefix == "angleWidth") {
			float num = float(atof(line.substr(spaceIdx + 1).c_str())); // degree to pi
			num *= Constants::Instance()->PI / 180;
			Raster::Instance()->angleWidth = num;
		}
		else if (prefix == "SampleNum") {
			int num = atoi(line.substr(spaceIdx + 1).c_str());
			Raster::Instance()->sampleNum = num;
		}
	}
	file.close();
	return;
}

/*
Model Config Format Specification
OBJ
Material 0.0 0.0 0.2
FileName
TextureName
CUBE Name
Material 0.0 0.0 0.2
...(8 points, 8 lines)
...(1 color)
CUBE_SIMPLE Name
Material 0.0 0.2 0.2
...(1 small)
...(1 large)
...(1 color)
SPHERE Name
Material 0.1 0.2 0.2
...(1 center)
...(Radius)
...(1 color)
*/
void loadModelConfig(string modelConfig) {
	// read model information
	ifstream file; file.open(modelConfig);
	string line = "";
	while (getline(file, line)) {
		line = trim(line);
		if (line.empty()) continue;
		stringstream ss(line);
		string modelType = "", modelName = "";
		ss >> modelType >> modelName;

		shared_ptr<Model> model = nullptr;
		shared_ptr<Shape> shape = nullptr;
		shared_ptr<Material> material = make_shared<Material>();
		getline(file, line);
		readMaterial(material, line);

		if (modelType == "OBJ") {
			getline(file, line); string fileName = line;
			getline(file, line); string textureName = line;
			shape = make_shared<OBJShape>(fileName);
		}
		else if (modelType == "CUBE") {
			vector<Vector3> positions;
			for (int i = 0; i < 8; i++) {
				getline(file, line);
				positions.push_back(readVector3(line));
			}
			getline(file, line); Vector3 color = readVector3(line);
			shape = make_shared<CubeShape>(positions, color);
		} 
		else if (modelType == "CUBE_SIMPLE") {
			getline(file, line); Vector3 small = readVector3(line);
			getline(file, line); Vector3 large = readVector3(line);
			getline(file, line); Vector3 color = readVector3(line);
			shape = make_shared<CubeShape>(small, large, color);
		} 
		else if (modelType == "SPHERE") {
			getline(file, line); Vector3 center = readVector3(line);
			getline(file, line); float radius = float(atof(line.c_str()));
			getline(file, line); Vector3 color = readVector3(line);
			shape = make_shared<SphereShape>(center, color, radius);
		}
		else assert__(false) {
			ERROR("Try to load unsupported type of model");
		}

		if (material->getEmissive() > 0.0f) {
			auto light = make_shared<Light>(shape, material, modelName);
			Raster::Instance()->addLight(light);
			model = shared_ptr<Model>(light);
		}
		else model = make_shared<Model>(shape, material, modelName);

		shape->setModel(model);

		Raster::Instance()->addModel(model);
		INFO(modelName + " model has been loaded into scene");
	}
	file.close();
	return;
}


/**
This function will read config data from single file to Constants Singleton class
**/
void loadConfigInfo(string sceneConfig, string modelConfig) {
	assertFileExist(sceneConfig);
	INFO("Begin to load scene config and model config");
	loadSceneConfig(sceneConfig);
	loadModelConfig(modelConfig);
	INFO("All models have been loaded into Ray Tracer");
}


void constructKDTree() {
	shared_ptr<ModelArr> models = Raster::Instance()->getModels();
	PatchArr patches;
	Vector3 small, large;
	for (auto & model : *models) {
		shared_ptr<PatchArr> modelPatches = model->getPatches();
		for (auto & patch : *modelPatches) {
			patches.push_back(patch);
			patch->expandKDTree(small, large);
		}
	}
	assert__((small == large) == false) {
		ERROR("KDTree has zero size (small == large)");
	}

	INFO("Begin to construct KD Tree");
	Raster::Instance()->kdTree = make_shared<KDTree>(small, large, X_DIM);
	shared_ptr<KDTree> kdTree = Raster::Instance()->getKDTree();
	kdTree->constructFromPatches(patches);
	INFO("KDTree depth: " + to_string(kdTree->getDepth()));
	INFO("KDTree size: " + to_string(kdTree->getTotalSize()));
	INFO("KD Tree has been constructed completely");
}


void renderScene() {
	Raster::Instance()->render();
	Raster::Instance()->writeToPPM("result");
}
