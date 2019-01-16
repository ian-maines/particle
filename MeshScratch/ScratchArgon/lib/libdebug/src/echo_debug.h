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

std::unique_ptr<IDebugLogging> _logger (new CCloudPublishLogger ());
}

void InitializeLogger (const String& name)
    {
    debug::_logger = std::move(std::unique_ptr<debug::IDebugLogging>(new debug::CCloudPublishLogger (name)));
    }

void SetLogger (std::unique_ptr<debug::IDebugLogging>&& logger)
    {
    debug::_logger = std::move(logger);
    }

#undef ASSERT

#ifdef _DEBUG
// FIXME: Get in impl file so we can hide some of this stuff.
#define _ASSERT(cond) \
do {if (!(cond)) debug::_logger->LogMsg(String::format("ASSERT (%s) Failed at %u in %s", (#cond), __LINE__, __FILE__));} while (false)

#define _VERIFY(cond) _ASSERT(cond);
#else
#define _ASSERT(cond)
#define _VERIFY(cond) cond;
#endif


#endif  // __LIBDEBUG_DEBUG_H__
