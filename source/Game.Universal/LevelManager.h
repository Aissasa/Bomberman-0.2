#pragma once

namespace DirectXGame
{
	class GameMain;
	class Bomb;

	/** Singleton that holds information about a level and its elements
	*/
	class LevelManager final
	{
	public:

		LevelManager(const LevelManager&) = delete;
		LevelManager(const LevelManager&&) = delete;
		LevelManager& operator=(const LevelManager&) = delete;
		LevelManager& operator=(const LevelManager&&) = delete;

		static LevelManager& GetInstance();

		void SetGameMain(GameMain& gameMain);

		std::vector<DirectX::XMUINT2> GetBombsTiles() const;
		std::vector<DirectX::XMUINT2> GetBombsAETiles() const;

		void AddBomb(const std::shared_ptr<Bomb>& bomb);
		bool RemoveBomb(const Bomb& bomb);

		void AddBombAE(const DirectX::XMUINT2& bombAE);
		bool RemoveBombAE(const DirectX::XMUINT2& bombAE);

	private:

		LevelManager() = default;
		~LevelManager() = default;

		GameMain* mGameMain;
		std::vector<std::shared_ptr<Bomb>> mBombs;
		std::vector<DirectX::XMUINT2> mBombsAE;
	};

}