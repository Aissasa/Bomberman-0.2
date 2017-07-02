#pragma once

#include "RenderingDataStructures.h"
#include <document.h>

namespace DirectXGame
{
	/** Singleton that handles parsing a spritesheet data from JSON.
	*/
	class SpriteSheetParser final
	{
	public:

		SpriteSheetParser(const SpriteSheetParser& rhs) = delete;
		SpriteSheetParser(const SpriteSheetParser&& rhs) = delete;
		SpriteSheetParser& operator=(const SpriteSheetParser& rhs) = delete;
		SpriteSheetParser& operator=(const SpriteSheetParser&& rhs) = delete;

		static SpriteSheetParser& GetInstance();

		SpriteSheet ParseSpriteSheet(const std::string& filePath);

	private:

		SpriteSheetParser() = default;
		~SpriteSheetParser() = default;

		std::shared_ptr<Sprite> PopulateASprite(const rapidjson::Value& frames, uint32_t index, float_t sortingLayer);
		std::shared_ptr<Animation> PopulateAnAnimation(const SpriteSheet& spriteSheet, const rapidjson::Value& animations, uint32_t index);
	};
}