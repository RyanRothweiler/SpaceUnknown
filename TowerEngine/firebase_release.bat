rem Making emscripten build
pushd Game
set BuildVars=-DRELEASE
call ./build_scripts/build_emscripten.bat
popd

rem copy over build files
robocopy Game/build/Emscripten Firebase/root /E

rem deploy
pushd Firebase
firebase deploy
popd
