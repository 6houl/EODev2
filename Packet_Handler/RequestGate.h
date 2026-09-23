#pragma once

#include <atomic>

class RequestGate
{
public:
	bool Begin(unsigned long now)
	{
		bool expected = false;
		if (!pending_.compare_exchange_strong(expected, true))
			return false;

		started_.store(now);
		return true;
	}

	bool Expired(unsigned long now, unsigned long timeout) const
	{
		return pending_.load() && now - started_.load() >= timeout;
	}

	void Complete()
	{
		pending_.store(false);
	}

	bool Pending() const
	{
		return pending_.load();
	}

private:
	std::atomic<bool> pending_{false};
	std::atomic<unsigned long> started_{0};
};
