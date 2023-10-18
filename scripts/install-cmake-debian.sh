#!/usr/bin/env bash

set -eu


source /etc/lsb-release
SUDO="sudo"
USE_CMAKE_RC="${USE_CMAKE_RC:-}"

preinstall () {
	echo "[+] preparing to install"
  if [ "$(whoami)" == "root" ]
  then
    SUDO=
  fi

	$SUDO apt-get -y update
	$SUDO apt-get -y install ca-certificates gpg wget
}

do_install () {
	if [ ! -f /etc/apt/sources.list.d/kitware.list ]
	then
		echo "[+] fetching initial keyring"
		wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | $SUDO tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null

		echo "[+] adding repo to sources.list.d"
		echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ ${DISTRIB_CODENAME} main" | \
			$SUDO tee /etc/apt/sources.list.d/kitware.list >/dev/null
					$SUDO apt-get -y update

					echo "[+] installing kitware keyring"
					if [ -f "/usr/share/keyrings/kitware-archive-keyring.gpg" ]
					then
						$SUDO rm /usr/share/keyrings/kitware-archive-keyring.gpg
					fi
					$SUDO apt-get -y install kitware-archive-keyring
	fi

	if [ "${USE_CMAKE_RC}" = "YES" ]
	then
		echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ ${DISTRIB_RELEASE}-rc main' | \
			$SUDO tee -a /etc/apt/sources.list.d/kitware.list >/dev/null
					$SUDO apt-get update
	fi
}

preinstall
do_install
$SUDO apt-get install -y cmake
