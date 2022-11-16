@echo off

if exist "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat" call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat"
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat" 


set WarningsIgnored=-wd4201 -wd4100 -wd4127 -wd4189 -wd4505 -wd4065 -wd4700 -wd4068 -wd4702 
set CommonCompilerFlags=-MTd -nologo -fp:fast -Gm- -GR- -EHa- -Od -Oi -WX -W4 %WarningsIgnored% -FC -Zi

pushd T:\Crossbow\build

cl %CommonCompilerFlags% T:\Crossbow\Crossbow.cpp /EHsc /link T:\Crossbow\curl\libcurl.lib Advapi32.lib ws2_32.lib winmm.lib wldap32.lib