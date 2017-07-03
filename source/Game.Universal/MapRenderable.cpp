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

		// urgent do stuff here
	}

	/************************************************************************/
	void MapRenderable::Render(const StepTimer& timer)
	{
		Renderable::Render(timer);
		RenderBasicMap();
	}

	/************************************************************************/
	Map MapRenderable::GetMap() const
	{
		return mMap;
	}

	/************************************************************************/
	void MapRenderable::InitializeSprites()
	{
		mRenderableSpriteSheet = SpriteSheetParser::GetInstance().ParseSpriteSheet(mSpriteSheetJSONPath);
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
				if (currentSpriteIndex > 0)
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
}