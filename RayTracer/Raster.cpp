#include "Raster.h"
#include "Helper.h"
#include "Constants.h"
#include <time.h>

#define RAY_MARCH 0.0001f

pair<float, float> rotate2D(float x, float y, float theta) {
	float c = cos(theta), s = sin(theta);
	float newx = c * x + s * y, newy = -s * x + c * y;
	return pair<float, float>(newx, newy);
}

Vector3 rotateDirection(Vector3 direction, float angleX, float angleY) {
	assert(direction.magnitude() == 1.0);
	float x = direction.value[0], y = direction.value[1], z = direction.value[2];
	float sxxzz = sqrt(x * x + z * z);
	pair<float, float> p = rotate2D(sxxzz, y, -angleY);
	x = sxxzz == 0.0f ? 0.0f : x * p.first / sxxzz;
	y = p.second;
	z = sxxzz == 0.0f ? p.first : z * p.first / sxxzz;
	p = rotate2D(x, z, angleX);
	x = p.first; z = p.second;
	return Vector3(x, y, z);
}


Vector3 biasDirection(Vector3 direction, float angleX, float angleY) {
	assert__(direction.value[0] != 0.0f || direction.value[2] != 0.0f) {
		ERROR("Camera is pointing upward, which is not supported in this tracer");
	}
	Vector3 dirX, dirY;
	dirX = Vector3(-direction.value[2], 0.0f, direction.value[0]).normalize();
	dirY = direction.cross(dirX) * -1.0f;
	return (direction + dirX * tan(-angleX) + dirY * tan(angleY)).normalize();
}


float calculateGGX(Vector3 k, Vector3 n, float roughness) {
	float nk = abs(n.dot(k));
	float rr = roughness * roughness;
	return nk * 2 / (nk + sqrt(rr + (1 - rr)*nk*nk));
}


float BRDF(Vector3 dir, shared_ptr<Patch> hitPatch, Vector3 oldDir, Vector3 n) {
	Vector3 l(Vector3() - oldDir);	// in light
	Vector3 v(dir);	// out light
	Vector3 h((l + v) / 2);
	float Fc = (float)pow(1 - v.dot(h), 5.0f);
	float F = 0.05f *(1 - Fc) + Fc;
	float roughness = 0.3f;
	float G = calculateGGX(l, n, roughness) * calculateGGX(v, n, roughness);
	float denominator = n.dot(v)*n.dot(h);
	if (denominator == 0.0f) denominator += Constants::Instance()->epsilon;
	return F * G*v.dot(h) / denominator;
}


Vector3 saturate(Vector3 color) {
	for (int i = 0; i < 3; i++) {
		if (color.value[i] < 0.0f) color.value[i] = 0.0f;
		if (color.value[i] > 1.0f) color.value[i] = 1.0f;
	}
	return color;
}

Raster::Raster() {
	models = make_shared<ModelArr>();
	lights = make_shared<LightArr>();
}

void Raster::addModel(shared_ptr<Model> modelPtr) { 
	models->push_back(modelPtr); 
}

shared_ptr<ModelArr> Raster::getModels() { 
	return models; 
}

void Raster::addLight(shared_ptr<Light> light) {
	lights->push_back(light);
}

shared_ptr<LightArr> Raster::getLights() {
	return lights;
}

shared_ptr<KDTree> Raster::getKDTree() {
	assert__(kdTree != nullptr) {
		ERROR("KD tree not initialized before using it");
	}
	return kdTree;
}


Rays Raster::emitRays(float angleX, float angleY, int num) {
	Rays rays;
	float epsilon = Constants::Instance()->epsilon;

	for (int i = 0; i < num; i++) {
		Vector3 direction = biasDirection(cameraDirection, angleX, angleY);
		assert(abs(direction.magnitude() - 1.0f) < epsilon);
		shared_ptr<Ray> ray = make_shared<Ray>(cameraPosition, direction);
		rays.push_back(ray);
	}

	return rays;
}


