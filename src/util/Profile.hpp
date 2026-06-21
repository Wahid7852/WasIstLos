#pragma once

#include <string>

namespace wil::util
{
    // Name of the active account profile (empty = the default profile). Set once from the
    // "--profile <name>" command-line option before any settings or web view are created.
    inline std::string& profileName()
    {
        static std::string name;
        return name;
    }

    // Path fragment appended to the per-user config/data/cache directories so each profile
    // keeps its own settings and WhatsApp session. Empty for the default profile.
    inline std::string profilePathSuffix()
    {
        return profileName().empty() ? std::string{} : ("/profiles/" + profileName());
    }
}
