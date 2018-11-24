//-----------------------------------------------------------------------------
// File: BasicHLSL.fx
//
// Desc: The effect file for the BasicHLSL sample.  It implemetns 
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
 

float m_sin(float x){
	return sin(x)*0.5+0.5;
}

float m_cos(float x){
	return cos(x)*0.5+0.5;
}


float2 m_c_mul(float2 x,float2 y){
	float2 temp;
	float2 t=x*y;
	temp.x=t.x-t.y;
	temp.y=x.x*y.y+x.y*y.x;
	return temp;
}
float2 m_c_power2(float2 x){
	float2 temp;
	temp.x=x.x*x.x-x.y*x.y;
	temp.y=2*x.x*x.y;
	return temp;
}
float2 m_c_add(float2 x,float2 y){
	return x+y;
}

float2 m_c_sin(float2 x){
	float2 temp;
	float s,c;
	sincos(x.x,s,c);
	temp.x=s*cosh(x.y);
	temp.y=c*sinh(x.y);
	return temp;
} 
float2 m_c_cos(float2 x){
	float2 temp;
	float s,c;
	sincos(x.x,s,c);
	temp.x=c*cosh(x.y) ; 
	temp.y=-s*sinh(x.y);
	return temp;
}

float2 m_c_conj(float2 x)
{

	float2 temp;
	temp.x=x.x;
	temp.y=-x.y;
	return temp;

}
float2 m_c_div(float2 x,float2 y){


	float modulo= (y.x*y.x+y.y*y.y);

	float r= x.x * y.x + x.y * y.y;
	float i= x.y * y.x - x.x * y.y;
	
	return float2(r/modulo,i/modulo);


} 