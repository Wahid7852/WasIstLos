#pragma once

#include <optional>
#include <string>

namespace wil::util
{
    void redirectOutputToLogger();

    // Capture an interactively-selected screen region to a temporary PNG and return its path
    // (caller owns the file and should unlink it). Picks the first available capture tool so it
    // works across desktops (KDE/GNOME/wlroots/X11). Returns nullopt if the capture was cancelled
    // or failed; `toolMissing` is set to true only when no supported tool is installed (so callers
    // can warn about that case without nagging on a normal cancel).
    std::optional<std::string> captureScreenRegionToPng(bool& toolMissing);
}
