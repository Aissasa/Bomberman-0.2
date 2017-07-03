#pragma once

#include <DirectXCollision.h>
#include <memory>
#include <vector>

namespace DirectXGame
{

	enum class PlayerCollisionType
	{
		None,
		Map,
		BombAE,
		Enemy,
		Perk,
		Door
	};

	class MapRenderable;

	class CollisionManager final
	{
	public:

		CollisionManager(const CollisionManager&) = delete;
		CollisionManager(const CollisionManager&&) = delete;
		CollisionManager& operator=(const CollisionManager&) = delete;
		CollisionManager& operator=(const CollisionManager&&) = delete;

		static CollisionManager& GetInstance();

		void SetMap(const std::shared_ptr<MapRenderable>& map);
		PlayerCollisionType PlayerCollisionCheck(const DirectX::XMFLOAT2& playerPosition, const DirectX::XMFLOAT2& playerVelocity);

	private:

		CollisionManager() = default;
		~CollisionManager() = default;

		bool CharacterCollisionWithMap(const DirectX::XMFLOAT2& characterPosition, const DirectX::XMFLOAT2& characterVelocity);
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