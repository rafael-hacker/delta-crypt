#!/usr/bin/env bash
# Instala as dependências (pkgconf e libsodium), compila e instala o delta-crypt.
set -euo pipefail

if [[ ! -r /etc/os-release ]]; then
    echo "Error: /etc/os-release not found, could not detect the distro." >&2
    exit 1
fi

. /etc/os-release
distro="${ID:-} ${ID_LIKE:-}"

SUDO=""
if [[ $EUID -ne 0 ]]; then
    SUDO="sudo"
fi

echo "==> Detected distro: ${PRETTY_NAME:-$ID}"

case "$distro" in
    *arch*|*manjaro*|*endeavouros*|*cachyos*)
        $SUDO pacman -S --needed --noconfirm pkgconf libsodium cmake gcc make git
        ;;
    *debian*|*ubuntu*|*mint*|*pop*)
        $SUDO apt-get update
        $SUDO apt-get install -y pkgconf libsodium-dev cmake g++ make git
        ;;
    *fedora*|*rhel*|*centos*|*rocky*|*almalinux*)
        $SUDO dnf install -y pkgconf-pkg-config libsodium-devel cmake gcc-c++ make git
        ;;
    *opensuse*|*suse*|*sles*)
        $SUDO zypper --non-interactive install pkgconf libsodium-devel cmake gcc-c++ make git
        ;;
    *alpine*)
        $SUDO apk add --no-cache pkgconf libsodium-dev cmake g++ make git
        ;;
    *void*)
        $SUDO xbps-install -Sy pkgconf libsodium-devel cmake gcc make git
        ;;
    *gentoo*)
        $SUDO emerge --noreplace dev-util/pkgconf dev-libs/libsodium dev-build/cmake dev-vcs/git
        ;;
    *)
        echo "Error: distro '${ID:-unknown}' is not supported. Please install pkgconf and libsodium manually." >&2
        exit 1
        ;;
esac

# Garante que o build rode a partir da raiz do projeto, onde está este script
cd "$(dirname "$(readlink -f "$0")")"

echo "==> Configuring the project"
cmake -B build

echo "==> Building"
cmake --build build

echo "==> Installing"
sudo cmake --install build

echo "==> Done!"
