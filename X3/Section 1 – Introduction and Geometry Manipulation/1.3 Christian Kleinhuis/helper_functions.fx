
float Hermite(float y1,float y2, float k, float k2,float t)
{	
 
	float a = 2.0f * y1 - 2.0f * y2 + k + k2;
	float b = -3.0f * y1 + 3.0f * y2 - 2.0f * k - k2;
    float c = k;
	float d = y1;
	//	cout << a <<"x^3" << b<< "x^3" << c <<"x" << d<< endl;
	return a * ( t * t * t ) + b * ( t * t ) + c * t + d;
}


float3x3 RotationAxis(float u,float v,float w,float angle)
{
	// This functions returns a rotation matrix around given axis u/v/w

	float3x3 coor;
	float rcos = cos(angle);
	float rsin = sin(angle);

	coor[0][0] =      rcos + u*u*(1-rcos);
	coor[1][0] =  w * rsin + v*u*(1-rcos);
	coor[2][0] = -v * rsin + w*u*(1-rcos);


	coor[0][1] = -w * rsin + u*v*(1-rcos);
	coor[1][1] =      rcos + v*v*(1-rcos);
	coor[2][1] =  u * rsin + w*v*(1-rcos);


	coor[0][2] =  v * rsin + u*w*(1-rcos);
	coor[1][2] = -u * rsin + v*w*(1-rcos);
	coor[2][2] =      rcos + w*w*(1-rcos);

	return coor;

}