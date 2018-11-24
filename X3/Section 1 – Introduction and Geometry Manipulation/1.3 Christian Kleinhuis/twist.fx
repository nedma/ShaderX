
 float twistf(float x,float fact){
    // this is the twist mapping formula 
	return (x)*fact;
	

}
void VS_DoTwistY(inout float4 vPos,inout float3 vNormal,float t){

	// Constants
	float st=sin(t);
	float ct=cos(t);

	float4 vNewPos;
	float3 vNewNorm;
	
	vNewPos.x=vPos.x*ct-vPos.z*st;
	vNewPos.z=vPos.x*st+vPos.z*ct;
	vNewPos.y=vPos.y;
	vNewPos.w=vPos.w;

	// The Normal is transformed the same way 
	vNewNorm.x=vNormal.x*ct-vNormal.z*st;
	vNewNorm.z=vNormal.x*st+vNormal.z*ct;
	vNewNorm.y=vNormal.y; 

	vNormal=vNewNorm;
	vPos=vNewPos;
 
} 

void VS_DoTwistX(inout float4 vPos,inout float3 vNormal,float t){

	// Constants
	float st=sin(t);
	float ct=cos(t);

	float4 vNewPos;
	float3 vNewNorm;
	
	vNewPos.y=vPos.y*ct-vPos.z*st;
	vNewPos.z=vPos.y*st+vPos.z*ct;
	vNewPos.x=vPos.x;
	vNewPos.w=vPos.w;

	// The Normal is transformed the same way 
	vNewNorm.y=vNormal.y*ct-vNormal.z*st;
	vNewNorm.z=vNormal.y*st+vNormal.z*ct;
	vNewNorm.x=vNormal.x; 

	vNormal=vNewNorm;
	vPos=vNewPos;
 
} 

void VS_DoTwistZ(inout float4 vPos,inout float3 vNormal,float t){

	// Constants
	float st=sin(t);
	float ct=cos(t);

	float4 vNewPos;
	float3 vNewNorm;
	
	vNewPos.x=vPos.x*ct-vPos.y*st;
	vNewPos.y=vPos.x*st+vPos.y*ct;
	vNewPos.z=vPos.z;
	vNewPos.w=vPos.w;

	// The Normal is transformed the same way 
	vNewNorm.x=vNormal.x*ct-vNormal.y*st;
	vNewNorm.y=vNormal.x*st+vNormal.y*ct;
	vNewNorm.z=vNormal.z; 

	vNormal=vNewNorm;
	vPos=vNewPos;
 
} 
