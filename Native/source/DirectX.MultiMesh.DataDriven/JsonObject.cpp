#include "pch.h"
#include "JsonObject.h"

using namespace std;

namespace Library
{
	RTTI_DEFINITIONS(JsonObject)

	JsonObject::JsonObject(Json::Value&& root) :
		mRoot(root)
	{
	}

	const Json::Value & JsonObject::Root() const
	{
		return mRoot;
	}	
}