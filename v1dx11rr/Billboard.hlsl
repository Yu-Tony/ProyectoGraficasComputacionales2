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

cbuffer luzAmbiental : register (b3)
{
	float4 ambient;
	float3 rgbColor;
	float atenuadorAmbiental;

};


struct VS_Input
{
	float4 pos : POSITION;
	float2 tex0 : TEXCOORD0;
};

struct PS_Input
{
	float4 pos : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};

PS_Input VS_Main(VS_Input vertex)
{
	PS_Input vsOut = (PS_Input)0;
	vsOut.pos = mul(vertex.pos, worldMatrix);
	vsOut.pos = mul(vsOut.pos, viewMatrix);
	vsOut.pos = mul(vsOut.pos, projMatrix);

	vsOut.tex0 = vertex.tex0;



	return vsOut;
}

float4 PS_Main(PS_Input pix) : SV_TARGET
{


	float4 text = colorMap.Sample(colorSampler, pix.tex0);

	if (text.a < 0.50) {
		clip(-1);
	}

	float4 aportacionAmbiental = ambient*atenuadorAmbiental * float4(rgbColor,1.f);

	return text*(aportacionAmbiental);
}