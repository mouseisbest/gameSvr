#ifndef __TIME_TOOL_H__
#define __TIME_TOOL_H__


#include <sys/time.h>


time_t get_time();
unsigned int time_diff_ms(struct timeval *pstTv1, struct timeval *pstTv2);








#endif
