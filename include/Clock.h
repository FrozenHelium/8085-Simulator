#pragma once

#include "common.h"
#include <thread>


// A general representation of a system clock
class Clock
{
public:
    // Frequency in Hz at which the clock ticks
    Clock(unsigned long long tickFrequency, std::function<void()> onTickCallback)
    {
        m_onTick = onTickCallback;
        m_delayDuration = 1000000000L / tickFrequency;
    }

    // Start ticking (blocking procedure)
    // for non-blocking interface,
    // use: Clock::StartAsync()
    void Start()
    {
        m_running = true;
        while (m_running)
        {
            m_onTick();
            //std::this_thread::sleep_until()
        }
    }

    // Start ticking (asynchronous)
    void StartAsync()
    {
        //m_tickThread = std::thread(std::bind(Clock::Start, this));
    }

    // Stop ticking
    void Stop()
    {
        m_running = false;
        if (m_tickThread.joinable())
        {
            m_tickThread.join();
        }
    }

private:
    unsigned long long m_frequency;
    // delay duration in nanoseconds
    unsigned long long m_delayDuration;
    std::function<void()> m_onTick;
    bool m_running;
    std::thread m_tickThread;
};