#!/bin/bash
set -xe

# install
brew update
brew install python3
pip3 install cython numpy
git clone https://github.com/google/googletest.git $TRAVIS_BUILD_DIR/googletest
git clone https://github.com/clMathLibraries/clBLAS.git $TRAVIS_BUILD_DIR/clBLAS
cd $TRAVIS_BUILD_DIR/clBLAS/src
cmake . -DBUILD_TEST=OFF
make && make install
git clone https://github.com/KhronosGroup/OpenCL-CLHPP.git $TRAVIS_BUILD_DIR/OpenCL-CLHPP
cd $TRAVIS_BUILD_DIR/OpenCL-CLHPP
cmake . -DBUILD_DOCS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF
make && make install

# script
export CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH::$TRAVIS_BUILD_DIR/googletest/googletest/include
cd $TRAVIS_BUILD_DIR
cmake . -DPRIMITIV_USE_OPENCL=ON -DPRIMITIV_BUILD_TESTS=ON -DPRIMITIV_GTEST_SOURCE_DIR=$TRAVIS_BUILD_DIR/googletest/googletest
make VERBOSE=1
make test ARGS='-V'
make install
cd $TRAVIS_BUILD_DIR/python-primitiv
./setup.py build --enable-opencl
./setup.py test
