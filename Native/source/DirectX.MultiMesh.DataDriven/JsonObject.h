#pragma once

#include "json/json.h"
#include <cstdint>

namespace Library
{
	class JsonObject final : public RTTI
	{
		RTTI_DECLARATIONS(JsonObject, RTTI)

	public:
		JsonObject(const JsonObject&) = default;
		JsonObject& operator=(const JsonObject&) = default;
		JsonObject(JsonObject&&) = default;
		JsonObject& operator=(JsonObject&&) = default;
		~JsonObject() = default;

		const Json::Value& Root() const;

	private:
		friend class JsonObjectReader;

		JsonObject(Json::Value&& root);

		Json::Value mRoot;
	};
}
