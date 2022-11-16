
:: try rename, maybe the previous release build didn't rename correctly
IF exist "C:\Digital Archive\Game Development\Active\MeepMorp\AndroidProject\app\src\main\ASSETS_BAK" (
	ren "C:\Digital Archive\Game Development\Active\MeepMorp\AndroidProject\app\src\main\ASSETS_BAK" "assets"
)

:: copy the debug.gradle
pushd "T:\AndroidProject\app\"
copy "android_release_build.gradle" "BUILD_COPY.gradle"
del "build.gradle"
ren "BUILD_COPY.gradle" "build.gradle"
popd

T:\Game\build_scripts\build_android.bat installDebug