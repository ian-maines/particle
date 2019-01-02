// Copyright 2019 Ian Maines
// Debug lbrary for Particle devices

#ifndef __LIBDEBUG_DEBUG_H__
#define __LIBDEBUG_DEBUG_H__

#include <memory>

namespace debug
{
class IDebugLogging
    {
    public:
        virtual ~IDebugLogging () {};
        virtual void LogMsg (const String& str);
    };

class CCloudPublishLogger : public IDebugLogging
    {
    public:
        CCloudPublishLogger (const String& name)
            : m_name (name)
            {}
        CCloudPublishLogger ()
            : CCloudPublishLogger ("Unitialized")
            {}
            
        void LogMsg (const String& str) override
            {
            if (Particle.connected())
                {
                Particle.publish (String::format("%s::DEBUG::%s",m_name, str));
                }
            }

    private:
        const String m_name;
    };

std::unique_ptr<IDebugLogging> _logger = std::make_unique<CCloudPublishLogger> ();
}

void InitializeLogger (const String& name)
    {
    debug::_logger = std::make_unique<debug::CCloudPublishLogger> (name);
    }

void SetLogger (std::unique_ptr<debug::IDebugLogger>&& logger)
    {
    debug::_logger = std::move(logger);
    }


#define ASSERT(cond) \
do {if (cond) sizeof(int); else _logger->LogMsg(#cond);} while (true)

#endif  // __LIBDEBUG_DEBUG_H__
