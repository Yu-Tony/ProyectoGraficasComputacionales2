Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
Texture2D texture2 : register(t2);



SamplerState colorSampler : register(s0);

cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projMatrix;
	float4 valores;
};
cbuffer ControlDiaNoche : register (b1)
{
	float mT;
	float tN;
	float padding0;
	float padding1;
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
	float4 text0= texture0.Sample(colorSampler, pix.tex0);
	float4 text1= texture1.Sample(colorSampler, pix.tex0);
	float4 text2= texture2.Sample(colorSampler, pix.tex0);


	float4 finalColor = lerp(text0,text1, mT);
	finalColor = lerp(finalColor, text2, tN);



	return finalColor;
}