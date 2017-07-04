#include "pch.h"
#include "LevelManager.h"
#include "GameMain.h"
#include "Bomb.h"

using namespace std;
using namespace DirectX;

namespace DirectXGame
{
	/************************************************************************/
	LevelManager& LevelManager::GetInstance()
	{
		static LevelManager sInstance;
		return sInstance;
	}

	/************************************************************************/
	void LevelManager::SetGameMain(GameMain& gameMain)
	{
		mGameMain = &gameMain;
	}

	/************************************************************************/
	void LevelManager::AddBomb(const shared_ptr<Bomb>& bomb)
	{
		mBombs.push_back(bomb);
		mGameMain->AddComponent(bomb);
	}

	/************************************************************************/
	bool LevelManager::RemoveBomb(const Bomb& bomb)
	{
		for (auto it = mBombs.begin(); it != mBombs.end(); ++it)
		{
			if ((*it).get() == &bomb)
			{
				mBombs.erase(it);
				mGameMain->RemoveComponent(bomb);

				return true;
			}
		}
		return false;
	}

	/************************************************************************/
	void LevelManager::AddBombAE(const XMUINT2& bombAE)
	{
		mBombsAE.push_back(bombAE);
	}

	/************************************************************************/
	bool LevelManager::RemoveBombAE(const XMUINT2& bombAE)
	{
		for (auto it = mBombsAE.begin(); it != mBombsAE.end(); ++it)
		{
			if ((*it).x == bombAE.x && (*it).y == bombAE.y)
			{
				mBombsAE.erase(it);
				return true;
			}
		}
		return false;
	}
}