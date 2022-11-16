:: Rename the assets folder to assets_bak so that it isn't used. The asset pack is used for aab builds
ren "C:\Digital Archive\Game Development\Active\MeepMorp\AndroidProject\app\src\main\assets" "ASSETS_BAK"

:: build
call T:\Game\build_scripts\build_android.bat bundleRelease

:: Put assets folder back to BAK name so that it isn't included in the debugging build.
ren "C:\Digital Archive\Game Development\Active\MeepMorp\AndroidProject\app\src\main\ASSETS_BAK" "assets"