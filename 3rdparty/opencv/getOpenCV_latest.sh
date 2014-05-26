#!/bin/bash
buildshLocation=$(dirname $0)
dir=$(cd $buildshLocation;pwd)

latest_stable_commit='ab2749d648b2792d332028d378cf7334498e517c'
current_dir=$(pwd)

cd $dir
if [ -d $dir/opencv ]; then
	# Update repository
	cd $dir/opencv
	git pull
else
	# Download repository
	git clone https://github.com/Itseez/opencv.git
fi

cd $dir/opencv
git checkout $latest_stable_commit
cd $current_dir
