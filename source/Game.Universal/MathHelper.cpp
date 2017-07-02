#include "pch.h"
#include "MathHelper.h"

using namespace std;

namespace DirectXGame
{
	/************************************************************************/
	MathHelper& MathHelper::GetInstance()
	{
		static MathHelper sInstance;

		return sInstance;
	}

	/************************************************************************/
	uint32_t MathHelper::GetRangedRandom(const uint32_t max, const uint32_t min)
	{
		uniform_int_distribution<uint32_t> dist(min, max);
		return dist(mRandomGenerator);
	}

	/************************************************************************/
	uint16_t MathHelper::GetRangedRandom(const uint16_t max, const uint16_t min)
	{
		uniform_int_distribution<uint16_t> dist(min, max);
		return dist(mRandomGenerator);
	}

	/************************************************************************/
	uint8_t MathHelper::GetRangedRandom(const uint8_t max, const uint8_t min)
	{
		return static_cast<uint8_t>(GetRangedRandom(static_cast<uint16_t>(max), static_cast<uint16_t>(min)));
	}

	/************************************************************************/
	int32_t MathHelper::GetRangedRandom(const int32_t max, const int32_t min)
	{
		uniform_int_distribution<int32_t> dist(min, max);
		return dist(mRandomGenerator);
	}

	/************************************************************************/
	int16_t MathHelper::GetRangedRandom(const int16_t max, const int16_t min)
	{
		uniform_int_distribution<int16_t> dist(min, max);
		return dist(mRandomGenerator);
	}

	/************************************************************************/
	int8_t MathHelper::GetRangedRandom(const int8_t max, const int8_t min)
	{
		return static_cast<int8_t>(GetRangedRandom(static_cast<int16_t>(max), static_cast<int16_t>(min)));
	}

	/************************************************************************/
	float_t MathHelper::GetRangedRandom(const float_t max, const float_t min)
	{
		uniform_real_distribution<float_t> dist(min, max);
		return dist(mRandomGenerator);
	}

	/************************************************************************/
	MathHelper::MathHelper() :
		mRandomGenerator(mRandomDevice())
	{
	}


}
