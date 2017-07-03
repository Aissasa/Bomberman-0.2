#include "pch.h"
#include "Renderable.h"

using namespace std;
using namespace DX;
using namespace DirectX;
using namespace Microsoft::WRL;

namespace DirectXGame
{
	const XMFLOAT2 Renderable::SpriteScale = XMFLOAT2(2.f, 2.f);
	const XMFLOAT2 Renderable::kMapStartPosition = XMFLOAT2(-48.f, -32.5f);
	const float_t Renderable::kModifier = 1.985f;

	/************************************************************************/
	Renderable::Renderable(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<Camera>& camera, const std::string& jsonPath, const wstring& textureMapPath, DirectX::XMFLOAT2 position) :
		DrawableGameComponent(deviceResources, camera),
		mLoadingComplete(false),
		mIndexCount(0),
		mPosition(position),
		mSpriteSheetJSONPath(jsonPath),
		mTextureMapFilePath(textureMapPath)
	{
	}

	/************************************************************************/
	const DirectX::XMFLOAT2 & Renderable::Position() const
	{
		return mPosition;
	}

	/************************************************************************/
	void Renderable::SetPositon(const DirectX::XMFLOAT2 & position)
	{
		mPosition = position;
	}

	/************************************************************************/
	void Renderable::CreateDeviceDependentResources()
	{
		auto loadVSTask = ReadDataAsync(L"SpriteRendererVS.cso");
		auto loadPSTask = ReadDataAsync(L"SpriteRendererPS.cso");

		// After the vertex shader file is loaded, create the shader and input layout.
		auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
		{
			ThrowIfFailed(
				mDeviceResources->GetD3DDevice()->CreateVertexShader(
					&fileData[0],
					fileData.size(),
					nullptr,
					mVertexShader.ReleaseAndGetAddressOf()
				)
			);

			// Create an input layout
			ThrowIfFailed(
				mDeviceResources->GetD3DDevice()->CreateInputLayout(
					VertexPositionTexture::InputElements,
					VertexPositionTexture::InputElementCount,
					&fileData[0],
					fileData.size(),
					mInputLayout.ReleaseAndGetAddressOf()
				)
			);

			CD3D11_BUFFER_DESC constantBufferDesc(sizeof(VSCBufferPerObject), D3D11_BIND_CONSTANT_BUFFER);
			ThrowIfFailed(
				mDeviceResources->GetD3DDevice()->CreateBuffer(
					&constantBufferDesc,
					nullptr,
					mVSCBufferPerObject.ReleaseAndGetAddressOf()
				)
			);
		});

		// After the pixel shader file is loaded, create the shader and texture sampler state.
		auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
		{
			ThrowIfFailed(
				mDeviceResources->GetD3DDevice()->CreatePixelShader(
					&fileData[0],
					fileData.size(),
					nullptr,
					mPixelShader.ReleaseAndGetAddressOf()
				)
			);

			D3D11_SAMPLER_DESC samplerStateDesc;
			ZeroMemory(&samplerStateDesc, sizeof(samplerStateDesc));
			samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerStateDesc.MinLOD = -FLT_MAX;
			samplerStateDesc.MaxLOD = FLT_MAX;
			samplerStateDesc.MipLODBias = 0.0f;
			samplerStateDesc.MaxAnisotropy = 1;
			samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateSamplerState(&samplerStateDesc, mTextureSampler.ReleaseAndGetAddressOf()));

