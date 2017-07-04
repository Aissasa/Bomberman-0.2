#include "pch.h"
#include "Bomb.h"
#include "Player.h"
#include "SpriteSheetParser.h"
#include "LevelManager.h"
#include "MapRenderable.h"

using namespace std;
using namespace DX;
using namespace DirectX;

namespace DirectXGame
{
	const double_t Bomb::kBombExplosionTime = 3;

	// rendering
	const string Bomb::kBombJSONFilePath = "Assets/JSONS/Bomb.json";
	const wstring Bomb::kBombTextureMapPath = L"Assets/SpriteSheets/BombSpriteSheet.png";
	const string Bomb::kBombAEJSONFilePath = "Assets/JSONS/BombAE.json";
	const wstring Bomb::kBombAETextureMapPath = L"Assets/SpriteSheets/BombAESpriteSheet.png";

	// animation
	const double_t Bomb::kBombAnimationTime = 0.2;
	const double_t Bomb::kBombAEAnimationTime = 0.1;

	const string Bomb::kBombTickingAnimationName = "BombTicking";
	const string Bomb::kBombAEBottomAnimationName = "BombAEBottom";
	const string Bomb::kBombAECenterAnimationName = "BombAECenter";
	const string Bomb::kBombAEHorizAnimationName = "BombAEHoriz";
	const string Bomb::kBombAELeftAnimationName = "BombAELeft";
	const string Bomb::kBombAERightAnimationName = "BombAERight";
	const string Bomb::kBombAETopAnimationName = "BombAETop";
	const string Bomb::kBombAEVertAnimationName = "BombAEVert";

	/************************************************************************/
	Bomb::Bomb(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<DX::Camera>& camera, Player& player,
			   const string & jsonPath, const std::wstring & textureMapPath) :
		Renderable(deviceResources, camera, jsonPath, textureMapPath),
		mPlayer(player),
		mCurrentState(BombState::Ticking),
		mExplosionTimer(0),
		mIsRemoteControlled(player.GetPerks().Remote),
		mTickingAnimationTimer(0)
	{
		mPosition = GetPositionFromTile(GetTileFromPosition(mPlayer.Position()));
		CreateDeviceDependentResources();
		InitializeSprites();
	}

	/************************************************************************/
	Bomb::~Bomb()
	{
		ReleaseDeviceDependentResources();
	}

	/************************************************************************/
	void Bomb::Update(const DX::StepTimer & timer)
	{
		Renderable::Update(timer);

		switch (mCurrentState)
		{
			case DirectXGame::BombState::Ticking:
			{
				if (!mIsRemoteControlled)
				{
					mExplosionTimer += timer.GetElapsedSeconds();
				}
				if (mExplosionTimer > kBombExplosionTime)
				{
					Explode();
				}
				else
				{
					UpdateAnimation(timer);
				}

				break;
			}
			case DirectXGame::BombState::Exploding:
			{
				UpdateAnimation(timer);
				if (mExplosionAEs[0].AnimEnded)
				{
					mCurrentState = BombState::Vanished;
				}
				break;
			}
			case DirectXGame::BombState::Vanished:
			{
				static bool done = false;
				if (!done)
				{
					Vanish();
				}
				break;
			}
			default:
				break;
		}
	}

	/************************************************************************/
	void Bomb::Render(const DX::StepTimer& timer)
	{
		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!mLoadingComplete)
		{
			return;
		}

		Renderable::Render(timer);

