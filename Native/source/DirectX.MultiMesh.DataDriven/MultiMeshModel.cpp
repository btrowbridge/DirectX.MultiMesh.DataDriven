#include "pch.h"
#include "MultiMeshModel.h"

using namespace std;
using namespace Library;
using namespace DirectX;

namespace MultiMesh {
	const float MultiMeshModel::RotationRate = XM_PI;

	MultiMeshModel::MultiMeshModel(Library::Game & game) 
		: DrawableGameComponent(game), mVertexShader(), mIndexCount(), mPixelShader(), mWorldMatrix(MatrixHelper::Identity), mAnimationEnabled(true)
	{
	}
	MultiMeshModel::MultiMeshModel(Library::Game & game, const std::shared_ptr<Library::Camera>& camera) 
		: DrawableGameComponent(game, camera)
	{
	}
	void MultiMeshModel::Initialize()
	{
		// Load a compiled vertex shader
		std::vector<char> compiledVertexShader;
		Utility::LoadBinaryFile(L"Content\\Shaders\\TexturedVS.cso", compiledVertexShader);
		ThrowIfFailed(mGame->Direct3DDevice()->CreateVertexShader(&compiledVertexShader[0], compiledVertexShader.size(), nullptr, mVertexShader.ReleaseAndGetAddressOf()), "ID3D11Device::CreatedVertexShader() failed.");

		// Load a compiled pixel shader
		std::vector<char> compiledPixelShader;
		Utility::LoadBinaryFile(L"Content\\Shaders\\TexturedPS.cso", compiledPixelShader);
		ThrowIfFailed(mGame->Direct3DDevice()->CreatePixelShader(&compiledPixelShader[0], compiledPixelShader.size(), nullptr, mPixelShader.ReleaseAndGetAddressOf()), "ID3D11Device::CreatedPixelShader() failed.");

		// Create an input layout
		D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 }
		};

		ThrowIfFailed(mGame->Direct3DDevice()->CreateInputLayout(inputElementDescriptions, ARRAYSIZE(inputElementDescriptions), &compiledVertexShader[0], compiledVertexShader.size(), mInputLayout.ReleaseAndGetAddressOf()), "ID3D11Device::CreateInputLayout() failed.");

		//load model

		unique_ptr<Library::Model> model = make_unique<Library::Model>("Content\\Models\\Sphere.obj.bin");
		for (UINT i = 0; i < model->Meshes().size(); i++) {
			Mesh* mesh = model->Meshes().at(i).get();

			//vertex buffer
			MakeVertexBuffer(mGame->Direct3DDevice(), *mesh, mVertexBuffer.GetAddressOf());

			//index buffer

			mesh->CreateIndexBuffer(*mGame->Direct3DDevice(), mIndexBuffer.ReleaseAndGetAddressOf());
			mIndexCount += static_cast<UINT>(mesh->Indices().size());
		}

		//Constant Buffer perObject
		D3D11_BUFFER_DESC constantBufferDesc = { 0 };
		constantBufferDesc.ByteWidth = sizeof(CBufferPerObject);
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ThrowIfFailed(mGame->Direct3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, mConstantBufferPerObjectVS.ReleaseAndGetAddressOf()), "ID3D11Device::CreateBuffer() failed.");

		//Load a texture
		wstring textureName = L"Content\\Textures\\EarthComposite.dds";
		ThrowIfFailed(CreateDDSTextureFromFile(mGame->Direct3DDevice(), textureName.c_str(), nullptr, mColorTexture.ReleaseAndGetAddressOf()), "CreateDDSTexture failed");


	}
	void MultiMeshModel::Draw(const Library::GameTime & gameTime)
	{
		UNREFERENCED_PARAMETER(gameTime);

		ID3D11DeviceContext* direct3DDeviceContext = mGame->Direct3DDeviceContext();
		direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		direct3DDeviceContext->IASetInputLayout(mInputLayout.Get());

		UINT stride = sizeof(VertexPositionTextureNormal);
		UINT offset = 0;
		direct3DDeviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
		direct3DDeviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		direct3DDeviceContext->VSSetShader(mVertexShader.Get(), nullptr, 0);
		direct3DDeviceContext->PSSetShader(mPixelShader.Get(), nullptr, 0);

		XMMATRIX worldMatrix = XMLoadFloat4x4(&mWorldMatrix);
		XMMATRIX wvp = worldMatrix * mCamera->ViewProjectionMatrix();
		wvp = XMMatrixTranspose(wvp);
		XMStoreFloat4x4(&mCBufferPerObject.WorldViewProjection, wvp);

		direct3DDeviceContext->VSSetConstantBuffers(0, 1, mConstantBufferPerObjectVS.GetAddressOf());

		//direct3DDeviceContext->VSSetConstantBuffers(0, 2, mConstantBufferPerObjectVS.GetAddressOf());
		//direct3DDeviceContext->VSSetConstantBuffers(1, 2, mConstantBufferPerFrameVS.GetAddressOf());

		direct3DDeviceContext->PSSetShaderResources(0, 1, mColorTexture.GetAddressOf());
		direct3DDeviceContext->PSSetSamplers(0, 1, SamplerStates::TrilinearMirror.ReleaseAndGetAddressOf());

		//direct3DDeviceContext->PSSetConstantBuffers(0, 2, mConstantBufferPerObjectPS.GetAddressOf());
		//direct3DDeviceContext->PSSetConstantBuffers(1, 2, mConstantBufferPerFramePS.GetAddressOf());

		direct3DDeviceContext->DrawIndexed(mIndexCount, 0, 0);
	}
	void MultiMeshModel::Update(const Library::GameTime & gameTime)
	{
		static float angle = 0.0f;

		if (mAnimationEnabled)
		{
			angle += gameTime.ElapsedGameTimeSeconds().count() * RotationRate;
			XMStoreFloat4x4(&mWorldMatrix, XMMatrixRotationY(angle));
		}
	}
	void MultiMeshModel::MakeVertexBuffer(ID3D11Device2* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const
	{
		const std::vector < XMFLOAT3 >& sourceVertices = mesh.Vertices();
		const std::vector < XMFLOAT3 >& sourceNormals = mesh.Normals();
		const auto& sourceUVs = mesh.TextureCoordinates().at(0);

		std::vector <VertexPositionTextureNormal> vertices;
		vertices.reserve(sourceVertices.size());
		for (UINT i = 0; i < sourceVertices.size(); i++) {
			const XMFLOAT3& position = sourceVertices.at(i);
			const XMFLOAT3& uv = sourceUVs->at(i);
			const XMFLOAT3& normal = sourceNormals.at(i);
			vertices.push_back(VertexPositionTextureNormal(XMFLOAT4(position.x, position.y, position.z, 1.0f), XMFLOAT2(uv.x, uv.y), normal));
		}
		//Vertex Buffer
		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.ByteWidth = sizeof(VertexPositionTextureNormal) * static_cast<UINT>(vertices.size());
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData = { 0 };
		vertexSubResourceData.pSysMem = &vertices[0];

		ThrowIfFailed(device->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, vertexBuffer), "ID3D11Device::CreateBuffer() failed.");

	}
}