			D3D11_BLEND_DESC blendStateDesc = { 0 };
			blendStateDesc.RenderTarget[0].BlendEnable = true;
			blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateBlendState(&blendStateDesc, mAlphaBlending.ReleaseAndGetAddressOf()));
		});

		auto loadSpriteSheetAndCreateSpritesTask = (createPSTask && createVSTask).then([this]()
		{
			ThrowIfFailed(CreateWICTextureFromFile(mDeviceResources->GetD3DDevice(), mTextureMapFilePath.c_str(), nullptr, mSpriteSheet.ReleaseAndGetAddressOf()));
			InitializeVertices();
			InitializeSprites();
		});

		// Once the cube is loaded, the object is ready to be rendered.
		loadSpriteSheetAndCreateSpritesTask.then([this]()
		{
			mLoadingComplete = true;
		});
	}

	/************************************************************************/
	void Renderable::ReleaseDeviceDependentResources()
	{
		mLoadingComplete = false;
		mVertexShader.Reset();
		mPixelShader.Reset();
		mInputLayout.Reset();
		mVertexBuffer.Reset();
		mIndexBuffer.Reset();
		mVSCBufferPerObject.Reset();
		mSpriteSheet.Reset();
		mTextureSampler.Reset();
	}

	/************************************************************************/
	void Renderable::Update(const StepTimer& timer)
	{
		UNREFERENCED_PARAMETER(timer);

		if (!mLoadingComplete)
		{
			return;
		}
	}

	/************************************************************************/
	void Renderable::Render(const StepTimer & timer)
	{
		UNREFERENCED_PARAMETER(timer);

		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!mLoadingComplete)
		{
			return;
		}

		ID3D11DeviceContext* direct3DDeviceContext = mDeviceResources->GetD3DDeviceContext();
		direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		direct3DDeviceContext->IASetInputLayout(mInputLayout.Get());

		static const UINT stride = sizeof(VertexPositionTexture);
		static const UINT offset = 0;
		direct3DDeviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
		direct3DDeviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		direct3DDeviceContext->VSSetShader(mVertexShader.Get(), nullptr, 0);
		direct3DDeviceContext->PSSetShader(mPixelShader.Get(), nullptr, 0);
		direct3DDeviceContext->VSSetConstantBuffers(0, 1, mVSCBufferPerObject.GetAddressOf());
		direct3DDeviceContext->PSSetShaderResources(0, 1, mSpriteSheet.GetAddressOf());
		direct3DDeviceContext->PSSetSamplers(0, 1, mTextureSampler.GetAddressOf());
		direct3DDeviceContext->OMSetBlendState(mAlphaBlending.Get(), 0, 0xFFFFFFFF);

		// call draw sprite here depending on how many there are
	}

	/************************************************************************/
	void Renderable::DrawSprite(const Sprite& sprite, const Transform2D& transform)
	{
		ID3D11DeviceContext* direct3DDeviceContext = mDeviceResources->GetD3DDeviceContext();

		XMFLOAT4X4 textureTransform = GetTextureTransformFromSprite(sprite);

		const XMMATRIX wvp = XMMatrixTranspose(transform.WorldMatrix() * mCamera->ViewProjectionMatrix());
		XMStoreFloat4x4(&mVSCBufferPerObjectData.WorldViewProjection, wvp);
		XMMATRIX textureTransformMat = XMLoadFloat4x4(&textureTransform);
		XMStoreFloat4x4(&mVSCBufferPerObjectData.TextureTransform, XMMatrixTranspose(textureTransformMat));
		direct3DDeviceContext->UpdateSubresource(mVSCBufferPerObject.Get(), 0, nullptr, &mVSCBufferPerObjectData, 0, 0);

		direct3DDeviceContext->DrawIndexed(mIndexCount, 0, 0);
	}

	/************************************************************************/
	XMFLOAT4X4 Renderable::GetTextureTransformFromSprite(const Sprite& sprite)
	{
		XMFLOAT4X4 textureTransform;
		XMMATRIX textureTransformMatrix = XMMatrixScaling(sprite.UVScalingFactor.x, sprite.UVScalingFactor.y, 0) * 
			XMMatrixTranslation(sprite.UVScalingFactor.x * sprite.X, sprite.UVScalingFactor.y * sprite.Y, 0); // todo maybe change this
		XMStoreFloat4x4(&textureTransform, textureTransformMatrix);
		
		return textureTransform;
	}

	/************************************************************************/
	void Renderable::InitializeVertices()
	{
		VertexPositionTexture vertices[] =
		{
			VertexPositionTexture(XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)),
			VertexPositionTexture(XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)),
			VertexPositionTexture(XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)),
			VertexPositionTexture(XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)),
		};

		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.ByteWidth = sizeof(VertexPositionTexture) * ARRAYSIZE(vertices);
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData = { 0 };
		vertexSubResourceData.pSysMem = vertices;
		ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, mVertexBuffer.ReleaseAndGetAddressOf()));

		// Create and index buffer
		const uint32_t indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};

		mIndexCount = ARRAYSIZE(indices);

		D3D11_BUFFER_DESC indexBufferDesc = { 0 };
		indexBufferDesc.ByteWidth = sizeof(uint32_t) * mIndexCount;
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexSubResourceData = { 0 };
		indexSubResourceData.pSysMem = indices;
		ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexSubResourceData, mIndexBuffer.ReleaseAndGetAddressOf()));
	}

	/************************************************************************/
	XMFLOAT2 Renderable::GetPositionFromTile(const XMUINT2& tile)
	{
		return XMFLOAT2(kMapStartPosition.x + kModifier * tile.x * SpriteScale.x, kMapStartPosition.y + kModifier * tile.y * SpriteScale.y);
	}

	/************************************************************************/
	XMUINT2 Renderable::GetTileFromPosition(const XMFLOAT2& position)
	{
		XMFLOAT2 center = GetCenterPositionOfSprite(position);
		return XMUINT2(static_cast<uint32_t>(((center.x - kMapStartPosition.x) / (kModifier * SpriteScale.x))), static_cast<uint32_t>(((center.y - kMapStartPosition.y) / (kModifier * SpriteScale.y))));
	}

	/************************************************************************/
	XMFLOAT2 Renderable::GetCenterPositionOfSprite(const XMFLOAT2 & position)
	{
		return { position.x + (kModifier * SpriteScale.x) / 2,  position.y + (kModifier * SpriteScale.y) / 2 };
	}

	/************************************************************************/
	XMFLOAT2 Renderable::GetSpriteExtents()
	{
		return { (kModifier * SpriteScale.x) / 2 , (kModifier * SpriteScale.y) / 2 };
	}
}