#include "pch.h"
#include "Ball.h"
#include "SpriteBatch.h"
#include "PongGame.h"

using namespace Library;
using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

namespace Pong {
	const int Ball::mMinSpeed = 100;
	const int Ball::mMaxSpeed = 300;
	const float Ball::mSpeedUp = 1.1f;

	Pong::Ball::Ball(Library::Game & game, PlayScreen* screen) :
		DrawableGameComponent(game), mBounds(Rectangle::Empty), mScreen(screen)
	{
	}

	void Pong::Ball::Initialize()
	{
		// Load a texture
		ComPtr<ID3D11Resource> textureResource;
		wstring textureName = L"Content\\Textures\\Ball.png";

		ThrowIfFailed(CreateWICTextureFromFile(mGame->Direct3DDevice(), textureName.c_str(),
			textureResource.ReleaseAndGetAddressOf(), mTexture.ReleaseAndGetAddressOf()),
			"CreateWICTextureFromFile() Failed.");

		ComPtr<ID3D11Texture2D> texture;
		ThrowIfFailed(textureResource.As(&texture), "Invalid ID3D11 texture resource");

		mBounds = TextureHelper::GetTextureBounds(texture.Get());

		//Set ball defaults and random velocity
		ResetBall();

		mSpriteBatch = make_unique<SpriteBatch>(mGame->Direct3DDeviceContext());

		//Sounds
		mAudioEngineComponent = reinterpret_cast<Library::AudioEngineComponent*>(
			mGame->Services().GetService(Library::AudioEngineComponent::TypeIdClass()));

		assert(mAudioEngineComponent != nullptr);

		AudioEngine* mAudioEngine = mAudioEngineComponent->AudioEngine().get();

		mSoundEffectPing = make_unique<SoundEffect>(mAudioEngine, L"Content\\Audio\\Ping.wav");

		mSoundEffectPong = make_unique<SoundEffect>(mAudioEngine, L"Content\\Audio\\Pong.wav");

		mSoundEffectWall = make_unique<SoundEffect>(mAudioEngine, L"Content\\Audio\\Wall.wav");

		mSoundEffectScore = make_unique<SoundEffect>(mAudioEngine, L"Content\\Audio\\Score.wav");

		//Grab scene objects
		mPlayer1 = mScreen->getPlayer1();
		mPlayer2 = mScreen->getPlayer2();
		mScoreBoard = mScreen->getScoreBoard();

		DrawableGameComponent::Initialize();
	}

