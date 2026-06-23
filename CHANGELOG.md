# Changelog

All notable changes to this project are documented here. This project adheres to
[Semantic Versioning](https://semver.org/).

## 2.0.0

First release of **YAWF** (Yet Another WhatsApp Fork), forked from
[WasIstLos](https://github.com/xeco23/WasIstLos) and rebranded to
`io.github.wahid7852.YAWF`.

### Added
* Automatic recovery from WhatsApp Web's crash screen, with reload backoff.
* Paste images from the clipboard straight into the composer (`Ctrl+V` /
  `Ctrl+Shift+V`).
* Capture a screen region directly into a chat (`Ctrl+Shift+S`), auto-detecting
  the desktop's screenshot tool (Spectacle / gnome-screenshot / grim+slurp /
  maim / flameshot / ImageMagick).
* Telegram-style markdown shortcuts: bold (`Ctrl+B`), italic (`Ctrl+I`),
  strikethrough (`Ctrl+Shift+X`), monospace (`Ctrl+Shift+M`), clear formatting
  (`Ctrl+Shift+N`).
* Optional "send with `Ctrl+Enter`" composer mode (Preferences).
* One-time migration of existing `wasistlos` config/session/settings on first run.
* `whatsapp:` scheme links open/raise the app; group-invite links are copied to
  the clipboard with a notification (WhatsApp Web can't join them directly).
* Automatic recovery from WhatsApp's "A database error occurred" screen by clearing
  the corrupted local storage and reloading for a clean re-link.

### Changed
* Rebranded to YAWF; binary is now `yawf`.
* The linked device now identifies as Chrome on Linux instead of "Safari (Mac OS)",
  which also makes device linking reliable.
* No longer force-kills the web process under memory pressure — that was corrupting
  WhatsApp's local database and logging you out / breaking device linking.
* Disabled the DMABUF renderer to avoid a libEGL teardown crash on Intel/Mesa.
* Faster warm reloads via the web-browser cache model.
* Smoother UI: the crash-screen watcher no longer forces a layout reflow on a timer.
* GStreamer logging capped by default and the logger pipe made non-blocking so a
  log flood can't stall the renderer.

### Credits
* Original application by [xeco23 (Enes Hecan)](https://github.com/xeco23) and
  contributors.
