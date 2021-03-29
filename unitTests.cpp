#define __STDC_FORMAT_MACROS

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <cstdlib>
#include <inttypes.h>
#include <math.h>
#include "mathFunctions.h"

bool testDefinitivePrimalityTest() {
	if (!definitivePrimalityTest((uint64_t)911)) return false; // true prime
	if (definitivePrimalityTest((uint64_t)912)) return false; // not prime
	if (!definitivePrimalityTest((uint64_t)101161)) return false; //true prime
	if (definitivePrimalityTest((uint64_t)100462)) return false; // not prime
	return true;
}

bool testPrimalityTest() {
	if (!primalityTest((uint64_t)911)) return false; // true prime
	if (primalityTest((uint64_t)912)) return false; // not prime
	if (!primalityTest((uint64_t)101161)) return false; //true prime
	if (primalityTest((uint64_t)100462)) return false; // not prime
	if (!primalityTest((uint64_t)9332631998382915707)) return false; // true prime
	return true;
}

void runMathTests() {
	printf("Unit tests:\r\n--------------------\r\n");
	printf("pow(7,455): %f\r\n", pow(7,455));
	printf("modular: %d\r\n", modular((uint64_t)7, (uint64_t)455, (uint64_t)911));
	printf("testDefinitivePrimalityTest: %s\r\n", testDefinitivePrimalityTest() ? "passed" : "failed");
	printf("testPrimalityTest: %s\r\n", testPrimalityTest() ? "passed" : "failed");
	printf("--------------------\r\n\r\n");
}