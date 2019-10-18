#include <string>
#include <fstream>
#include <istream>
#include <sstream>
#include <iostream>
#include <vector>
#include "Constants.h"
#include "Helper.h"
#include "Model.h"
using namespace std;


shared_ptr<PatchArr> Shape::getPatches() {
	return make_shared<PatchArr>(patches);
}

shared_ptr<Model> Shape::getModel() {
	assert__(model != nullptr) {
		ERROR("Model has not been initialized for this shape");
	}
	return model;
}

void Shape::setModel(shared_ptr<Model> mModel) {
	model = mModel;
	for (auto patch : patches) {
		patch->setModel(model);
	}
}

Model::Model(shared_ptr<Shape> mShape, shared_ptr<Material> mMaterial, string mName) {
	assert__(mShape != nullptr) {
		ERROR("Try setting NONE Shape to model");
	}
	assert__(mName.size() > 0) {
		ERROR("The name of model is invalid");
	}
	shape = mShape;
	material = mMaterial;
	name = mName;

}


shared_ptr<Material> Model::getMaterial() {
	assert(material != nullptr);
	return material;
}

shared_ptr<PatchArr> Model::getPatches() {
	assert(shape != nullptr);
	return shape->getPatches();
}

shared_ptr<Shape> Model::getShape() {
	assert(shape != nullptr);
	return shape;
}

string Model::getName() {
	assert(name.size() > 0);
	return name;
}

bool Model::isLightSource() {
	return material->getEmissive() > 0.0f;
}


Light::Light(shared_ptr<Shape> mShape, shared_ptr<Material> mMaterial, string mName) 
	: Model(mShape, mMaterial, mName) {
	auto patches = mShape->getPatches();
	assert(patches->size() > 0);
	center = Vector3(); lightArea = 0.0f;
	for (auto patch : *patches) {
		center += patch->getCenter();
		lightArea += patch->getSurfaceArea();
	}
	center /= (float)patches->size();
	
	assert(lightArea > 0.0f);
}


Vector3 Light::getCenter() {
	return center;
}

float Light::getLightPDF(Vector3 point) {
	float pdf = 0.0f;
	float d = (point - center).magnitude();
	static float PI = Constants::Instance()->PI;
	pdf = 4 * PI * d * d / lightArea;
	if (pdf < 1.0f) pdf = 1.0f;
	return pdf;
}


// load OBJ Model (with(out) Normal/Texcoord)
OBJShape::OBJShape(string fileName) {

	assertFileExist(fileName);
	ifstream inf; inf.open(fileName);
	string line, mark, s[3];

//	if (textureName != "")
//		texture = imread(textureName, CV_LOAD_IMAGE_UNCHANGED);

	while (getline(inf, line))
	{
		line = trim(line);
		if (line.empty()) continue;

		istringstream in(line);
		in >> mark >> s[0] >> s[1] >> s[2];

		if (line[0] == 'v' && line[1] == ' ') {
			Vector3 position(float(atof(s[0].c_str())), float(atof(s[1].c_str())), float(atof(s[2].c_str())));
			positions.push_back(position);

		};

		if (line[0] == 'v' && line[1] == 'n') {
			Vector3 n(float(atof(s[0].c_str())), float(atof(s[1].c_str())), float(atof(s[2].c_str())));
			n.normalize();
			normals.push_back(n);
		}

		if (line[0] == 'v' && line[1] == 't') {
			Vector2 t(float(atof(s[0].c_str())), float(atof(s[1].c_str())));
			texcoords.push_back(t);
		}

		if (line[0] == 'f') {
			vector<vector<int>> nums(3, vector<int>());
			for (int i = 0; i < 3; i++) {
				int count = 1;
				for (auto & c : s[i]) {
					count += (c == '/');
					c = (c == '/' ? ' ' : c);
				}
				istringstream iss(s[i]);
				while (count--) {
					int idx = 0; iss >> idx; idx -= 1;
					nums[i].push_back(idx);
				}
			}

			int dataLen = nums[0].size();
			assert__(dataLen >= 1) {
				ERROR("No enough points in face");
			}
			Vertex v[3];
			for (int i = 0; i < 3; i++) {
				switch (dataLen) {
					case 1: 
						v[i] = Vertex(make_shared<Vector3>(positions[nums[i][0]])); 
						break;
					case 2: 
						v[i] = Vertex(
							make_shared<Vector3>(positions[nums[i][0]]), 
							make_shared<Vector3>(normals[nums[i][1]])
						); 
						break;
					case 3: 
						v[i] = Vertex(
							make_shared<Vector3>(positions[nums[i][0]]), 
							make_shared<Vector3>(normals[nums[i][1]]),
							make_shared<Vector2>(texcoords[nums[i][2]])
						);
						break;
					default: assert__(false) { ERROR("Invalid length of data in face"); } break;
				}
				
			}
			shared_ptr<Patch> triangle = make_shared<Triangle>(v[0], v[1], v[2]);
			patches.push_back(triangle);
		}
	}

	inf.close();
}