void Raster::printParameters() {
	INFO("Height: " + to_string(height) + "; Width: " + to_string(width));
	INFO("xStart: " + to_string(xStart) + "; xEnd: " + to_string(xEnd));
	INFO("yStart: " + to_string(yStart) + "; yEnd: " + to_string(yEnd));
	INFO("Sample Num: " + to_string(sampleNum));
	INFO("angleHeight: " + to_string(angleHeight) + "; angleWidth: " + to_string(angleWidth));
	return;
}


void Raster::render() {
	assert__(sampleNum > 0 && xEnd > xStart && yEnd > yStart) {
		ERROR("Invalid Initialization parameters");
	}
	printParameters();

	picture = vector<vector<Vector3>>(yEnd - yStart, vector<Vector3>(xEnd - xStart));

	clock_t begin = clock();
	INFO("Start tracing rays recursively");

#if OMP_ACCR == 1
#pragma omp parallel for
#else 
	int yPixel = 46, xPixel = 46;
	debugSinglePixel(xPixel, yPixel);
	return;
#endif
	for (int y = yStart; y < yEnd; y++) {
		if (y >= Raster::Instance()->height) continue;
#if OMP_ACCR == 1
#pragma omp parallel for
#endif
		for (int x = xStart; x < xEnd; x++) {
			float angleX = atan(tan(angleWidth/2) * ((x*1.0f / width) - 0.5f));
			float angleY = -atan(tan(angleHeight/2) * ((y*1.0f / height) - 0.5f));
			Rays rays = emitRays(angleX, angleY, sampleNum);

			Vector3 color;
			for (auto & ray : rays) {
				color += traceRay(ray, kdTree, nullptr);
			}
			color /= sampleNum * 1.0f;
			picture[y - yStart][x - xStart] = color;
		}
		float progress = (y - yStart) * 1.0f / (yEnd - yStart);
		string pg = "Line " + to_string(y) + " has been rendered;";
#if OMP_ACCR == 1
		pg += " progress: " + to_string(progress);
#endif
		INFO(pg);
	}

	float end = float(clock() - begin) / CLOCKS_PER_SEC;
	INFO("The tracing process runs for " + to_string(end) + " seconds");
	return;
}


void Raster::debugSinglePixel(int xPixel, int yPixel) {
	INFO("Trace particular ray at pixel(" + to_string(yPixel) + ", " + to_string(xPixel) + ")");
	float angleX = atan(tan(angleWidth / 2) * ((xPixel*1.0f / width) - 0.5f));
	float angleY = -atan(tan(angleHeight / 2) * ((yPixel*1.0f / height) - 0.5f));
	Rays rays = emitRays(angleX, angleY, sampleNum);

	int idx = 0;
	Vector3 color;
	for (auto & ray : rays) {
		shared_ptr<HitHistory> hitHistory = make_shared<HitHistory>();
		Vector3 curColor = traceRay(ray, kdTree, hitHistory);
		color += curColor;
		INFO("  Information for Ray " + to_string(idx++) + " -- Depth: " + to_string(hitHistory->size()));
		INFO("    Color: " + curColor.toString());
		for (auto & p : *hitHistory) {
			INFO("    Hit: " + p.first + "; dist: " + to_string(p.second));
		}
		INFO("");
	}
	color /= sampleNum * 1.0f;
	INFO("Final Color: " + color.toString());
}


Vector3 Raster::directLight(shared_ptr<Ray> ray, shared_ptr<Patch> hitPatch, Vector3 hitPoint) {
	Vector3 color;
	static float epsilon = Constants::Instance()->epsilon;
	for (auto & light : *lights) {
		// first: whether it is in shadow?
		shared_ptr<Ray> ray = make_shared<Ray>(hitPoint, light->getCenter() - hitPoint);
		ray->march(RAY_MARCH);
		HitInfo hitInfo(nullptr, INT_MAX * 1.0f);
		kdTree->intersectWithRay(ray, hitInfo);
		if (hitInfo.first->getModel()->getName() != light->getName()) continue;

		float pdf = light->getLightPDF(hitPoint);
		assert(pdf > 0.0f);
		color += hitInfo.first->getColor() *light->getMaterial()->getEmissive() / pdf;
	}
	return color;
}


