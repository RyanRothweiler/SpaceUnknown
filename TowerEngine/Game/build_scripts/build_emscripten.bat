set EM_PATH="C:\emsdk-main\emsdk-main"
set EMSDK_QUIET=1

:: Run preprocessor
pushd T:\Game\build\Preprocessor

:: Preprocessor build. Uncocmment below to compile preprocessor.
Preprocessor > ..\..\code\Engine\Generated.h
popd

:: setup emscripen build variables / environment
pushd %EM_PATH%
call "emsdk_env.bat"
popd

cd /d "T:/"

set WarningsIgnored=-Wno-writable-strings -Wno-format -Wno-switch

emcc ^
 T:/Game/code/Platform/platform_emscripten.cpp ^
 -o T:/Game/build/Emscripten/output.html ^
 --preload-file T:\Game\assets ^
 -sALLOW_MEMORY_GROWTH ^
 -sMAX_WEBGL_VERSION=2 ^
 -sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=$stackTrace ^
 -sFULL_ES3 ^
 -pthread ^
 -sPTHREAD_POOL_SIZE=4 ^
 -g ^
 -DRELEASE ^
 %WarningsIgnored%
