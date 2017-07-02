#pragma once

#include "RenderingDataStructures.h"

namespace DirectXGame
{
	/** Singleton that handles generating a level randomly.
	*/
	class LevelGenerator final
	{
	public:

		LevelGenerator(const LevelGenerator& rhs) = delete;
		LevelGenerator(const LevelGenerator&& rhs) = delete;
		LevelGenerator& operator=(const LevelGenerator& rhs) = delete;
		LevelGenerator& operator=(const LevelGenerator&& rhs) = delete;

		static LevelGenerator& GetInstance();

		Map GenerateLevel();

	private:

		LevelGenerator() = default;
		~LevelGenerator() = default;

		void GenerateSoftBlocks(Map& map);
		void GeneratePerk(Map& map);
		void GenerateDoor(Map& map);

		DirectX::XMUINT2 GetRandomTile(const Map& map);
		uint8_t GetRandomPerk(const Map& map);
		bool IsSameTile(const DirectX::XMUINT2& first, const DirectX::XMUINT2& second);

		static const uint32_t kMinNumberOfSoftBlocks = 80;
		static const uint32_t kMaxNumberOfSoftBlocks = 120;
	};
}