/**
 * \file
 * \author Christien Alden (34119981)
**/
#include "TimeStep.h"

namespace {
	constexpr float ratio = 1000.0f;
}

namespace AEngine
{

	TimeStep::TimeStep(Interval step)
		: m_step{ step }
	{

	}

	TimeStep::TimeStep(float seconds)
		: m_step{}
	{
		// from StackOverflow answer -> https://stackoverflow.com/a/69199519
		using namespace std::chrono;
		using fsec = duration<float>;
		m_step = round<nanoseconds>(fsec{seconds});
	}

	float TimeStep::Nanoseconds(float scale) const
	{
		return static_cast<float>(m_step.count()) * scale;
	}

	float TimeStep::Microseconds(float scale) const
	{
		return Nanoseconds(scale) / ratio;
	}

	float TimeStep::Milliseconds(float scale) const
	{
		return Microseconds(scale) / ratio;
	}

	float TimeStep::Seconds(float scale) const
	{
		return Milliseconds(scale) / ratio;
	}

	TimeStep::operator float() const
	{
		return Seconds();
	}

	TimeStep TimeStep::operator+(const TimeStep &rhs) const
	{
		return TimeStep{m_step + rhs.m_step};
	}

	TimeStep& TimeStep::operator=(const TimeStep &rhs)
	{
		if (this == &rhs)
		{
			return *this;
		}

		this->m_step = rhs.m_step;
		return *this;
	}

	TimeStep& TimeStep::operator+=(const TimeStep &rhs)
	{
		m_step = m_step + rhs.m_step;
		return *this;
	}

	TimeStep &TimeStep::operator-=(const TimeStep &rhs)
	{
		m_step = m_step - rhs.m_step;
		return *this;
	}
}
