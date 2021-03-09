#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


uint32_t addto32(uint32_t as[], int sz_a, uint32_t bs[], int sz_b) {
// Assume that sz_b <= sz_a
// Compute as += bs
int i;
uint32_t c = 0;
uint64_t s;
for (i=0; i< sz_b; i++) {
s = (uint64_t) as[i] + (uint64_t) bs[i] + (uint64_t) c; // s is a 33 bit value
c = s >> 32;
as[i] = (uint32_t) s;
}
for ( ; i< sz_a; i++) {
s = (uint64_t) as[i] + (uint64_t) c; // s is a 33 bit value
c = s >> 32;
as[i] = (uint32_t) s;
}

return c;
}


uint32_t partialprod32(uint32_t as[], int sz_a, uint32_t bs[], int sz_b, uint32_t d) {
// Assume that sz_b <= sz_a
// Compute as += bs * d
int i, j; //iterators
uint32_t c = 0, prod_c = 0; //carry values
uint64_t s = 0, temp = 0;

if(sz_a < sz_b + 1)
{
  printf("ERROR: in function call partialprod32(), sz_a must be at least one greater than sz_b.\n\n\n");
  printf("\tuint32_t partialprod32(uint32_t as[], int sz_a, uint32_t bs[], int sz_b, uint32_t d)\n");
  exit(0);
}

uint32_t prod_bd32[sz_b+1];//array holds value bs*d
memset(prod_bd32, 0, (sz_b+1)*4);//initializes array to zeroes. There are 4 bytes in a uint32_t so it sets to 0 for (sz_b+1)*4 bytes

for(j=0; j<sz_b; j++) //multiplies b * d
{
  temp = (uint64_t)prod_c + (((uint64_t) bs[j]) * (uint64_t) d); //multiplies each index by d and accounts for carry
  prod_c = temp >> 32;
  prod_bd32[j] = (uint32_t) temp;
}
prod_bd32[sz_b] = prod_c; //final carry computed from bs*d is assigned to last index

for (i=0; i<sz_b+1; i++)
{
  s = (uint64_t) as[i] + (uint64_t) prod_bd32[i] + (uint64_t) c; // s is a 33 bit value
  c = s >> 32;
  as[i] = (uint32_t) s;

}
for ( ; (i<sz_a && (c > 0)) ; i++)
{ //for loop only executes if carry is not zero and sz_a > sz_b
  s = (uint64_t) as[i] + (uint64_t) c; // s is a 33 bit value
  c = s >> 32;
  as[i] = (uint32_t) s;
}

return c;

}


void bigmul64(uint64_t a[], int sz_a, uint64_t b[], int sz_b, uint64_t c[], int sz_c)
{
    int i, j;
    uint32_t c_in = 0;
    uint32_t *a32 = (uint32_t *) a;
    uint32_t *b32 = (uint32_t *) b;
    uint32_t *c32 = (uint32_t *) c;
    int sz_a32 = 2*sz_a;
    int sz_b32 = 2*sz_b;
    int sz_c32 = 2*sz_c;

    uint64_t prod_temp[sz_c];//holds partial product
    uint64_t prod_shifted[sz_c];//holds partial products shifted by base
    uint32_t *prod_temp32 = (uint32_t *) prod_temp;
    uint32_t *prod_shifted32 = (uint32_t *) prod_shifted;



    for(i=0; i<sz_b32; i++) //iterate through each index in b32, increment c by partial product a32*b32[i]
    {
      //initialize both temporary arrays to zero
      memset(prod_temp32, 0, sz_c32*4);
      memset(prod_shifted32, 0, sz_c32*4);

      partialprod32(prod_temp32, sz_c32, a32, sz_a32, b32[i]); //computes partial product in prod_temp32

      for(j=i; j<(sz_c32 + i); j++)
      {
        //shifts prod_temp32 to prod_shifted32 by its base
        prod_shifted32[j] = prod_temp32[j-i];
      }

      //adds final shifted partial product to array c
      addto32(c32, sz_c32, prod_shifted32, sz_c32);

    }
}




int main(int argc, char const *argv[])
{
  uint64_t n1[6] = {0x06a205, 0x00ff10, 0x8162a0, 0x1b42, 0x888f21, 0x173da923602f};
  uint64_t n2[3] = {0x022735703, 0x0255acc05, 0xfddd445f};
  uint64_t n3[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint64_t ntest[4] = {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff};

  uint64_t f1[4] = {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff};
  uint64_t f2[4] = {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff};
  uint64_t f3[8] = {0, 0, 0, 0, 0, 0, 0, 0};


  int szn1 = 6;
  int szn2 = 3;
  int sznres = szn1 + szn2;
  int szf1 = 4;
  int szf2 = 4;
  int szfres = szf1 + szf2;

  int j;

  bigmul64(n1, szn1, n2, szn2, n3, sznres); //multiply arrays
  bigmul64(f1, szf1, f2, szf2, f3, szfres); //multiply arrays


//* Prints out the numbers
  for(j = 0; j<szn1; j++)
  {
    printf("n1[%u]\t%016lx \n", j, n1[j]);
  }
  printf("\n");
  for(j = 0; j<szn2; j++)
  {
    printf("n2[%u]\t%016lx \n", j, n2[j]);
  }

  printf("\n\n");
  for(j = 0; j<sznres; j++)
  {
    printf("n3[%u]\t%016lx\n", j, n3[j]);
  }
//*/
/*
  for(j = 0; j<szf1; j++)
  {
    printf("f1[%u]\t%016lx \n", j, f1[j]);
  }
  printf("\n");
  for(j = 0; j<szf2; j++)
  {
    printf("f2[%u]\t%016lx \n", j, f2[j]);
  }

  printf("\n\n");
  for(j = 0; j<szfres; j++)
  {
    printf("f3[%u]\t%016lx\n", j, f3[j]);
  }
//*/


}
