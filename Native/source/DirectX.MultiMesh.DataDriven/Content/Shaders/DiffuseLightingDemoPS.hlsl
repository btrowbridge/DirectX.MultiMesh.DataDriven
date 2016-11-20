cbuffer CBufferPerFrame
{
	float3 AmbientColor;
	float3 LightDirection;
	float3 LightColor;
};

Texture2D ColorTexture;
SamplerState ColorSampler;

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinates : TEXCOORD;
	float3 Normal : NORMAL;
};

float4 main(VS_OUTPUT IN) : SV_TARGET
{
	float4 color = ColorTexture.Sample(ColorSampler, IN.TextureCoordinates);
	float3 ambient = color.rgb * AmbientColor;
	float n_dot_l = dot(LightDirection, IN.Normal);
	float3 diffuse = color.rgb * saturate(n_dot_l) * LightColor;

	return float4(saturate(ambient + diffuse), color.a);
}