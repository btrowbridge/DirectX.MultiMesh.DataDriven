#include "pch.h"
#include "MultiMeshModel.h"

using namespace std;
using namespace Library;
using namespace DirectX;

namespace MultiMesh {
	RTTI_DEFINITIONS(MultiMeshModel)

		MultiMeshModel::MultiMeshModel(Game& game, const shared_ptr<Camera>& camera, const std::string& modelFileName, const std::string& textureFileName, XMFLOAT3 position, XMFLOAT3 rollPitchYaw, float scale) :
		DrawableGameComponent(game, camera), mStartPosition(position), mStartRotation(rollPitchYaw),
		mModelFileName(modelFileName), mTextureFileName(textureFileName), mIndexCount(0), 
		mWorldMatrix(MatrixHelper::Identity), mScaleMatrix(MatrixHelper::Identity), mDisplayWireframe(true),
		mPosition(Vector3Helper::Zero), mDirection(Vector3Helper::Forward), mUp(Vector3Helper::Up), mRight(Vector3Helper::Right)
	{
		XMStoreFloat4x4(&mScaleMatrix, XMMatrixScaling(scale, scale, scale));
	}

	const XMFLOAT3& MultiMeshModel::Position() const
	{
		return mPosition;
	}

	const XMFLOAT3& MultiMeshModel::Direction() const
	{
		return mDirection;
	}

	const XMFLOAT3& MultiMeshModel::Up() const
	{
		return mUp;
	}

	const XMFLOAT3& MultiMeshModel::Right() const
	{
		return mRight;
	}

	XMVECTOR MultiMeshModel::PositionVector() const
	{
		return XMLoadFloat3(&mPosition);
	}

	XMVECTOR MultiMeshModel::DirectionVector() const
	{
		return XMLoadFloat3(&mDirection);
	}

	XMVECTOR MultiMeshModel::UpVector() const
	{
		return XMLoadFloat3(&mUp);
	}

	XMVECTOR MultiMeshModel::RightVector() const
	{
		return XMLoadFloat3(&mRight);
	}

	bool& MultiMeshModel::DisplayWireframe()
	{
		return mDisplayWireframe;
	}

	void MultiMeshModel::SetPosition(FLOAT x, FLOAT y, FLOAT z)
	{
		XMVECTOR position = XMVectorSet(x, y, z, 1.0f);
		SetPosition(position);
	}

	void MultiMeshModel::SetPosition(FXMVECTOR position)
	{
		XMStoreFloat3(&mPosition, position);
	}

	void MultiMeshModel::SetPosition(const XMFLOAT3& position)
	{
		mPosition = position;
	}

	void MultiMeshModel::ApplyRotation(CXMMATRIX transform)
	{
		XMVECTOR direction = XMLoadFloat3(&mDirection);
		XMVECTOR up = XMLoadFloat3(&mUp);

		direction = XMVector3TransformNormal(direction, transform);
		direction = XMVector3Normalize(direction);

		up = XMVector3TransformNormal(up, transform);
		up = XMVector3Normalize(up);

		XMVECTOR right = XMVector3Cross(direction, up);
		up = XMVector3Cross(right, direction);

		XMStoreFloat3(&mDirection, direction);
		XMStoreFloat3(&mUp, up);
		XMStoreFloat3(&mRight, right);
	}

	void MultiMeshModel::ApplyRotation(const XMFLOAT4X4& transform)
	{
		XMMATRIX transformMatrix = XMLoadFloat4x4(&transform);
		ApplyRotation(transformMatrix);
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
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		ThrowIfFailed(mGame->Direct3DDevice()->CreateInputLayout(inputElementDescriptions, ARRAYSIZE(inputElementDescriptions), &compiledVertexShader[0], compiledVertexShader.size(), mInputLayout.ReleaseAndGetAddressOf()), "ID3D11Device::CreateInputLayout() failed.");

		// Create constant buffers
		D3D11_BUFFER_DESC constantBufferDesc = { 0 };
		constantBufferDesc.ByteWidth = sizeof(VertexCBufferPerObject);
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ThrowIfFailed(mGame->Direct3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, mVertexCBufferPerObject.ReleaseAndGetAddressOf()), "ID3D11Device::CreateBuffer() failed.");

		// Load a model
		Model model = Library::Model(mModelFileName);
		std::vector<VertexPositionTexture> vptVector;
		std::vector<uint32_t> indexVector;
		
		//Origin chunk
		VertexChunkData vertexChunk = { 0 };
		UINT previousVertexOffset = 0;
		UINT previousIndexOffset = 0;

