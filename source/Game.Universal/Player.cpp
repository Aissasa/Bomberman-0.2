#include "pch.h"
#include "Player.h"
#include "MapParser.h"
#include "LevelGenerator.h"
#include "SpriteSheetParser.h"
#include "CollisionManager.h"
#include "KeyboardComponent.h"
#include "GamePadComponent.h"
#include "Bomb.h"
#include "LevelManager.h"
#include "MapRenderable.h"

using namespace std;
using namespace DirectX;
using namespace DX;

namespace DirectXGame
{

#pragma region Static consts

	// movement
	const XMFLOAT2 Player::kBaseSpeed = { 6.f, 8.f };
	const XMFLOAT2 Player::kSpeedIncrement = { 1.5f, 2.f };
	const float_t Player::kButtonCooldownTime = 0.2f;

	// rendering
	const string Player::kJSONFilePath = "Assets/JSONS/MC.json";
	const wstring Player::kTextureMapPath = L"Assets/SpriteSheets/MCSpriteSheet.png";

	// animation
	const double_t Player::kDeathAnimationLength = 0.3;
	const string Player::kDeathAnimationName = "Death";
	const string Player::kIdleLeftAnimationName = "IdleLeft";
	const string Player::kIdleRightAnimationName = "IdleRight";
	const string Player::kIdleDownAnimationName = "IdleDown";
	const string Player::kIdleUpAnimationName = "IdleUp";
	const string Player::kWalkingLeftAnimationName = "WalkingLeft";
	const string Player::kWalkingRightAnimationName = "WalkingRight";
	const string Player::kWalkingDownAnimationName = "WalkingDown";
	const string Player::kWalkingUpAnimationName = "WalkingUp";

#pragma endregion


	/************************************************************************/
	Player::Player(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<Camera>& camera,
				   const shared_ptr<KeyboardComponent>& keyboard, const shared_ptr<GamePadComponent>& gamePad,
				   MapRenderable& map, const string& jsonPath, const wstring& textureMapPath) :
		Renderable(deviceResources, camera, jsonPath, textureMapPath),
		mKeyBoard(keyboard),
		mGamePad(gamePad),
		mMap(map),
		mCurrentPlayerState(PlayerState::Idle),
		mCurrentAnimation(nullptr),
		mAnimationTimer(0),
		mVelocity(0, 0),
		mBaseSpeed(kBaseSpeed),
		mCurrentMovementState(),
		mPreviousMovementState()
	{
		mPosition = GetPositionFromTile(mMap.GetMap().PlayerSpawnTile);
		// for debug
		//++mPerks.BombUp;
		//++mPerks.Fire;
		//mPerks.Remote = true;
	}

	/************************************************************************/
	void Player::Update(const DX::StepTimer& timer)
	{
		switch (mCurrentPlayerState)
		{
			case DirectXGame::PlayerState::Idle:
			case DirectXGame::PlayerState::Moving:
			{
				ProcessInput();
				UpdateVelocity();
				UpdateAnimation(timer);
				VelocityRestrictions velocityRestrictions = CheckCollisions(timer);
				UpdatePosition(timer, velocityRestrictions);
				break;
			}

			case DirectXGame::PlayerState::Dying:
			{
				UpdateAnimation(timer);
				break;
			}

			case DirectXGame::PlayerState::Dead:
			default:
				break;
		}
	}

	/************************************************************************/
	void Player::Render(const DX::StepTimer & timer)
	{
		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!mLoadingComplete)
		{
			return;
		}

		Renderable::Render(timer);
		auto sprite = mCurrentAnimation->Sprites[mCurrentAnimation->CurrentSpriteIndex];
		Transform2D transform(mPosition, 0, SpriteScale);

