#include "Model.h"
#include <string>
#include <fstream>
#include <istream>
#include <sstream>
#include <iostream>
#include <vector>
#include "Helper.h"
using namespace std;


shared_ptr<PatchArr> Shape::getPatches() {
	return make_shared<PatchArr>(&patches);
}

// collect vertex data and store as Vertex struct format
void OBJShape::generateVertexes(vector<Vector3>& positions, vector<Vector3>& normals, vector<Vector2>& texcoords) {
	static bool vertexStored = false;
	if (vertexStored) return; else vertexStored = true;

	assert__(positions.size() > 0) {
		cout << "ERROR: Empty Model with no position data" << endl;
	}
	
	bool sample = true;
	if (texcoords.empty()) sample = false;

	for (int i = 0; i < positions.size(); i++) {
		Vector3 normal = normals.empty() ? Vector3(0.0f, 1.0f, 0.0f) : normals[i];
		if (!sample) {
			Vector3 defaultColor = Vector3(1.0f, 1.0f, 1.0f);
			vertexes.push_back(Vertex(positions[i], normal, defaultColor));
		}
		else {
			vertexes.push_back(Vertex(positions[i], normal, texcoords[i]));
		}
	}
}

// load OBJ Model (with(out) Normal/Texcoord)
void OBJShape::loadOBJShape(string fileName) {
	assertFileExist(fileName);
	ifstream inf; inf.open(fileName);
	string line, mark, s[3];

//	if (textureName != "")
//		texture = imread(textureName, CV_LOAD_IMAGE_UNCHANGED);

	while (getline(inf, line))
	{
		istringstream in(line);
		in >> mark >> s[0] >> s[1] >> s[2];

		if (line[0] == 'v' && line[1] == ' ') {
			Vector3 position = Vector3(float(atof(s[0].c_str())), float(atof(s[1].c_str())), float(atof(s[2].c_str())));
			positions.push_back(position);

		};

		if (line[0] == 'v' && line[1] == 'n') {
			Vector3 n = Vector3(float(atof(s[0].c_str())), float(atof(s[1].c_str())), float(atof(s[2].c_str())));
			n.normalize();
			normals.push_back(n);
		}

		if (line[0] == 'v'&& line[1] == 't') {
			Vector2 t = Vector2(float(atof(s[0].c_str())), float(atof(s[1].c_str())));
			texcoords.push_back(t);
		}

		if (line[0] == 'f') {
			generateVertexes(positions, normals, texcoords);
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
			assert(dataLen >= 1);
			Vertex v[3];
			for (int i = 0; i < 3; i++) {
				switch (dataLen) {
					case 1: v[i] = Vertex(positions[nums[i][0]]); break;
					case 2: v[i] = Vertex(positions[nums[i][0]], normals[nums[i][1]]); break;
					case 3: v[i] = Vertex(positions[nums[i][0]], normals[nums[i][1]], texcoords[nums[i][2]]); break;
					default: assert(false); break;
				}
				
			}
			shared_ptr<Patch> triangle(new Triangle(v[0], v[1], v[2]));
			patches.push_back(triangle);
		}
	}

	inf.close();
}


void Model::setShape(shared_ptr<Shape> mShape) {
	assert(mShape != NULL);
	shape = mShape;
}

shared_ptr<vector<Patch>> Model::getPatches() {
	return make_shared<vector<Patch>>(shape);
}