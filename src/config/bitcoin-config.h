#ifndef __BITCOIN_CONFIG_H__
#define __BITCOIN_CONFIG_H__

#define CLIENT_VERSION_MAJOR 0
#define CLIENT_VERSION_MINOR 10
#define CLIENT_VERSION_REVISION 99
#define CLIENT_VERSION_BUILD 0

//! Set to true for release, false for prerelease or test build
#define CLIENT_VERSION_IS_RELEASE false

/**
* Copyright year (2009-this)
* Todo: update this when changing our copyright comments in the source
*/
#define COPYRIGHT_YEAR 2015

#define HAVE_WORKING_BOOST_SLEEP_FOR

#if defined(_WIN64)
typedef __int64 ssize_t;
#else
typedef long ssize_t;
#endif

#endif  // __BITCOIN_CONFIG_H__