
# build
CommonCompilerFlags="-Wno-write-strings -std=c++11 -g"
BuildVariables="-DDEBUG=1 -DDEBUG_AUDIO=0 -DUNIT_TESTING=1"

cd ../build/Linux/
g++ ../../code/platform_linux.cpp -oTowerLinux -lm -ldl -lGL -lX11 -L/usr/X11/lib $CommonCompilerFlags $BuildVariables
g++ ../../code/Game.cpp -shared -fPIC -oGame.so $CommonCompilerFlags $BuildVariables