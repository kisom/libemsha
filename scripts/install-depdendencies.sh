#!/usr/bin/env bash

#####################################################################
# This script attempts to install the appopriate build dependencies #
# for the host system.                                              #
#                                                                   #
# This is primarily developed on the latest Ubuntu LTS release and  #
# MacOS; other platforms are not supported.                         #
#####################################################################

set -eu

AUTOMATED_MODE=${AUTOMATED_MODE:-}

install_debianesque () {
	APTARGS=""
	SUDO="sudo"
	if [ "$(whoami)" == "root" ]
	then
	  SUDO=
  fi

	if [ ! -z "${AUTOMATED_MODE}" ]
	then
		APTARGS="-y"
	fi

	echo "[+] distribution is ${DISTRIB_ID}, choosing Debianesque install."

	if [ -z "$(command -v cmake)" ]
	then
		./scripts/install-cmake-debian.sh
  fi

	echo "[+] installing tools"
	sudo $SUDO apt-get $APTARGS install git clang scdoc python3-pip
}

install_unsupported () {
	echo "[+] distribution is ${DISTRIB_ID}, choosing Redhat install."
	echo "[!] This distribution is unsupported." > /dev/stderr
	exit 1;
}

install_macos () {
	# TODO: consider supporting macports?
	echo "[+] host system is MacOS"

	echo "[+] installing tools"
	brew install git cmake scdoc

	echo "[+] installing libraries and development headers"
	# TODO: look up proper package names in homebrew
}


install_linux () {
	echo "[+] host system is Linux"
	[[ -f "/etc/lsb-release" ]] && source /etc/lsb-release
	if [ -z "${DISTRIB_ID}" ]
	then
		if [ -d /etc/apt ]
		then
			DISTRIB_ID="apt-based"
		else
			DISTRIB_ID="unsupported/unknown"
		fi
	fi

	case ${DISTRIB_ID} in
		Ubuntu)		install_debianesque ;;
		Debian)		install_debianesque ;;
		apt-based)	install_debianesque ;;
		*)
			echo "[!] distribution ${DISTRIB_ID} isn't supported in this script." > /dev/null
			;;
	esac
}


case "$(uname -s)" in
	Linux)		install_linux ;;
	Darwin)		install_macos ;;
	*)
		echo "[!] platform $(uname -s) isn't supported in this script." > /dev/null
		;;
esac


