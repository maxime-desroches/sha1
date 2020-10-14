#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

static u_int32_t S(int n, u_int32_t X);
static u_int32_t f(int t, u_int32_t B, u_int32_t C, u_int32_t D);

int main(void)
{
    char *msg = "abcdefghi";
    u_int32_t block[16];
    for(int i = 0; i < 16; i++)
    {
        block[i] = 0;
    }

    int counter = 0;
    int index = 0;
    int need_shifting = 0;

    for(int i = 0; i < strlen(msg); i++)
    {
        index = i / 4;
        block[index] = block[index] | *(msg + i);
        if(((i + 1) % 4))
        {
            block[index] = block[index] << 8;
        }
        counter++;
    }

    //if counter is multiple of 4 (4 bytes), add 0x10 and dont shift
    //if counter is not a multiple of 4 (4 bytes), add 0x10 and shift
    //in both cases, pad with a 1 (aka 0x80 because dealing with char)
    if(!((counter + 1) % 4))
    {
        block[index] = block[index] | 0x80;
    }else
    {
        index = counter / 4;
        block[index] = block[index] | 0x80;
        counter++;
        while((counter % 4))
        {
            block[index] = block[index] << 8;
            counter++;
        }

    }
    
    //add size (in bits) of msg
    block[15] = block[15] | strlen(msg) * 8;

    u_int32_t W[64];
    u_int32_t H[5];
    H[0] = 0x67452301;
    H[1] = 0xEFCDAB89;
    H[2] = 0x98BADCFE;
    H[3] = 0x10325476;
    H[4] = 0xC3D2E1F0;

    u_int32_t K[4];
    K[0] = 0x5A827999;
    K[1] = 0x6ED9EBA1;
    K[2] = 0x8F1BBCDC;
    K[3] = 0xCA62C1D6;

    u_int32_t LETTER[5];
    u_int32_t TEMP = 0;

    for(int i = 0; i < 16; i++)
    {
        W[i] = block[i];
    }


    for(int t = 16; t < 80; t++)
    {
        W[t] = S(1, (W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]));
    }

    for(int i = 0; i < 5; i++)
    {
        LETTER[i] = H[i];
    }

    for(int t = 0; t < 80; t++)
    {
        TEMP = S(5, LETTER[0]) + f(t, LETTER[1], LETTER[2], LETTER[3]) + LETTER[4] + W[t] + K[t / 20];
        LETTER[4] = LETTER[3];
        LETTER[3] = LETTER[2];
        LETTER[2] = S(30, LETTER[1]);
        LETTER[1] = LETTER[0];
        LETTER[0] = TEMP;
    }

    for(int i = 0; i < 5; i++)
    {
        H[i] += LETTER[i];
        printf("%08x", H[i]);
    }
    printf("\n");
}

static u_int32_t S(int n, u_int32_t X)
{
    return ((X << n) | (X >> (32 - n)));
}

static u_int32_t f(int t, u_int32_t B, u_int32_t C, u_int32_t D)
{
    if(t <= 19)
    {
        return (B & C) | ((~B) & D);
    }else if(t <= 39)
    {
        return B ^ C ^ D;
    }else if(t <= 59)
    {
        return (B & C) | (B & D) | (C & D);
    }else
    {
        return B ^ C ^ D;
    }
}
