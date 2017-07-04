#pragma once

#include "Renderable.h"

namespace DirectXGame
{
	enum class BombState
	{
		Ticking,
		Exploding,
		Vanished
	};

	struct ExplosionAE
	{
		ExplosionAE()
		{
		}

		ExplosionAE(const std::shared_ptr<Animation>& anim, const DirectX::XMFLOAT2 position):
			Anim(*anim), Position(position), AnimTimer(0), AnimEnded(false)
		{
		}

		Animation Anim;
		DirectX::XMFLOAT2 Position;
		double_t AnimTimer;
		bool AnimEnded;
	};

	class Player;

	/** Class representing a renderable bomb in the game.
	*/
	class Bomb final : public Renderable
	{
	public:

		Bomb(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, Player& player, 
			 const std::string& jsonPath = kBombJSONFilePath, const std::wstring& textureMapPath = kBombTextureMapPath);
		~Bomb();

		virtual void Update(const DX::StepTimer& timer) override;
		virtual void Render(const DX::StepTimer& timer) override;

		void Explode();

	protected:

		virtual void InitializeSprites() override;

	private:

		void UpdateAnimation(const DX::StepTimer& timer);
		void UpdateTickingAnimation(const DX::StepTimer& timer);
		void UpdateExplosionAnimation(const DX::StepTimer& timer);
		void Vanish();

		bool AddExplosionAE(const ExplosionAE& explosionAE);

		Player& mPlayer;
		BombState mCurrentState;
		double_t mExplosionTimer;
		bool mIsRemoteControlled;

		static const double_t kBombExplosionTime;

		// rendering
		static const std::string kBombJSONFilePath;
		static const std::wstring kBombTextureMapPath;
		static const std::string kBombAEJSONFilePath;
		static const std::wstring kBombAETextureMapPath;
		SpriteSheet mBombSpriteSheet;
		SpriteSheet mBombAESpriteSheet;

		// animation
		std::shared_ptr<Animation> mTickingAnimation;
		double_t mTickingAnimationTimer;
		std::vector<ExplosionAE> mExplosionAEs;

		static const double_t kBombAnimationTime;
		static const double_t kBombAEAnimationTime;

		static const std::string kBombTickingAnimationName;
		static const std::string kBombAEBottomAnimationName;
		static const std::string kBombAECenterAnimationName;
		static const std::string kBombAEHorizAnimationName;
		static const std::string kBombAELeftAnimationName;
		static const std::string kBombAERightAnimationName;
		static const std::string kBombAETopAnimationName;
		static const std::string kBombAEVertAnimationName;
	};
}