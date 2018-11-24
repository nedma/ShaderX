
float taperf(float x,float tfact,float tcenter,float trange){
 
	float t=(tcenter-x)/trange;
	if(abs(t)>1){
		return 1;
	}else{
	return Hermite(1-tfact,1,0,0,abs(t));
	
	}
 
}
  
void VS_DoTaperX(inout float4 vPos,inout float3 vNormal,float t){

	float4 vNewPos;

	vNewPos.x=vPos.x ;
	vNewPos.y=vPos.y*t;
	vNewPos.z=vPos.z*t;
	vNewPos.w=vPos.w;

    vPos=vNewPos; 
}


void VS_DoTaperY(inout float4 vPos,inout float3 vNormal,float t){
	
	float4 vNewPos;

	vNewPos.x=vPos.x*t;
	vNewPos.y=vPos.y ;
	vNewPos.z=vPos.z*t;
	vNewPos.w=vPos.w;

    vPos=vNewPos; 
}

  
void VS_DoTaperZ(inout float4 vPos,inout float3 vNormal ,float t){
	
	float4 vNewPos;
	float4 vNewPos2;
	
	vNewPos.x=vPos.x*t;
	vNewPos.y=vPos.y*t;
	vNewPos.z=vPos.z;
	vNewPos.w=vPos.w;

/*
	float3 vcross=cross(normalize(vNewPos.xyz),normalize(vPos.xyz));
 	float  dot=-dot(normalize(vPos.xyz),normalize(vNewPos.xyz));
 
	float3x3 m=RotationAxis(vcross.x,vcross.y,vcross.z,acos(dot));

	vNormal=mul(vNormal,m);
*/

    vPos=vNewPos;  

}


  
