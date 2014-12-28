#!/bin/bash
buildshLocation=$(dirname $0)
dir=$(cd $buildshLocation;pwd)

currentDir=$(pwd)
cd $dir

# Get opencv scons builder
if [ -d openCV-sconsbuilder ]; then
	cd openCV-sconsbuilder
	git pull
	cd ..
else
	git clone https://github.com/bverhagen/openCV-sconsbuilder.git
fi

cd openCV-sconsbuilder
./getOpenCv.sh
cd ..

# Get Catch
mkdir catch
cd catch
wget https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp
cd ..

cd $currentDir