	void Pong::Ball::Update(const Library::GameTime & gameTime)
	{
		//While in play mode...
		if (mScreen->getGameState() == GameState::Play) {
			float elapsedTime = gameTime.ElapsedGameTimeSeconds().count();

			auto& mViewport = mGame->Viewport();

			//Move ball according to velocity
			XMFLOAT2 positionDelta(mVelocity.x * elapsedTime, mVelocity.y * elapsedTime);
			mBounds.X += static_cast<int>(std::round(positionDelta.x));
			mBounds.Y += static_cast<int>(std::round(positionDelta.y));

			//Right Wall
			if (mBounds.Right() > mViewport.Width && mVelocity.x > 0.0f)
			{
				mScoreBoard->Player1Scores();
				mSoundEffectScore->Play();
				ResetBall();
				return;
			}//Left Wall
			else if (mBounds.Left() < 0 && mVelocity.x < 0.0f)
			{
				mScoreBoard->Player2Scores();
				mSoundEffectScore->Play();
				ResetBall();
				return;
			}

			//Bottom Wall
			if (mBounds.Bottom() > mViewport.Height && mVelocity.y > 0.0f)
			{
				mVelocity.y *= -1;
				mSoundEffectWall->Play();
			}//Top Wall
			else if (mBounds.Top() < 0 && mVelocity.y < 0.0f)
			{
				mVelocity.y *= -1;
				mSoundEffectWall->Play();
			}

			//Intersecting Player 1...
			if (mBounds.Intersects(mPlayer1->Bounds()))
			{
				mSoundEffectPing->Play();
				//Bounce off the bottom or top
				if ((abs(mBounds.Left() - mPlayer1->Bounds().Right()) >
					abs(mBounds.Bottom() - mPlayer1->Bounds().Top())) ||
					(abs(mBounds.Left() - mPlayer1->Bounds().Right()) >
						abs(mBounds.Top() - mPlayer1->Bounds().Bottom())))
				{
					//Avoid unnecessary collision
					if (mBounds.Center().Y - mPlayer1->Bounds().Center().Y < 0) {
						mVelocity.y = abs(mVelocity.y) * -1;
						mBounds.Y = mPlayer1->Bounds().Top() - mBounds.Height; //put above
					}
					else {
						mVelocity.y = abs(mVelocity.y);
						mBounds.Y = mPlayer1->Bounds().Bottom(); //put below
					}
				}
				else {
					//Bouncing left or right
					mVelocity.x *= -1;

					//Avoid unecessary colision
					if (mBounds.Center().X > mPlayer1->Bounds().Center().X) {
						mBounds.X = mPlayer1->Bounds().Right();
					}
					else {
						mBounds.X = mPlayer1->Bounds().Left() - mBounds.Width;
					}
				}

				//Transfer paddle momentum if paddle is moving
				if (mPlayer1->Velocity().y != 0)
				{
					mVelocity.y = (mPlayer1->Velocity().y + mVelocity.y) / 2; //transfer some paddle momentum
				}

				mVelocity.x *= mSpeedUp; //speed up ball a little to progress game
			}
			else if (mBounds.Intersects(mPlayer2->Bounds())) //Intersecting Player 2
			{
				mSoundEffectPong->Play();
				//Bounce off the bottom or top>
				if ((abs(mBounds.Right() - mPlayer2->Bounds().Left()) >
					abs(mBounds.Bottom() - mPlayer2->Bounds().Top())) ||
					(abs(mBounds.Right() - mPlayer2->Bounds().Left()) >
						abs(mBounds.Top() - mPlayer2->Bounds().Bottom())))
				{
					//Avoid Unnecessary Collision
					if (mBounds.Center().Y - mPlayer2->Bounds().Center().Y < 0) {
						mVelocity.y = abs(mVelocity.y) * -1;
						mBounds.Y = mPlayer2->Bounds().Top() - mBounds.Height; //put above
					}
					else {
						mVelocity.y = abs(mVelocity.y);
						mBounds.Y = mPlayer2->Bounds().Bottom(); //put below
					}
				}
				else {
					//Bounce left or right
					mVelocity.x *= -1;
					//Avoid unnecessary colllision
					if (mBounds.Center().X > mPlayer2->Bounds().Center().X) {
						mBounds.X = mPlayer2->Bounds().Right();
					}
					else {
						mBounds.X = mPlayer2->Bounds().Left() - mBounds.Width;
					}
				}
				//If paddle is moving, transfer momentum
				if (mPlayer2->Velocity().y != 0)
				{
					mVelocity.y = (mPlayer2->Velocity().y + mVelocity.y) / 2; //transfer some paddle momentum
				}
				//periodically speed up the ball
				mVelocity.x *= mSpeedUp;
			}
		}
		DrawableGameComponent::Update(gameTime);
	}

	void Pong::Ball::Draw(const Library::GameTime & gameTime)
	{
		XMFLOAT2 position(static_cast<float>(mBounds.X), static_cast<float>(mBounds.Y));

		mSpriteBatch->Begin();
		mSpriteBatch->Draw(mTexture.Get(), position);
		mSpriteBatch->End();

		DrawableGameComponent::Draw(gameTime);
	}
	//Set default ball velocity and position
	void Pong::Ball::ResetBall()
	{
		Point mTextureHalfSize;
		mTextureHalfSize.X = mBounds.Width / 2;
		mTextureHalfSize.Y = mBounds.Height / 2;

		auto& mViewport = mGame->Viewport();

		Library::Rectangle viewportSize(static_cast<int>(mViewport.TopLeftX), static_cast<int>(mViewport.TopLeftY),
			static_cast<int>(mViewport.Width), static_cast<int>(mViewport.Height));
		Point center = viewportSize.Center();

		mBounds.X = center.X - mTextureHalfSize.X;
		mBounds.Y = center.Y - mTextureHalfSize.Y;

		mVelocity = RandomVelocity();
	}

	const Library::Point Pong::Ball::Position() const
	{
		return mBounds.Center();
	}

	const DirectX::XMFLOAT2 Pong::Ball::Velocity() const
	{
		return mVelocity;
	}

	DirectX::XMFLOAT2 Ball::RandomVelocity()
	{
		int range = mMaxSpeed - mMinSpeed;
		int xmag = rand() % range + mMinSpeed;
		int ymag = rand() % range + mMinSpeed;

		int xdir = (rand() % 2 == 1) ? 1 : -1;
		int ydir = (rand() % 2 == 1) ? 1 : -1;
		float dx = static_cast<float>(xmag * xdir);
		float dy = static_cast<float>(ymag * ydir);
		return XMFLOAT2(dx, dy);
	}
}