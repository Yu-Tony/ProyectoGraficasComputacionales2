Texture2D colorMap : register(t0);

Texture2D normalMap : register(t1);


SamplerState colorSampler : register(s0);

cbuffer cbChangerEveryFrame : register(b0)
{
	matrix worldMatrix;
};

cbuffer cbNeverChanges : register(b1)
{
	matrix viewMatrix;
};

cbuffer cbChangeOnResize : register(b2)
{
	matrix projMatrix;
};

cbuffer ControlDiaNoche : register (b3)
{
	float4 ambient;
	float4 rgbColor;
	float4 dirLuz;
};


struct VS_Input
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;

	float3 normal : NORMAL0;
	float3 tangente : NORMAL1;
	float3 binormal : NORMAL2;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;

	float3 normal : TEXCOORD1;
	float3 tangent : TEXCOORD2;
	float3 binorm : TEXCOORD3;
};

PS_Input VS_Main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = mul(vertex.pos, worldMatrix);
	vsOut.pos = mul(vsOut.pos, viewMatrix);
	vsOut.pos = mul(vsOut.pos, projMatrix);

	vsOut.tex0 = vertex.tex0;

	vsOut.normal = normalize(mul(vertex.normal, worldMatrix));

	vsOut.tangent= mul(vertex.tangente, worldMatrix);

	vsOut.binorm = normalize(cross(vsOut.tangent, vsOut.normal));

	return vsOut;
}

float4 PS_Main(PS_Input pix) : SV_TARGET
{ 
		float4 text0 = colorMap.Sample(colorSampler, pix.tex0);
		float4 normal0 = normalMap.Sample(colorSampler, pix.tex0);
	
		float4 aportacionAmbiental = ambient * rgbColor*0.6f;

		float4 DiffuseColor = float4(1.0f, 1.0f, 1.f, 1.0f);
	
	
		float3 bump = 2.0 * normal0 - 1.0;
		bump = normalize(bump);

		float3x3 TBN = { {pix.tangent}, {pix.binorm}, {pix.normal} };
	
		float3 newnormal = mul(TBN, normalize(dirLuz));
		float4 FALL = dot(bump, normalize(newnormal));
	
		float4 aportacionDifusa = saturate(DiffuseColor * FALL*0.6f);

	
	


	return text0 * (aportacionAmbiental + aportacionDifusa);
}