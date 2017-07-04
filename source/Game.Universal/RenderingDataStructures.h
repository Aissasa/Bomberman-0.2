#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <DirectXMath.h>

namespace DirectXGame
{
	// todo consider adding this property to each animation in the JSON files.
	const std::double_t kAnimationLength = 0.125f;

#pragma region Structures

	/** Structure representing the data of a sprite in a sprite sheet.
	*/
	struct Sprite
	{
		Sprite(const uint32_t width = 52, const uint32_t height = 52, const uint32_t x = 0, const uint32_t y = 0, const DirectX::XMFLOAT2& uvScalingFactor = DirectX::XMFLOAT2(), const float_t sortingLayer = 0) :
			Width(width), Height(height), X(x), Y(y), UVScalingFactor(uvScalingFactor), SortingLayer(sortingLayer)
		{
		}

		uint32_t Width;
		uint32_t Height;
		uint32_t X;
		uint32_t Y;
		DirectX::XMFLOAT2 UVScalingFactor;
		float_t SortingLayer;
	};

	/** Structure representing an animation.
	*/
	struct Animation
	{
		std::string Name;
		std::vector<std::shared_ptr<Sprite>> Sprites;
		double_t AnimationLength; //speed
		uint32_t CurrentSpriteIndex;
	};

	/** Structure representing a sprite sheet for an animated object.
	*/
	struct SpriteSheet
	{
		std::vector<std::shared_ptr<Sprite>> Sprites;
		std::map<std::string, std::shared_ptr<Animation>> Animations;
		float_t TextureXUnit;
		float_t TextureYUnit;
	};

	/** Structure representing a tile and its Sprite.
	*/
	struct TileWithSpriteIndex
	{
		DirectX::XMUINT2 Tile;
		uint8_t SpriteIndex;
	};

	/** Structure representing a sprite sheet for an animated object.
	*/
	struct Map
	{
		TileWithSpriteIndex PerkTile;
		TileWithSpriteIndex DoorTile;

		DirectX::XMUINT2 PlayerSpawnTile;
		std::vector<DirectX::XMUINT2> RestrictedTiles;

		uint32_t MapWidth;
		uint32_t MapHeight;

		uint32_t TileWidth;
		uint32_t TileHeight;

		std::vector<std::vector<uint8_t>> BackgroundLayer;
		std::vector<std::vector<uint8_t>> BlocksLayer;
	};

#pragma endregion


#pragma region Enums

	/** Enumeration representing the sprites indices in a spritesheet object.
	 *@see SpriteSheet
	*/
	enum class SpriteIndicesInSpriteSheet
	{
		Door = 2,
		SoftBlock = 9,
		SolidBlock = 16
	};

	/** Enumeration representing the perks sprites indices in a spritesheet object.
	 *@see SpriteSheet
	*/
	enum class PerksIndicesInSpriteSheet
	{
		BombUp = 1,
		Fire = 3,
		PassBomb = 5,
		PassSoftBlock = 6,
		Remote = 7,
		Skate = 8
	};

	/** Enumeration representing the sprites indices in a map object.
	 *@see Map
	*/
	enum class SpriteIndicesInMap
	{
		None = 0,
		Door = static_cast<uint8_t>(SpriteIndicesInSpriteSheet::Door) + 1,
		SoftBlock = static_cast<uint8_t>(SpriteIndicesInSpriteSheet::SoftBlock) + 1,
		SolidBlock = static_cast<uint8_t>(SpriteIndicesInSpriteSheet::SolidBlock) + 1
	};

	/** Enumeration representing the perks sprites indices in a map object.
	*@see Map
	*/
	enum class PerksIndicesInMap
	{
		BombUp = static_cast<uint8_t>(PerksIndicesInSpriteSheet::BombUp) + 1,
		Fire = static_cast<uint8_t>(PerksIndicesInSpriteSheet::Fire) + 1,
		PassBomb = static_cast<uint8_t>(PerksIndicesInSpriteSheet::PassBomb) + 1,
		PassSoftBlock = static_cast<uint8_t>(PerksIndicesInSpriteSheet::PassSoftBlock) + 1,
		Remote = static_cast<uint8_t>(PerksIndicesInSpriteSheet::Remote) + 1,
		Skate = static_cast<uint8_t>(PerksIndicesInSpriteSheet::Skate) + 1
	};

#pragma endregion

}