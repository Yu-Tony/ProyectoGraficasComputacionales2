Texture2D colorMap0 : register(t0);
Texture2D normalMap0 : register(t1);
Texture2D blendMap0 : register(t2);
Texture2D blendMap1 : register(t3);

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

cbuffer lago:register (b5) {
	float movimientoText;
	float p0;
	float p1;
	float p2;
}
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
	float3 normal : TEXCOORD3;
	float3 tangent : TEXCOORD4;
	float3 binorm : TEXCOORD5;
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


	float4 aportacionAmbiental = ambient * float4(rgbColor,1.f) * atenuadorAmbiental;


	float4 text0 = colorMap0.Sample(colorSampler, pix.tex0+movimientoText);



	float4 normal0 = normalMap0.Sample(colorSampler, pix.tex0+movimientoText);



	float3 bump = 2.0 * normal0 - 1.0;


	float3x3 TBN = { {pix.tangent}, {pix.binorm}, {pix.normal} };
	float3 newnormal = mul(TBN, normalize(bump));

	float3 vectorLuz = ubicacionLuz.xyz - pix.position.xyz;


	float4 aportacionDifusa = saturate(dot(normalize(vectorLuz), normalize(newnormal)));


	float ViewDir = normalize(ubicacionCamara - pix.position.xyz);

	float3 bumpTBN = normalize(mul(bump, TBN));
	float3 Reflect = normalize(2 * bumpTBN - normalize(vectorLuz));
	float specular = pow(saturate(dot(Reflect, ViewDir)), 4);

	float4 luzEspecular = float4(0.8f, 0.8f, 1.f, 1.f);

	float4 aporteEspecular = luzEspecular * specular;



	float4 fColor = text0 * (aportacionAmbiental + aportacionDifusa+aporteEspecular*0.05f);
	

	return fColor;
}