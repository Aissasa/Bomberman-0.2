#pragma once

#include "Renderable.h"
#include "CollisionManager.h"

namespace DX
{
	class KeyboardComponent;
	class GamePadComponent;
}

namespace DirectXGame
{
	/** Enumeration representing the different states the player can have in the game.
	*/
	enum class PlayerState
	{
		Idle,
		Moving,
		Dying,
		Dead
	};

	/** Structure describing the player's movement state.
	*/
	struct PlayerMovementState
	{
		PlayerMovementState(const bool goingUp = false, const bool goingDown = false, const bool goingLeft = false, const bool goingRight = false) :
			GoingUp(goingUp),
			GoingDown(goingDown),
			GoingLeft(goingLeft),
			GoingRight(goingRight)
		{
		}

		bool operator==(const PlayerMovementState& rhs) const
		{
			return GoingDown == rhs.GoingDown && GoingUp == rhs.GoingUp && GoingLeft == rhs.GoingLeft && GoingRight == rhs.GoingRight;
		}

		bool operator!=(const PlayerMovementState& rhs) const { return !operator==(rhs); }

		bool IsMoving() const { return GoingDown || GoingLeft || GoingRight || GoingUp; }
		bool IsMovingOnX() const { return GoingLeft || GoingRight; }
		bool IsMovingOnY() const { return GoingDown || GoingUp; }

		bool GoingUp;
		bool GoingDown;
		bool GoingLeft;
		bool GoingRight;
	};

	/** Structure representing the player's perks.
	*/
	struct Perks
	{
		Perks(const uint8_t bombUp = 0, const uint8_t fire = 0, const uint8_t skate = 0, const bool remote = false,
			  const bool passBomb = false, const bool passSoftBlocks = false) :
			BombUp(bombUp),
			Fire(fire),
			Skate(skate),
			Remote(remote),
			PassBomb(passBomb),
			PassSoftBlocks(passSoftBlocks)
		{
		}

		uint8_t BombUp;
		uint8_t Fire;
		uint8_t Skate;

		bool Remote;
		bool PassBomb;
		bool PassSoftBlocks;
	};

	class Bomb;
	class MapRenderable;

	/** Class representing an animated renderable player.
	*/
	class Player final : public Renderable
	{
	public:

		Player(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, const std::shared_ptr<DX::KeyboardComponent>& keyboard, 
			   const std::shared_ptr<DX::GamePadComponent>& gamePad, MapRenderable& map, const std::string& jsonPath = kJSONFilePath, 
			   const std::wstring& textureMapPath = kTextureMapPath);
		
		virtual void Update(const DX::StepTimer& timer) override;
		virtual void Render(const DX::StepTimer& timer) override;

		const Perks& GetPerks() const;
		bool RemoveBomb(const Bomb& bomb);
		Map& GetMap();
		MapRenderable& GetMapRenderable();

	protected:

		virtual void InitializeSprites() override;

	private:

		void ProcessInput();
		void UpdateVelocity();
		void UpdateAnimation(const DX::StepTimer& timer);
		VelocityRestrictions CheckCollisions(const DX::StepTimer& timer);
		void UpdatePosition(const DX::StepTimer& timer, const VelocityRestrictions& velocityRestrictions);
		void HandleIdleStateAnimationUpdate(const DX::StepTimer& timer);
		void HandleMovingStateAnimationUpdate(const DX::StepTimer& timer);
		void HandleDyingStateAnimationUpdate(const DX::StepTimer& timer);

		void PlaceBomb();
		void ExplodeBombs();

		Perks mPerks;
		std::shared_ptr<DX::KeyboardComponent> mKeyBoard;
		std::shared_ptr<DX::GamePadComponent> mGamePad;
		MapRenderable& mMap;
		std::vector<std::shared_ptr<Bomb>> mBombs;
		PlayerState mCurrentPlayerState;

		// movement
		DirectX::XMFLOAT2 mVelocity;
		DirectX::XMFLOAT2 mBaseSpeed;
		PlayerMovementState mCurrentMovementState;
		PlayerMovementState mPreviousMovementState;

		static const DirectX::XMFLOAT2 kBaseSpeed;
		static const DirectX::XMFLOAT2 kSpeedIncrement;
		static const float_t kButtonCooldownTime;

		// rendering
		static const std::string kJSONFilePath;
		static const std::wstring kTextureMapPath;

		// animation
		std::shared_ptr<Animation> mCurrentAnimation;
		std::double_t mAnimationTimer;

		static const double_t kDeathAnimationLength;

		static const std::string kDeathAnimationName;
		static const std::string kIdleLeftAnimationName;
		static const std::string kIdleRightAnimationName;
		static const std::string kIdleDownAnimationName;
		static const std::string kIdleUpAnimationName;
		static const std::string kWalkingLeftAnimationName;
		static const std::string kWalkingRightAnimationName;
		static const std::string kWalkingDownAnimationName;
		static const std::string kWalkingUpAnimationName;
	};
}