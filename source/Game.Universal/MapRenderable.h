#pragma once

#include "Renderable.h"

namespace DirectXGame
{
	/** Class handling a renderable map.
	*/
	class MapRenderable final : public Renderable
	{
	public:

		MapRenderable(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera,
					  const std::string& jsonPath = kJSONFilePath, const std::wstring& textureMapPath = kTextureMapPath, 
					  DirectX::XMFLOAT2 position = kMapStartPosition);

		virtual void Update(const DX::StepTimer& timer) override;
		virtual void Render(const DX::StepTimer& timer) override;

		const Map& GetMap() const;

	protected:

		virtual void InitializeSprites() override;

	private:

		void RenderBasicMap();
		void RenderTile(const DirectX::XMUINT2& tile, const std::uint32_t spriteIndex);

		Map mMap;
		bool mIsPerkConsumed;

		static const std::string kJSONFilePath;
		static const std::wstring kTextureMapPath;
	};
}