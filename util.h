#ifndef UTIL_H_
#define UTIL_H_		1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

#define SECONDS_PER_DAY 86400

/* error printing macro */
#define ERR(call_description) \
	do { \
		fprintf(stderr, "(%s, %s, %d): ", \
				__FILE__, __FUNCTION__, __LINE__); \
		perror(call_description); \
	} while (0)

/* print error (call ERR) and exit */
#define DIE(assertion, call_description) \
	do { \
		if (assertion) { \
			ERR(call_description); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

#define KICK(assertion, call_description) \
	do { \
		if (assertion) { \
			fprintf(stderr, "(%s, %s, %d): ", \
					__FILE__, __FUNCTION__, __LINE__); \
			fprintf(stderr, "%s\n", call_description); \
			exit(EXIT_FAILURE); \
		} \
	} while(0)

#define eprintf(...) \
	do { \
		fprintf(stdout, ##__VA_ARGS__); \
		fflush(stdout); \
	} while (0)

static inline uint64_t diff(struct timeval *t2, struct timeval *t1)
{
	uint64_t n1, n2;

	n1 = 1000000 * t1->tv_sec + t1->tv_usec;
	n2 = 1000000 * t2->tv_sec + t2->tv_usec;
	if (n2 < n1) {
		uint64_t d = SECONDS_PER_DAY;
		d *= 1000000;
		n2 += d;
	}

	return n2 - n1;
}

#ifdef __cplusplus
}
#endif

#endif

