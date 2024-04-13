#ifdef _WIN32
    #ifdef _WIN64
        #define ON_PLATFORM_WINDOW
    #endif
    #if defined(_MSC_VER)
        #define FORCEINLINE __forceinline
    #endif
#endif