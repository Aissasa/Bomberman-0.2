#pragma once

#include "Renderable.h"

namespace DirectXGame
{
	/** Structure representing a renderable fading soft block.
	*/
	struct FadingSoftBlock
	{
		FadingSoftBlock()
		{
		}

		FadingSoftBlock(const std::shared_ptr<Animation>& anim, const DirectX::XMFLOAT2 position) :
			Anim(*anim), Position(position), AnimTimer(0), AnimEnded(false)
		{
		}

		Animation Anim;
		DirectX::XMFLOAT2 Position;
		double_t AnimTimer;
		bool AnimEnded;
	};

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

		Map& GetMap();
		void AddFadingBlock(const DirectX::XMUINT2& tile);
		void PerkConsumed();
		bool IsPerkConsumed() const;

	protected:

		virtual void InitializeSprites() override;

	private:

		void RenderBasicMap();
		void RenderTile(const DirectX::XMUINT2& tile, const std::uint32_t spriteIndex);
		void RenderFadingSoftBlocks();

		void UpdateAnimations(const DX::StepTimer& timer);

		Map mMap;
		bool mIsPerkConsumed;

		std::vector<FadingSoftBlock> mFadingBlocks;

		static const std::string kJSONFilePath;
		static const std::wstring kTextureMapPath;

		static const std::string kSoftBlockFadingAnimationName;
		static const double_t kSoftBlockFadingAnimationLength;
	};
}