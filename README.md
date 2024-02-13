# LightController
An Arduino and Raspberry Pi based DMX light controller with focus on cheap but yet effective production

## How to build from sorce
- install vcpkg (if not present) via ```git clone https://github.com/Microsoft/vcpkg.git``` in any directory you like
- stay in that directory and run ```./vcpkg/bootstrap-vcpkg.sh``` (mac, linux) or ```.\vcpkg\bootstrap-vcpkg.bat``` (windows)
- change the path to the vcpkg folder in the CMakeLists.txt file (currently line 37)
- ```cd``` in the ```LightController``` project directory
- run ```cmake -S ./ -B ./build```
- ```cd``` in the mewly created ```build``` directory and run ```make```
- run the program via ```./LightController```
