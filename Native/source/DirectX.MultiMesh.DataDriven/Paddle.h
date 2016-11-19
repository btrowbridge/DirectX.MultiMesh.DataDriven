#pragma once
#include "DrawableGameComponent.h"
#include <d3d11_2.h>
#include "Rectangle.h"
#include <DirectXMath.h>
#include <wrl.h>

namespace Pong
{
	class PlayScreen;

	enum PlayerOptions;

	class Paddle final : public Library::DrawableGameComponent
	{
	public:
		Paddle(Library::Game& game, PlayScreen* screen, PlayerOptions playerOption);

		virtual void Initialize() override;
		virtual void Update(const Library::GameTime& gameTime) override;
		virtual void Draw(const Library::GameTime& gameTime) override;

		virtual void ResetPaddle();

		const Library::Rectangle Bounds() const;
		const DirectX::XMFLOAT2 Velocity() const;

	private:
		Library::Keys mUpKey;
		Library::Keys mDownKey;
		PlayerOptions mPlayerOption;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTexture;
		std::unique_ptr<DirectX::SpriteBatch> mSpriteBatch;

		Library::Point mTextureHalfSize;
		Library::Point mDefaultPosition;
		Library::Rectangle mBounds;

		Library::KeyboardComponent * mKeyboard;
		static const int mSpeed;
		static const int mAISpeedMultiplier;
		DirectX::XMFLOAT2 mVelocity;

		bool isAIEnabled;
		Ball * mBall;

		PlayScreen* mScreen;
	};
}
