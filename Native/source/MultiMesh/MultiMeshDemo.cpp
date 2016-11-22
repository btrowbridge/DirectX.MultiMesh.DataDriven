#include "pch.h"
#include "MultiMeshDemo.h"


using namespace std;
using namespace DirectX;
using namespace Library;

namespace MultiMesh
{
	const XMVECTORF32 MultiMeshDemo::BackgroundColor = Colors::MediumPurple;

	MultiMeshDemo::MultiMeshDemo(std::function<void*()> getWindowCallback, std::function<void(SIZE&)> getRenderTargetSizeCallback) :
		Game(getWindowCallback, getRenderTargetSizeCallback), mRenderStateHelper(*this)
	{
	}

	void MultiMeshDemo::Initialize()
	{
		SamplerStates::Initialize(mDirect3DDevice.Get());
		BlendStates::Initialize(mDirect3DDevice.Get());
		RasterizerStates::Initialize(mDirect3DDevice.Get());
		SpriteManager::Initialize(*this);
		
		auto camera = make_shared<FirstPersonCamera>(*this);
		mComponents.push_back(camera);
		mServices.AddService(Camera::TypeIdClass(), camera.get());

		
		mKeyboard = make_shared<KeyboardComponent>(*this);
		mComponents.push_back(mKeyboard);
		mServices.AddService(KeyboardComponent::TypeIdClass(), mKeyboard.get());

		mMouse = make_shared<MouseComponent>(*this, MouseModes::Absolute);
		mComponents.push_back(mMouse);
		mServices.AddService(MouseComponent::TypeIdClass(), mMouse.get());

		mGamePad = make_shared<GamePadComponent>(*this);
		mComponents.push_back(mGamePad);
		mServices.AddService(GamePadComponent::TypeIdClass(), mGamePad.get());
		
		mFpsComponent = make_shared<FpsComponent>(*this);
		mFpsComponent->Initialize();
		

		auto object = make_shared<MultiMeshModel>(*this, camera,"Content\\Models\\potion_MediumPoly.obj.bin",
				"Content\\Textures\\potion_green.png",XMFLOAT3(),XMFLOAT3(), 1.0f);

		mComponents.push_back(object);



		Game::Initialize();
		camera->SetPosition(0.0f, 0.0f, 10.0f);
	}

	void MultiMeshDemo::Update(const GameTime &gameTime)
	{
		if (mKeyboard->WasKeyPressedThisFrame(Keys::Escape) || mGamePad->WasButtonPressedThisFrame(GamePadButtons::Back))
		{
			Exit();
		}

		mFpsComponent->Update(gameTime);

		Game::Update(gameTime);
	}

	void MultiMeshDemo::Draw(const GameTime &gameTime)
	{
		mDirect3DDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), reinterpret_cast<const float*>(&BackgroundColor));
		mDirect3DDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		Game::Draw(gameTime);

		mRenderStateHelper.SaveAll();
		mFpsComponent->Draw(gameTime);
		mRenderStateHelper.RestoreAll();

		HRESULT hr = mSwapChain->Present(1, 0);

		// If the device was removed either by a disconnection or a driver upgrade, we must recreate all device resources.
		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
			HandleDeviceLost();
		}
		else
		{
			ThrowIfFailed(hr, "IDXGISwapChain::Present() failed.");
		}
	}

	void MultiMeshDemo::Shutdown()
	{
		SpriteManager::Shutdown();
		RasterizerStates::Shutdown();
		BlendStates::Shutdown();
		SamplerStates::Shutdown();

		Game::Shutdown();
	}

	void MultiMeshDemo::Exit()
	{
		PostQuitMessage(0);
	}
	
}