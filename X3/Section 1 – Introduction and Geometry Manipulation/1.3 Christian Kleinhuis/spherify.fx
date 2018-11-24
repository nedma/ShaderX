
// This is the method which deforms a single point.
float3 SpherifyDeformer( float3 p,float percent) 
{
	float x, y, z;
	float xw,yw,zw,vdist,mfac;
	float dx, dy, dz;
	
    float cx,cy,cz;
	cx=0;
	cy=0;
	cz=0;
	float size=1; 

	// Spherify the point
	x = p.x; y = p.y; z = p.z;
	xw= x-cx; yw= y-cy; zw= z-cz;
	if(xw==0.0 && yw==0.0 && zw==0.0)xw=yw=zw=1.0f;

	vdist=(float) sqrt(xw*xw+yw*yw+zw*zw);

	mfac=size/vdist;
	dx = xw+sign(xw)*((float) (abs(xw*mfac)-abs(xw))*percent);
	dy = yw+sign(yw)*((float) (abs(yw*mfac)-abs(yw))*percent);
	dz = zw+sign(zw)*((float) (abs(zw*mfac)-abs(zw))*percent);

	// Add The Global Center
	x=dx+cx; y=dy+cy; z=dz+cz;


	p.x = x; p.y = y; p.z = z;

	return p;
}



float4 VS_DoSpherifyNormal(float4 vPos,float3 Norm,float t){
 
	float3 vnPos=SpherifyDeformer(float3(vPos.x,vPos.y,vPos.z),1 );
	vnPos=normalize(vnPos);
	Norm=normalize(Norm);
	
	float dot=dot(Norm,vnPos);
	float3 tcross=cross(float3(vnPos.x,vnPos.y,vnPos.z),Norm);

	tcross=normalize(tcross);	
    float3x3 m=RotationAxis(tcross.x,tcross.y,tcross.z,acos(dot)*t);

	return float4(mul(Norm,m),0);
}

void  VS_DoSpherify(inout float4 vPos,inout float3 vNormal,float t){
 
	float3 vNewPos=SpherifyDeformer(float3(vPos.x,vPos.y,vPos.z),t);
	vPos=float4(vNewPos,vPos.w);
	vNormal=VS_DoSpherifyNormal(vPos,vNormal,t);

}