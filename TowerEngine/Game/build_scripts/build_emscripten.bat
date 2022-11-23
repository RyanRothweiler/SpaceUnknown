set EM_PATH="C:\emsdk-main\emsdk-main"
set EMSDK_QUIET=1

:: setup emscripen build variables / environment
pushd %EM_PATH%
call "emsdk_env.bat"
popd

cd /d "T:/"
::pushd "T:\Game\build\Emscripten"

set WarningsIgnored=-Wno-writable-strings

::-sALLOW_MEMORY_GROWTH

emcc ^
 T:/Game/code/Platform/platform_emscripten.cpp ^
 -o T:/Game/build/Emscripten/output.html ^
 --preload-file T:\Game\assets ^
 -sALLOW_MEMORY_GROWTH ^
 -g ^
 %WarningsIgnored%