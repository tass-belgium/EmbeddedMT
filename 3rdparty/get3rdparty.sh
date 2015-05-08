#!/bin/bash

# First arg: folder name of unpacked tar
# Second arg: name of tar archive
# Third arg: url where to get tar archive
function getTar {
	if [ ! -d $1 ]; then
		[ ! -f $2 ] && wget $3
		echo "Unpacking $2"
		tar -xf $2
		cd ..
	else
		echo 'Already up-to-date'
	fi
}

function getLatestGit {
	if [ -d $1 ]; then
		cd $1
		git pull
		cd ..
	else
		git clone $2
	fi
}

buildshLocation=$(dirname $0)
dir=$(cd $buildshLocation;pwd)

currentDir=$(pwd)
cd $dir

# Get opencv scons builder
echo 'Getting OpenCV...'
getLatestGit 'openCV-sconsbuilder' https://github.com/bverhagen/openCV-sconsbuilder.git
#if [ -d openCV-sconsbuilder ]; then
#	cd openCV-sconsbuilder
#	git pull
#	cd ..
#else
#	git clone https://github.com/bverhagen/openCV-sconsbuilder.git
#fi

cd openCV-sconsbuilder
./getOpenCv.sh
cd ..

# Get Catch
# Since Catch is just a header file, we add it to Git
#echo 'Getting Catch...'
#catchDir='catch'
#if [ ! -d $catchDir ]; then
#	mkdir catch
#	cd catch
#	wget https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp
#	cd ..
#else
#	echo 'Already up-to-date'
#fi

# Get Freak
echo 'Getting Freak...'
getLatestGit 'freak' https://github.com/kikohs/freak.git


# Get Boost
boostVersion='1.57.0'
boostDir='boost'

# Replace all dots in boostVersion with underscores
boostVersionUnderscore=${boostVersion//./_}
boostArchive="boost_$boostVersionUnderscore.tar.gz"

echo "Getting boost $boostVersion..."
[ ! -d $boostDir ] && mkdir $boostDir
cd $boostDir
getTar "boost_$boostVersionUnderscore" $boostArchive https://downloads.sourceforge.net/project/boost/boost/$boostVersion/$boostArchive
cd ..

cd $currentDir
