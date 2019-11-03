# Maintainer: Vincent Davis <vincedav2495@gmail.com>
pkgname='lucurious'
pkgver=r110.e6fe16e
pkgrel=1
license=('MIT')
pkgdesc='[Desktop Engine, Library] for building and styling 2D/3D Vulkan Wayland Compositors'
url='https://github.com/EasyIP2023/lucurious'
arch=('x86_64')
provides=("lucurious=${pkgver%%.r*}")
conflicts=('lucurious')
makedepends=('meson' 'ninja' 'git' 'wayland-protocols')
source=("${pkgname}::git+${url}")
sha512sums=('SKIP')

# https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=wlroots-git
pkgver () {
	cd "${pkgname}"
	(
		set -o pipefail
		git describe --long 2>/dev/null | sed 's/\([^-]*-g\)/r\1/;s/-/./g' ||
		printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
	)
}

build () {
	cd "${pkgname}"
	rm -rf build
	meson build
	ninja -C build
}

package () {
	cd "${pkgname}"
	DESTDIR="${pkgdir}" ninja -C build install
	install -Dm644 LICENSE "${pkgdir}/usr/share/licenses/${pkgname}/LICENSE"
}
