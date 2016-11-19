#pragma once

#include "DrawableGameComponent.h"
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <Box2D\Box2D.h>

namespace Library
{
	class Box2DComponent;
	class Box2DDebugDraw;
	class KeyboardComponent;
	class MouseComponent;
	class VertexShader;
	class PixelShader;
	class Sprite;
	class Box2DSprite;
	class Texture2D;
}

namespace DirectX
{
	class SpriteFont;
}

namespace AngryBirds
{
	class PhysicsDemo final : public Library::DrawableGameComponent
	{
	public:
		PhysicsDemo(Library::Game& game, const std::shared_ptr<Library::Camera>& camera);

		virtual void Initialize() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;

	private:
		class ContactListener final : public b2ContactListener
		{
		public:
			ContactListener(Library::Box2DComponent* physicsEngine);

			const b2Fixture* FloorFixture() const;
			void SetFloorFixture(b2Fixture* fixture);
			void SetSpriteDestroyedCallback(std::function<void(Library::Box2DSprite*)> callback);

			virtual void EndContact(b2Contact* contact) override;			

		private:
			Library::Box2DComponent* mPhysicsEngine;
			b2Fixture* mFloorFixture;
			std::function<void(Library::Box2DSprite*)> mSpriteDestroyedCallback;
		};

		class QueryCallback final : public b2QueryCallback
		{
		public:
			QueryCallback(const b2Vec2& point);

			virtual bool ReportFixture(b2Fixture* fixture) override;

			b2Vec2 Point;
			b2Fixture* Fixture;
		};

		class DestructionListener final : public b2DestructionListener
		{
		public:
			DestructionListener();

			void SetMouseJoint(b2MouseJoint* mouseJoint);
			void SetMouseJointDestroyedCallback(std::function<void()> callback);

			virtual void SayGoodbye(b2Joint* joint) override;
			virtual void SayGoodbye(b2Fixture* fixture) { UNREFERENCED_PARAMETER(fixture); }

		private:
			b2MouseJoint* mMouseJoint;
			std::function<void()> mMouseJointDestroyedCallback;
		};

		enum class ObjectTypes
		{
			Box,
			Circle,
			Triangle,
			Bolas,
			Stick,
			End
		};

		static const DirectX::XMFLOAT2 TextPosition;
		static const std::wstring HelpText;
		static const DirectX::XMVECTORF32 BodySpawnPosition;
		static const std::map<ObjectTypes, std::wstring> SpawnObjectNames;

		void AddGround();
		void AddEdge();
		void AddChain();
		void SpawnObject(ObjectTypes type, DirectX::FXMVECTOR position);
		void SpawnBox(DirectX::FXMVECTOR position);
		void SpawnCircle(DirectX::FXMVECTOR position);
		void SpawnTriangle(DirectX::FXMVECTOR position);
		void SpawnBolas(DirectX::FXMVECTOR position);
		void SpawnStick(DirectX::FXMVECTOR position);
		void ResetWorld();
		void SpawnObjectWithMouse();
		void IncrementMouseSpawnObject();
		void CreateMouseJoint();
		void ApplyForceWithMouse();
		DirectX::XMVECTOR GetMouseWorldPosition() const;

		Library::Box2DComponent* mPhysicsEngine;
		Library::Box2DDebugDraw* mPhysicsDebugDraw;
		std::unique_ptr<ContactListener> mContactListener;
		Library::KeyboardComponent* mKeyboard;
		Library::MouseComponent* mMouse;
		std::shared_ptr<DirectX::SpriteFont> mHelpFont;		
		std::uint32_t mShapeCount;
		b2Body* mGroundBody;
		ObjectTypes mMouseSpawnObject;
		std::map<ObjectTypes, std::function<void(DirectX::FXMVECTOR position)>> mSpawnMethods;
		std::map<Library::Keys, std::function<void()>> mKeyMappings;
		b2MouseJoint* mMouseJoint;
		DestructionListener mDestructionListener;

		std::vector<std::shared_ptr<Library::Sprite>> mSprites;
		std::shared_ptr<Library::Texture2D> mBoxTexture;
		std::shared_ptr<Library::Texture2D> mCatYellowTexture;
		std::shared_ptr<Library::Texture2D> mDogTexture;
		std::shared_ptr<Library::Texture2D> mStickTexture;
		std::shared_ptr<Library::Texture2D> mTriangleTexture;
		std::shared_ptr<Library::Texture2D> mGroundTexture;
		std::shared_ptr<Library::Texture2D> mFloorTexture;
	};
}