#include "pch.h"
#include "SpriteSheetParser.h"
#include "RenderingDataStructures.h"
#include <document.h>
#include <istreamwrapper.h>

using namespace std;
using namespace rapidjson;

namespace DirectXGame
{
	/************************************************************************/
	SpriteSheetParser& SpriteSheetParser::GetInstance()
	{
		static SpriteSheetParser sInstance;
		return sInstance;
	}

	/************************************************************************/
	SpriteSheet SpriteSheetParser::ParseSpriteSheet(const string& filePath)
	{
		SpriteSheet spriteSheet;
		float_t sortingLayer = -20.0f;

		ifstream ifs(filePath);
		IStreamWrapper ist(ifs);

		Document jsonDoc;
		jsonDoc.ParseStream(ist);
		assert(jsonDoc.IsObject());

		if (jsonDoc.HasMember("sortingLayer"))
		{
			sortingLayer = jsonDoc["sortingLayer"].GetFloat();
		}

		const Value& frames = jsonDoc["frames"];
		assert(frames.IsArray());

		spriteSheet.Sprites.resize(frames.Size());

		// note if sprite sheets with multiple rows are gonna be added, change this
		spriteSheet.TextureXUnit = 1.0f / spriteSheet.Sprites.size();
		spriteSheet.TextureYUnit = 1.0f;

		// store sprites
		for (uint32_t i = 0; i < spriteSheet.Sprites.size(); ++i) 
		{
			spriteSheet.Sprites[i] = PopulateASprite(frames, i, sortingLayer);
		}

		// store animations
		const Value& anims = jsonDoc["Animations"];
		assert(anims.IsArray());

		map<string, shared_ptr<Animation>> animationsMap;
		for (uint32_t i = 0; i < anims.Size(); ++i)
		{
			auto animation = PopulateAnAnimation(spriteSheet, anims, i);
			animationsMap[animation->Name] = animation;
		}

		spriteSheet.Animations = move(animationsMap);
		return spriteSheet;
	}

	/************************************************************************/
	shared_ptr<Sprite> SpriteSheetParser::PopulateASprite(const Value& frames, uint32_t index, float_t sortingLayer)
	{
		shared_ptr<Sprite> newSprite = make_shared<Sprite>();

		newSprite->Width = frames[index]["frame"]["w"].GetUint();
		newSprite->Height = frames[index]["frame"]["h"].GetUint();
		newSprite->X = frames[index]["frame"]["x"].GetUint();
		//newSprite->Y = frames[index]["frame"]["y"].GetUint();
		newSprite->X = newSprite->X / newSprite->Width;
		newSprite->Y = 0; //(newSprite->Y + newSprite->Height) / newSprite->Height;

		newSprite->UVScalingFactor.x = 1.0f / frames.Size();
		newSprite->UVScalingFactor.y = 1.0f;
		if (sortingLayer < -10)
		{
			newSprite->SortingLayer = frames[index]["sortingLayer"].GetFloat();
		}
		else
		{
			newSprite->SortingLayer = sortingLayer; // default for all sprites
		}

		return newSprite;
	}

	/************************************************************************/
	shared_ptr<Animation> SpriteSheetParser::PopulateAnAnimation(const SpriteSheet& spriteSheet, const Value& animations, uint32_t index)
	{
		shared_ptr<Animation> newAnimation = make_shared<Animation>();

		string name(animations[index]["Name"].GetString());
		newAnimation->Name = name;

		newAnimation->Sprites.resize(animations[index]["Sprites"].Size());

		for (uint32_t j = 0; j < newAnimation->Sprites.size(); ++j)
		{
			uint32_t spriteIndex = animations[index]["Sprites"][j].GetUint();
			newAnimation->Sprites[j] = spriteSheet.Sprites[spriteIndex];
		}

		newAnimation->AnimationLength = kAnimationLength;
		newAnimation->CurrentSpriteIndex = 0;

		return newAnimation;
	}
}