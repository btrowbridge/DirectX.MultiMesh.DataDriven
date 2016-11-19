#pragma once

#include "Game.h"
#include "RenderStateHelper.h"

namespace Library
{
	class Camera;
	class KeyboardComponent;
	class MouseComponent;
	class GamePadComponent;
	class FpsComponent;
}

namespace MultiMesh
{
	class MultiMeshDemo final : public Library::Game
	{
	public:
		MultiMeshDemo(std::function<void*()> getWindowCallback, std::function<void(SIZE&)> getRenderTargetSizeCallback);

		virtual void Initialize() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;
		virtual void Shutdown() override;

		void Exit();

	private:		
		static const DirectX::XMVECTORF32 BackgroundColor;		

		Library::RenderStateHelper mRenderStateHelper;
		std::shared_ptr<Library::KeyboardComponent> mKeyboard;
		std::shared_ptr<Library::MouseComponent> mMouse;
		std::shared_ptr<Library::GamePadComponent> mGamePad;
		std::shared_ptr<Library::FpsComponent> mFpsComponent;
	};
}
