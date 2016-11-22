#include "pch.h"
#include "DataDrivenDemo.h"
#include <sstream>
#include <map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

using namespace std;
using namespace DirectX;
using namespace Library;

namespace DataDriven
{
	const XMVECTORF32 DataDrivenDemo::BackgroundColor = Colors::Black;

	DataDrivenDemo::DataDrivenDemo(std::function<void*()> getWindowCallback, std::function<void(SIZE&)> getRenderTargetSizeCallback) :
		Game(getWindowCallback, getRenderTargetSizeCallback), mRenderStateHelper(*this)
	{
	}

	void DataDrivenDemo::Initialize()
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
		

		LoadSceneDescription("Content\\Prefabs\\Potions.json");

		for (auto& instance : mInstances) {

			auto object = make_shared<MultiMeshModel>(*this, camera, instance.prototype.model,
				instance.prototype.texture,instance.position,instance.rotation, 1.0f);

			mComponents.push_back(object);
		}


		Game::Initialize();
		camera->SetPosition(0.0f, 0.0f, 10.0f);
	}

	void DataDrivenDemo::Update(const GameTime &gameTime)
	{
		if (mKeyboard->WasKeyPressedThisFrame(Keys::Escape) || mGamePad->WasButtonPressedThisFrame(GamePadButtons::Back))
		{
			Exit();
		}

		mFpsComponent->Update(gameTime);

		Game::Update(gameTime);
	}

	void DataDrivenDemo::Draw(const GameTime &gameTime)
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

	void DataDrivenDemo::Shutdown()
	{
		SpriteManager::Shutdown();
		RasterizerStates::Shutdown();
		BlendStates::Shutdown();
		SamplerStates::Shutdown();

		Game::Shutdown();
	}

	void DataDrivenDemo::Exit()
	{
		PostQuitMessage(0);
	}
	
	void DataDrivenDemo::LoadSceneDescription(string filename)
	{
		
		ptree Data;
		
		read_json(filename, Data);
		
		//Load Prototypes
		for (ptree::value_type& prototype : Data.get_child("ModelPrototypes") ){
			Prototype newPrototype;
			newPrototype.model = prototype.second.get<string>("ModelFile");
			newPrototype.texture = prototype.second.get<string>("Textures.ColorFile");
			mPrototypes.insert(std::pair<string,Prototype>(prototype.first.data(),newPrototype));
		}

		//Load instances
		for (ptree::value_type& instance : Data.get_child("ObjectInstance")) 
		{
			Instance newInstance;
			newInstance.prototype = mPrototypes[instance.second.get<string>("Prototype")];

			auto position = instance.second.get_child("Position");
			newInstance.position.x = position.get<float>("X");
			newInstance.position.y = position.get<float>("Y");
			newInstance.position.z = position.get<float>("Z");
			
			auto rotation = instance.second.get_child("Rotation");
			newInstance.rotation.x = rotation.get<float>("X");
		    newInstance.rotation.y = rotation.get<float>("Y");
			newInstance.rotation.z = rotation.get<float>("Z");


			mInstances.push_back(newInstance);
		}
	}
}