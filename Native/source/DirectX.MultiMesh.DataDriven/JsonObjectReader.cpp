#include "pch.h"
#include "JsonObjectReader.h"

using namespace std;

namespace Library
{
	RTTI_DEFINITIONS(JsonObjectReader)

	JsonObjectReader::JsonObjectReader(Game& game) :
		ContentTypeReader(game, JsonObject::TypeIdClass())
	{
	}

	shared_ptr<JsonObject> JsonObjectReader::_Read(const wstring& assetName)
	{
		Json::Value root;
		ifstream jsonStream(assetName, ifstream::binary);		
		jsonStream >> root;		
		
		return shared_ptr<JsonObject>(new JsonObject(move(root)));
	}
}