		// Create vertex and index buffers for the model
		for (auto& mesh : model.Meshes()) {

			vertexChunk.IndexCount = static_cast<UINT>(mesh->Indices().size());
			vertexChunk.VertexOffset = previousVertexOffset;
			vertexChunk.IndexOffset = previousIndexOffset;
			mVertexChunks.push_back(vertexChunk);

			previousVertexOffset += static_cast<UINT>(mesh->Vertices().size());
			previousIndexOffset += static_cast<UINT>(mesh->Indices().size());

			AddVertices(*mesh.get(), vptVector);
			indexVector.insert(indexVector.end(), mesh->Indices().begin(), mesh->Indices().end());
			
		}
		//Vertex Buffer
		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.ByteWidth = sizeof(VertexPositionTexture) * static_cast<UINT>(vptVector.size());
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData = { 0 };
		vertexSubResourceData.pSysMem = &vptVector[0];
		ThrowIfFailed(mGame->Direct3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, mVertexBuffer.ReleaseAndGetAddressOf()), "ID3D11Device::CreateBuffer() failed.");

		//Index
		D3D11_BUFFER_DESC indexBufferDesc = { 0 };
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<UINT>(indexVector.size());
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexSubResourceData = { 0 };
		indexSubResourceData.pSysMem = &indexVector[0];
		ThrowIfFailed(mGame->Direct3DDevice()->CreateBuffer(&indexBufferDesc, &indexSubResourceData, mIndexBuffer.ReleaseAndGetAddressOf()), "ID3D11Device::CreateBuffer() failed.");

		// Load a texture
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring textureName = converter.from_bytes(mTextureFileName);
	
		ThrowIfFailed(CreateWICTextureFromFile(mGame->Direct3DDevice(), textureName.c_str(), nullptr, mColorTexture.ReleaseAndGetAddressOf()), "CreateWICTextureFromFile() failed.");

		mKeyboard = reinterpret_cast<KeyboardComponent*>(mGame->Services().GetService(KeyboardComponent::TypeIdClass()));

		//Starting transforms
		ApplyRotation((XMMatrixRotationRollPitchYaw(mStartRotation.x, mStartRotation.y, mStartRotation.z)));
		SetPosition(mStartPosition);
	}

	void MultiMeshModel::Update(const GameTime& gameTime)
	{
		UNREFERENCED_PARAMETER(gameTime);

		if (mKeyboard->WasKeyPressedThisFrame(Keys::Space)) {
			mDisplayWireframe = !mDisplayWireframe;
		}

		XMMATRIX worldMatrix = XMMatrixIdentity();
		MatrixHelper::SetForward(worldMatrix, mDirection);
		MatrixHelper::SetUp(worldMatrix, mUp);
		MatrixHelper::SetRight(worldMatrix, mRight);
		MatrixHelper::SetTranslation(worldMatrix, mPosition);

		XMStoreFloat4x4(&mWorldMatrix, XMLoadFloat4x4(&mScaleMatrix) * worldMatrix);
	}

	void MultiMeshModel::Draw(const GameTime& gameTime)
	{
		UNREFERENCED_PARAMETER(gameTime);

		ID3D11DeviceContext* direct3DDeviceContext = mGame->Direct3DDeviceContext();
		direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		direct3DDeviceContext->IASetInputLayout(mInputLayout.Get());

		UINT stride = sizeof(VertexPositionTexture);
		UINT offset = 0;
		direct3DDeviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
		direct3DDeviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		direct3DDeviceContext->VSSetShader(mVertexShader.Get(), nullptr, 0);
		direct3DDeviceContext->PSSetShader(mPixelShader.Get(), nullptr, 0);

		direct3DDeviceContext->PSSetShaderResources(0, 1, mColorTexture.GetAddressOf());

		XMMATRIX worldMatrix = XMLoadFloat4x4(&mWorldMatrix);
		XMMATRIX wvp = worldMatrix * mCamera->ViewProjectionMatrix();
		XMStoreFloat4x4(&mVertexCBufferPerObjectData.WorldViewProjection, XMMatrixTranspose(wvp));

		direct3DDeviceContext->UpdateSubresource(mVertexCBufferPerObject.Get(), 0, nullptr, &mVertexCBufferPerObjectData, 0, 0);
		direct3DDeviceContext->VSSetConstantBuffers(0, 1, mVertexCBufferPerObject.GetAddressOf());
		
		FLOAT empty [4];
		mGame->Direct3DDeviceContext()->OMSetBlendState(BlendStates::AlphaBlending.Get(), empty, UINT_MAX);

		std::vector<UINT> renderOrder = { 0,2,1 };
		for ( auto& i : renderOrder){ 
		
			auto& vertexChunk = mVertexChunks.at(i);

			if (mDisplayWireframe)
			{
				mGame->Direct3DDeviceContext()->RSSetState(RasterizerStates::Wireframe.Get());
				direct3DDeviceContext->DrawIndexed(vertexChunk.IndexCount,vertexChunk.IndexOffset, vertexChunk.VertexOffset );
				mGame->Direct3DDeviceContext()->RSSetState(nullptr);
			}
			else
			{
				direct3DDeviceContext->DrawIndexed(vertexChunk.IndexCount,vertexChunk.IndexOffset, vertexChunk.VertexOffset );
			}
		}
	}

	void MultiMeshModel::AddVertices(const Mesh& mesh, std::vector<VertexPositionTexture>& vptVector) const
	{
		const std::vector<XMFLOAT3>& sourceVertices = mesh.Vertices();
		const auto& sourceUVs = mesh.TextureCoordinates().at(0);

		std::vector<VertexPositionTexture> vertices;
		vertices.reserve(sourceVertices.size());

		for (UINT i = 0; i < sourceVertices.size(); i++)
		{
			const XMFLOAT3& position = sourceVertices.at(i);
			const XMFLOAT3& uv = sourceUVs->at(i);

			vertices.push_back(VertexPositionTexture(XMFLOAT4(position.x, position.y, position.z, 1.0f), XMFLOAT2(uv.x, uv.y)));
		}
		vptVector.insert(vptVector.end(), vertices.begin(), vertices.end());
	}
}