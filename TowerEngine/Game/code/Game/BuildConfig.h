
enum class platform_type {
	web, windows
};

class BuildConfig {
	public:
	static bool Release;
	static platform_type Platform;
};

#ifdef RELEASE
	bool BuildConfig::Release = true;
#else
	bool BuildConfig::Release = false;
#endif

#ifdef PLATFORM_EMSCRIPTEN
	 platform_type BuildConfig::Platform = platform_type::web;
#else
	platform_type BuildConfig::Platform = platform_type::windows;
#endif
