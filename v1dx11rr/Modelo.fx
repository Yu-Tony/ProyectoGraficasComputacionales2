Texture2D colorMap : register(t0);

Texture2D normalMap : register(t1);

Texture2D opacityMap: register(t2);

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
	float3 position: TEXCOORD4;
	float3 dirView: TEXCOORD5;
};

PS_Input VS_Main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = mul(vertex.pos, worldMatrix);
	vsOut.position = vsOut.pos;
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
	
		float4 aportacionAmbiental = ambient * float4(rgbColor,1.f)*atenuadorAmbiental;

		float3 bump = 2.0 * normal0 - 1.0;
	

		float3x3 TBN = { {pix.tangent}, {pix.binorm}, {pix.normal} };
	

		float3 vectorLuz = ubicacionLuz - pix.position.xyz;

		float3 newnormal = mul(TBN, normalize(vectorLuz));  

		float4 FALL = dot(normalize(bump), newnormal);
	
		float4 aportacionDifusa = saturate(difusa * FALL) * atenuadorDifuso;

		float ViewDir = normalize(ubicacionCamara - pix.position.xyz);

		float3 bumpTBN = normalize(mul(bump, TBN));
		float3 Reflect = normalize(2 * bumpTBN - normalize(vectorLuz));
		float specular = pow(saturate(dot(Reflect, ViewDir)), 4);

		float4 luzEspecular = float4(1.f, 1.f, 1.f, 1.f);

		float4 aporteEspecular = luzEspecular * specular;

		float4 opacity = opacityMap.Sample(colorSampler, pix.tex0);

		if (opacity.r < 0.1) {
			clip(-1);
		}

	return text0 * (aportacionAmbiental + aportacionDifusa+ aporteEspecular*0.f);
}