#!/bin/bash

if [[ ! -f /usr/share/fonts/truetype/droid/DroidSansMono.ttf ]];
then
	sudo mkdir -p /usr/share/fonts/truetype/droid/
	sudo cp DroidSansMono.ttf /usr/share/fonts/truetype/droid/DroidSansMono.ttf
fi

if [[ ! -d ~/wiringPi ]]
then
	pushd ~
	git clone git://git.drogon.net/wiringPi
	cd wiringPi
	./build
	popd
fi

sudo apt-get update
sudo apt-get install libsdl2-dev -y
sudo apt-get install libsdl2-ttf-dev -y
sudo apt-get install libsdl2-gfx-dev -y
#sudo apt-get install fonts-droid -y
sudo apt-get install libcurl4-openssl-dev -y
sudo apt-get install libncurses5-dev -y

# Install other useful stuff...
sudo apt-get install apt-file -y
git config --global alias.hist    'log --date=short --format="%Cred%h%Cgreen %<(6,trunc)%an %C(yellow)%cd%Creset %s"'

