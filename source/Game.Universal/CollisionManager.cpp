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
	PlayerCollisionType CollisionManager::PlayerCollisionCheck(const XMFLOAT2& playerPosition, const XMFLOAT2& playerVelocity, VelocityRestrictions& velocityRestrictions)
	{
		if (CharacterCollisionWithMap(playerPosition, playerVelocity, velocityRestrictions))
		{
			return PlayerCollisionType::Map;
		}

		return PlayerCollisionType::None;
	}

	/************************************************************************/
	bool CollisionManager::CharacterCollisionWithMap(const DirectX::XMFLOAT2 & characterPosition, const XMFLOAT2& characterVelocity, VelocityRestrictions& velocityRestrictions)
	{
		if (characterVelocity.x == 0 && characterVelocity.y == 0)
		{
			return false;
		}

		XMUINT2 characterTile = Renderable::GetTileFromPosition(characterPosition);
		vector<BoundingBox> vect = GetSurroundingBlocks(characterTile);

		XMFLOAT2 potentialPosition(characterPosition.x + characterVelocity.x, characterPosition.y + characterVelocity.y);
		XMFLOAT2 potentialCenter = Renderable::GetCenterPositionOfSprite(potentialPosition);
		XMFLOAT2 extents = Renderable::GetSpriteExtents();
		BoundingBox characterBoundingBox({ potentialCenter.x, potentialCenter.y, 0.1f }, { extents.x - sMarginForMapCollision, extents.y - sMarginForMapCollision, 0.1f });

		bool collided = false;
		uint8_t counterOfBlocks = 0;

		float_t minXDistance = 0.f;
		float_t minYDistance = 0.f;

		for (auto& box : vect)
		{
			if (characterBoundingBox.Intersects(box))
			{
				++counterOfBlocks;
				float_t xDistance = potentialCenter.x - box.Center.x;
				float_t yDistance = potentialCenter.y - box.Center.y;
				velocityRestrictions.CanMoveOnX |= abs(xDistance) < extents.x;
				velocityRestrictions.CanMoveOnY |= abs(yDistance) < extents.y;

				minXDistance = abs(minXDistance) < abs(xDistance) ? xDistance : minXDistance;
				minYDistance = abs(minYDistance) < abs(yDistance) ? yDistance : minYDistance;

				collided = true;
			}
		}

		if (!collided)
		{
			velocityRestrictions = VelocityRestrictions(true, true);
		}
		else
		{
			if (!velocityRestrictions.CanMoveOnX && characterVelocity.x != 0.f && abs(minYDistance) > extents.y * 0.8f)
			{
				velocityRestrictions.YVel = minYDistance > 0 ? 1.f : -1.f;
			}

			if (!velocityRestrictions.CanMoveOnY && characterVelocity.y != 0.f && abs(minXDistance) > extents.x * 0.8f)
			{
				velocityRestrictions.XVel = minXDistance > 0 ? 1.f : -1.f;
			}

			if (counterOfBlocks > 1 && velocityRestrictions.CanMoveOnX && velocityRestrictions.CanMoveOnY || counterOfBlocks > 2)
			{
				velocityRestrictions = VelocityRestrictions();
			}
		}

		return collided;
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