#pragma once

#include "DrawableGameComponent.h"
#include "MatrixHelper.h"
#include "RenderingDataStructures.h"

namespace DirectXGame
{
	/** Class representing a renderable object in the game. The rendering is sprite based.
	 * This class encapsulates all the boiler plate code for rendering sprites.
	*/
	class Renderable : public DX::DrawableGameComponent
	{
	public:

		Renderable(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, 
				   const std::string& jsonPath = "", const std::wstring& textureMapPath = L"", DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(-48.f, -32.5f));

		const DirectX::XMFLOAT2& Position() const;
		void SetPositon(const DirectX::XMFLOAT2& position);

		virtual void CreateDeviceDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;
		virtual void Update(const DX::StepTimer& timer) override;
		virtual void Render(const DX::StepTimer& timer) override;

		static DirectX::XMFLOAT2 GetPositionFromTile(const DirectX::XMUINT2& tile);
		static DirectX::XMUINT2 GetTileFromPosition(const DirectX::XMFLOAT2& position);
		static DirectX::XMFLOAT2 GetCenterPositionOfSprite(const DirectX::XMFLOAT2& position);
		static DirectX::XMFLOAT2 GetSpriteExtents();

		static const DirectX::XMFLOAT2 SpriteScale;

	protected:

		struct VSCBufferPerObject
		{
			DirectX::XMFLOAT4X4 WorldViewProjection;
			DirectX::XMFLOAT4X4 TextureTransform;

			VSCBufferPerObject() :
				WorldViewProjection(DX::MatrixHelper::Identity), TextureTransform(DX::MatrixHelper::Identity)
			{
			};

			VSCBufferPerObject(const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& textureTransform) :
				WorldViewProjection(wvp), TextureTransform(textureTransform)
			{
			}
		};

		virtual void InitializeSprites() = 0;
		void InitializeVertices();
		void Renderable::DrawSprite(const Sprite& sprite, const DX::Transform2D& transform);
		DirectX::XMFLOAT4X4 GetTextureTransformFromSprite(const Sprite& sprite);

		std::wstring mTextureMapFilePath;
		std::string mSpriteSheetJSONPath;
		SpriteSheet mRenderableSpriteSheet;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVSCBufferPerObject;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSpriteSheet;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mTextureSampler;
		Microsoft::WRL::ComPtr<ID3D11BlendState> mAlphaBlending;
		VSCBufferPerObject mVSCBufferPerObjectData;
		bool mLoadingComplete;
		std::uint32_t mIndexCount;
		DirectX::XMFLOAT2 mPosition;

		static const DirectX::XMFLOAT2 kMapStartPosition;
		static const std::float_t kModifier;
	};
}
