#!/bin/bash
buildshLocation=$(dirname $0)
dir=$(cd $buildshLocation;pwd)

currentDir=$(pwd)
cd $dir

3rdparty/get3rdparty.sh

cd $currentDir
