#pragma once

#include <random>

namespace DirectXGame
{
	class MathHelper final
	{
	public:

		MathHelper(const MathHelper&) = delete;
		MathHelper(const MathHelper&&) = delete;
		MathHelper& operator=(const MathHelper&) = delete;
		MathHelper& operator=(const MathHelper&&) = delete;

		static MathHelper& GetInstance();

		uint32_t GetRangedRandom(const uint32_t max, const uint32_t min = 0);
		uint16_t GetRangedRandom(const uint16_t max, const uint16_t min = 0);
		uint8_t GetRangedRandom(const uint8_t max, const uint8_t min = 0);
		int32_t GetRangedRandom(const int32_t max, const int32_t min);
		int16_t GetRangedRandom(const int16_t max, const int16_t min);
		int8_t GetRangedRandom(const int8_t max, const int8_t min);
		float_t GetRangedRandom(const float_t max, const float_t min);

	private:

		MathHelper();
		~MathHelper() = default;

		std::random_device mRandomDevice;
		std::default_random_engine mRandomGenerator;
	};
}