#include <gmp.h>



void encryptCharArray(char* outputBuff, mpz_t n, mpz_t e, char* charArray, int arraySize) {
    mpz_t data;
    mpz_init(data);
    mpz_import(data, arraySize/sizeof(charArray[0]), 1, sizeof(charArray[0]), 0, 0, charArray);

    printf("Message before encryption: %s\r\n", charArray);
    printf("number before encryption:  ");
    mpz_out_str(stdout, 10, data);

    mpz_powm(data, data, e, n); // Calculate c = m^e mod n

    char encryptBuffer[256];
    //mpz_export(encryptBuffer, NULL, 1, sizeof(char), 0, 0, data);
    mpz_get_str(encryptBuffer, 10, data);

    int i;
    for (int i=0; i<256; i++) {
        *(outputBuff+i) = encryptBuffer[i];
    }

    printf("\r\nNumber after encryption:   ");
    mpz_out_str(stdout, 10, data);
    printf("\r\n");
}

void decryptCharArray(mpz_t n, mpz_t d, mpz_t dataNumber) {
    mpz_powm(dataNumber, dataNumber, d, n); // m = encryptedNumber^d mod n

    printf("Number after decryption:   ");
    mpz_out_str(stdout, 10, dataNumber);
    char decryptBuffer[256];
    mpz_export(decryptBuffer, NULL, 1, sizeof(char), 0, 0, dataNumber);
    printf("String after decryption: %s\r\n", decryptBuffer);
}