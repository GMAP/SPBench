# Configurations

## Attention.
The folowing packages are required to install SPBench dependencies:
 
- build-essential 
- cmake
- yasm
- python (tested on python3 and python2.7)

You can run the command bellow to install theses dependencies or ask a system admin.

``
sudo apt-get install -y build-essential cmake yasm python3
``

## Installation
For Lane Detection you first must install ffmpeg and then OpenCV.

For Bzip2 you only need to install the bzlib library.

For Ferret, you must install GSL and JPEG

## ffmpeg

Go inside the ffmpeg folder

``
cd ffmpeg
``

Compile the sources

``
./setup_ffmpeg.sh
``

Load the configurations.

``
source setup_ffmpeg_vars.sh
``

## OpenCV

Go inside the opencv folder

``
cd opencv
``

Compile the sources

``
./setup_opencv.sh
``

Load the configurations.

``
source setup_opencv_vars.sh
``

## GSL

Go inside the gsl folder

``
cd gsl
``

Compile the sources

``
./setup_gsl.sh
``

Load the configurations.

``
source setup_gsl_vars.sh
``

## JPEG

Go inside the jpeg folder

``
cd jpeg
``

Compile the sources

``
./setup_jpeg.sh
``

Load the configurations.

``
source setup_jpeg_vars.sh
``


## bzlib

Go inside the bzlib folder

``
cd bzlib
``

Compile the sources

``
./setup_bzlib.sh
``

Load the configurations.

``
source setup_bzlib_vars.sh
``

