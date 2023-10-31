rem Making emscripten build
pushd Game
call ./build_scripts/build_emscripten.bat
popd

rem copy over build files
robocopy Game/build/Emscripten Firebase/root /E

rem deploy
cd Firebase
firebase deploy
