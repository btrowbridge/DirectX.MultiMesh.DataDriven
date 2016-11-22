#pragma once

#include "ContentTypeReader.h"
#include "JsonObject.h"

namespace Library
{
	class JsonObjectReader : public ContentTypeReader<JsonObject>
	{
		RTTI_DECLARATIONS(JsonObjectReader, AbstractContentTypeReader)

	public:
		JsonObjectReader(Game& game);
		JsonObjectReader(const JsonObjectReader&) = default;
		JsonObjectReader& operator=(const JsonObjectReader&) = default;
		JsonObjectReader(JsonObjectReader&&) = default;
		JsonObjectReader& operator=(JsonObjectReader&&) = default;
		~JsonObjectReader() = default;

	protected:
		virtual std::shared_ptr<JsonObject> _Read(const std::wstring& assetName) override;
	};
}