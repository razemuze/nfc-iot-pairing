#define __STDC_FORMAT_MACROS

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <cstdlib>
#include <inttypes.h>
#include <math.h>
#include "gmp-6.1.2/gmp.h"

const uint8_t lowPrimeCount = 167;
const uint16_t lowPrimes[] = {3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97
               					  ,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179
               					  ,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269
               					  ,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367
               					  ,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461
               					  ,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571
               					  ,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661
               					  ,673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773
               					  ,787,797,809,811,821,823,827,829,839,853,857,859,863,877,881,883
               					  ,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997};


uint64_t modular(uint64_t base, uint64_t exp, uint64_t mod) {
	uint64_t x = 1;
	uint64_t i;
	uint64_t power = base % mod;

	for (i=0; i<sizeof(uint64_t) * 8; i++) {
		uint64_t least_sig_bit = 0x00000001 & (exp >> i);
		if (least_sig_bit) x = (x * power) % mod;
		power = (power * power) % mod;
	}

	return x;
}


// Pseudoprime test using rabin-miller primality test
// Returns true if primeCandidate is probably a prime
// Returns false if primeCandidate is definitely a composite
bool rabinMillerPrimalityTest(uint64_t primeCandidate) {
	// Step 1: Find N-1 = 2^k * m
	uint32_t kValue = 1;

	while (true) {
		printf("1: %" PRIu64 "\r\n", (uint64_t)(pow(2, kValue) + 0.5));
		printf("2: %" PRIu64 "\r\n", (primeCandidate-1) % (uint64_t)(pow(2, kValue) + 0.5));
		if ((primeCandidate-1) % (uint64_t)(pow(2, kValue) + 0.5) == 0) {
			kValue++;
		} else {
			kValue--; // Restore previous value that passed check
			break;
		}
	}

	uint64_t mValue = (primeCandidate-1) / (uint64_t)(pow(2, kValue)+0.5);

	printf("kValue: %" PRIu64 "\r\n", kValue);
	printf("mValue: %" PRIu64 "\r\n", mValue);
	printf("modular: %" PRIu64 "\r\n", modular((uint64_t)3, mValue, primeCandidate));

	// Step 2: Choose a: 1 < a < n-1
	uint32_t aValue = 3; // Non-random, not suited to multiple executions
	//while (true) {
	//	aValue = rand();
	//	if (aValue > 1 && aValue < primeCandidate-1 && aValue < 10) break;
	//}

	// Step 3: Compute b0 = a^m (mod n)

	// b0
	uint64_t bValue = modular((uint64_t)aValue, mValue, primeCandidate);
	if (bValue == 1 || bValue == primeCandidate-1) return true;

	// b1...bn
	for (uint32_t b = 1; b <= kValue-1; b++) {
		//bValue = (uint64_t)(pow(bValue, 2)+0.5) % primeCandidate;
		bValue = modular(bValue, (uint64_t)2, primeCandidate);
		if (bValue == 1) return false; // Definitely composite
		if (bValue == primeCandidate-1) return true; // Probably prime
	}

	return false; // Definitely composite
}


// Definitive prime test
// Returns true if definitely prime
// Returns false if definitely composite
bool definitivePrimalityTest(uint64_t primeCandidate) {
	for (uint64_t divisor = 2; divisor < primeCandidate; divisor++) {
		if (primeCandidate % divisor == 0) {
			return false;
		} 
	}
	return true;
}


// Primality test function to automatically run all other primality checkers
bool primalityTest(uint64_t primeCandidate) {
	// Ensure primeCandidate is odd
	//printf("test odd\r\n");
	if (primeCandidate % 2 == 0) return false;

	// Eliminate some composites by checking if they are divisible by a list of primes IS THIS IN EFFECT A PRIME SIEVE???
	//printf("test against table\r\n");
	for (int i=0; i<lowPrimeCount; i++) {
		if (primeCandidate % lowPrimes[i] == 0 && primeCandidate > lowPrimes[i]) return false;
	}

	// Run rabin-miller primality test
	//printf("test rabin-miller\r\n");
	//for (int i = 0; i<64; i++) {
		if (!rabinMillerPrimalityTest(primeCandidate)) {
			//printf("Fail\r\n");
			return false;
		}
	//}

	printf("test definitive\r\n");
	// If this is reached, millerRabin -> probably prime, run definitivePrimalityTest
	return (definitivePrimalityTest(primeCandidate)) ? true : false;
}