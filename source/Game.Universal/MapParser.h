#pragma once

#include "RenderingDataStructures.h"
#include <document.h>

namespace DirectXGame
{
	/** Singleton that handles parsing a level map from JSON.
	*/
	class MapParser final
	{
	public:

		MapParser(const MapParser& rhs) = delete;
		MapParser(const MapParser&& rhs) = delete;
		MapParser& operator=(const MapParser& rhs) = delete;
		MapParser& operator=(const MapParser&& rhs) = delete;

		static MapParser& GetInstance();

		Map ParseMapSpriteSheet();

	private:

		MapParser() = default;
		~MapParser() = default;

		static const std::string kMapJSONPath;

		void PopulateLayers(Map& map, const rapidjson::Value & layers);
		void PopulateRestrictedTiles(Map& map, const rapidjson::Value & restrictedTiles);
	};
}