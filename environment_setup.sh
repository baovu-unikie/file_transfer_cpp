#!/bin/bash

# Functions
install_bazel () {
	curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel.gpg
	sudo mv bazel.gpg /etc/apt/trusted.gpg.d/
	echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
	sudo apt update && sudo apt install bazel
}

# Installation using apt
REQUIRED_PACKAGES=( git libboost-all-dev libgtest-dev apt-transport-https curl gnupg )

for each in "${REQUIRED_PACKAGES[@]}"
do
	IS_EXIST=$(dpkg-query -W -f='${Status}' $each | grep -c "ok installed")
	if [ $IS_EXIST -eq 0 ]
	then
		sudo apt install --assume-yes "$each"
	elif [ $IS_EXIST -eq 1 ]
	then
		echo $each " exist."
	fi
done

# Installation of other packages not using apt
REQUIRED_BINS=( bazel )

for each in "${REQUIRED_BINS[@]}"
do
	if which $each > /dev/null; then
		echo $each " exist."
	else
		echo $each " not exist. Install now..."
		install_$each
	fi
done

# Update
# sudo apt update
# sudo apt upgrade

# Cleaning
sudo apt autoremove
