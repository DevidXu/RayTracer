#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <mpi.h>
#include "Helper.h"
#include "Raster.h"
using namespace std;

#define Scene Raster::Instance()

int world_rank = 0, world_size = 0;

void collectFromRanks(Picture &picture) {
	assert__(picture.size() > 0 && picture[0].size() == Scene->width) {
		INFO("Incorrect output size to fit all data");
	}

	int subRow = picture.size(), subCol = picture[0].size();
	int length = subRow * subCol * 3;
	float *buf = new float[length]();

	MPI_Request request; MPI_Status status;

	if (world_rank != 0) {
		int idx = 0;
		for (int i = 0; i<subRow; i++)
			for (int j = 0; j < subCol; j++) {
				memcpy(buf + idx, picture[i][j].value, sizeof(float) * 3);
				idx += 3;
			}
		MPI_Isend(buf, length, MPI_FLOAT, 0, world_rank, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
	}
	else {
		Picture graph(Scene->height, vector<Vector3>(Scene->width, Vector3()));
		for (int i = 0; i<subRow; i++)
			for (int j = 0; j < subCol; j++) {
				memcpy(graph[i][j].value, picture[i][j].value, sizeof(float) * 3);
			}
		for (int k = 1; k < world_size; k++) {
			MPI_Recv(buf, length, MPI_FLOAT, k, k, MPI_COMM_WORLD, &status);
			int idx = 0;
			for (int i = 0; i < subRow; i++) {
				int row = i + k * subRow;
				if (row >= Scene->height) continue;
				for (int j = 0; j < subCol; j++) {
					memcpy(graph[row][j].value, buf + idx, sizeof(float) * 3);
					idx += 3;
				}
			}
		}
		Scene->writeToPPM("result", graph);
	}

	free(buf);
}


int main(int argc, char **argv) {
	cout << "Ray Tracer. USC CSCI-653: High Performance Computing" << endl;
	srand((int)time(NULL));

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	loadConfigInfo("./Config/sceneConfig", "./Config/modelConfig");
	constructKDTree();

	assert(Scene->height >= world_size);
	int height = Scene->height / world_size;
	Scene->xStart = 0; Scene->xEnd = Scene->width;
	Scene->yStart = world_rank * height;
	Scene->yEnd = (world_rank + 1)*height;

	Scene->render();

	collectFromRanks(Scene->picture);
	
	MPI_Finalize();
}