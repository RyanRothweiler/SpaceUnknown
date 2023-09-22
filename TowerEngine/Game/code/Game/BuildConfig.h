
class BuildConfig {
	public:

	static bool Release;
};

#ifdef RELEASE
bool BuildConfig::Release = true;
#else
bool BuildConfig::Release = false;
#endif
