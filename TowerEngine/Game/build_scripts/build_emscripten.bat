set EM_PATH="C:\emsdk-main\emsdk-main"
set EMSDK_QUIET=1

:: Run preprocessor
pushd T:\Game\build\Preprocessor

:: Preprocessor build. Uncocmment below to compile preprocessor.
::if exist "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat" call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat"
::if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" 
::cl %CommonCompilerFlags% ..\..\code\preprocessor.cpp
Preprocessor > ..\..\code\Engine\Generated.h
popd

:: setup emscripen build variables / environment
pushd %EM_PATH%
call "emsdk_env.bat"
popd

cd /d "T:/"
::pushd "T:\Game\build\Emscripten"

set WarningsIgnored=-Wno-writable-strings -Wno-format -Wno-switch

::-sALLOW_MEMORY_GROWTH

emcc ^
 T:/Game/code/Platform/platform_emscripten.cpp ^
 -o T:/Game/build/Emscripten/output.html ^
 --preload-file T:\Game\assets ^
 -sALLOW_MEMORY_GROWTH ^
 -sMAX_WEBGL_VERSION=2 ^
 -sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=$stackTrace ^
 -sFULL_ES3 ^
 -g ^
 -DRELEASE ^
 %WarningsIgnored%
