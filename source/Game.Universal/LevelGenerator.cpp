#include "pch.h"
#include "LevelGenerator.h"
#include "MapParser.h"
#include "MathHelper.h"
#include "RenderingDataStructures.h"

using namespace std;
using namespace DirectX;

namespace DirectXGame
{
	/************************************************************************/
	LevelGenerator& LevelGenerator::GetInstance()
	{
		static LevelGenerator sInstance;

		return sInstance;
	}

	/************************************************************************/
	Map LevelGenerator::GenerateLevel()
	{
		auto map = MapParser::GetInstance().ParseMapSpriteSheet();
		GenerateSoftBlocks(map);
		GeneratePerk(map);
		GenerateDoor(map);

		return map;
	}

	/************************************************************************/
	void LevelGenerator::GenerateSoftBlocks(Map& map)
	{
		// starts from bottom left
		uint32_t softBlocksNum = MathHelper::GetInstance().GetRangedRandom(kMaxNumberOfSoftBlocks, kMinNumberOfSoftBlocks);

		while (softBlocksNum > 0)
		{
			XMUINT2 randomTile = GetRandomTile(map);
			uint32_t index = map.BlocksLayer[randomTile.x][randomTile.y];

			if (!index)
			{
				bool restricted = false;
				// check if it's a restricted tile
				for (uint32_t i = 0; i < map.RestrictedTiles.size(); ++i)
				{
					restricted = IsSameTile(randomTile, map.RestrictedTiles[i]);
					if (restricted)
					{
						break;
					}
				}

				if (!restricted)
				{
					// if it's not restricted, add a soft block there
					map.BlocksLayer[randomTile.x][randomTile.y] = static_cast<uint32_t>(SpriteIndicesInMap::SoftBlock);
					--softBlocksNum;
				}
			}
		}
	}

	/************************************************************************/
	void LevelGenerator::GeneratePerk(Map& map)
	{
		bool itemAdded = false;

		while (!itemAdded)
		{
			XMUINT2 randomTile = GetRandomTile(map);
			uint32_t index = map.BlocksLayer[randomTile.x][randomTile.y];

			if (index == static_cast<uint32_t>(SpriteIndicesInMap::SoftBlock))
			{
				itemAdded = true;
				map.PerkTile.Tile = randomTile;
				map.PerkTile.SpriteIndex = GetRandomPerk(map);
			}
		}
	}

	/************************************************************************/
	void LevelGenerator::GenerateDoor(Map& map)
	{
		bool itemAdded = false;

		while (!itemAdded)
		{
			XMUINT2 randomTile = GetRandomTile(map);
			if (IsSameTile(randomTile, map.PerkTile.Tile))
			{
				continue;
			}

			uint32_t index = map.BlocksLayer[randomTile.x][randomTile.y];

			if (index == static_cast<uint32_t>(SpriteIndicesInMap::SoftBlock))
			{
				itemAdded = true;
				map.DoorTile.Tile = randomTile;
				map.DoorTile.SpriteIndex = static_cast<uint32_t>(SpriteIndicesInSpriteSheet::Door);
			}
		}
	}

	/************************************************************************/
	DirectX::XMUINT2 LevelGenerator::GetRandomTile(const Map & map)
	{
		MathHelper& mathHelper = MathHelper::GetInstance();

		XMUINT2 randomTile;

		randomTile.x = mathHelper.GetRangedRandom(map.MapWidth - 1, map.PlayerSpawnTile.x);
		randomTile.y = mathHelper.GetRangedRandom(map.PlayerSpawnTile.y + 1, 1U);

		return randomTile;
	}

	/************************************************************************/
	uint8_t LevelGenerator::GetRandomPerk(const Map& map)
	{
		// todo add randomness to perks
		UNREFERENCED_PARAMETER(map);
		return static_cast<uint8_t>(PerksIndicesInSpriteSheet::Fire);
	}

	/************************************************************************/
	bool LevelGenerator::IsSameTile(const DirectX::XMUINT2& first, const DirectX::XMUINT2& second)
	{
		return first.x == second.x && first.y == second.y;
	}
}