		switch (mCurrentState)
		{
			case DirectXGame::BombState::Ticking:
			{
				auto sprite = mTickingAnimation->Sprites[mTickingAnimation->CurrentSpriteIndex];
				Transform2D transform(mPosition, 0, SpriteScale);

				DrawSprite(*sprite, transform);
				break;
			}
			case DirectXGame::BombState::Exploding:
			{
				for (auto& explosionAE : mExplosionAEs)
				{
					auto sprite = explosionAE.Anim.Sprites[explosionAE.Anim.CurrentSpriteIndex];
					Transform2D transform(explosionAE.Position, 0, SpriteScale);

					DrawSprite(*sprite, transform);
				}
				break;
			}
			case DirectXGame::BombState::Vanished:
			{
				break;
			}
			default:
				break;
		}
	}

	/************************************************************************/
	void Bomb::Explode()
	{
		uint32_t range = mPlayer.GetPerks().Fire;

		XMUINT2 centerTile = GetTileFromPosition(mPosition);

		// center
		ExplosionAE centerExplosionAE(mBombAESpriteSheet.Animations[kBombAECenterAnimationName], GetPositionFromTile(centerTile));
		mExplosionAEs.push_back(centerExplosionAE);

		// left horiz
		bool canAddLeft = true;
		for (uint32_t i = 1; i <= range; ++i)
		{
			XMUINT2 leftHorizTile(centerTile.x - i, centerTile.y);

			ExplosionAE leftHorizExplosionAE(mBombAESpriteSheet.Animations[kBombAEHorizAnimationName], GetPositionFromTile(leftHorizTile));

			if (!AddExplosionAE(leftHorizExplosionAE))
			{
				canAddLeft = false;
				break;
			}
		}

		// right horiz
		bool canAddRight = true;
		for (uint32_t i = 1; i <= range; ++i)
		{
			XMUINT2 rightHorizTile(centerTile.x + i, centerTile.y);

			ExplosionAE rightHorizExplosionAE(mBombAESpriteSheet.Animations[kBombAEHorizAnimationName], GetPositionFromTile(rightHorizTile));

			if (!AddExplosionAE(rightHorizExplosionAE))
			{
				canAddRight = false;
				break;
			}
		}

		// bottom vert
		bool canAddBottom = true;
		for (uint32_t i = 1; i <= range; ++i)
		{
			XMUINT2 bottomVertTile(centerTile.x, centerTile.y - i);

			ExplosionAE bottomVertExplosionAE(mBombAESpriteSheet.Animations[kBombAEVertAnimationName], GetPositionFromTile(bottomVertTile));

			if (!AddExplosionAE(bottomVertExplosionAE))
			{
				canAddBottom = false;
				break;
			}
		}

		// top vert
		bool canAddTop = true;
		for (uint32_t i = 1; i <= range; ++i)
		{
			XMUINT2 topVertTile(centerTile.x, centerTile.y + i);

			ExplosionAE topVertExplosionAE(mBombAESpriteSheet.Animations[kBombAEVertAnimationName], GetPositionFromTile(topVertTile));

			if (!AddExplosionAE(topVertExplosionAE))
			{
				canAddTop = false;
				break;
			}
		}

		// left
		if (canAddLeft)
		{
			XMUINT2 leftTile(centerTile.x - (range + 1), centerTile.y);
			ExplosionAE leftExplosionAE(mBombAESpriteSheet.Animations[kBombAELeftAnimationName], GetPositionFromTile(leftTile));

			AddExplosionAE(leftExplosionAE);
		}

		// right
		if (canAddRight)
		{
			XMUINT2 rightTile(centerTile.x + (range + 1), centerTile.y);
			ExplosionAE rightExplosionAE(mBombAESpriteSheet.Animations[kBombAERightAnimationName], GetPositionFromTile(rightTile));

			AddExplosionAE(rightExplosionAE);
		}

		// bottom
		if (canAddBottom)
		{
			XMUINT2 bottomTile(centerTile.x, centerTile.y - (range + 1));
			ExplosionAE bottomExplosionAE(mBombAESpriteSheet.Animations[kBombAEBottomAnimationName], GetPositionFromTile(bottomTile));

			AddExplosionAE(bottomExplosionAE);
		}

		// top
		if (canAddTop)
		{
			XMUINT2 topTile(centerTile.x, centerTile.y + (range + 1));
			ExplosionAE topExplosionAE(mBombAESpriteSheet.Animations[kBombAETopAnimationName], GetPositionFromTile(topTile));

			AddExplosionAE(topExplosionAE);
		}

		// add them to level manager
		for (auto& ae : mExplosionAEs)
		{
			LevelManager::GetInstance().AddBombAE(GetTileFromPosition(ae.Position));
		}

		// update assets
		mPlayer.RemoveBomb(*this);
		mRenderableSpriteSheet = mBombAESpriteSheet;
		mTextureMapFilePath = kBombAETextureMapPath;
		ReleaseDeviceDependentResources();
		CreateDeviceDependentResources();
		mCurrentState = BombState::Exploding;
	}

	/************************************************************************/
	void Bomb::InitializeSprites()
	{
		// set ticking animation
		mBombSpriteSheet = SpriteSheetParser::GetInstance().ParseSpriteSheet(kBombJSONFilePath);
		mRenderableSpriteSheet = mBombSpriteSheet;
		mTickingAnimation = mRenderableSpriteSheet.Animations[kBombTickingAnimationName];
		mTickingAnimation->AnimationLength = kBombAnimationTime;

		// set explosion ae animations
		mBombAESpriteSheet = SpriteSheetParser::GetInstance().ParseSpriteSheet(kBombAEJSONFilePath);
		for (auto& anim : mBombAESpriteSheet.Animations)
		{
			anim.second->AnimationLength = kBombAEAnimationTime;
		}
	}

	/************************************************************************/
	void Bomb::UpdateAnimation(const DX::StepTimer& timer)
	{
		switch (mCurrentState)
		{
			case DirectXGame::BombState::Ticking:
			{
				UpdateTickingAnimation(timer);
				break;
			}
			case DirectXGame::BombState::Exploding:
			{
				UpdateExplosionAnimation(timer);
				break;
			}
			case DirectXGame::BombState::Vanished:
			{
				break;
			}
			default:
				break;
		}
	}

	/************************************************************************/
	void Bomb::UpdateTickingAnimation(const StepTimer& timer)
	{
		mTickingAnimationTimer += timer.GetElapsedSeconds();

		if (mTickingAnimationTimer > mTickingAnimation->AnimationLength)
		{
			mTickingAnimationTimer -= mTickingAnimation->AnimationLength;
			mTickingAnimation->CurrentSpriteIndex = (mTickingAnimation->CurrentSpriteIndex + 1) % mTickingAnimation->Sprites.size();
		}
	}

	/************************************************************************/
	void Bomb::UpdateExplosionAnimation(const StepTimer& timer)
	{
		for (auto& explosionAE : mExplosionAEs)
		{
			explosionAE.AnimTimer += timer.GetElapsedSeconds();

			if (explosionAE.AnimTimer > explosionAE.Anim.AnimationLength)
			{
				// last sprite
				if (explosionAE.Anim.CurrentSpriteIndex == explosionAE.Anim.Sprites.size() - 1)
				{
					explosionAE.AnimEnded = true;
					explosionAE.Anim.CurrentSpriteIndex = 0;
				}
				else
				{
					explosionAE.AnimTimer -= explosionAE.Anim.AnimationLength;
					++explosionAE.Anim.CurrentSpriteIndex;
				}
			}
		}
	}

	/************************************************************************/
	void Bomb::Vanish()
	{
		for (auto& ae : mExplosionAEs)
		{
			LevelManager::GetInstance().RemoveBombAE(GetTileFromPosition(ae.Position));
		}

		LevelManager::GetInstance().RemoveBomb(*this);
	}

	/************************************************************************/
	bool Bomb::AddExplosionAE(const ExplosionAE& explosionAE)
	{
		const Map& map = mPlayer.GetMap();
		XMUINT2 tile = GetTileFromPosition(explosionAE.Position);

		if (map.BlocksLayer[tile.x][tile.y] != static_cast<uint8_t>(SpriteIndicesInMap::SoftBlock) &&
			map.BlocksLayer[tile.x][tile.y] != static_cast<uint8_t>(SpriteIndicesInMap::SolidBlock))
		{
			mExplosionAEs.push_back(explosionAE);
			return true;
		}
		else
		{
			if (map.BlocksLayer[tile.x][tile.y] == static_cast<uint8_t>(SpriteIndicesInMap::SoftBlock))
			{
				mPlayer.GetMapRenderable().AddFadingBlock(tile);
			}
			return false;
		}
	}
}