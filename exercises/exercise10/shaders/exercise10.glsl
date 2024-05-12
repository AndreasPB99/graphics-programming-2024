
// Uniforms
uniform vec3 SphereColor = vec3(0, 0, 1);
uniform vec3 SphereCenter = vec3(-2, 0, -10);
uniform float SphereRadius = 1.25f;
uniform bool SphereBlend = true;
uniform bool SphereEnabled = true;

uniform vec3 BoxColor = vec3(1, 0, 0);
uniform mat4 BoxMatrix = mat4(1,0,0,0,   0,1,0,0,   0,0,1,0,   2,0,-10,1);
uniform mat4 BoxMatrix2 = mat4(1,0,0,0,   0,1,0,0,   0,0,1,0,   2,0,-10,1);
uniform vec3 BoxSize = vec3(1, 1, 1);
uniform bool BoxBlend = true;
uniform bool BoxEnabled = true;

uniform vec3 CylinderColor = vec3(0, 1, 0);
uniform mat4 CylinderMatrix = mat4(1,0,0,0,   0,1,0,0,   0,0,1,0,   2,0,-10,1);
uniform float CylinderRadius = 1.5f;
uniform float CylinderHeight = 2.1f;
uniform bool CylinderBlend = true;
uniform bool CylinderEnabled = true;

uniform vec3 TriPrismColor = vec3(0, 1, 1);
uniform mat4 TriPrismMatrix = mat4(1,0,0,0,   0,1,0,0,   0,0,1,0,   2,0,-10,1);
uniform vec2 TriPrismHeight = vec2(1, 1);
uniform bool TriPrismBlend = true;
uniform bool TriPrismEnabled = true;

uniform float SphereSmoothness = 1.0f;
uniform float CylinderSmoothness = 1.0f;
uniform float BoxSmoothness = 1.0f;
uniform float TriPrismSmoothness = 1.0f;

struct SDFHelper
{
	float d;
	vec3 color;
	float smoothness;
	bool blend;
	bool enabled;
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
	SDFHelper SDFSphere = SDFHelper(dSphere, SphereColor, SphereSmoothness, SphereBlend, SphereEnabled);

	float dBox = BoxSDF(TransformToLocalPoint(p, BoxMatrix), BoxSize);
	SDFHelper SDFBox = SDFHelper(dBox, BoxColor, BoxSmoothness, BoxBlend, BoxEnabled);

	float dCylinder = CylinderSDF(TransformToLocalPoint(p, CylinderMatrix), CylinderHeight, CylinderRadius);
	SDFHelper SDFCylinder = SDFHelper(dCylinder, CylinderColor, CylinderSmoothness, CylinderBlend, CylinderEnabled);

	float dTriPrism = TriPrismSDF(TransformToLocalPoint(p, TriPrismMatrix), TriPrismHeight);
	SDFHelper SDFTriPrism = SDFHelper(dTriPrism, TriPrismColor, TriPrismSmoothness, TriPrismBlend, TriPrismEnabled);

	float blend = 0.0f;
	int closest = 2;
	int secondClosest = 1;

	// length of array and max iteration for i must be the same
	SDFHelper distances[4] = SDFHelper[](SDFSphere, SDFBox, SDFCylinder, SDFTriPrism);
	for(int i=0;i<4;++i){
		SDFHelper curr = distances[i];
		SDFHelper currClosest = distances[closest];
		SDFHelper currSecondClosest = distances[secondClosest];
		if (!curr.enabled){
			continue;
		}
		else if (i == closest){
			continue;
		}
		else if (i == secondClosest){
			if (!currClosest.enabled) {
				secondClosest = closest;
				closest = i;
			}
			else if (curr.d < currClosest.d){
					secondClosest = closest;
					closest = i;
			}
		}
		else if (!currClosest.enabled) {
				secondClosest = closest;
				closest = i;
			}
		else if (!currSecondClosest.enabled) {
				secondClosest = i;
		}
		else if (curr.d < currClosest.d)
		{
			secondClosest = closest;
			closest = i;
		} 
		else if (curr.d < currSecondClosest.d)
		{
			secondClosest = i;
		}
	}

	SDFHelper a = distances[closest];
	SDFHelper b = distances[secondClosest];
	float smoothness = (a.smoothness + b.smoothness) / 2;
	float abd;
	if (a.blend && b.blend)
	{
		abd = SmoothUnion(a.d, b.d, smoothness, blend);
	} else {
		abd = SmoothUnion(a.d, b.d, smoothness);
	}
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
