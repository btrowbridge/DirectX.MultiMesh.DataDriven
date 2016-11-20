cbuffer CBufferPerFrame
{
	float3 CameraPosition;
	float3 LightPosition;
	float LightRadius;
};


cbuffer CBufferPerObject
{
	float4x4 WorldViewProjection;
	float4x4 World;
};


struct VS_INPUT
{
	float4 ObjectPosition : POSITION;
	float2 TextureCoordinates : TEXCOORD;
	float3 Normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float2 TextureCoordinates : TEXCOORD;
	float3 Normal : NORMAL;
	float3 ViewDirection : VIEWDIR;
	float3 LightDirection : LIGHTDIR;
	float Attenuation : ATTENUATION;
};

VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT OUT = (VS_OUTPUT) 0;
	
	OUT.Position = mul(IN.ObjectPosition, WorldViewProjection);
	OUT.TextureCoordinates = IN.TextureCoordinates;
	OUT.Normal = mul(float4(IN.Normal, 0), World);
	
	float3 worldPosition = mul(IN.ObjectPosition, World).xyz;
	OUT.ViewDirection = normalize(CameraPosition - worldPosition);
	float3 lightDirection = LightPosition - worldPosition;
	OUT.LightDirection = normalize(lightDirection);
	OUT.Attenuation = saturate(1.0f - length(lightDirection) / LightRadius);

	return OUT;
}