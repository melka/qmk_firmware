
// Check here for options to reduce filesize if needed
// https://thomasbaart.nl/2018/12/01/reducing-firmware-size-in-qmk/
//

#if __GNUC__ > 5
#    if __has_include("passwd.h")
#        include "passwd.h"
#    endif  // if file exists
#endif      // __GNUC__

#define ADB_PSW_BIT     1
#define ADB_IS_ISO_LAYOUT

#define DISABLE_LEADER
// #define NO_ACTION_TAPPING
#define NO_ACTION_ONESHOT

// #ifndef NO_DEBUG
// #define NO_DEBUG
// #endif // !NO_DEBUG
// #if !defined(NO_PRINT) && !defined(CONSOLE_ENABLE)
// #define NO_PRINT
// #endif // !NO_PRINT
