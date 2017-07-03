#include "pch.h"
#include "Player.h"
#include "MapParser.h"
#include "LevelGenerator.h"
#include "SpriteSheetParser.h"
#include "CollisionManager.h"

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
				   const shared_ptr<KeyboardComponent>& keyboard, XMUINT2& startTile, const string& jsonPath, const wstring& textureMapPath) :
		Renderable(deviceResources, camera, jsonPath, textureMapPath),
		mKeyBoard(keyboard),
		mCurrentPlayerState(PlayerState::Idle),
		mCurrentAnimation(nullptr),
		mAnimationTimer(0),
		mVelocity(0, 0),
		mBaseSpeed(kBaseSpeed),
		mCurrentMovementState(),
		mPreviousMovementState()
	{
		mPosition = GetPositionFromTile(startTile);
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
		Renderable::Render(timer);
		auto sprite = mCurrentAnimation->Sprites[mCurrentAnimation->CurrentSpriteIndex];
		Transform2D transform(mPosition, 0, SpriteScale);

		DrawSprite(*sprite, transform);
	}

	/************************************************************************/
	void Player::InitializeSprites()
	{
		mRenderableSpriteSheet = SpriteSheetParser::GetInstance().ParseSpriteSheet(kJSONFilePath);
		mCurrentAnimation = mRenderableSpriteSheet.Animations[kIdleRightAnimationName];
	}

	/************************************************************************/
	void Player::ProcessInput()
	{
		if (!((mKeyBoard->IsKeyDown(Keys::A) || mKeyBoard->IsKeyDown(Keys::Left)) && (mKeyBoard->IsKeyDown(Keys::D) || mKeyBoard->IsKeyDown(Keys::Right))))
		{
			mCurrentMovementState.GoingLeft = mKeyBoard->IsKeyDown(Keys::A) || mKeyBoard->IsKeyDown(Keys::Left);
			mCurrentMovementState.GoingRight = mKeyBoard->IsKeyDown(Keys::D) || mKeyBoard->IsKeyDown(Keys::Right);
		}

		if (!((mKeyBoard->IsKeyDown(Keys::W) || mKeyBoard->IsKeyDown(Keys::Up)) && (mKeyBoard->IsKeyDown(Keys::S) || mKeyBoard->IsKeyDown(Keys::Left))))
		{
			mCurrentMovementState.GoingUp = mKeyBoard->IsKeyDown(Keys::W) || mKeyBoard->IsKeyDown(Keys::Up);
			mCurrentMovementState.GoingDown = mKeyBoard->IsKeyDown(Keys::S) || mKeyBoard->IsKeyDown(Keys::Down);
		}

		// todo add z for placing bombs and x for exploding them
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
				break;
			}
			case DirectXGame::PlayerCollisionType::Enemy:
			{
				break;
			}
			case DirectXGame::PlayerCollisionType::Perk:
			{
				break;
			}
			case DirectXGame::PlayerCollisionType::Door:
			{
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
}