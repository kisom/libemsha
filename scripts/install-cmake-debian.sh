#!/usr/bin/env bash

set -eu


source /etc/lsb-release

preinstall () {
	echo "[+] preparing to install"
	sudo apt-get update
	sudo apt-get install ca-certificates gpg wget
}

do_install () {
	if [ ! -f /etc/apt/sources.list.d/kitware.list ]
	then
		echo "[+] fetching initial keyring"
		wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null

		echo "[+] adding repo to sources.list.d"
		echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ ${DISTRIB_CODENAME} main" | \
			sudo tee /etc/apt/sources.list.d/kitware.list >/dev/null
					sudo apt-get update

					echo "[+] installing kitware keyring"
					if [ -f "/usr/share/keyrings/kitware-archive-keyring.gpg" ]
					then
						sudo rm /usr/share/keyrings/kitware-archive-keyring.gpg
					fi
					sudo apt-get install kitware-archive-keyring
	fi

	if [ "${USE_CMAKE_RC}" = "YES" ]
	then
		echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ ${DISTRIB_RELEASE}-rc main' | \
			sudo tee -a /etc/apt/sources.list.d/kitware.list >/dev/null
					sudo apt-get update
	fi
}

do_install
sudo apt-get install cmake
