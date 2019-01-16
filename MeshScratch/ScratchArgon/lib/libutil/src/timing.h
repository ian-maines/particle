// Copyright 2019 Ian Maines
// Timing lbrary for Particle devices

#ifndef __LIBUTIL_TIMING_H__
#define __LIBUTIL_TIMING_H__

#include <cstdint>

#include "echo_debug.h"

namespace util
{
    class CTiming
        {
        public:
            CTiming (unsigned int total_ms)
                : m_total_ms (total_ms)
                , m_tracked_ms (0)
                {}

            // Activate a delay for the requested # of ms if we haven't filled out our entire delay
            bool delay (unsigned int ms)
                {
                m_tracked_ms += ms;
                if (m_tracked_ms <= m_total_ms)
                    {
                    ::delay (ms);
                    return true;
                    }
                return false;
                }

            // Activate ordinary delay if time remains
            bool finalize ()
                {
                if (m_tracked_ms <= m_total_ms)
                    {
                    ::delay (m_total_ms - m_tracked_ms);
                    return true;
                    }
                return false;
                }

            // Don't activate a delay, tell us how much time is left and set us to completed
            unsigned int finalize_nop ()
                {                
                if (m_tracked_ms <= m_total_ms)
                    {
                    m_tracked_ms = m_total_ms + 1;
                    return m_tracked_ms <= m_total_ms;
                    }
                return 0;
                }

        private:
            const unsigned int m_total_ms;
            unsigned int m_tracked_ms;
        };
}

#endif  // __LIBUTIL_TIMING_H__