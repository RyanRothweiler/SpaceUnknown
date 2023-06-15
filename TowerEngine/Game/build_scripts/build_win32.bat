:: first param 	-> pass 1 for automated testing build, 0 for release
:: second param	-> pass 1 for multithreaded, or 0 for not

@echo off

if not exist T:\Game\build\Windows mkdir T:\Game\build\Windows
if not exist T:\Game\build\Preprocessor mkdir T:\Game\build\Preprocessor

:: -Z7 DEBUG FILES SWITCH

set WarningsIgnored=-wd4201 -wd4100 -wd4127 -wd4189 -wd4505 -wd4065 -wd4700 -wd4068 -wd4702 -wd4307
set CommonCompilerFlags=-MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 %WarningsIgnored% -FC -Zi
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib opengl32.lib Rpcrt4.lib shcore.lib winmm.lib Comdlg32.lib

set BuildVariables= -DDEBUG=1 -DDEBUG_AUDIO=0 -DUNIT_TESTING=1 -DWINDOWS=1 -DAUTOMATED_TESTING=0 -DRELEASE=0 -DPLATFORM_WINDOWS=1
::set BuildVariables= -DDEBUG=1 -DDEBUG_AUDIO=0 -DUNIT_TESTING=1 -DWINDOWS=1 -DAUTOMATED_TESTING=1


:: if exist "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" 
if exist "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat" call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat"
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" 

pushd T:\Game\build\Preprocessor

echo Preprocessor
:: Build preprocessor
:: cl %CommonCompilerFlags% T:\Game\code\preprocessor.cpp /EHsc
:: Run preprocessor
Preprocessor > T:\Game\code\Engine\Generated.h
echo Done
:: ----------------------------------------------------------------------------

popd

pushd T:\Game\build\Windows

:: 64-bit build
::IF %2 equ 1 (set MultiThread= start /B)

del *.pdb > NUL 2> NUL
%MultiThread% cl /Fdwin32.PDB  %BuildVariables% %CommonCompilerFlags% T:\Game\code\Platform\Platform_win32.cpp /EHsc /F 10000000 /link %CommonLinkerFlags% Shell32.lib
%MultiThread% cl /Fdopengl.PDB %BuildVariables% %CommonCompilerFlags% T:\Game\code\Platform\render_win32_ogles3.cpp /EHsc -LD /link -incremental:no -opt:ref opengl32.lib /PDB:origin_%random%.pdb /EXPORT:Init
%MultiThread% cl /Fdgame.PDB   %BuildVariables% %CommonCompilerFlags% T:\Game\code\Engine\EngineCore.cpp /EHsc -LD /link -incremental:no -opt:ref /PDB:origin_%random%.pdb /EXPORT:GameLoop
popd

if %errorlevel% neq 0 exit /b %errorlevel%
