#!/bin/bash
#
# This script installs OpenCV 3.0 and other required dependencies.
# For this it is necessary to enable the Ubuntu packet source 'universe' (which is likely to be already enabled).
# On systems using apt-get this is done with the command
#   sudo add-apt-repository universe
#
SUDO=''
if (( $EUID != 0 )); then
  distri_name=$(uname -n)
  if [ "$distri_name" == "debian" ]; then
    echo you need to be root, please try again
    exit 1
  else
    echo will use sudo when needed
    SUDO='sudo'
  fi
fi

build_path="build"

build_name=''
if [ 'getconf LONG_BIT' == '64' ]; then
    echo "Build on x64 system"
    build_name="opencv_x64"
else
    echo "Build on x86 system"
    build_name="opencv_x86"
fi

PM='apt-get'
if [[ ! -x `which $PM 2>/dev/null` ]]; then
  PM='yum'
fi
echo Using packet manager $PM

echo -n "do you want to install build tools?(y/n)?"
read build_tools_install_answer
if echo "$build_tools_install_answer" | grep -iq "^y" ;then
  echo "setup build tools (gcc, git, cmake, arm-gnueabi)"
  $SUDO $PM install build-essential \
                        git \
                        cmake \
                        cmake-qt-gui 
  if [ $? != 0 ]; then
     echo "ERROR: could not install build tools"
     exit 1
  fi
fi

if [ ! -d "opencv_build" ]; then
  mkdir opencv_build
fi
cd opencv_build
if [ ! -d "opencv" ]; then
  echo "getting open cv source from git"
  git clone https://github.com/opencv/opencv.git
  if [ $? != 0 ]; then
     echo "ERROR: could not clone OpenCV repository"
     exit 1
  fi
  echo "rolling back to open cv 3.0"
  cd opencv
  # this causes deteached head  
  git checkout c12243c
  if [ $? != 0 ]; then
     echo "ERROR: could not checkout OpenCV to 3.0.0 tag"
     exit 1
  fi  
  cd ..
fi

if [ ! -d "$build_path" ]; then
  mkdir $build_path
fi
cd $build_path

echo "configuring opencv build"
cmake ../opencv
if [ $? != 0 ]; then
   echo "ERROR: cmake could not generate make project"
   exit 1
fi

echo "start build"
make -j4
if [ $? != 0 ]; then
   echo "ERROR: make failed"
   exit 1
fi

echo -n "going to install, are you sure(y/n)?"
read make_install_answer
if echo "$make_install_answer" | grep -iq "^y" ;then
   $SUDO make install
   link_lib_path="/usr/local/lib"
   $SUDO sh -c "echo $link_lib_path > /etc/ld.so.conf.d/$build_name.conf"
   $SUDO ldconfig
fi
