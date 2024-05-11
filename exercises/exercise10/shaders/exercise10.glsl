
// Uniforms
// TODO 10.1 : Replace constants with uniforms with the same name
uniform vec3 SphereColor = vec3(0, 0, 1);
uniform vec3 SphereCenter = vec3(-2, 0, -10);
uniform float SphereRadius = 1.25f;

uniform vec3 BoxColor = vec3(1, 0, 0);
uniform mat4 BoxMatrix = mat4(1,0,0,0,   0,1,0,0,   0,0,1,0,   2,0,-10,1);
uniform vec3 BoxSize = vec3(1, 1, 1);

uniform vec3 CylinderColor = vec3(0, 0, 1);
uniform mat4 CylinderMatrix = mat4(1,0,0,0,   0,1,0,0,   0,0,1,0,   2,0,-10,1);
uniform float CylinderRadius = 1.5f;
uniform float CylinderHeight = 2.1f;


uniform float Smoothness = 1.0f;

struct SDFHelper
{
	float d;
	vec3 color;
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
	// Sphere in position "SphereCenter" and size "SphereRadius"
	float dSphere = SphereSDF(TransformToLocalPoint(p, SphereCenter), SphereRadius);

	// Box with worldView transform "BoxMatrix" and dimensions "BoxSize"
	float dBox = BoxSDF(TransformToLocalPoint(p, BoxMatrix), BoxSize);

	float dCylinder = CylinderSDF(TransformToLocalPoint(p, CylinderMatrix), CylinderHeight, CylinderRadius);

	// TODO 10.2 : Replace Union with SmoothUnion and try different small values of smoothness
	float ablend = 0.0f;
	float bblend = 0.0f;
	float cblend = 0.0f;
	

	float a = SmoothUnion(dSphere, dBox, Smoothness, ablend);
	float b = SmoothUnion(dCylinder, dBox, Smoothness, bblend);
	float c = SmoothUnion(dCylinder, dSphere, Smoothness, cblend);


	vec3 acolor = mix(SphereColor, BoxColor, ablend);
	vec3 bcolor = mix(CylinderColor, BoxColor, bblend);
	vec3 ccolor = mix(CylinderColor, SphereColor, cblend);


	SDFHelper distances[3] = SDFHelper[](SDFHelper(a, acolor), SDFHelper(b, bcolor), SDFHelper(c, ccolor));
	int index = 0;
	float mind = distances[index].d;
	for(int i=0;i<3;++i)
	{
		if(distances[i].d < mind){
			index = i;
			mind = distances[i].d;
		}
	}

	SDFHelper res = distances[index];
	o.color = res.color;
	return res.d;
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
