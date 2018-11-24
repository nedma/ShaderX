uniform vec3 LightAmbient;
uniform vec3 LightColor;
uniform vec3 LightPosition;

uniform vec3 CameraPosition;

const float SpecularContribution = 0.3;
const float DiffuseContribution  = 1.0 - SpecularContribution;

varying vec4 Color;

void main(void)
{

	float LightIntensity;

    vec3 tnorm      = normalize(gl_NormalMatrix * gl_Normal);
	vec3 viewVec	= normalize(CameraPosition - vec3(gl_Vertex));
	vec3 lightVec	= normalize(LightPosition - vec3(gl_Vertex));
	vec3 halfVec	= normalize(viewVec + lightVec);

    float diffuse   = max(dot(lightVec, tnorm), 0.0);
    float spec      = 0.0;

    if (diffuse > 0.0)
    {
        spec = max(dot(halfVec, tnorm), 0.0);
        spec = pow(spec, 32.0);
    }

    LightIntensity  = DiffuseContribution * diffuse +
                      SpecularContribution * spec;

    gl_Position     = ftransform();
	Color			= gl_Color * vec4((LightIntensity * LightColor + LightAmbient), 1.0); 

}