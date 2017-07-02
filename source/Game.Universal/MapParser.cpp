#include "pch.h"
#include "MapParser.h"
#include <istreamwrapper.h>

using namespace std;
using namespace rapidjson;
using namespace DirectX;

namespace DirectXGame
{
	const string MapParser::kMapJSONPath = "Assets/JSONS/BasicMap.json";

	/************************************************************************/
	MapParser& MapParser::GetInstance()
	{
		static MapParser sInstance;
		return sInstance;
	}

	/************************************************************************/
	Map MapParser::ParseMapSpriteSheet()
	{
		ifstream ifs(kMapJSONPath);
		IStreamWrapper ist(ifs);

		Document jsonDoc;
		jsonDoc.ParseStream(ist);
		assert(jsonDoc.IsObject());

		Map basicMap;
		basicMap.MapWidth = jsonDoc["width"].GetUint();
		basicMap.MapHeight = jsonDoc["height"].GetUint();
		basicMap.TileWidth = jsonDoc["tilewidth"].GetUint();
		basicMap.TileHeight = jsonDoc["tileheight"].GetUint();

		const Value& layers = jsonDoc["layers"];
		assert(layers.IsArray());

		PopulateLayers(basicMap, layers);

		assert(jsonDoc.HasMember("restrictedTiles"));

		const Value& restrictedTiles = jsonDoc["restrictedTiles"];
		assert(restrictedTiles.IsArray());

		PopulateRestrictedTiles(basicMap, restrictedTiles);
		basicMap.PlayerSpawnTile = basicMap.RestrictedTiles[0];

		return basicMap;
	}

	/************************************************************************/
	void MapParser::PopulateLayers(Map& map, const Value& layers)
	{
		// build layers
		map.BackgroundLayer.resize(map.MapWidth);
		map.BlocksLayer.resize(map.MapWidth);

		for (uint32_t x = 0; x < map.MapWidth; ++x)
		{
			map.BackgroundLayer[x].resize(map.MapHeight);
			map.BlocksLayer[x].resize(map.MapHeight);
		}

		// populate background layer and blocks layer
		for (uint32_t y = 0; y < map.MapHeight; ++y)
		{
			for (uint32_t x = 0; x < map.MapWidth; ++x)
			{
				// inverse the y
				uint32_t newY = map.MapHeight - y - 1;
				map.BackgroundLayer[x][y] = static_cast<uint8_t>(layers[0]["data"][newY * map.MapWidth + x].GetUint());
				map.BlocksLayer[x][y] = static_cast<uint8_t>(layers[1]["data"][newY * map.MapWidth + x].GetUint());
			}
		}
	}

	/************************************************************************/
	void MapParser::PopulateRestrictedTiles(Map& map, const Value& restrictedTiles)
	{
		uint32_t restrictedTilesCount = restrictedTiles.Size();
		map.RestrictedTiles.resize(restrictedTilesCount);

		for (uint32_t i = 0; i < restrictedTilesCount; ++i)
		{
			XMUINT2 tileCoor;
			tileCoor.x = restrictedTiles[i]["x"].GetUint();
			tileCoor.y = restrictedTiles[i]["y"].GetUint();
			map.RestrictedTiles[i] = tileCoor;
		}
	}
}