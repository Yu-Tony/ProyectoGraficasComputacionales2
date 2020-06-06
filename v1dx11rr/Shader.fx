Texture2D colorMap0 : register(t0);
Texture2D colorMap1 : register(t1);
Texture2D colorMap2 : register(t2);
Texture2D normalMap0 : register(t3);
Texture2D normalMap1 : register(t4);
Texture2D normalMap2 : register(t5);
Texture2D blendMap0 : register(t6);
Texture2D blendMap1 : register(t7);

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

cbuffer luzAmbiental : register (b3)
{
	float4 ambient;
	float3 rgbColor;
	float atenuadorAmbiental;
	
};

cbuffer luzDifusa : register (b4)
{
	float4 difusa;
	float4 ubicacionLuz;
	float3 ubicacionCamara;
	float atenuadorDifuso;

};

struct VS_Input
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
	float2 blendTex : TEXCOORD1;
	float3 normal : NORMAL0;
	float3 tangente : NORMAL1;
	float3 binormal : NORMAL2;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
	float2 blendTex : TEXCOORD1;
	float4 position: TEXCOORD2;
	float3 normal : NORMAL0;
	float3 tangent : NORMAL1;
	float3 binorm : NORMAL2;
};

PS_Input VS_Main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = mul(vertex.pos, worldMatrix);
	vsOut.position = vsOut.pos;
	vsOut.pos = mul(vsOut.pos, viewMatrix);
	vsOut.pos = mul(vsOut.pos, projMatrix);

	vsOut.tex0 = vertex.tex0;
	vsOut.blendTex = vertex.blendTex;
	vsOut.normal = normalize(mul(vertex.normal, worldMatrix));
	vsOut.tangent = normalize(mul(vertex.tangente, worldMatrix));
	vsOut.binorm = normalize(mul(vertex.binormal, worldMatrix));

	return vsOut;
}

float4 PS_Main(PS_Input pix) : SV_TARGET
{
	float4 fColor = float4(1,0,0,1);

	float4 aportacionAmbiental = ambient * float4(rgbColor,1.f) * atenuadorAmbiental;


	float4 text0 = colorMap0.Sample(colorSampler, pix.tex0);
	float4 text1 = colorMap1.Sample(colorSampler, pix.tex0);
	float4 text2 = colorMap2.Sample(colorSampler, pix.tex0);


	float4 normal0 = normalMap0.Sample(colorSampler, pix.tex0);
	float4 normal1 = normalMap1.Sample(colorSampler, pix.tex0);
	float4 normal2 = normalMap2.Sample(colorSampler, pix.tex0);


	float4 Blend = blendMap1.Sample(colorSampler, pix.blendTex);

	
	
	float4 textf;

	textf = text0;

	textf = text2 * Blend.y + (1.0 - Blend.y) * textf;

	textf = text1 * Blend.x + (1.0 - Blend.x) * textf;

	float4 normalf;

	normalf = normal0;

	normalf = normal2 * Blend.y + (1.0 - Blend.y) * normalf;

	normalf = normal1 * Blend.x + (1.0 - Blend.x) * normalf;



	float3 bump = 2.0 * normalf - 1.0;


	float3x3 TBN = { {pix.tangent}, {pix.binorm}, {pix.normal} };
	float3 newnormal = mul(TBN, normalize(bump));

	float3 vectorLuz = ubicacionLuz.xyz - pix.position.xyz;
	

	float4 aportacionDifusa = saturate(dot(normalize(vectorLuz), normalize(newnormal))) *atenuadorDifuso;

	
	

	fColor = float4(textf.rgb, 1.0f)* (aportacionAmbiental+aportacionDifusa);

	return fColor;
}