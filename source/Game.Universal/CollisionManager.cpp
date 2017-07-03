#include "pch.h"
#include "CollisionManager.h"
#include "Renderable.h"
#include "MapRenderable.h"

using namespace std;
using namespace DirectX;

namespace DirectXGame
{
	const float_t CollisionManager::sMarginForMapCollision = 0.25f;    // collision margin with the blocks
	const float_t CollisionManager::sMarginForBombAECollision = 1.f; // collision margin with the bombAE
	const float_t CollisionManager::sMarginForPerksCollision = 2.f; // collision margin with the perk
	const float_t CollisionManager::sMarginForDoorCollision = 4.f; // collision margin with the door

	/************************************************************************/
	CollisionManager& CollisionManager::GetInstance()
	{
		static CollisionManager sInstance;
		return sInstance;
	}

	/************************************************************************/
	void CollisionManager::SetMap(const shared_ptr<MapRenderable>& map)
	{
		mMap = map;
	}

	/************************************************************************/
	PlayerCollisionType CollisionManager::PlayerCollisionCheck(const XMFLOAT2& playerPosition, const XMFLOAT2& playerVelocity)
	{
		if (CharacterCollisionWithMap(playerPosition, playerVelocity))
		{
			return PlayerCollisionType::Map;
		}

		return PlayerCollisionType::None;
	}

	/************************************************************************/
	bool CollisionManager::CharacterCollisionWithMap(const DirectX::XMFLOAT2 & characterPosition, const XMFLOAT2& characterVelocity)
	{
		if (characterVelocity.x == 0 && characterVelocity.y == 0)
		{
			return false;
		}

		XMUINT2 playerTile = Renderable::GetTileFromPosition(characterPosition);
		vector<BoundingBox> vect = GetSurroundingBlocks(playerTile);

		XMFLOAT2 potentialPosition(characterPosition.x + characterVelocity.x, characterPosition.y + characterVelocity.y);
		XMFLOAT2 potentialCenter = Renderable::GetCenterPositionOfSprite(potentialPosition);
		XMFLOAT2 extents = Renderable::GetSpriteExtents();
		BoundingBox playerBoundingBox({ potentialCenter.x, potentialCenter.y, 0.1f }, { extents.x - sMarginForMapCollision, extents.y - sMarginForMapCollision, 0.1f });

		for (auto& box : vect)
		{
			if (playerBoundingBox.Intersects(box))
			{
				return true;
			}
		}

		return false;
	}

	/************************************************************************/
	bool CollisionManager::CharacterCollisionWithBombsAE(const DirectX::XMFLOAT2 & characterPosition)
	{
		UNREFERENCED_PARAMETER(characterPosition);
		return false;
	}

	/************************************************************************/
	bool CollisionManager::PlayerCollisionWithDoor(const DirectX::XMFLOAT2 & playerPosition)
	{
		UNREFERENCED_PARAMETER(playerPosition);
		return false;
	}

	/************************************************************************/
	bool CollisionManager::PlayerCollisionWithPerk(const DirectX::XMFLOAT2 & playerPosition)
	{
		UNREFERENCED_PARAMETER(playerPosition);
		return false;
	}

	/************************************************************************/
	vector<BoundingBox> CollisionManager::GetSurroundingBlocks(const XMUINT2& tile)
	{
		vector<BoundingBox> vect;

		for (uint32_t y = tile.y - 1; y <= tile.y + 1; ++y)
		{
			for (uint32_t x = tile.x - 1; x <= tile.x + 1; ++x)
			{
				if (x == tile.x && y == tile.y)
				{
					continue;
				}

				XMUINT2 currentTile(x, y);

				if (mMap.lock()->GetMap().BlocksLayer[x][y] == static_cast<uint32_t>(SpriteIndicesInMap::SoftBlock) ||
					mMap.lock()->GetMap().BlocksLayer[x][y] == static_cast<uint32_t>(SpriteIndicesInMap::SolidBlock))
				{
					XMFLOAT2 position = Renderable::GetPositionFromTile(currentTile);
					XMFLOAT2 center = Renderable::GetCenterPositionOfSprite(position);
					XMFLOAT2 extents = Renderable::GetSpriteExtents();
					XMFLOAT3 bbCenter(center.x, center.y, 0.1f);
					XMFLOAT3 bbExtents(extents.x, extents.y, 0.1f);
					BoundingBox boundingBox(bbCenter, bbExtents);

					vect.push_back(boundingBox);
				}
			}
		}

		return move(vect);
	}
}