Vector3 Raster::traceRay(shared_ptr<Ray> ray, const shared_ptr<KDTree>& kdTree, shared_ptr<HitHistory> hitHistory) {
	static int MAX_DEPTH = Constants::Instance()->TRACE_DEPTH;
	static Vector3 ENV = Constants::Instance()->ENVIR_AMBIENT;
	static float epsilon = Constants::Instance()->epsilon;
	static float attenuation = Constants::Instance()->attenuation;
	int depth = 0; 
	Vector3 throughput(1.0f, 1.0f, 1.0f);
	Vector3 radiance;

	while (depth < MAX_DEPTH) {
		HitInfo hitInfo(nullptr, INT_MAX * 1.0f);
		kdTree->intersectWithRay(ray, hitInfo);
		shared_ptr<Patch> hitPatch = hitInfo.first;
		Vector3 hitPoint = ray->getPosition() + ray->getDirection() * hitInfo.second;
		Vector3 normal = hitPatch == nullptr ? Vector3() : hitPatch->getNormal(hitPoint);
		if (hitHistory != nullptr) {
			string name = hitPatch == nullptr ? "Empty" : hitPatch->getModel()->getName();
			hitHistory->push_back(pair<string, float>(name, hitInfo.second));
		}

		if (hitPatch == nullptr) {
			radiance += ENV * throughput;
			return radiance;
		} else if (hitPatch->getModel()->isLightSource()) {
			radiance += hitPatch->getColor() * hitPatch->getMaterial()->getEmissive() * throughput;
			return radiance;
		} else {
			float diffusePara = 1.0f - hitPatch->getMaterial()->getSpecular() - hitPatch->getMaterial()->getGlass();
			radiance += directLight(ray, hitPatch, hitPoint) * hitPatch->getColor() * throughput * diffusePara;	// point light source
			Vector3 oldDir = ray->getDirection(), dir = hitPatch->transmit(ray, normal);
			ray->updateTrace(hitPoint, dir); ray->march(RAY_MARCH);
			throughput = throughput * hitPatch->getColor() * attenuation;// BRDF(dir, hitPatch, oldDir, normal);
		}

		depth += 1;
	}

	return radiance;
}


void Raster::normalize(Picture &graph) {
	assert__(graph.size() > 0) {
		ERROR("Cannot write empty image to file");
	}
	const int rows = graph.size(), cols = graph[0].size();

	Vector3 max(1.0f, 1.0f, 1.0f);

	for (int i=0; i<rows; i++)
		for (int j = 0; j < cols; j++) {
			for (int k = 0; k < 3; k++)
				if (graph[i][j].value[k] > max.value[k])
					max.value[k] = graph[i][j].value[k];
		}
	for (int i = 0; i<rows; i++)
		for (int j = 0; j < cols; j++) {
			for (int k = 0; k < 3; k++) {
				float origin = graph[i][j].value[k];
				origin /= max.value[k];
				graph[i][j].value[k] = pow(origin, 0.4f);
			}
		}
}


void Raster::writeToPPM(string fileName, Picture &graph) {
	assert__(graph.size() > 0) {
		ERROR("Cannot write empty image to file");
	}

	normalize(graph);
	const int rows = graph.size(), cols = graph[0].size();
	
	FILE *fp = NULL;
	fopen_s(&fp, (fileName + ".ppm").c_str(), "wb"); /* b - binary mode */
	INFO("Create and write to file " + fileName + ".ppm");

	(void)fprintf(fp, "P6\n%d %d\n255\n", cols, rows);
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			static unsigned char color[3];
			for (int k = 0; k < 3; k++)
				color[k] = (int)(graph[i][j].value[k] * 255) % 256;
			fwrite(color, 1, 3, fp);
		}
	}

	INFO("File has been saved. Please check local workspace");
	fclose(fp);
}