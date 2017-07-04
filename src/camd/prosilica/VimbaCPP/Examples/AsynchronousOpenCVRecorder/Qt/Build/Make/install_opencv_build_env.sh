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

echo -n "do you want to install build tools?(y/n)?"
read build_tools_install_answer
if echo "$build_tools_install_answer" | grep -iq "^y" ;then
  echo "setup build tools (gcc, git, cmake, arm-gnueabi)"
  $SUDO apt-get install build-essential \
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
  git clone https://github.com/Itseez/opencv.git
  if [ $? != 0 ]; then
     echo "ERROR: could not clone OpenCV repository"
     exit 1
  fi
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