		DrawSprite(*sprite, transform);
	}

	/************************************************************************/
	const Perks& Player::GetPerks() const
	{
		return mPerks;
	}

	/************************************************************************/
	bool Player::RemoveBomb(const Bomb& bomb)
	{
		for (auto it = mBombs.begin(); it != mBombs.end(); ++it)
		{
			if ((*it).get() == &bomb)
			{
				//LevelManager::GetInstance().RemoveBomb(*it);
				mBombs.erase(it);
				return true;
			}
		}
		return false;
	}

	/************************************************************************/
	Map& Player::GetMap()
	{
		return mMap.GetMap();
	}

	/************************************************************************/
	MapRenderable& Player::GetMapRenderable()
	{
		return mMap;
	}

	/************************************************************************/
	void Player::InitializeSprites()
	{
		mRenderableSpriteSheet = SpriteSheetParser::GetInstance().ParseSpriteSheet(kJSONFilePath);
		mRenderableSpriteSheet.Animations[kDeathAnimationName]->AnimationLength = 0.3;
		mCurrentAnimation = mRenderableSpriteSheet.Animations[kIdleRightAnimationName];
	}

	/************************************************************************/
	void Player::ProcessInput()
	{
		if (!((mKeyBoard->IsKeyDown(Keys::A) || mKeyBoard->IsKeyDown(Keys::Left) || mGamePad->IsButtonDown(GamePadButtons::DPadLeft))
			  && (mKeyBoard->IsKeyDown(Keys::D) || mKeyBoard->IsKeyDown(Keys::Right) || mGamePad->IsButtonDown(GamePadButtons::DPadRight))))
		{
			mCurrentMovementState.GoingLeft = mKeyBoard->IsKeyDown(Keys::A) || mKeyBoard->IsKeyDown(Keys::Left) || mGamePad->IsButtonDown(GamePadButtons::DPadLeft);
			mCurrentMovementState.GoingRight = mKeyBoard->IsKeyDown(Keys::D) || mKeyBoard->IsKeyDown(Keys::Right) || mGamePad->IsButtonDown(GamePadButtons::DPadRight);
		}

		if (!((mKeyBoard->IsKeyDown(Keys::W) || mKeyBoard->IsKeyDown(Keys::Up) || mGamePad->IsButtonDown(GamePadButtons::DPadUp))
			  && (mKeyBoard->IsKeyDown(Keys::S) || mKeyBoard->IsKeyDown(Keys::Down) || mGamePad->IsButtonDown(GamePadButtons::DPadDown))))
		{
			mCurrentMovementState.GoingUp = mKeyBoard->IsKeyDown(Keys::W) || mKeyBoard->IsKeyDown(Keys::Up) || mGamePad->IsButtonDown(GamePadButtons::DPadUp);
			mCurrentMovementState.GoingDown = mKeyBoard->IsKeyDown(Keys::S) || mKeyBoard->IsKeyDown(Keys::Down) || mGamePad->IsButtonDown(GamePadButtons::DPadDown);
		}

		if (mKeyBoard->IsKeyDown(Keys::Z) && !mKeyBoard->IsKeyHeldDown(Keys::Z)
			|| mGamePad->IsButtonDown(GamePadButtons::A) && !mGamePad->IsButtonHeldDown(GamePadButtons::A))
		{
			PlaceBomb();
		}

		if (mKeyBoard->IsKeyDown(Keys::X) && !mKeyBoard->IsKeyHeldDown(Keys::X)
			|| mGamePad->IsButtonDown(GamePadButtons::B) && !mGamePad->IsButtonHeldDown(GamePadButtons::B))
		{
			ExplodeBombs();
		}
	}

	/************************************************************************/
	void Player::UpdateVelocity()
	{
		mVelocity = { 0, 0 };
		if (!mCurrentMovementState.IsMoving())
		{
			mCurrentPlayerState = PlayerState::Idle;
		}
		else
		{
			if (mCurrentMovementState.IsMovingOnX())
			{
				float_t xMult = mCurrentMovementState.GoingRight ? 1.0f : -1.0f;
				mVelocity.x = (mBaseSpeed.x + mPerks.Skate * kSpeedIncrement.x) * xMult;
			}

			if (mCurrentMovementState.IsMovingOnY())
			{
				float_t yMult = mCurrentMovementState.GoingUp ? 1.0f : -1.0f;
				mVelocity.y = (mBaseSpeed.y + mPerks.Skate * kSpeedIncrement.y) * yMult;
			}
			mCurrentPlayerState = PlayerState::Moving;
		}
	}

	/************************************************************************/
	void Player::UpdateAnimation(const StepTimer& timer)
	{
		switch (mCurrentPlayerState)
		{
			case DirectXGame::PlayerState::Idle:
			{
				HandleIdleStateAnimationUpdate(timer);
				break;
			}

			case DirectXGame::PlayerState::Moving:
			{
				HandleMovingStateAnimationUpdate(timer);
				break;
			}

			case DirectXGame::PlayerState::Dying:
			{
				HandleDyingStateAnimationUpdate(timer);
				break;
			}

			case DirectXGame::PlayerState::Dead:
			default:
				break;
		}
	}

	/************************************************************************/
	VelocityRestrictions Player::CheckCollisions(const DX::StepTimer& timer)
	{
		VelocityRestrictions restrictions;
		XMFLOAT2 frameVelocity(static_cast<float_t>(mVelocity.x * timer.GetElapsedSeconds()), static_cast<float_t>(mVelocity.y * timer.GetElapsedSeconds()));
		PlayerCollisionType collsionType = CollisionManager::GetInstance().PlayerCollisionCheck(mPosition, frameVelocity, restrictions);

		switch (collsionType)
		{
			case DirectXGame::PlayerCollisionType::None:
			{
				break;
			}

			case DirectXGame::PlayerCollisionType::Map:
			{
				break;
			}

			case DirectXGame::PlayerCollisionType::BombAE:
			{
				mAnimationTimer = 0;
				mCurrentAnimation = mRenderableSpriteSheet.Animations[kDeathAnimationName];
				mCurrentPlayerState = PlayerState::Dying;
				break;
			}
			case DirectXGame::PlayerCollisionType::Enemy:
			{
				break;
			}
			case DirectXGame::PlayerCollisionType::Perk:
			{
				ApplyPerk();
				break;
			}
			case DirectXGame::PlayerCollisionType::Door:
			{
				if (mMap.IsPerkConsumed())
				{
					mCurrentPlayerState = PlayerState::Dead;
				}
				break;
			}
			default:
				break;
		}
		return restrictions;
	}

	/************************************************************************/
	void Player::UpdatePosition(const StepTimer& timer, const VelocityRestrictions& velocityRestrictions)
	{
		bool xUpdated = false;
		bool yUpdated = false;

		if (velocityRestrictions.CanMoveOnX)
		{
			mPosition.x = static_cast<float_t>(mPosition.x + mVelocity.x * timer.GetElapsedSeconds());
			xUpdated = true;
		}
		else
		{
			if (mVelocity.y == 0 || mVelocity.y > 0 && velocityRestrictions.YVel > 0 || mVelocity.y < 0 && velocityRestrictions.YVel < 0)
			{
				mPosition.y = static_cast<float_t>(mPosition.y + (mBaseSpeed.y + mPerks.Skate * kSpeedIncrement.y) * velocityRestrictions.YVel * timer.GetElapsedSeconds());
				yUpdated = true;
			}
		}
		if (!yUpdated && velocityRestrictions.CanMoveOnY)
		{
			mPosition.y = static_cast<float_t>(mPosition.y + mVelocity.y * timer.GetElapsedSeconds());
		}
		else
		{
			if (!xUpdated && (mVelocity.x == 0 || mVelocity.x > 0 && velocityRestrictions.XVel > 0 || mVelocity.x < 0 && velocityRestrictions.XVel < 0))
			{
				mPosition.x = static_cast<float_t>(mPosition.x + (mBaseSpeed.x + mPerks.Skate * kSpeedIncrement.x) * velocityRestrictions.XVel * timer.GetElapsedSeconds());
			}
		}
	}

	/************************************************************************/
	void Player::HandleIdleStateAnimationUpdate(const StepTimer& timer)
	{
		UNREFERENCED_PARAMETER(timer);

		if (mPreviousMovementState.IsMoving())
		{
			mCurrentAnimation->CurrentSpriteIndex = 0;
			if (mPreviousMovementState.IsMovingOnX())
			{
				mCurrentAnimation = mPreviousMovementState.GoingRight ?
					mRenderableSpriteSheet.Animations[kIdleRightAnimationName] : mRenderableSpriteSheet.Animations[kIdleLeftAnimationName];
			}
			if (mPreviousMovementState.IsMovingOnY())
			{
				mCurrentAnimation = mPreviousMovementState.GoingUp ?
					mRenderableSpriteSheet.Animations[kIdleUpAnimationName] : mRenderableSpriteSheet.Animations[kIdleDownAnimationName];
			}
		}

		mPreviousMovementState = mCurrentMovementState;
	}

	/************************************************************************/
	void Player::HandleMovingStateAnimationUpdate(const StepTimer& timer)
	{
		// same direction
		if (mCurrentMovementState == mPreviousMovementState)
		{
			mAnimationTimer += timer.GetElapsedSeconds();

			if (mAnimationTimer > mCurrentAnimation->AnimationLength)
			{
				mAnimationTimer -= mCurrentAnimation->AnimationLength;
				mCurrentAnimation->CurrentSpriteIndex = (mCurrentAnimation->CurrentSpriteIndex + 1) % mCurrentAnimation->Sprites.size();
			}
		}
		else
		{
			mCurrentAnimation->CurrentSpriteIndex = 0;
			mAnimationTimer = 0;
			if (mCurrentMovementState.IsMovingOnX())
			{
				mCurrentAnimation = mCurrentMovementState.GoingRight ?
					mRenderableSpriteSheet.Animations[kWalkingRightAnimationName] : mRenderableSpriteSheet.Animations[kWalkingLeftAnimationName];
			}
			if (mCurrentMovementState.IsMovingOnY())
			{
				mCurrentAnimation = mCurrentMovementState.GoingUp ?
					mRenderableSpriteSheet.Animations[kWalkingUpAnimationName] : mRenderableSpriteSheet.Animations[kWalkingDownAnimationName];
			}
		}

		mPreviousMovementState = mCurrentMovementState;
	}

	/************************************************************************/
	void Player::HandleDyingStateAnimationUpdate(const StepTimer & timer)
	{
		mAnimationTimer += timer.GetElapsedSeconds();

		if (mAnimationTimer > mCurrentAnimation->AnimationLength)
		{
			// last sprite
			if (mCurrentAnimation->CurrentSpriteIndex == mCurrentAnimation->Sprites.size() - 1)
			{
				SetVisible(false);
				mCurrentPlayerState = PlayerState::Dead;
				mCurrentAnimation->CurrentSpriteIndex = 0;
			}
			else
			{
				mAnimationTimer -= mCurrentAnimation->AnimationLength;
				++mCurrentAnimation->CurrentSpriteIndex;
			}
		}
	}

	/************************************************************************/
	void Player::ApplyPerk()
	{
		if (mMap.IsPerkConsumed())
		{
			return;
		}

		auto perkIndex = static_cast<PerksIndicesInSpriteSheet>(mMap.GetMap().PerkTile.SpriteIndex);

		switch (perkIndex)
		{
			case DirectXGame::PerksIndicesInSpriteSheet::BombUp:
			{
				++mPerks.BombUp;
				break;
			}

			case DirectXGame::PerksIndicesInSpriteSheet::Fire:
			{
				++mPerks.Fire;
				break;
			}

			case DirectXGame::PerksIndicesInSpriteSheet::PassBomb:
			{
				mPerks.PassBomb = true;
				break;
			}

			case DirectXGame::PerksIndicesInSpriteSheet::PassSoftBlock:
			{
				mPerks.PassSoftBlocks = true;
				break;
			}

			case DirectXGame::PerksIndicesInSpriteSheet::Remote:
			{
				mPerks.Remote = true;
				break;
			}

			case DirectXGame::PerksIndicesInSpriteSheet::Skate:
			{
				++mPerks.Skate;
				break;
			}

			default:
				break;
		}

		mMap.PerkConsumed();
	}

	/************************************************************************/
	void Player::PlaceBomb()
	{
		if (mBombs.size() <= mPerks.BombUp)
		{
			for (auto bomb : mBombs)
			{
				XMUINT2 bombTile = GetTileFromPosition(bomb->Position());
				XMUINT2 playerTile = GetTileFromPosition(mPosition);

				if (bombTile.x == playerTile.x && bombTile.y == playerTile.y)
				{
					return;
				}
			}

			auto bomb = make_shared<Bomb>(mDeviceResources, mCamera, *this);
			mBombs.push_back(bomb);
			LevelManager::GetInstance().AddBomb(bomb);
		}
	}

	/************************************************************************/
	void Player::ExplodeBombs()
	{
		if (mPerks.Remote)
		{
			vector<shared_ptr<Bomb>> vect = mBombs;
			for (auto& bomb : vect)
			{
				bomb->Explode();
			}
		}
	}
}