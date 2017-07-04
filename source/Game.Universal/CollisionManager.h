#pragma once

#include <DirectXCollision.h>
#include <memory>
#include <vector>

namespace DirectXGame
{

	/** Enum representing the player's collision type.
	*/
	enum class PlayerCollisionType
	{
		None,
		Map,
		BombAE,
		Enemy,
		Perk,
		Door
	};

	/** Structure that holds information about the character movement restrictions.
	 * It is used to help characters cut corners.
	*/
	struct VelocityRestrictions
	{
		VelocityRestrictions(const bool x = false, const bool y = false, const float_t xVel = 0.f, const float_t yVel = 0.f) :
			CanMoveOnX(x), CanMoveOnY(y), XVel(xVel), YVel(yVel)
		{
		}

		bool CanMoveOnX;
		bool CanMoveOnY;

		float_t XVel;
		float_t YVel;
	};

	class MapRenderable;

	/** Singleton that handles the collisions in the game.
	 * This Manager interfaces with the level manager to get the elements in the map.
	 * @see LevelManager
	*/
	class CollisionManager final
	{
	public:

		CollisionManager(const CollisionManager&) = delete;
		CollisionManager(const CollisionManager&&) = delete;
		CollisionManager& operator=(const CollisionManager&) = delete;
		CollisionManager& operator=(const CollisionManager&&) = delete;

		static CollisionManager& GetInstance();

		void SetMap(const std::shared_ptr<MapRenderable>& map);
		PlayerCollisionType PlayerCollisionCheck(const DirectX::XMFLOAT2& playerPosition, const DirectX::XMFLOAT2& playerVelocity, VelocityRestrictions& velocityRestrictions);

	private:

		CollisionManager() = default;
		~CollisionManager() = default;

		bool CharacterCollisionWithMap(const DirectX::XMFLOAT2& characterPosition, const DirectX::XMFLOAT2& characterVelocity, VelocityRestrictions& velocityRestrictions);
		//bool PlayerCollisionWithEnemies(const DirectX::BoundingBox& playerBoundingBox);
		bool CharacterCollisionWithBombsAE(const DirectX::XMFLOAT2& characterPosition);
		bool PlayerCollisionWithDoor(const DirectX::XMFLOAT2& playerPosition);
		bool PlayerCollisionWithPerk(const DirectX::XMFLOAT2& playerPosition);

		std::vector<DirectX::BoundingBox> GetSurroundingBlocks(const DirectX::XMUINT2& tile);

		std::weak_ptr<MapRenderable> mMap;

		static const float_t sMarginForMapCollision;
		static const float_t sMarginForBombAECollision;
		static const float_t sMarginForPerksCollision;
		static const float_t sMarginForDoorCollision;
	};

}