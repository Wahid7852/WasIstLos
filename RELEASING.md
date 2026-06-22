# Releasing

YAWF's version lives in `CMakeLists.txt` (`project(... VERSION x.y.z)`); all
package metadata derives from it.

## Cut a release

1. Bump `VERSION` in `CMakeLists.txt` and add a matching top entry to
   `CHANGELOG.md`, the AppStream `<releases>` block in
   `resource/desktop/io.github.wahid7852.YAWF.metainfo.xml.in`, and a new
   `debian/changelog` stanza.
2. Merge to `master`, then push the **`release`** branch:
   ```bash
   git push origin master:release
   ```
   The `Release` workflow (`.github/workflows/release.yml`) builds the `.deb`,
   `.snap` and `.AppImage`, tags `vX.Y.Z`, and creates the GitHub Release with a
   generated changelog and the artifacts attached.

## Per-channel publishing

* **AUR** — update `pkgver` and the real `sha256sums` in `packaging/aur/PKGBUILD`
  (`updpkgsums`), regenerate `.SRCINFO` (`makepkg --printsrcinfo > .SRCINFO`),
  and push to the `yawf` AUR repo.
* **Snap Store** — register the `yawf` name once (`snapcraft register yawf`), then
  `snapcraft` + `snapcraft upload --release=stable *.snap`. The snap uses
  `base: core22` (required for webkit2gtk-4.1); test-build before the first upload.
* **Flathub** — fill in real `sha256`/commits in `flatpak/io.github.wahid7852.YAWF.yml`,
  test with `flatpak-builder`, then open a PR to the `flathub/flathub` repo.

## Pre-release checks

```bash
cmake -S . -B build && cmake --build build
desktop-file-validate build/resource/io.github.wahid7852.YAWF.desktop
appstreamcli validate build/resource/io.github.wahid7852.YAWF.metainfo.xml
```
