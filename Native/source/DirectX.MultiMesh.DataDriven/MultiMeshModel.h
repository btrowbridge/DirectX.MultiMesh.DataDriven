#pragma once

#include <wrl.h>
#include <d3d11_2.h>
#include <DirectXMath.h>
#include "DrawableGameComponent.h"
#include <memory>

using namespace DirectX;
using namespace Library;

namespace MultiMesh 
{
	class MultiMeshModel :
		public Library::DrawableGameComponent
	{
	public:
		MultiMeshModel(Library::Game& game);
		MultiMeshModel(Library::Game& game, const std::shared_ptr<Library::Camera>& camera);

		virtual void Initialize() override;
		virtual void Draw(const Library::GameTime& gameTime) override;
		virtual void Update(const Library::GameTime& gameTime) override;


	private:
		const static float RotationRate;

		std::vector<UINT> offsets;

		void MakeVertexBuffer(ID3D11Device2* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const;

		struct CBufferPerObject
		{
			DirectX::XMFLOAT4X4 WorldViewProjection;

			CBufferPerObject() : WorldViewProjection() { }
			CBufferPerObject(const DirectX::XMFLOAT4X4& wvp) : WorldViewProjection(wvp) { }
		};

		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
		

		//Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBufferPerFrameVS;
		//Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBufferPerFramePS;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBufferPerObjectVS;
		//Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBufferPerObjectPS;

		CBufferPerObject mCBufferPerObject;

		DirectX::XMFLOAT3 mLightDirection;
		DirectX::XMFLOAT3 mAmbientColor;
		DirectX::XMFLOAT4X4 mWorldMatrix;

		UINT mIndexCount;
		bool mAnimationEnabled;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mColorTexture;

	};

}