/*
how to record cube points:
    Y
	0----1
  3-+--2 |
  | 7--+-6 Z
  4 -- 5
X
*/
CubeShape::CubeShape(vector<Vector3>& mPositions, Vector3 mColor) {
	for (auto & point : mPositions) {
		positions.push_back(point);
	}
	color = mColor;

	generatePatches();
}

CubeShape::CubeShape(Vector3 small, Vector3 large, Vector3 mColor) {

	for (int i = 0; i < 3; i++) {
		assert__(small.value[i] < large.value[i]) {
			ERROR("Cube_Simple has small > large");
		}
	}
	positions.push_back(Vector3(small.value[0], large.value[1], small.value[2]));
	positions.push_back(Vector3(small.value[0], large.value[1], large.value[2]));
	positions.push_back(Vector3(large.value[0], large.value[1], large.value[2]));
	positions.push_back(Vector3(large.value[0], large.value[1], small.value[2]));
	positions.push_back(Vector3(large.value[0], small.value[1], small.value[2]));
	positions.push_back(Vector3(large.value[0], small.value[1], large.value[2]));
	positions.push_back(Vector3(small.value[0], small.value[1], large.value[2]));
	positions.push_back(Vector3(small.value[0], small.value[1], small.value[2]));

	color = mColor;

	generatePatches();
}


void CubeShape::generatePatches() {
	assert(positions.size() > 0);
	vector<Vertex> vertexes;
	normals.push_back(Vector3());

	for (int i = 0; i < (int)positions.size(); i++) {
		Vertex vertex = Vertex(
			make_shared<Vector3>(positions[i]),
			make_shared<Vector3>(normals[0]),
			make_shared<Vector3>(color)
		);
		vertexes.push_back(vertex);
	}

	vector<vector<int>> pointSets = { { 0,3,1 },{ 1,3,2 },{ 1,2,6 },{ 6,2,5 },{ 6,5,7 },{ 7,5,4 },{ 7,4,0 },{ 0,4,3 },{ 3,2,4 },{ 4,2,5 },{ 7,6,0 },{ 0,6,1 } };
	// Build patches
	for (auto & pointSet : pointSets) {
		shared_ptr<Triangle> patchPtr = make_shared<Triangle>(vertexes[pointSet[0]], vertexes[pointSet[1]], vertexes[pointSet[2]]);
		patches.push_back(patchPtr);
	}
	return;
}


SphereShape::SphereShape(Vector3 mCenter, Vector3 mColor, float mRadius) {
	
	positions.push_back(mCenter);
	normals.push_back(Vector3(1.0, 0.0, 0.0));
	texcoords.push_back(Vector2(0.0, 0.0));

	shared_ptr<Patch> patchPtr = make_shared<Sphere>(mCenter, mColor, mRadius);
	patches.push_back(patchPtr);
}