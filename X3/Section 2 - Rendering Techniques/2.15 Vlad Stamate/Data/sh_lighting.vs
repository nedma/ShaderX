uniform vec3 Coefficients[9];

varying vec4 Color;

void main(void)
{
	vec3 color;

 	vec3 norm = gl_Normal;
	
	color  = Coefficients[0];
	color += Coefficients[1] * norm.x;
	color += Coefficients[2] * norm.y;
	color += Coefficients[3] * norm.z;
	color += Coefficients[4] * norm.x * norm.z;
	color += Coefficients[5] * norm.y * norm.z;
	color += Coefficients[6] * norm.x * norm.y;
	color += Coefficients[7] * (3.0 * norm.z * norm.z  - 1.0);
	color += Coefficients[8] * (norm.x * norm.x  - norm.y * norm.y);

    gl_Position     = ftransform();
    
    Color = vec4(color, 1.0) * gl_Color;
}