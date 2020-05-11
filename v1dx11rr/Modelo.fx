Texture2D colorMap : register(t0);

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

struct VS_Input
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float3 normal : NORMAL0;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float3 normal : TEXCOORD1;
};

PS_Input VS_Main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = mul(vertex.pos, worldMatrix);
	vsOut.pos = mul(vsOut.pos, viewMatrix);
	vsOut.pos = mul(vsOut.pos, projMatrix);

	vsOut.tex0 = vertex.tex0;
	vsOut.normal = normalize(mul(vertex.normal, worldMatrix));

	return vsOut;
}

float4 PS_Main(PS_Input pix) : SV_TARGET
{
	float4 fColor = float4(1,0,0,1);

	float3 ambient = float3(0.4f, 0.4f, 0.4f);

	float4 text = colorMap.Sample(colorSampler, pix.tex0);

	float3 DiffuseDirection = float3(0.0f, -1.0f, 0.2f);
	float4 DiffuseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float3 normal2 = normalize(pix.normal);
	float3 diffuse = dot(-DiffuseDirection, normal2);
	diffuse = saturate(diffuse * DiffuseColor.rgb);
	diffuse = saturate(diffuse + ambient);

	fColor = float4(text.rgb * diffuse, 1.0f);

	return fColor;
}