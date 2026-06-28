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

Both Snap Store and PPA publishing are **automated** by the `Release` workflow
once the one-time prerequisites below are complete.

### Snap Store (automated after first-time setup)

The workflow uses `snapcore/action-publish@v1`.

**One-time setup:**

1. Register the snap name:
   ```bash
   snapcraft register yawf
   ```
2. Export CI credentials (scoped to this snap only):
   ```bash
   snapcraft export-login \
     --snaps=yawf \
     --acls package_access,package_push,package_update,package_release \
     store-credentials.txt
   cat store-credentials.txt   # copy the output
   ```
3. Add the output as a GitHub repository secret named **`STORE_LOGIN`**
   (Settings → Secrets and variables → Actions → New repository secret).

The workflow then publishes every release to the **stable** channel automatically.

### Launchpad PPA (automated after first-time setup)

The workflow uses `debuild -S` + `dput` to upload a source package; Launchpad
builds the binary server-side.

**One-time setup:**

1. Create a Launchpad account and a PPA at
   `https://launchpad.net/~wahid2584/+archive/ubuntu/yawf`.
2. Generate a GPG key (skip if you already have one):
   ```bash
   gpg --gen-key
   ```
3. Publish the key to the Ubuntu keyserver:
   ```bash
   gpg --keyserver keyserver.ubuntu.com --send-keys YOUR_KEY_ID
   ```
4. Add the key fingerprint to your Launchpad account at
   `https://launchpad.net/~/+editpgpkeys` and confirm the encrypted email.
5. Export the private key as base64 for GitHub:
   ```bash
   gpg --export-secret-keys --armor YOUR_KEY_ID | base64 -w0
   ```
6. Add two GitHub repository secrets:
   - **`PPA_GPG_PRIVATE_KEY`** — the base64 output from step 5
   - **`PPA_GPG_KEY_ID`** — your key ID (16-char hex), e.g. `C51AF2B42BC70DA7`

The workflow stamps the changelog with `~noble1` and the `noble` series, builds
a signed source package, and uploads it to `ppa:wahid7852/yawf`.

### AUR

Update `pkgver` and the real `sha256sums` in `packaging/aur/PKGBUILD`
(`updpkgsums`), regenerate `.SRCINFO` (`makepkg --printsrcinfo > .SRCINFO`),
and push to the `yawf` AUR repo.

### Flathub

Fill in real `sha256`/commits in `packaging/flatpak/io.github.wahid7852.YAWF.yml`,
test with `flatpak-builder`, then open a PR to the `flathub/flathub` repo.

## Pre-release checks

```bash
cmake -S . -B build && cmake --build build
desktop-file-validate build/resource/io.github.wahid7852.YAWF.desktop
appstreamcli validate build/resource/io.github.wahid7852.YAWF.metainfo.xml
```
