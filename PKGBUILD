pkgname=gotunos-installer
pkgver=1.0
pkgrel=3
pkgdesc="Elite gotunOS-x8664 System Installer"
arch=('x86_64')
url="https://yourlink.com"
license=('GPL')
depends=('yay' 'flatpak')
makedepends=('rust' 'cargo')
source=()

build() {
  cd "$startdir"
  cargo build --release
}

package() {
  cd "$startdir"
  # Install the binary
  install -Dm755 "target/release/gotunos_installer" "$pkgdir/usr/bin/gotunos-installer"
  
  # Create a directory for assets
  install -d "$pkgdir/usr/share/gotunos-installer"
  
  # Install scripts and logo
  install -m755 carnage.sh "$pkgdir/usr/share/gotunos-installer/carnage.sh"
  install -m755 NovaChoom.sh "$pkgdir/usr/share/gotunos-installer/NovaChoom.sh"
  install -m755 lunarglass.sh "$pkgdir/usr/share/gotunos-installer/lunarglass.sh"
  install -m755 PS4.sh "$pkgdir/usr/share/gotunos-installer/PS4.sh"
  install -m644 gotunos_logo.txt "$pkgdir/usr/share/gotunos-installer/gotunos_logo.txt"

  # Install Desktop Entry
  install -Dm644 gotunos-installer.desktop "$pkgdir/usr/share/applications/gotunos-installer.desktop"
  
  install -d "$pkgdir/etc/skel/Desktop"
  install -m644 gotunos-installer.desktop "$pkgdir/etc/skel/Desktop/gotunos-installer.desktop"
}
