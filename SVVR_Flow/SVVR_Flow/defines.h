#pragma once

/* Debugging verbosity */
// Error messages
#define dbglvl_ERROR 0
//warning messages
#define dbglvl_WARNING 5
//Notification messages (such as loading file...)
#define dbglvl_NOTIFICATION 6
//Verbose: Extreme verbosity like debug prints
#define dbglvl_VERBOSE 10

//Debug level
#define dbglvl dbglvl_VERBOSE

#define dprintf(lvl,fmt,...) if (lvl<=dbglvl)  printf("[%s:%i] "fmt, __func__, __LINE__, __VA_ARGS__)