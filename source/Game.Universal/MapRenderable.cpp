#include "pch.h"
#include "MapRenderable.h"
#include "MapParser.h"
#include "LevelGenerator.h"
#include "SpriteSheetParser.h"
#include "CollisionManager.h"

using namespace std;
using namespace DirectX;
using namespace DX;

namespace DirectXGame
{
	const string MapRenderable::kJSONFilePath = "Assets/JSONS/Props.json";
	const wstring MapRenderable::kTextureMapPath = L"Assets/SpriteSheets/PropsSpriteSheet.png";
	const string MapRenderable::kSoftBlockFadingAnimationName = "SoftBlockFading";
	const double_t MapRenderable::kSoftBlockFadingAnimationLength = 0.1;

	/************************************************************************/
	MapRenderable::MapRenderable(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<Camera>& camera,
								 const string& jsonPath, const wstring & textureMapPath, XMFLOAT2 position) :
		Renderable(deviceResources, camera, jsonPath, textureMapPath, position), mIsPerkConsumed(false)
	{
		InitializeSprites();
	}

	/************************************************************************/
	void MapRenderable::Update(const StepTimer& timer)
	{
		Renderable::Update(timer);

		UpdateAnimations(timer);
	}

	/************************************************************************/
	void MapRenderable::Render(const StepTimer& timer)
	{
		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!mLoadingComplete)
		{
			return;
		}

		Renderable::Render(timer);
		RenderBasicMap();
		RenderFadingSoftBlocks();
	}

	/************************************************************************/
	Map& MapRenderable::GetMap()
	{
		return mMap;
	}

	/************************************************************************/
	void MapRenderable::AddFadingBlock(const DirectX::XMUINT2& tile)
	{
		mMap.BlocksLayer[tile.x][tile.y] = static_cast<uint8_t>(SpriteIndicesInMap::None);

		FadingSoftBlock fadingSoftBlock(mRenderableSpriteSheet.Animations[kSoftBlockFadingAnimationName], GetPositionFromTile(tile));
		mFadingBlocks.push_back(fadingSoftBlock);
	}

	/************************************************************************/
	void MapRenderable::PerkConsumed()
	{
		mIsPerkConsumed = true;
	}

	/************************************************************************/
	bool MapRenderable::IsPerkConsumed() const
	{
		return mIsPerkConsumed;
	}

	/************************************************************************/
	void MapRenderable::InitializeSprites()
	{
		mRenderableSpriteSheet = SpriteSheetParser::GetInstance().ParseSpriteSheet(mSpriteSheetJSONPath);
		mRenderableSpriteSheet.Animations[kSoftBlockFadingAnimationName]->AnimationLength = kSoftBlockFadingAnimationLength;
		mMap = LevelGenerator::GetInstance().GenerateLevel();
	}

	/************************************************************************/
	void MapRenderable::RenderBasicMap()
	{
		for (uint32_t y = 0; y < mMap.MapHeight; ++y)
		{
			for (uint32_t x = 0; x < mMap.MapWidth; ++x)
			{
				XMUINT2 currentTile(x, y);

				// render bg tile
				uint32_t currentSpriteIndex = mMap.BackgroundLayer[x][y];
				if (currentSpriteIndex > 0 && currentSpriteIndex != 5) // todo fix the gray background problem
				{
					RenderTile(currentTile, --currentSpriteIndex);
				}
			}
		}

		if (!mIsPerkConsumed)
		{
			RenderTile(mMap.PerkTile.Tile, mMap.PerkTile.SpriteIndex);
		}

		RenderTile(mMap.DoorTile.Tile, mMap.DoorTile.SpriteIndex);

		for (uint32_t y = 0; y < mMap.MapHeight; ++y)
		{
			for (uint32_t x = 0; x < mMap.MapWidth; ++x)
			{
				XMUINT2 currentTile(x, y);

				// render blocks
				uint32_t currentSpriteIndex = mMap.BlocksLayer[x][y];
				if (currentSpriteIndex > 0)
				{
					RenderTile(currentTile, --currentSpriteIndex);
				}
			}
		}
	}

	/************************************************************************/
	void MapRenderable::RenderTile(const XMUINT2& tile, const uint32_t spriteIndex)
	{
		auto sprite = mRenderableSpriteSheet.Sprites[spriteIndex];
		XMFLOAT2 tilePosition = GetPositionFromTile(tile);
		Transform2D transform(tilePosition, 0, SpriteScale);

		DrawSprite(*sprite, transform);
	}

	/************************************************************************/
	void MapRenderable::RenderFadingSoftBlocks()
	{
		for (auto& block : mFadingBlocks)
		{
			auto sprite = block.Anim.Sprites[block.Anim.CurrentSpriteIndex];
			Transform2D transform(block.Position , 0, SpriteScale);

			DrawSprite(*sprite, transform);
		}
	}

	/************************************************************************/
	void MapRenderable::UpdateAnimations(const DX::StepTimer & timer)
	{
		if (mFadingBlocks.size() == 0)
		{
			return;
		}

		for (auto& block : mFadingBlocks)
		{
			block.AnimTimer += timer.GetElapsedSeconds();

			if (block.AnimTimer > block.Anim.AnimationLength)
			{
				// last sprite
				if (block.Anim.CurrentSpriteIndex == block.Anim.Sprites.size() - 1)
				{
					block.AnimEnded = true;
					block.Anim.CurrentSpriteIndex = 0;
				}
				else
				{
					block.AnimTimer -= block.Anim.AnimationLength;
					++block.Anim.CurrentSpriteIndex;
				}
			}
		}

		bool done = false;

		while (!done)
		{
			done = true;
			for (auto it = mFadingBlocks.begin(); it != mFadingBlocks.end(); ++it)
			{
				if ((*it).AnimEnded)
				{
					mFadingBlocks.erase(it);
					done = false;
					break;
				}
			}
		}
	}
}