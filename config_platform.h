/* $Id$ */

/*
 * Do not edit me!
 * You should edit config.h instead!
 */

#ifndef __CONFIG_PLATFORM_H__
#define __CONFIG_PLATFORM_H__

#ifdef NO_SSL
#define SSL void
#endif

#if (defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || (defined(__WATCOMC__) && !defined(__OS2__) && !defined(__NETWARE__))) && defined(BUILD_GUI) && !defined(SERVICE)
#define BUILD_GUI_VALID
#endif

#if (defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)) && defined(USE_POLL)
#undef USE_POLL
/* Force select(2) for Windows */
#endif

#if (defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)) && defined(USE_WINSOCK1) && !defined(NO_GETNAMEINFO)
#define NO_GETNAMEINFO
/* getnameinfo is not on winsock 1 */
#endif

#if (defined(__MINGW32__) || defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)) && defined(HAS_CHROOT)
#undef HAS_CHROOT
/* Windows should not have chroot */
#endif

#if (defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__)) && !defined(NO_GETNAMEINFO)
#define NO_GETNAMEINFO
/* Do not use getnameinfo */
#endif

#if (defined(_PSP) || defined(__PPU__) || defined(__ps2sdk__)) && !defined(NO_IPV6)
#define NO_IPV6
/* PSP/PS2/PS3 does not have IPv6 */
#endif

#if (defined(_PSP) || defined(__ps2sdk__)) && defined(USE_POLL)
#undef USE_POLL
/* Force select(2) for PSP/PS2 */
#endif

#if (defined(_PSP) || defined(__PPU__) || defined(__ps2sdk__)) && defined(HAS_CHROOT)
#undef HAS_CHROOT
/* PSP/PS2/PS3 should not have chroot */
#endif

#if (defined(_PSP) || defined(__PPU__) || defined(__ps2sdk__)) && !defined(NO_GETNAMEINFO)
#define NO_GETNAMEINFO
/* PSP/PS2/PS3 should not have getnameinfo */
#endif

#if (defined(__minix)) && !defined(NO_IPV6)
#define NO_IPV6
/* Minix does not have IPv6 */
#endif

#if defined(__WATCOMC__) && defined(__OS2__) && !defined(NO_IPV6)
#define NO_IPV6
#endif

#if defined(__WATCOMC__) && defined(__NETWARE__) && !defined(NO_IPV6)
#define NO_IPV6
#endif

#if defined(__WATCOMC__) && defined(__DOS__) && !defined(NO_IPV6)
#define NO_IPV6
#endif

#if defined(__NeXT__) && !defined(NO_IPV6)
#define NO_IPV6
#endif

#if defined(__NeXT__) && !defined(NO_GETNAMEINFO)
#define NO_GETNAMEINFO
#endif

#if defined(__NeXT__) && defined(USE_POLL)
#undef USE_POLL
#endif

#endif

/*
vim: syntax=c
*/
