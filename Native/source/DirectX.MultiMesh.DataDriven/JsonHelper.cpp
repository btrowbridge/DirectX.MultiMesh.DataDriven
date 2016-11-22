#include "pch.h"
#include "JsonHelper.h"

using namespace DirectX;
using namespace Json;

namespace Library
{
	XMFLOAT2 JsonHelper::GetVector2(const Value& jsonVector2, const XMFLOAT2& defaultValue)
	{
		float x = jsonVector2.get("x", defaultValue.x).asFloat();
		float y = jsonVector2.get("y", defaultValue.y).asFloat();

		return XMFLOAT2(x, y);
	}

	XMFLOAT3 JsonHelper::GetVector3(const Value& jsonVector3, const XMFLOAT3& defaultValue)
	{
		float x = jsonVector3.get("x", defaultValue.x).asFloat();
		float y = jsonVector3.get("y", defaultValue.y).asFloat();
		float z = jsonVector3.get("z", defaultValue.z).asFloat();

		return XMFLOAT3(x, y, z);
	}
}
