#!/bin/bash

# version to install
fuse_version="fuse-3.2.3"

# enable program termination at first error
set -e

# install dependencies
sudo apt-get install pkg-config meson ninja-build

# change into the home folder
cd ~

# don't download files that are already present
if [ ! -d "${fuse_version}" ]
then
	if [ ! -f "${fuse_version}.tar" ]
	then
		if [ ! -f "${fuse_version}.tar.xz" ]
		then
			# download libfuse
			wget "https://github.com/libfuse/libfuse/releases/download/${fuse_version}/${fuse_version}.tar.xz"
		fi
		
		# extract the xz archive
		unxz "${fuse_version}.tar.xz"
	fi
	
	# extract the tar archive
	tar -xf "${fuse_version}.tar"
fi

# create the build directory
if [ ! -d "${fuse_version}/build" ]
then
	mkdir "${fuse_version}/build"
fi

# change directories
cd "${fuse_version}/build"

# configure, compile, link, install
meson ".."
ninja
sudo ninja install

# clean up
cd "../.."
rm -rf "${fuse_version}.tar"

echo "Success!"
