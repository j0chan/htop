#ifndef HEADER_LinuxCRT
#define HEADER_LinuxCRT
/*
htop - LinuxCRT.h
(C) 2014 Hisham H. Muhammad
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#ifdef HAVE_EXECINFO_H
#endif

void CRT_handleSIGSEGV(int sgn);

#endif
