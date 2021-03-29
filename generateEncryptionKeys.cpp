#define __STDC_FORMAT_MACROS

#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <cstdlib>
#include <inttypes.h>
#include <math.h>
#include <gmp.h>
#include "mathFunctions.h"
#include "encryptionFunctions.h"
#include "unitTests.h"
#include <sys/random.h>
#include <bitset>
#include "udpBroadcast.h"
#include <unistd.h>
#include <thread>
#include "nfc-dep-initiator.h"
#include "nfc-dep-target.h"

const bool deviceIsInitiator = true; // Else, assume target


int main() {
    printf("Press Enter to start program\r\n");
    std::cin.ignore();

    const uint8_t primeBytes = 16; // Length of primeBuffer array, 16=128 bit prime
    uint8_t primeBuffer[primeBytes]; // Buffer to store data from /dev/urandom before conversion to GMP integer

    mpz_t primeOne;
    mpz_t primeTwo;
    mpz_t e;
    mpz_t tempGmp; // GMP variable to use as a temporary variable for various purposes
    mpz_init_set_ui(primeOne, 0);
    mpz_init_set_ui(primeTwo, 0);
    mpz_init_set_ui(e, 65537);
    mpz_init(tempGmp);

    bool primeOneValid = false;
    bool primeTwoValid = false;

    // Can be used to print binary!
    /*for (int i=0; i<primeBytes; i++) {
        std::bitset<8> x(primeBuffer[i]);
        std::cout << x << '\n';
    }*/

    // Loop prime-generation until both primes are valid primes
    while (!(primeOneValid && primeTwoValid)) {
        
        if (!primeOneValid) {
            getrandom(&primeBuffer, primeBytes, 0); // Fill prime buffer from /dev/urandom
            mpz_import(primeOne, primeBytes, 1, sizeof(primeBuffer[0]), 0, 0, primeBuffer); // Convert prime buffer to GMP variable
            int primeStatus = mpz_probab_prime_p(primeOne, 25); // Primality test. 2=definitely prime, 1=probably prime, 0=not prime
            mpz_sub_ui(primeOne, primeOne, 1);
            mpz_gcd(tempGmp, e, primeOne); // Check greatest common divisor
            mpz_add_ui(primeOne, primeOne, 1);
            if (mpz_cmp_si(tempGmp, 1) == 0 && primeStatus > 0) {primeOneValid = true;} // Ensure prime is coprime to E
        }

        if (!primeTwoValid) {
            getrandom(&primeBuffer, primeBytes, 0); // Fill prime buffer from /dev/urandom
            mpz_import(primeTwo, primeBytes, 1, sizeof(primeBuffer[0]), 0, 0, primeBuffer); // Convert prime buffer to GMP variable
            int primeStatus = mpz_probab_prime_p(primeTwo, 25); // Primality test. 2=definitely prime, 1=probably prime, 0=not prime
            mpz_sub_ui(primeTwo, primeTwo, 1);
            mpz_gcd(tempGmp, e, primeTwo); // Check greatest common divisor
            mpz_add_ui(primeTwo, primeTwo, 1);
            if (mpz_cmp_si(tempGmp, 1) == 0 && primeStatus > 0) {primeTwoValid = true;} // Ensure prime is coprime to E
        }
    }

    // Initialize variables to store information about public and private keys
    mpz_t n; // This, in combination with e, is the public key
    mpz_t d; // This, is private key
    mpz_t totient;
    mpz_init(n);
    mpz_init(d);
    mpz_init(totient);

    mpz_mul(n, primeOne, primeTwo); // Calculate n = pq = primeOne*primeTwo

    // Calculate totient = (p-1)(q-1)
    mpz_sub_ui(primeOne, primeOne, 1);
    mpz_sub_ui(primeTwo, primeTwo, 1);
    mpz_mul(totient, primeOne, primeTwo);
    mpz_add_ui(primeOne, primeOne, 1);
    mpz_add_ui(primeTwo, primeTwo, 1);

    // Calculate d
    int dStatus = mpz_invert(d, e, totient);
    if (dStatus == 0) {printf("Error calculating D!\r\n");}


    printf("RESULTS:\r\n");

    // Once this part is reached, both values should theoretically be valid primes.
    printf("Prime 1: ");
    mpz_out_str(stdout,10, primeOne);
    printf("\r\nPrime 2: ");
    mpz_out_str(stdout,10, primeTwo);
    printf("\r\ne:       ");
    mpz_out_str(stdout, 10, e);
    printf("\r\nn:       ");
    mpz_out_str(stdout, 10, n);
    printf("\r\nd:       ");
    mpz_out_str(stdout, 10, d);
    printf("\r\ntotient: ");
    mpz_out_str(stdout, 10, totient);
    printf("\r\n\r\n");

    // Test encryption and decryption
    //char dataBuffer[] = "This is the data to encrypt";
    //snprintf(dataBuffer, 256, "This is the data to encrypt");
    //mpz_t data;
    //mpz_init(data);
    //encryptCharArray(data, n, e, dataBuffer, sizeof(dataBuffer));
    //decryptCharArray(n, d, data);
    //mpz_export (dataBuffer, (long unsigned int*)256, 1, sizeof(dataBuffer[0]), 0, 0, data);
    //mpz_get_str(dataBuffer, /*base*/256, data);
    //printf("\r\nMessage after decryption:  %s\r\n", dataBuffer);

    //mpz_import(result, sizeof(charArray)/sizeof(charArray[0]), 1, sizeof(charArray[0]), 0, 0, charArray);

    // Run correct NFC function until it returns
    char publicKeyString1[256];
    mpz_get_str(publicKeyString1, 10, n);
    uint8_t* publicKeyString2 = reinterpret_cast<uint8_t*>(publicKeyString1); // This type conversion is due to libnfc and gmp incompatibility
    int nfcResult;

    // Data buffer for received encryption key
    char receivedPublicKeyBuffer[256];

    if (deviceIsInitiator) {
        nfcResult = nfcStartInitiator(publicKeyString2, receivedPublicKeyBuffer);
    } else {
        nfcResult = nfcStartTarget(publicKeyString2, receivedPublicKeyBuffer);
    }

    printf("NFC result: %i\r\n", nfcResult);

    // Convert received buffer from uint8_t* to char*, and on to mpz_t
    mpz_t remotePublicKey;
    mpz_init(remotePublicKey);
    mpz_set_str(remotePublicKey, receivedPublicKeyBuffer, 10);
    //mpz_import(remotePublicKey, strlen(receivedPublicKeyBuffer)/sizeof(receivedPublicKeyBuffer[0]), 1, sizeof(receivedPublicKeyBuffer[0]), 0, 0, receivedPublicKeyBuffer);
    printf("Remote key: ");
    mpz_out_str(stdout, 10, remotePublicKey);
    printf("\r\n");

    // Start UDP listener
    std::thread thread_obj(udpBroadcastListener, n, d);

    // Main program loop
    while (true) {
        char transmitBuffer[] = "This is the data to encrypt";
        //mpz_t transmitData;
        //mpz_init(transmitData);
        char returnBuffer[256];
        encryptCharArray(returnBuffer, remotePublicKey, e, transmitBuffer, sizeof(transmitBuffer));
        // mpz_get_str(transmitBuffer, /*base*/10, transmitData);
        //decryptCharArray(n, d, data); // not used in this context. Will be used in udpBroadcastListener.

        char lastBuffer[strlen(returnBuffer)];

        for (int i=0; i<strlen(returnBuffer); i++) {
            lastBuffer[i] = returnBuffer[i];
        }

        printf("transmitBuffer: %s\r\n", lastBuffer);

        // Send data and sleep
        udpSendBroadcast(returnBuffer);
        //mpz_clear(transmitData);
        sleep(5);
    }
}