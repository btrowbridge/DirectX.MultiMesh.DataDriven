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
		RTTI_DECLARATIONS(MultiMeshModel, DrawableGameComponent)

	public:
		MultiMeshModel(Game& game, const std::shared_ptr<Camera>& camera, const std::string& modelFileName, float scale = 1.0f);
		MultiMeshModel(Game& game, const shared_ptr<Camera>& camera, const Model& model, float scale = 1.0f);
		MultiMeshModel(const MultiMeshModel&) = delete;
		MultiMeshModel& operator=(const MultiMeshModel&) = delete;
		MultiMeshModel(MultiMeshModel&&) = delete;
		MultiMeshModel& operator=(MultiMeshModel&&) = delete;
		~MultiMeshModel() = default;

		const DirectX::XMFLOAT3& Position() const;
		const DirectX::XMFLOAT3& Direction() const;
		const DirectX::XMFLOAT3& Up() const;
		const DirectX::XMFLOAT3& Right() const;

		DirectX::XMVECTOR PositionVector() const;
		DirectX::XMVECTOR DirectionVector() const;
		DirectX::XMVECTOR UpVector() const;
		DirectX::XMVECTOR RightVector() const;

		bool& DisplayWireframe();

		void SetPosition(FLOAT x, FLOAT y, FLOAT z);
		void SetPosition(DirectX::FXMVECTOR position);
		void SetPosition(const DirectX::XMFLOAT3& position);

		void ApplyRotation(DirectX::CXMMATRIX transform);
		void ApplyRotation(const DirectX::XMFLOAT4X4& transform);

		virtual void Initialize() override;
		virtual void Update(const GameTime& gameTime) override;		
		virtual void Draw(const GameTime& gameTime) override;

	private:
		struct VertexCBufferPerObject
		{
			DirectX::XMFLOAT4X4 WorldViewProjection;

			VertexCBufferPerObject() : WorldViewProjection() { }

			VertexCBufferPerObject(const DirectX::XMFLOAT4X4& wvp) : WorldViewProjection(wvp) { }
		};

		struct VertexChunkData {
			UINT VertexOffset;
			UINT IndexOffset;
			UINT IndexCount;
		};

		std::vector<VertexChunkData> mVertexChunks;

		void AddVertices(const Mesh& mesh, std::vector<VertexPositionTexture>& vertexVector) const;

		DirectX::XMFLOAT4X4 mWorldMatrix;
		DirectX::XMFLOAT4X4 mScaleMatrix;
		DirectX::XMFLOAT3 mPosition;
		DirectX::XMFLOAT3 mDirection;
		DirectX::XMFLOAT3 mUp;
		DirectX::XMFLOAT3 mRight;
		std::string mModelFileName;

		std::shared_ptr<Model> mModel;

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mColorTexture;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;		
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexCBufferPerObject;
		VertexCBufferPerObject mVertexCBufferPerObjectData;
		UINT mIndexCount;
		bool mDisplayWireframe;

		Library::KeyboardComponent* mKeyboard;
	};

}