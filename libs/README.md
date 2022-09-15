# Configurations

## Attention.
The folowing packages are required to install SPBench dependencies:
 
- build-essential
- pkg-config
- cmake
- python (tested on python3)

You can run the command bellow to install theses dependencies or ask a system admin.

`sudo apt-get install -y build-essential pkg-config cmake python3`

## Installation
For Lane Detection or Person Recognition you first must install yasm -> ffmpeg -> OpenCV -> UPL.

For Bzip2 you need to install the bzlib -> UPL.

For Ferret, you must install GSL -> JPEG -> UPL

## yasm

Go inside the yasm folder

``
cd yasm
``

Compile the sources

``
./setup_yasm.sh
``

Load the configurations.

``
source setup_yasm_vars.sh
``

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
