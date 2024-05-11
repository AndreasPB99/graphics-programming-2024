
// Uniforms
uniform vec3 SphereColor = vec3(0, 0, 1);
uniform vec3 SphereCenter = vec3(-2, 0, -10);
uniform float SphereRadius = 1.25f;

uniform vec3 BoxColor = vec3(1, 0, 0);
uniform mat4 BoxMatrix = mat4(1,0,0,0,   0,1,0,0,   0,0,1,0,   2,0,-10,1);
uniform mat4 BoxMatrix2 = mat4(1,0,0,0,   0,1,0,0,   0,0,1,0,   2,0,-10,1);
uniform vec3 BoxSize = vec3(1, 1, 1);

uniform vec3 CylinderColor = vec3(0, 1, 0);
uniform mat4 CylinderMatrix = mat4(1,0,0,0,   0,1,0,0,   0,0,1,0,   2,0,-10,1);
uniform float CylinderRadius = 1.5f;
uniform float CylinderHeight = 2.1f;

uniform vec3 TriPrismColor = vec3(0, 1, 1);
uniform mat4 TriPrismMatrix = mat4(1,0,0,0,   0,1,0,0,   0,0,1,0,   2,0,-10,1);
uniform vec2 TriPrismHeight = vec2(1, 1);

uniform float SphereSmoothness = 1.0f;
uniform float CylinderSmoothness = 1.0f;
uniform float BoxSmoothness = 1.0f;
uniform float TriPrismSmoothness = 1.0f;

struct SDFHelper
{
	float d;
	vec3 color;
	float smoothness;
};

// Output structure
struct Output
{
	// color of the closest figure
	vec3 color;
};


// Signed distance function
float GetDistance(vec3 p, inout Output o)
{

	float dSphere = SphereSDF(TransformToLocalPoint(p, SphereCenter), SphereRadius);
	SDFHelper SDFSphere = SDFHelper(dSphere, SphereColor, SphereSmoothness);

	float dBox = BoxSDF(TransformToLocalPoint(p, BoxMatrix), BoxSize);
	SDFHelper SDFBox = SDFHelper(dBox, BoxColor, BoxSmoothness);

	float dCylinder = CylinderSDF(TransformToLocalPoint(p, CylinderMatrix), CylinderHeight, CylinderRadius);
	SDFHelper SDFCylinder = SDFHelper(dCylinder, CylinderColor, CylinderSmoothness);

	float dTriPrism = TriPrismSDF(TransformToLocalPoint(p, TriPrismMatrix), TriPrismHeight);
	SDFHelper SDFTriPrism = SDFHelper(dTriPrism, TriPrismColor, TriPrismSmoothness);

	float blend = 0.0f;
	int closest = 0;
	int secondClosest = 1;

	// length of array and max iteration for i must be the same
	SDFHelper distances[4] = SDFHelper[](SDFSphere, SDFBox, SDFCylinder, SDFTriPrism);
	for(int i=0;i<4;++i){
		SDFHelper curr = distances[i];
		if (i != closest){
			if (i != secondClosest){
				if (curr.d < distances[closest].d){
					secondClosest = closest;
					closest = i;
				} else if (curr.d < distances[secondClosest].d){
					secondClosest = i;
				}
			}
			else {
				if (curr.d < distances[closest].d){
					secondClosest = closest;
					closest = i;
				}
			}
		}
	}
	SDFHelper a = distances[closest];
	SDFHelper b = distances[secondClosest];
	float smoothness = (a.smoothness + b.smoothness) / 2;
	float abd = SmoothUnion(a.d, b.d, smoothness, blend);
	o.color = mix(a.color, b.color, blend);
	return abd;

}


// Default value for o
void InitOutput(out Output o)
{
	o.color = vec3(0.0f);
}

// Output function: Just a dot with the normal and view vectors
vec4 GetOutputColor(vec3 p, float distance, Output o)
{
	vec3 normal = CalculateNormal(p);
	vec3 viewDir = normalize(-p);
	float dotNV = dot(normalize(-p), normal);
	return vec4(dotNV * o.color, 1.0f);
}
