:: 1st param is the gradle build method to call

:: Run preprocessor
pushd T:\Game\build\Preprocessor

:: Preprocessor build. Uncocmment below to compile preprocessor.
::if exist "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat" call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat"
::if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" 
::cl %CommonCompilerFlags% ..\..\code\preprocessor.cpp
Preprocessor > ..\..\code\Engine\Generated.h

:: Build and install
cd T:/AndroidProject
call gradlew.bat %1