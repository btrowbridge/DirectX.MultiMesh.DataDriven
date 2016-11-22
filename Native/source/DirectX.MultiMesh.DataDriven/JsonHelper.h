#pragma once

#include <DirectXMath.h>
#include <VectorHelper.h>

namespace Json
{
	class Value;
}

namespace Library
{
	class JsonHelper final
	{
	public:
		static DirectX::XMFLOAT2 GetVector2(const Json::Value& jsonVector2, const DirectX::XMFLOAT2& defaultValue = Vector2Helper::Zero);
		static DirectX::XMFLOAT3 GetVector3(const Json::Value& jsonVector3, const DirectX::XMFLOAT3& defaultValue = Vector3Helper::Zero);
	};
}

