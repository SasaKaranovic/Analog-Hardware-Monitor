#ifndef __DEBUG_H__
#define __DEBUG_H__


#if 1

void debug(const char *fmt, ...);
void debugPrint(const char *fmt, ...);
#define debug(...)      do { debugPrint("[%s] ", __FUNCTION__); debugPrint(__VA_ARGS__); debugPrint("\r\n"); } while (0)
#define debug_log(...)  do { debugPrint("[%s] ", __FUNCTION__); debugPrint(__VA_ARGS__); debugPrint("\r\n"); } while (0)

#else

#define debug(...)    
#define debug_log(...)

#endif

#endif