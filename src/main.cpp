#include <algorithm>
#include <clocale>
#include <string>
#include <vector>
#include <glibmm/i18n.h>
#include <webkit2/webkit2.h>
#include "Config.hpp"
#include "ui/Application.hpp"
#include "util/Helper.hpp"
#include "util/Profile.hpp"

namespace
{
    // Consume "--profile <name>" (so multiple accounts can run side by side) before GTK parses
    // the arguments, compacting it out of argv. Returns the remaining argument count.
    int extractProfile(int argc, char** argv)
    {
        auto remaining = std::vector<char*>{};
        for (int i = 0; i < argc; ++i)
        {
            if (std::string{argv[i]} == "--profile" && i + 1 < argc)
            {
                wil::util::profileName() = argv[++i];
                continue;
            }
            remaining.push_back(argv[i]);
        }

        std::copy(remaining.begin(), remaining.end(), argv);
        return static_cast<int>(remaining.size());
    }

    void sigterm(int)
    {
        wil::ui::Application::getInstance().quit();
    }

    // Bound the web process so a long-running WhatsApp Web session releases caches and
    // gets garbage-collected instead of ballooning to several GB. Must be configured
    // before the first web context (i.e. the first WebView) is created.
    void applyMemoryPressureSettings()
    {
        // Only ask WebKit to release caches under memory pressure; never set a kill threshold.
        // SIGKILLing the web process mid-IndexedDB-write corrupts WhatsApp's local database (the
        // "A database error occurred on your browser" screen) and logs the user out, so we leave
        // the kill threshold at its default of 0 (disabled). Thresholds are set high so a normal
        // ~1-1.5 GB session never triggers routine cache churn. Strict before conservative: the
        // setters assert conservative < strict.
        WebKitMemoryPressureSettings* const settings = webkit_memory_pressure_settings_new();
        webkit_memory_pressure_settings_set_memory_limit(settings, 8192U);           // MB accounting base
        webkit_memory_pressure_settings_set_strict_threshold(settings, 0.85);        // release all caches
        webkit_memory_pressure_settings_set_conservative_threshold(settings, 0.65);  // start releasing caches
        webkit_memory_pressure_settings_set_poll_interval(settings, 30.0);           // seconds

        webkit_website_data_manager_set_memory_pressure_settings(settings);
        webkit_memory_pressure_settings_free(settings);
    }

    // On this stack (Intel + WebKitGTK's GStreamer GL video sink under wlroots) hardware-decoded
    // frames arrive as DMABuf/external-OES textures the GL sink cannot map ("Cannot map External OES
    // textures"), so WhatsApp videos render glitched or not at all. Route video through a plain
    // system-memory path: disable the GL video sink (WebKit falls back to an appsink the compositor
    // reads) and derank the VA decoders so software decode produces mappable frames. The page's
    // DMABUF *renderer*, which keeps typing/scrolling smooth, is a separate subsystem and stays on;
    // this only adds CPU while a video is actually playing. Set here in the UI process before any web
    // process is spawned so the child inherits them; an explicit user override of either is respected.
    void applyVideoWorkarounds()
    {
        g_setenv("WEBKIT_GST_DISABLE_GL_SINK", "1", FALSE);
        g_setenv("GST_PLUGIN_FEATURE_RANK", "vah264dec:NONE,vah265dec:NONE,vavp8dec:NONE,vavp9dec:NONE,vaav1dec:NONE", FALSE);

        // Keep GStreamer at error level by default so it can't flood the logger; only set when the
        // user hasn't asked for verbose output, so an explicit GST_DEBUG is still honored.
        g_setenv("GST_DEBUG", "1", FALSE);
    }
}

int main(int argc, char** argv)
{
    argc = extractProfile(argc, argv);

    wil::util::migrateLegacyUserData();

    applyVideoWorkarounds();
    applyMemoryPressureSettings();

    setlocale(LC_ALL, "");

    bindtextdomain(GETTEXT_PACKAGE, WIL_LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    auto app = wil::ui::Application{argc, argv};

    wil::util::redirectOutputToLogger();

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);
    signal(SIGPIPE, SIG_IGN);

    return app.run();
}
