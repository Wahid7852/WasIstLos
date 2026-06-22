#include "Helper.hpp"
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <giomm/file.h>
#include "Config.hpp"

namespace wil::util
{
    void redirectOutputToLogger()
    {
        if (!Gio::File::create_for_path("/dev/log")->query_exists() && !Gio::File::create_for_path("/var/run/syslog")->query_exists())
        {
            std::cerr << "Skipping redirection of output to logger since the user doesn't have an active syslog" << std::endl;
            return;
        }

        // No "-s": that echoes every line back to stderr, doubling volume and risking duplication.
        auto const fl = ::popen("logger -i -t " WIL_NAME, "w");
        if (!fl)
        {
            auto const errorNumber = errno;
            std::cerr << "Failed to open pipe to logger: " << strerror(errorNumber) << std::endl;
            return;
        }

        auto const fd = ::fileno(fl);
        ::dup2(fd, STDERR_FILENO);

        // The web process inherits this stderr. A flood (e.g. GST_DEBUG) must never block it on a
        // full pipe and stall the renderer, so make writes non-blocking: under flood they drop with
        // EAGAIN instead of stalling. Dropped log lines are an acceptable trade for a responsive UI.
        ::fcntl(STDERR_FILENO, F_SETFL, O_NONBLOCK);
    }
}
