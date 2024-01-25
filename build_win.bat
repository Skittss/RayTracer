if not exist build mkdir build
if not exist exports\ppm mkdir exports\ppm
if not exist exports\png mkdir exports\png
cd build
cmake -S ../ -B . -G "MinGW Makefiles"
mingw32-make.exe
cd ..