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
        // Note: WhatsApp Web's live JS heap (~1 GB) cannot be reclaimed by cache eviction; these
        // thresholds keep caches small and, as a last resort, kill a truly runaway web process
        // (the kill is caught by WebView's crash handler, which reloads the page).
        WebKitMemoryPressureSettings* const settings = webkit_memory_pressure_settings_new();
        webkit_memory_pressure_settings_set_memory_limit(settings, 1536U);           // MB
        webkit_memory_pressure_settings_set_conservative_threshold(settings, 0.33);  // start releasing caches
        webkit_memory_pressure_settings_set_strict_threshold(settings, 0.5);         // release all caches
        webkit_memory_pressure_settings_set_kill_threshold(settings, 1.5);           // kill a runaway process
        webkit_memory_pressure_settings_set_poll_interval(settings, 15.0);           // seconds

        webkit_website_data_manager_set_memory_pressure_settings(settings);
        webkit_memory_pressure_settings_free(settings);
    }
}

int main(int argc, char** argv)
{
    argc = extractProfile(argc, argv);

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
