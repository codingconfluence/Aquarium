

:: 3rd libs
set FREEGLUT=../freeglut
set GLEW=../glew-1.12.0
set SDL2_IMAGE=../SDL2_image-2.0.5/i686-w64-mingw32
set SDL2=../SDL2-2.0.14/i686-w64-mingw32
set BOOST=../boost_1_75_0
set CC_PATH=C:\mingw32\bin


set INCLUDE=-I%FREEGLUT%/include -I%GLEW%/include/ -I%SDL2_IMAGE%/include  -I%SDL2%/include  -I%SDL2%/include/SDL2 -I%BOOST%
::echo %INCLUDE%
set LIB_PATH=-L%SDL2%/lib/ -L%SDL2_IMAGE%/lib -L%FREEGLUT%/lib/ -L%GLEW%/lib/ -L%BOOST%/stage/lib/
set LIBs=-lmingw32 -lfreeglut_static  -lglu32 -lglew32.dll -lSDL2main -lSDL2.dll -lSDL2_image.dll -lopengl32 -lboost_filesystem-mgw5-mt-d-x32-1_75.dll

del *.o
%CC_PATH%/gcc -g -c src/aqua.cpp     -o aqua.o     %INCLUDE%
%CC_PATH%/gcc -g -c src/aquarium.cpp -o aquarium.o %INCLUDE%
%CC_PATH%/gcc -g -c src/fish.cpp     -o fish.o     %INCLUDE%
%CC_PATH%/gcc -g -c src/model.cpp    -o model.o    %INCLUDE%
%CC_PATH%/gcc -g -c src/modellib.cpp -o modellib.o %INCLUDE%

%CC_PATH%/g++ -g aqua.o aquarium.o fish.o model.o modellib.o -o exe/test.exe %LIBs% %LIB_PATH%


goto end


:: build boost
cd C:\mingw32\mytest\boost_build
cd C:\mingw32\mytest\boost_1_75_0
bootstrap gcc
b2 --build-dir="C:\mingw32\mytest\boost_build" --prefix="C:\mingw32\mytest\boost" threading=multi link=shared toolset=gcc stage

:end
echo done