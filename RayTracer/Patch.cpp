#include "Patch.h"

Vertex::Vertex() {
	sample = false;
}

Vertex::Vertex(Vector3& mPos) {
	sample = false;
	position = shared_ptr<Vector3>(&mPos);
}

Vertex::Vertex(Vector3& mPos, Vector3& mNor) {
	sample = false;
	position = shared_ptr<Vector3>(&mPos);
	normal = shared_ptr<Vector3>(&mNor);
}



Vertex::Vertex(Vector3& mPos,Vector3& mNor, Vector3& mColor) {
	sample = false;
	position = shared_ptr<Vector3>(&mPos);
	normal = shared_ptr<Vector3>(&mNor);
	color = shared_ptr<Vector3>(&mColor);
}

Vertex::Vertex(Vector3& mPos, Vector3& mNor, Vector2& mTex) {
	sample = true;
	position = shared_ptr<Vector3>(&mPos);
	normal = shared_ptr<Vector3>(&mNor);
	texCoord = shared_ptr<Vector2>(&mTex);
}

Triangle::Triangle(Vertex& v0, Vertex& v1, Vertex& v2) {
	points[0] = shared_ptr<Vertex>(&v0);
	points[1] = shared_ptr<Vertex>(&v1);
	points[2] = shared_ptr<Vertex>(&v2);
}