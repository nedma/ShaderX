matrix matWorld;
matrix matViewProj;
float4 vec4LightDir;
float4 vec4Diffuse;

float fSquishFactor;
float fAnimTime;
float fHandsRot;

float aKeys[] =
{
	0.000000f,
	0.000000f,
	0.000000f,
	0.000000f,
	0.000000f,
	0.045616f,
	0.319601f,
	0.624319f,
	0.777266f,
	0.867588f,
	0.925081f,
	0.961976f,
	0.984535f,
	0.996425f,
	1.000000f,
};	// Half the fcurve

float aHandsKeys[] =
{
	0.000000f,
	0.045616f,
	0.319601f,
	0.624319f,
	0.777266f,
	0.867588f,
	0.925081f,
	0.961976f,
	0.984535f,
	0.996425f,
	1.000000f,
};	// Half the fcurve

struct VS_INPUT
{
	float3 vPos : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 vPos : POSITION;
	float4 vDiffuse : COLOR0;
	float2 vTexCoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT Input)
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float4 vec4WorldPos = mul(float4(Input.vPos,1),matWorld);

	float fSquishMask = (vec4WorldPos.y < 0.0f)?1.0f:0.0f;
	float fSquishVal = (vec4WorldPos.y < 0.0f)?1.0f-vec4WorldPos.y*fSquishFactor:1.0f;
	vec4WorldPos.x *= fSquishVal;
	vec4WorldPos.y *= 1.0f-fSquishMask;
	vec4WorldPos.z *= fSquishVal;

	Out.vPos = mul(vec4WorldPos,matViewProj);

	float3 vec3WorldNormal = mul(Input.vNormal,matWorld);
	fSquishVal *= fSquishMask;
	float fBackValX = (abs(vec3WorldNormal.x)<fSquishVal)?vec3WorldNormal.x:fSquishVal;
	float fBackValZ = (abs(vec3WorldNormal.z)<fSquishVal)?vec3WorldNormal.z:fSquishVal;
	
	float fFlipNormal = (vec3WorldNormal.y < 0.0f)?1.0f:0.0f;
	vec3WorldNormal.x -= fBackValX;
	vec3WorldNormal.y -= 2.0f*fSquishMask*vec3WorldNormal.y*fFlipNormal;
	vec3WorldNormal.z -= fBackValZ;
	vec3WorldNormal = normalize(vec3WorldNormal);
	Out.vDiffuse = vec4Diffuse*dot(vec3WorldNormal,(float3)-vec4LightDir);
	Out.vDiffuse += float4(.3,.3,.3,0);
	Out.vTexCoord = Input.vTexCoord;
	return Out;
}

technique tecSquish
{
	pass passSquish
	{
		AddressU[0] = Clamp;
		AddressV[0] = Clamp;
		vertexshader = compile vs_1_1 main();
	}
}