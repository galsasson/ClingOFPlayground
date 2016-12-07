#!/bin/sh

DIR=$1
if [ "x$DIR" = "x" ] ; then
	echo "USAGE: $0 <dir>"
	exit 1
fi

# make DIR absolute
pushd $DIR &> /dev/null
DIR=`pwd`
popd &> /dev/null

if [ ! -d $DIR ] ; then
	echo "error: could not find directory: "$DIR
	exit 2
fi

pushd $DIR

# checkout sources
git clone http://root.cern.ch/git/llvm.git
cd llvm
git checkout cling-patches
cd tools
git clone http://root.cern.ch/git/cling.git
git clone http://root.cern.ch/git/clang.git
cd clang
git checkout cling-patches
cd ../..

# build debug
mkdir cling-build-debug
pushd cling-build-debug
cmake -DCMAKE_INSTALL_PREFIX=`pwd`/install -DLLVM_TARGETS_TO_BUILD="X86" -DCMAKE_BUILD_TYPE="Debug" ..
cmake --build . --target install -- -j8
popd

# build release
mkdir cling-build-release
pushd cling-build-release
cmake -DCMAKE_INSTALL_PREFIX=`pwd`/install -DLLVM_TARGETS_TO_BUILD="X86" -DCMAKE_BUILD_TYPE="Release" ..
cmake --build . --target install -- -j8
popd

popd

echo "Cling build done"
echo "Debug build:    $DIR/llvm/cling-build-debug"
echo "Release build:  $DIR/llvm/cling-build-release"

