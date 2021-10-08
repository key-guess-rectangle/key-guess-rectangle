#include <stdio.h>
#include <stdlib.h>
#include<map>
#include<vector>
#include<ctime>
#include <string.h>
#include <math.h>
#include <iostream>
using namespace std;

#define Nthreads 32
#define TAKE_BIT(x,pos) (((x)>>pos)&0x1)
typedef unsigned long long int UINT64;
map<unsigned char, vector<unsigned char> > Sbox_4_y;
map<unsigned long long, int> keycounter;

unsigned char roundkeyCells_k1[17][2][4];
unsigned char roundkeyCells_k2[17][2][4];
unsigned char roundkeyCells_k3[17][2][4];
unsigned char roundkeyCells_k4[17][2][4];

unsigned char roundDK1[17][2][4];
unsigned char roundDK2[17][2][4];

unsigned char roundkey16[2][4];
//#define DEBUG 1

// Table that encodes the parameters of the various Skinny versions:
// (block size, key size, number of rounds)
// Skinny-64-128: 36 rounds
int versions[6][3]={{64,64,32},{64,128,36},{64,192,40},{128,128,40},{128,256,48},{128,384,56}};

// 4-bit Sbox
const unsigned char sbox_4[16] = {12,6,9,0,1,10,2,11,3,8,5,13,4,14,7,15};
const unsigned char sbox_4_inv[16] = {3,4,6,8,12,10,1,14,9,2,5,7,0,11,13,15};
// 8-bit Sbox
const unsigned char sbox_8[256] = {0x65 , 0x4c , 0x6a , 0x42 , 0x4b , 0x63 , 0x43 , 0x6b , 0x55 , 0x75 , 0x5a , 0x7a , 0x53 , 0x73 , 0x5b , 0x7b ,0x35 , 0x8c , 0x3a , 0x81 , 0x89 , 0x33 , 0x80 , 0x3b , 0x95 , 0x25 , 0x98 , 0x2a , 0x90 , 0x23 , 0x99 , 0x2b ,0xe5 , 0xcc , 0xe8 , 0xc1 , 0xc9 , 0xe0 , 0xc0 , 0xe9 , 0xd5 , 0xf5 , 0xd8 , 0xf8 , 0xd0 , 0xf0 , 0xd9 , 0xf9 ,0xa5 , 0x1c , 0xa8 , 0x12 , 0x1b , 0xa0 , 0x13 , 0xa9 , 0x05 , 0xb5 , 0x0a , 0xb8 , 0x03 , 0xb0 , 0x0b , 0xb9 ,0x32 , 0x88 , 0x3c , 0x85 , 0x8d , 0x34 , 0x84 , 0x3d , 0x91 , 0x22 , 0x9c , 0x2c , 0x94 , 0x24 , 0x9d , 0x2d ,0x62 , 0x4a , 0x6c , 0x45 , 0x4d , 0x64 , 0x44 , 0x6d , 0x52 , 0x72 , 0x5c , 0x7c , 0x54 , 0x74 , 0x5d , 0x7d ,0xa1 , 0x1a , 0xac , 0x15 , 0x1d , 0xa4 , 0x14 , 0xad , 0x02 , 0xb1 , 0x0c , 0xbc , 0x04 , 0xb4 , 0x0d , 0xbd ,0xe1 , 0xc8 , 0xec , 0xc5 , 0xcd , 0xe4 , 0xc4 , 0xed , 0xd1 , 0xf1 , 0xdc , 0xfc , 0xd4 , 0xf4 , 0xdd , 0xfd ,0x36 , 0x8e , 0x38 , 0x82 , 0x8b , 0x30 , 0x83 , 0x39 , 0x96 , 0x26 , 0x9a , 0x28 , 0x93 , 0x20 , 0x9b , 0x29 ,0x66 , 0x4e , 0x68 , 0x41 , 0x49 , 0x60 , 0x40 , 0x69 , 0x56 , 0x76 , 0x58 , 0x78 , 0x50 , 0x70 , 0x59 , 0x79 ,0xa6 , 0x1e , 0xaa , 0x11 , 0x19 , 0xa3 , 0x10 , 0xab , 0x06 , 0xb6 , 0x08 , 0xba , 0x00 , 0xb3 , 0x09 , 0xbb ,0xe6 , 0xce , 0xea , 0xc2 , 0xcb , 0xe3 , 0xc3 , 0xeb , 0xd6 , 0xf6 , 0xda , 0xfa , 0xd3 , 0xf3 , 0xdb , 0xfb ,0x31 , 0x8a , 0x3e , 0x86 , 0x8f , 0x37 , 0x87 , 0x3f , 0x92 , 0x21 , 0x9e , 0x2e , 0x97 , 0x27 , 0x9f , 0x2f ,0x61 , 0x48 , 0x6e , 0x46 , 0x4f , 0x67 , 0x47 , 0x6f , 0x51 , 0x71 , 0x5e , 0x7e , 0x57 , 0x77 , 0x5f , 0x7f ,0xa2 , 0x18 , 0xae , 0x16 , 0x1f , 0xa7 , 0x17 , 0xaf , 0x01 , 0xb2 , 0x0e , 0xbe , 0x07 , 0xb7 , 0x0f , 0xbf ,0xe2 , 0xca , 0xee , 0xc6 , 0xcf , 0xe7 , 0xc7 , 0xef , 0xd2 , 0xf2 , 0xde , 0xfe , 0xd7 , 0xf7 , 0xdf , 0xff};
const unsigned char sbox_8_inv[256] = {0xac , 0xe8 , 0x68 , 0x3c , 0x6c , 0x38 , 0xa8 , 0xec , 0xaa , 0xae , 0x3a , 0x3e , 0x6a , 0x6e , 0xea , 0xee ,0xa6 , 0xa3 , 0x33 , 0x36 , 0x66 , 0x63 , 0xe3 , 0xe6 , 0xe1 , 0xa4 , 0x61 , 0x34 , 0x31 , 0x64 , 0xa1 , 0xe4 ,0x8d , 0xc9 , 0x49 , 0x1d , 0x4d , 0x19 , 0x89 , 0xcd , 0x8b , 0x8f , 0x1b , 0x1f , 0x4b , 0x4f , 0xcb , 0xcf ,0x85 , 0xc0 , 0x40 , 0x15 , 0x45 , 0x10 , 0x80 , 0xc5 , 0x82 , 0x87 , 0x12 , 0x17 , 0x42 , 0x47 , 0xc2 , 0xc7 ,0x96 , 0x93 , 0x03 , 0x06 , 0x56 , 0x53 , 0xd3 , 0xd6 , 0xd1 , 0x94 , 0x51 , 0x04 , 0x01 , 0x54 , 0x91 , 0xd4 ,0x9c , 0xd8 , 0x58 , 0x0c , 0x5c , 0x08 , 0x98 , 0xdc , 0x9a , 0x9e , 0x0a , 0x0e , 0x5a , 0x5e , 0xda , 0xde ,0x95 , 0xd0 , 0x50 , 0x05 , 0x55 , 0x00 , 0x90 , 0xd5 , 0x92 , 0x97 , 0x02 , 0x07 , 0x52 , 0x57 , 0xd2 , 0xd7 ,0x9d , 0xd9 , 0x59 , 0x0d , 0x5d , 0x09 , 0x99 , 0xdd , 0x9b , 0x9f , 0x0b , 0x0f , 0x5b , 0x5f , 0xdb , 0xdf ,0x16 , 0x13 , 0x83 , 0x86 , 0x46 , 0x43 , 0xc3 , 0xc6 , 0x41 , 0x14 , 0xc1 , 0x84 , 0x11 , 0x44 , 0x81 , 0xc4 ,0x1c , 0x48 , 0xc8 , 0x8c , 0x4c , 0x18 , 0x88 , 0xcc , 0x1a , 0x1e , 0x8a , 0x8e , 0x4a , 0x4e , 0xca , 0xce ,0x35 , 0x60 , 0xe0 , 0xa5 , 0x65 , 0x30 , 0xa0 , 0xe5 , 0x32 , 0x37 , 0xa2 , 0xa7 , 0x62 , 0x67 , 0xe2 , 0xe7 ,0x3d , 0x69 , 0xe9 , 0xad , 0x6d , 0x39 , 0xa9 , 0xed , 0x3b , 0x3f , 0xab , 0xaf , 0x6b , 0x6f , 0xeb , 0xef ,0x26 , 0x23 , 0xb3 , 0xb6 , 0x76 , 0x73 , 0xf3 , 0xf6 , 0x71 , 0x24 , 0xf1 , 0xb4 , 0x21 , 0x74 , 0xb1 , 0xf4 ,0x2c , 0x78 , 0xf8 , 0xbc , 0x7c , 0x28 , 0xb8 , 0xfc , 0x2a , 0x2e , 0xba , 0xbe , 0x7a , 0x7e , 0xfa , 0xfe ,0x25 , 0x70 , 0xf0 , 0xb5 , 0x75 , 0x20 , 0xb0 , 0xf5 , 0x22 , 0x27 , 0xb2 , 0xb7 , 0x72 , 0x77 , 0xf2 , 0xf7 ,0x2d , 0x79 , 0xf9 , 0xbd , 0x7d , 0x29 , 0xb9 , 0xfd , 0x2b , 0x2f , 0xbb , 0xbf , 0x7b , 0x7f , 0xfb , 0xff};


// ShiftAndSwitchRows permutation
const unsigned char P[16] = {0,1,2,3,7,4,5,6,10,11,8,9,13,14,15,12};
const unsigned char P_inv[16] = {0,1,2,3,5,6,7,4,10,11,8,9,15,12,13,14};

// Tweakey permutation
const unsigned char TWEAKEY_P[16] = {9,15,8,13,10,14,12,11,0,1,2,3,4,5,6,7};
const unsigned char TWEAKEY_P_inv[16] = {8,9,10,11,12,13,14,15,2,0,4,7,6,3,5,1};

// round constants
const unsigned char RC[62] = {
       0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F,
       0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 0x1D, 0x3A, 0x35, 0x2B,
       0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B, 0x17, 0x2E,
       0x1C, 0x38, 0x31, 0x23, 0x06, 0x0D, 0x1B, 0x36, 0x2D, 0x1A,
       0x34, 0x29, 0x12, 0x24, 0x08, 0x11, 0x22, 0x04, 0x09, 0x13,
       0x26, 0x0c, 0x19, 0x32, 0x25, 0x0a, 0x15, 0x2a, 0x14, 0x28,
       0x10, 0x20};

FILE* fic;

void display_matrix(unsigned char state[4][4], int ver)
{
   int i;
   unsigned char input[16];

   if (versions[ver][0]==64)
   {
       for(i = 0; i < 8; i++) input[i] = ((state[(2*i)>>2][(2*i)&0x3] & 0xF) << 4) | (state[(2*i+1)>>2][(2*i+1)&0x3] & 0xF);
        for(i = 0; i < 8; i++) cout << hex << (int) input[i] << " ";
   }
   else if (versions[ver][0]==128)
   {
       for(i = 0; i < 16; i++) input[i] = state[i>>2][i&0x3] & 0xFF;
       for(i = 0; i < 16; i++) cout << hex << (int) input[i] << " ";
   }
    cout << endl;
}

void display_key(unsigned char state[2][4], int ver)
{
   int i;
   unsigned char input[16];

  
       for(i = 0; i < 4; i++) input[i] = ((state[(2*i)>>2][(2*i)&0x3] & 0xF) << 4) | (state[(2*i+1)>>2][(2*i+1)&0x3] & 0xF);
        for(i = 0; i < 4; i++) cout << hex << (int) input[i] << " ";
   
   
    cout << endl;
}

// Extract and apply the subtweakey to the internal state (must be the two top rows XORed together), then update the tweakey state
void AddKey(unsigned char state[4][4], unsigned char keyCells[3][4][4], int ver)
{
   int i, j, k;
   unsigned char pos;
   unsigned char keyCells_tmp[3][4][4];

   // apply the subtweakey to the internal state
   for(i = 0; i <= 1; i++)
   {
       for(j = 0; j < 4; j++)
       {
           state[i][j] ^= keyCells[0][i][j];
           if (2*versions[ver][0]==versions[ver][1])
               state[i][j] ^= keyCells[1][i][j];
           else if (3*versions[ver][0]==versions[ver][1])
              state[i][j] ^= keyCells[1][i][j] ^ keyCells[2][i][j];
       }
   }

   // update the subtweakey states with the permutation
   for(k = 0; k <(int)(versions[ver][1]/versions[ver][0]); k++){
       for(i = 0; i < 4; i++){
           for(j = 0; j < 4; j++){
               //application of the TWEAKEY permutation
               pos=TWEAKEY_P[j+4*i];
               keyCells_tmp[k][i][j]=keyCells[k][pos>>2][pos&0x3];
           }
       }
   }

   // update the subtweakey states with the LFSRs
   for(k = 0; k <(int)(versions[ver][1]/versions[ver][0]); k++){
       for(i = 0; i <= 1; i++){
           for(j = 0; j < 4; j++){
               //application of LFSRs for TK updates
               if (k==1)
               {
                   if (versions[ver][0]==64)
                       keyCells_tmp[k][i][j]=((keyCells_tmp[k][i][j]<<1)&0xE)^((keyCells_tmp[k][i][j]>>3)&0x1)^((keyCells_tmp[k][i][j]>>2)&0x1);
                   else
                       keyCells_tmp[k][i][j]=((keyCells_tmp[k][i][j]<<1)&0xFE)^((keyCells_tmp[k][i][j]>>7)&0x01)^((keyCells_tmp[k][i][j]>>5)&0x01);
               }
               else if (k==2)
               {
                   if (versions[ver][0]==64)
                       keyCells_tmp[k][i][j]=((keyCells_tmp[k][i][j]>>1)&0x7)^((keyCells_tmp[k][i][j])&0x8)^((keyCells_tmp[k][i][j]<<3)&0x8);
                   else
                       keyCells_tmp[k][i][j]=((keyCells_tmp[k][i][j]>>1)&0x7F)^((keyCells_tmp[k][i][j]<<7)&0x80)^((keyCells_tmp[k][i][j]<<1)&0x80);
               }
           }
       }
   }

   for(k = 0; k <(int)(versions[ver][1]/versions[ver][0]); k++){
       for(i = 0; i < 4; i++){
           for(j = 0; j < 4; j++){
               keyCells[k][i][j]=keyCells_tmp[k][i][j];
           }
       }
   }
}


// Extract and apply the subtweakey to the internal state (must be the two top rows XORed together), then update the tweakey state (inverse function}
void AddKey_inv(unsigned char state[4][4], unsigned char keyCells[3][4][4], int ver)
{
   int i, j, k;
   unsigned char pos;
   unsigned char keyCells_tmp[3][4][4];

   // update the subtweakey states with the permutation
   for(k = 0; k <(int)(versions[ver][1]/versions[ver][0]); k++){
       for(i = 0; i < 4; i++){
           for(j = 0; j < 4; j++){
               //application of the inverse TWEAKEY permutation
               pos=TWEAKEY_P_inv[j+4*i];
               keyCells_tmp[k][i][j]=keyCells[k][pos>>2][pos&0x3];
           }
       }
   }

   // update the subtweakey states with the LFSRs
   for(k = 0; k <(int)(versions[ver][1]/versions[ver][0]); k++){
       for(i = 2; i <= 3; i++){
           for(j = 0; j < 4; j++){
               //application of inverse LFSRs for TK updates
               if (k==1)
               {
                   if (versions[ver][0]==64)
                       keyCells_tmp[k][i][j]=((keyCells_tmp[k][i][j]>>1)&0x7)^((keyCells_tmp[k][i][j]<<3)&0x8)^((keyCells_tmp[k][i][j])&0x8);
                   else
                       keyCells_tmp[k][i][j]=((keyCells_tmp[k][i][j]>>1)&0x7F)^((keyCells_tmp[k][i][j]<<7)&0x80)^((keyCells_tmp[k][i][j]<<1)&0x80);
               }
               else if (k==2)
               {
                   if (versions[ver][0]==64)
                       keyCells_tmp[k][i][j]=((keyCells_tmp[k][i][j]<<1)&0xE)^((keyCells_tmp[k][i][j]>>3)&0x1)^((keyCells_tmp[k][i][j]>>2)&0x1);
                   else
                       keyCells_tmp[k][i][j]=((keyCells_tmp[k][i][j]<<1)&0xFE)^((keyCells_tmp[k][i][j]>>7)&0x01)^((keyCells_tmp[k][i][j]>>5)&0x01);
               }
           }
       }
   }

   for(k = 0; k <(int)(versions[ver][1]/versions[ver][0]); k++){
       for(i = 0; i < 4; i++){
           for(j = 0; j < 4; j++){
               keyCells[k][i][j]=keyCells_tmp[k][i][j];
           }
       }
   }


   // apply the subtweakey to the internal state
   for(i = 0; i <= 1; i++)
   {
       for(j = 0; j < 4; j++)
       {
           state[i][j] ^= keyCells[0][i][j];
           if (2*versions[ver][0]==versions[ver][1])
               state[i][j] ^= keyCells[1][i][j];
           else if (3*versions[ver][0]==versions[ver][1])
               state[i][j] ^= keyCells[1][i][j] ^ keyCells[2][i][j];
       }
   }
}


// Apply the constants: using a LFSR counter on 6 bits, we XOR the 6 bits to the first 6 bits of the internal state
void AddConstants(unsigned char state[4][4], int r)
{
   state[0][0] ^= (RC[r] & 0xf);
   state[1][0] ^= ((RC[r]>>4) & 0x3);
   state[2][0] ^= 0x2;
}

// apply the 4-bit Sbox
void SubCell4(unsigned char state[4][4])
{
   int i,j;
   for(i = 0; i < 4; i++)
       for(j = 0; j <  4; j++)
           state[i][j] = sbox_4[state[i][j]];
}

// apply the 4-bit inverse Sbox
void SubCell4_inv(unsigned char state[4][4])
{
   int i,j;
   for(i = 0; i < 4; i++)
       for(j = 0; j <  4; j++)
           state[i][j] = sbox_4_inv[state[i][j]];
}

// apply the 8-bit Sbox
void SubCell8(unsigned char state[4][4])
{
   int i,j;
   for(i = 0; i < 4; i++)
       for(j = 0; j <  4; j++)
           state[i][j] = sbox_8[state[i][j]];
}

// apply the 8-bit inverse Sbox
void SubCell8_inv(unsigned char state[4][4])
{
   int i,j;
   for(i = 0; i < 4; i++)
       for(j = 0; j <  4; j++)
           state[i][j] = sbox_8_inv[state[i][j]];
}

// Apply the ShiftRows function
void ShiftRows(unsigned char state[4][4])
{
   int i, j, pos;

   unsigned char state_tmp[4][4];
   for(i = 0; i < 4; i++)
   {
       for(j = 0; j < 4; j++)
       {
           //application of the ShiftRows permutation
           pos=P[j+4*i];
           state_tmp[i][j]=state[pos>>2][pos&0x3];
       }
   }

   for(i = 0; i < 4; i++)
   {
       for(j = 0; j < 4; j++)
       {
           state[i][j]=state_tmp[i][j];
       }
   }
}

// Apply the inverse ShiftRows function
void ShiftRows_inv(unsigned char state[4][4])
{
   int i, j, pos;

   unsigned char state_tmp[4][4];
   for(i = 0; i < 4; i++)
   {
       for(j = 0; j < 4; j++)
       {
           //application of the inverse ShiftRows permutation
           pos=P_inv[j+4*i];
           state_tmp[i][j]=state[pos>>2][pos&0x3];
       }
   }

   for(i = 0; i < 4; i++)
   {
       for(j = 0; j < 4; j++)
       {
           state[i][j]=state_tmp[i][j];
       }
   }
}

// Apply the linear diffusion matrix
//M =
//1 0 1 1
//1 0 0 0
//0 1 1 0
//1 0 1 0
void MixColumn(unsigned char state[4][4])
{
   int j;
   unsigned char temp;

   for(j = 0; j < 4; j++){
       state[1][j]^=state[2][j];
       state[2][j]^=state[0][j];
       state[3][j]^=state[2][j];

       temp=state[3][j];
       state[3][j]=state[2][j];
       state[2][j]=state[1][j];
       state[1][j]=state[0][j];
       state[0][j]=temp;
   }
}

// Apply the inverse linear diffusion matrix
void MixColumn_inv(unsigned char state[4][4])
{
   int j;
   unsigned char temp;

   for(j = 0; j < 4; j++){
       temp=state[3][j];
       state[3][j]=state[0][j];
       state[0][j]=state[1][j];
       state[1][j]=state[2][j];
       state[2][j]=temp;

       state[3][j]^=state[2][j];
       state[2][j]^=state[0][j];
       state[1][j]^=state[2][j];
   }
}



// encryption function of Skinny
void ENC_p(unsigned char* input, const unsigned char* userkey, int ver, int r, int witchk)
{
   unsigned char state[4][4];
   unsigned char keyCells[3][4][4];
   int i,j,k;

   memset(keyCells, 0, 48);
   for(i = 0; i < 16; i++) {
       if (versions[ver][0]==64){
           if(i&1)
           {
               state[i>>2][i&0x3] = input[i>>1]&0xF;
               keyCells[0][i>>2][i&0x3] = userkey[i>>1]&0xF;
               if (versions[ver][1]>=128)
                   keyCells[1][i>>2][i&0x3] = userkey[(i+16)>>1]&0xF;
               if (versions[ver][1]>=192)
                   keyCells[2][i>>2][i&0x3] = userkey[(i+32)>>1]&0xF;
           }
           else
           {
               state[i>>2][i&0x3] = (input[i>>1]>>4)&0xF;
               keyCells[0][i>>2][i&0x3] = (userkey[i>>1]>>4)&0xF;
               if (versions[ver][1]>=128)
                   keyCells[1][i>>2][i&0x3] = (userkey[(i+16)>>1]>>4)&0xF;
               if (versions[ver][1]>=192)
                   keyCells[2][i>>2][i&0x3] = (userkey[(i+32)>>1]>>4)&0xF;
           }
       }
       else if (versions[ver][0]==128){
           state[i>>2][i&0x3] = input[i]&0xFF;
           keyCells[0][i>>2][i&0x3] = userkey[i]&0xFF;
           if (versions[ver][1]>=256)
               keyCells[1][i>>2][i&0x3] = userkey[i+16]&0xFF;
           if (versions[ver][1]>=384)
               keyCells[2][i>>2][i&0x3] = userkey[i+32]&0xFF;
       }
   }

   for(i = 0; i < r; i++){
       //display_matrix(state,ver);
       if (versions[ver][0]==64)
           SubCell4(state);
       else
           SubCell8(state);
       //display_matrix(state,ver);
       AddConstants(state, i+13);
       //display_matrix(state,ver);
       //AddKey(state, keyCells, ver);
       for(k = 0; k <= 1; k++)
       {
           for(j = 0; j < 4; j++)
           {
               if (witchk == 1)
                   state[k][j] ^= roundkeyCells_k1[13+i][k][j];
               if (witchk == 2)
                   state[k][j] ^= roundkeyCells_k2[13+i][k][j];
               if (witchk == 3)
                   state[k][j] ^= roundkeyCells_k3[13+i][k][j];
               if (witchk == 4)
                   state[k][j] ^= roundkeyCells_k4[13+i][k][j];
           }
       }
       //display_key(roundkeyCells_k1[13+i],ver);
       //display_matrix(state,ver);
       if (i <r-1) {
           ShiftRows(state);
           //display_matrix(state,ver);
           MixColumn(state);
       }
   }  //The last subtweakey should not be added

   if (versions[ver][0]==64)
   {
       for(i = 0; i < 8; i++)
           input[i] = ((state[(2*i)>>2][(2*i)&0x3] & 0xF) << 4) | (state[(2*i+1)>>2][(2*i+1)&0x3] & 0xF);
   }
   else if (versions[ver][0]==128)
   {
       for(i = 0; i < 16; i++)
           input[i] = state[i>>2][i&0x3] & 0xFF;
   }
}

void ENC(unsigned char* input, const unsigned char* userkey, int ver, int r)
{
   unsigned char state[4][4];
   unsigned char keyCells[3][4][4];
   int i;

   memset(keyCells, 0, 48);
   for(i = 0; i < 16; i++) {
       if (versions[ver][0]==64){
           if(i&1)
           {
               state[i>>2][i&0x3] = input[i>>1]&0xF;
               keyCells[0][i>>2][i&0x3] = userkey[i>>1]&0xF;
               if (versions[ver][1]>=128)
                   keyCells[1][i>>2][i&0x3] = userkey[(i+16)>>1]&0xF;
               if (versions[ver][1]>=192)
                   keyCells[2][i>>2][i&0x3] = userkey[(i+32)>>1]&0xF;
           }
           else
           {
               state[i>>2][i&0x3] = (input[i>>1]>>4)&0xF;
               keyCells[0][i>>2][i&0x3] = (userkey[i>>1]>>4)&0xF;
               if (versions[ver][1]>=128)
                   keyCells[1][i>>2][i&0x3] = (userkey[(i+16)>>1]>>4)&0xF;
               if (versions[ver][1]>=192)
                   keyCells[2][i>>2][i&0x3] = (userkey[(i+32)>>1]>>4)&0xF;
           }
       }
       else if (versions[ver][0]==128){
           state[i>>2][i&0x3] = input[i]&0xFF;
           keyCells[0][i>>2][i&0x3] = userkey[i]&0xFF;
           if (versions[ver][1]>=256)
               keyCells[1][i>>2][i&0x3] = userkey[i+16]&0xFF;
           if (versions[ver][1]>=384)
               keyCells[2][i>>2][i&0x3] = userkey[i+32]&0xFF;
       }
   }

   for(i = 0; i < r; i++){
       if (versions[ver][0]==64)
           SubCell4(state);
       else
           SubCell8(state);
       AddConstants(state, i);
       AddKey(state, keyCells, ver);

       if (i <r-1) {
           ShiftRows(state);
           MixColumn(state);
       }
   }  //The last subtweakey should not be added

   if (versions[ver][0]==64)
   {
       for(i = 0; i < 8; i++)
           input[i] = ((state[(2*i)>>2][(2*i)&0x3] & 0xF) << 4) | (state[(2*i+1)>>2][(2*i+1)&0x3] & 0xF);
   }
   else if (versions[ver][0]==128)
   {
       for(i = 0; i < 16; i++)
           input[i] = state[i>>2][i&0x3] & 0xFF;
   }
}

// dencryption function of Skinny
void DEC(unsigned char* input, const unsigned char* userkey, int ver, int r)
{
   unsigned char state[4][4];
   unsigned char dummy[4][4]={{0}};
   unsigned char keyCells[3][4][4];
   int i;

   memset(keyCells, 0, 48);
   for(i = 0; i < 16; i++) {
       if (versions[ver][0]==64){
           if(i&1)
           {
               state[i>>2][i&0x3] = input[i>>1]&0xF;
               keyCells[0][i>>2][i&0x3] = userkey[i>>1]&0xF;
               if (versions[ver][1]>=128)
                   keyCells[1][i>>2][i&0x3] = userkey[(i+16)>>1]&0xF;
               if (versions[ver][1]>=192)
                   keyCells[2][i>>2][i&0x3] = userkey[(i+32)>>1]&0xF;
           }
           else
           {
               state[i>>2][i&0x3] = (input[i>>1]>>4)&0xF;
               keyCells[0][i>>2][i&0x3] = (userkey[i>>1]>>4)&0xF;
               if (versions[ver][1]>=128)
                   keyCells[1][i>>2][i&0x3] = (userkey[(i+16)>>1]>>4)&0xF;
               if (versions[ver][1]>=192)
                   keyCells[2][i>>2][i&0x3] = (userkey[(i+32)>>1]>>4)&0xF;
           }
       }
       else if (versions[ver][0]==128){
           state[i>>2][i&0x3] = input[i]&0xFF;

           keyCells[0][i>>2][i&0x3] = userkey[i]&0xFF;
           if (versions[ver][1]>=256)
               keyCells[1][i>>2][i&0x3] = userkey[i+16]&0xFF;
           if (versions[ver][1]>=384)
               keyCells[2][i>>2][i&0x3] = userkey[i+32]&0xFF;
       }
   }

   for(i = r-1; i >=0 ; i--){
       AddKey(dummy, keyCells, ver);
   }

   for(i = r-1; i >=0 ; i--){
       MixColumn_inv(state);
       ShiftRows_inv(state);
       AddKey_inv(state, keyCells, ver);
       AddConstants(state, i);

       if (versions[ver][0]==64)
           SubCell4_inv(state);
       else
           SubCell8_inv(state);
   }

   if (versions[ver][0]==64)
   {
       for(i = 0; i < 8; i++)
           input[i] = ((state[(2*i)>>2][(2*i)&0x3] & 0xF) << 4) | (state[(2*i+1)>>2][(2*i+1)&0x3] & 0xF);
   }
   else if (versions[ver][0]==128)
   {
       for(i = 0; i < 16; i++)
           input[i] = state[i>>2][i&0x3] & 0xFF;
   }
}

void generateRoundKey(unsigned char* userkey, unsigned char roundkeyCells[15][2][4], int ver, int r)
{
   unsigned char keyCells[3][4][4];
   int i,j,k,round;
   unsigned char pos;
   unsigned char keyCells_tmp[3][4][4];
   memset(keyCells, 0, 48);
    
   for(i = 0; i < 16; i++) {
       if (versions[ver][0]==64){
           if(i&1)
           {
               keyCells[0][i>>2][i&0x3] = userkey[i>>1]&0xF;
               if (versions[ver][1]>=128)
                   keyCells[1][i>>2][i&0x3] = userkey[(i+16)>>1]&0xF;
               if (versions[ver][1]>=192)
                   keyCells[2][i>>2][i&0x3] = userkey[(i+32)>>1]&0xF;
           }
           else
           {
               keyCells[0][i>>2][i&0x3] = (userkey[i>>1]>>4)&0xF;
               if (versions[ver][1]>=128)
                   keyCells[1][i>>2][i&0x3] = (userkey[(i+16)>>1]>>4)&0xF;
               if (versions[ver][1]>=192)
                   keyCells[2][i>>2][i&0x3] = (userkey[(i+32)>>1]>>4)&0xF;
           }
       }
       else if (versions[ver][0]==128){
           keyCells[0][i>>2][i&0x3] = userkey[i]&0xFF;
           if (versions[ver][1]>=256)
               keyCells[1][i>>2][i&0x3] = userkey[i+16]&0xFF;
           if (versions[ver][1]>=384)
               keyCells[2][i>>2][i&0x3] = userkey[i+32]&0xFF;
       }
   }
    //display_matrix(keyCells[0], ver);

   for(round = 0; round < r; round++){
       // apply the subtweakey to the internal state
       for(i = 0; i <= 1; i++)
       {
           for(j = 0; j < 4; j++)
           {
               roundkeyCells[round][i][j] = keyCells[0][i][j];
               if (2*versions[ver][0]==versions[ver][1])
                   roundkeyCells[round][i][j] ^= keyCells[1][i][j];
               else if (3*versions[ver][0]==versions[ver][1])
                   roundkeyCells[round][i][j] ^= keyCells[1][i][j] ^ keyCells[2][i][j];
           }
       }
       // update the subtweakey states with the permutation
       for(k = 0; k <(int)(versions[ver][1]/versions[ver][0]); k++){
           for(i = 0; i < 4; i++){
               for(j = 0; j < 4; j++){
                   //application of the TWEAKEY permutation
                   pos=TWEAKEY_P[j+4*i];
                   keyCells_tmp[k][i][j]=keyCells[k][pos>>2][pos&0x3];
               }
           }
       }

       // update the subtweakey states with the LFSRs
       for(k = 0; k <(int)(versions[ver][1]/versions[ver][0]); k++){
           for(i = 0; i <= 1; i++){
               for(j = 0; j < 4; j++){
                   //application of LFSRs for TK updates
                   if (k==1)
                   {
                       if (versions[ver][0]==64)
                           keyCells_tmp[k][i][j]=((keyCells_tmp[k][i][j]<<1)&0xE)^((keyCells_tmp[k][i][j]>>3)&0x1)^((keyCells_tmp[k][i][j]>>2)&0x1);
                       else
                           keyCells_tmp[k][i][j]=((keyCells_tmp[k][i][j]<<1)&0xFE)^((keyCells_tmp[k][i][j]>>7)&0x01)^((keyCells_tmp[k][i][j]>>5)&0x01);
                   }
                   else if (k==2)
                   {
                       if (versions[ver][0]==64)
                           keyCells_tmp[k][i][j]=((keyCells_tmp[k][i][j]>>1)&0x7)^((keyCells_tmp[k][i][j])&0x8)^((keyCells_tmp[k][i][j]<<3)&0x8);
                       else
                           keyCells_tmp[k][i][j]=((keyCells_tmp[k][i][j]>>1)&0x7F)^((keyCells_tmp[k][i][j]<<7)&0x80)^((keyCells_tmp[k][i][j]<<1)&0x80);
                   }
               }
           }
       }

       for(k = 0; k <(int)(versions[ver][1]/versions[ver][0]); k++){
           for(i = 0; i < 4; i++){
               for(j = 0; j < 4; j++){
                   keyCells[k][i][j]=keyCells_tmp[k][i][j];
               }
           }
       }

   }
}

unsigned long geneindex(unsigned char* p1,unsigned char* p2)
{
   unsigned long index=0;
   index = (p1[2]>>4)&0x0F;
   index = (index << 4)|((p1[5]>>4)&0x0F);
   index = (index << 4)|((p1[6]>>4)&0x0F);
   index = (p2[2]>>4)&0x0F;
   index = (index << 4)|((p2[5]>>4)&0x0F);
   index = (index << 4)|((p2[6]>>4)&0x0F);
   return index;
}

void generateSbox_4_y()
{
    for(unsigned char y=0; y<16; y++)
        for(unsigned char deltain=0; deltain<16; deltain++)
        {
            unsigned char deltaout=sbox_4[y]^sbox_4[y^deltain];
            unsigned char index=((deltain<<4)&0xf0)|(deltaout&0x0f);
            if(Sbox_4_y.find(index) != Sbox_4_y.end()) {
                vector<unsigned char> ttmp=Sbox_4_y[index];
                ttmp.push_back(y);
                Sbox_4_y[index] = ttmp;
            } else {
                vector<unsigned char> ttmp;
                ttmp.push_back(y);
                Sbox_4_y[index] = ttmp;
            }
        }
    /*for(unsigned char deltain=0; deltain<16; deltain++)
        for(unsigned char deltaout=0; deltaout<16; deltaout++)
    {
        unsigned char index=((deltain<<4)&0xf0)|(deltaout&0x0f);
        if(Sbox_4_y.find(index) != Sbox_4_y.end()) {
            //cout << hex << (int)deltain << " " << (int)deltaout << " : ";
            cout << hex << (int)index<< " : ";
            vector<unsigned char> ttmp=Sbox_4_y[index];
            for(int k=0; k<ttmp.size(); k++)
                cout << hex << (int)ttmp[k] << " ";
            cout << endl;
        }
    }*/
}

int keyrecovery15(unsigned char c[4][8])
{
    unsigned char X_15[4][4][4]={0};
    unsigned char Y_15[4][4][4]={0};
    for (int ci=0; ci<4; ci++)
    {
        for (int i=0; i<16; i++)
        {
            if(i&1)
                Y_15[ci][i>>2][i&0x3] = c[ci][i>>1]&0x0F;
            else
                Y_15[ci][i>>2][i&0x3] = (c[ci][i>>1]>>4)&0xF;
        }
        Y_15[ci][0][0] ^= (RC[15]&0xf);
        Y_15[ci][1][0] ^= ((RC[15]>>4)&0x3);
        Y_15[ci][2][0] ^= 0x2;
    }
    //display_matrix(Y_14[3],1);
    for (int ci=0; ci<4; ci++)
        for (int i=8; i<16; i++)
        {
            if(i&1)
                X_15[ci][i>>2][i&0x3] = sbox_4_inv[Y_15[ci][i>>2][i&0x3]];
            else
                X_15[ci][i>>2][i&0x3] = sbox_4_inv[Y_15[ci][i>>2][i&0x3]];
        }
    //display_matrix(X_14[3],1);
    unsigned char deltaXc1c3_15[4][4]={0};
    unsigned char deltaXc2c4_15[4][4]={0};
    for (int i=2; i<4; i++)
        for (int j=0; j<4; j++)
        {
            deltaXc1c3_15[i][j]=X_15[0][i][j]^X_15[2][i][j];
            deltaXc2c4_15[i][j]=X_15[1][i][j]^X_15[3][i][j];
        }
    
    if (deltaXc1c3_15[2][1] != deltaXc1c3_15[3][1] || deltaXc2c4_15[2][1] != deltaXc2c4_15[3][1])
        return 0;
    deltaXc1c3_15[0][1] = deltaXc1c3_15[2][1];
    deltaXc1c3_15[0][3] = deltaXc1c3_15[3][3];
    deltaXc1c3_15[1][3] = deltaXc1c3_15[3][3];
    deltaXc2c4_15[0][1] = deltaXc2c4_15[2][1];
    deltaXc2c4_15[0][3] = deltaXc2c4_15[3][3];
    deltaXc2c4_15[1][3] = deltaXc2c4_15[3][3];
    //display_matrix(deltaXc1c3_14,1);
    //display_matrix(deltaXc2c4_14,1);
    unsigned char STK15c1c3_1, STK15c1c3_3, STK15c1c3_7;
    unsigned char indexSTKc1c3_15_1, indexSTKc1c3_15_3, indexSTKc1c3_15_7;
    unsigned char STK15c2c4_1, STK15c2c4_3, STK15c2c4_7;
    unsigned char indexSTKc2c4_15_1, indexSTKc2c4_15_3, indexSTKc2c4_15_7;
    indexSTKc1c3_15_1 = ((deltaXc1c3_15[0][1]<<4)&0xF0) | ((Y_15[0][0][1]^Y_15[2][0][1])&0x0F);
    indexSTKc1c3_15_3 = ((deltaXc1c3_15[0][3]<<4)&0xF0) | ((Y_15[0][0][3]^Y_15[2][0][3])&0x0F);
    indexSTKc1c3_15_7 = ((deltaXc1c3_15[1][3]<<4)&0xF0) | ((Y_15[0][1][3]^Y_15[2][1][3])&0x0F);
    indexSTKc2c4_15_1 = ((deltaXc2c4_15[0][1]<<4)&0xF0) | ((Y_15[1][0][1]^Y_15[3][0][1])&0x0F);
    indexSTKc2c4_15_3 = ((deltaXc2c4_15[0][3]<<4)&0xF0) | ((Y_15[1][0][3]^Y_15[3][0][3])&0x0F);
    indexSTKc2c4_15_7 = ((deltaXc2c4_15[1][3]<<4)&0xF0) | ((Y_15[1][1][3]^Y_15[3][1][3])&0x0F);
    //cout << hex << (int)indexSTKc2c4_14_1 << endl;
    if((Sbox_4_y.find(indexSTKc1c3_15_1) != Sbox_4_y.end()) && (Sbox_4_y.find(indexSTKc2c4_15_1) != Sbox_4_y.end()) && (Sbox_4_y.find(indexSTKc1c3_15_3) != Sbox_4_y.end()) && (Sbox_4_y.find(indexSTKc2c4_15_3) != Sbox_4_y.end()) && (Sbox_4_y.find(indexSTKc1c3_15_7) != Sbox_4_y.end()) && (Sbox_4_y.find(indexSTKc2c4_15_7) != Sbox_4_y.end())) {
        vector<unsigned char> ttmpc1c3_15_1=Sbox_4_y[indexSTKc1c3_15_1];
        vector<unsigned char> ttmpc2c4_15_1=Sbox_4_y[indexSTKc2c4_15_1];
        vector<unsigned char> ttmpc1c3_15_3=Sbox_4_y[indexSTKc1c3_15_3];
        vector<unsigned char> ttmpc2c4_15_3=Sbox_4_y[indexSTKc2c4_15_3];
        vector<unsigned char> ttmpc1c3_15_7=Sbox_4_y[indexSTKc1c3_15_7];
        vector<unsigned char> ttmpc2c4_15_7=Sbox_4_y[indexSTKc2c4_15_7];
        //for (int k1=0; k1<ttmpc2c4_14_7.size(); k1++)
            //cout << hex << (int)ttmpc2c4_14_7[k1] << endl;
        for (int k1=0; k1<ttmpc1c3_15_1.size(); k1++)
            for (int k2=0; k2<ttmpc2c4_15_1.size(); k2++)
                for (int k3=0; k3<ttmpc1c3_15_3.size(); k3++)
                    for (int k4=0; k4<ttmpc2c4_15_3.size(); k4++)
                        for (int k5=0; k5<ttmpc1c3_15_7.size(); k5++)
                            for (int k6=0; k6<ttmpc2c4_15_7.size(); k6++) {
                                STK15c1c3_1 = sbox_4[ttmpc1c3_15_1[k1]]^Y_15[0][0][1];
                                STK15c2c4_1 = sbox_4[ttmpc2c4_15_1[k2]]^Y_15[1][0][1];
                                STK15c1c3_3 = sbox_4[ttmpc1c3_15_3[k3]]^Y_15[0][0][3];
                                STK15c2c4_3 = sbox_4[ttmpc2c4_15_3[k4]]^Y_15[1][0][3];
                                STK15c1c3_7 = sbox_4[ttmpc1c3_15_7[k5]]^Y_15[0][1][3];
                                STK15c2c4_7 = sbox_4[ttmpc2c4_15_7[k6]]^Y_15[1][1][3];
                                //cout << hex << (int)sbox_4[ttmpc2c4_14_7[k6]] << " " << (int)Y_14[1][1][3] << endl;
                                //cout << hex << (int)STK14c1c3_7 << " " << (int)STK14c2c4_7 << endl;
                                if (((STK15c1c3_1^STK15c2c4_1)==roundDK2[15][0][1]) && ((STK15c1c3_3^STK15c2c4_3)==roundDK2[15][0][3]) &&((STK15c1c3_7^STK15c2c4_7)==roundDK2[15][1][3])) {
                                    //cout << hex << (int)STK14c1c3_1 << " " << (int)STK14c2c4_1 << endl;
                                    //cout << hex << (int)STK14c1c3_3 << " " << (int)STK14c2c4_3 << endl;
                                    //cout << hex << (int)STK14c1c3_7 << " " << (int)STK14c2c4_7 << endl;
                                    unsigned char STK15_2, STK15_4, STK15_5;
                                    for (STK15_2=0; STK15_2<16; STK15_2++)
                                        for (STK15_4=0; STK15_4<16; STK15_4++)
                                            for (STK15_5=0; STK15_5<16; STK15_5++) {
                                                //decrypt to Z_13
                                                unsigned char Z_14[4][4];
                                                Z_14[0][0]=sbox_4_inv[Y_15[0][1][3]^STK15c1c3_7];
                                                Z_14[0][1]=X_15[0][2][0]^X_15[0][3][0]^sbox_4_inv[Y_15[0][1][0]^STK15_4];
                                                Z_14[0][2]=X_15[0][3][1]^sbox_4_inv[Y_15[0][1][1]^STK15_5];
                                                Z_14[0][3]=X_15[0][3][2]^sbox_4_inv[Y_15[0][0][2]^STK15_2];
                                                
                                                Z_14[1][0]=sbox_4_inv[Y_15[1][1][3]^STK15c1c3_7];
                                                Z_14[1][1]=X_15[1][2][0]^X_15[1][3][0]^sbox_4_inv[Y_15[1][1][0]^STK15_4^roundDK1[15][1][0]];
                                                Z_14[1][2]=X_15[1][3][1]^sbox_4_inv[Y_15[1][1][1]^STK15_5^roundDK1[15][1][1]];
                                                Z_14[1][3]=X_15[1][3][2]^sbox_4_inv[Y_15[1][0][2]^STK15_2^roundDK1[15][0][2]];
                                                
                                                Z_14[2][0]=sbox_4_inv[Y_15[2][1][3]^STK15c1c3_7];
                                                Z_14[2][1]=X_15[2][2][0]^X_15[2][3][0]^sbox_4_inv[Y_15[2][1][0]^STK15_4^roundDK2[15][1][0]];
                                                Z_14[2][2]=X_15[2][3][1]^sbox_4_inv[Y_15[2][1][1]^STK15_5^roundDK2[15][1][1]];
                                                Z_14[2][3]=X_15[2][3][2]^sbox_4_inv[Y_15[2][0][2]^STK15_2^roundDK2[15][0][2]];
                                                
                                                Z_14[3][0]=sbox_4_inv[Y_15[3][1][3]^STK15c1c3_7];
                                                Z_14[3][1]=X_15[3][2][0]^X_15[3][3][0]^sbox_4_inv[Y_15[3][1][0]^STK15_4^roundDK1[15][1][0]^roundDK2[15][1][0]];
                                                Z_14[3][2]=X_15[3][3][1]^sbox_4_inv[Y_15[3][1][1]^STK15_5^roundDK1[15][1][1]^roundDK2[15][1][1]];
                                                Z_14[3][3]=X_15[3][3][2]^sbox_4_inv[Y_15[3][0][2]^STK15_2^roundDK1[15][0][2]^roundDK2[15][0][2]];
                                                //if ((STK14_2==0xa)&&(STK14_4==0x1)&&(STK14_5==0x6))
                                                    //display_matrix(Z_13,1);
                                                unsigned char X_14[4][4];
                                                for (int ci=0; ci<4; ci++) {
                                                    X_14[ci][2]=sbox_4_inv[Z_14[ci][2]];
                                                    X_14[ci][3]=sbox_4_inv[Z_14[ci][3]];
                                                }
                                                unsigned char deltaXc1c3_14[4];
                                                unsigned char deltaXc2c4_14[4];
                                                deltaXc1c3_14[2]=X_14[0][2]^X_14[2][2];
                                                deltaXc1c3_14[3]=X_14[0][3]^X_14[2][3];
                                                deltaXc2c4_14[2]=X_14[1][2]^X_14[3][2];
                                                deltaXc2c4_14[3]=X_14[1][3]^X_14[3][3];
                                                if ((deltaXc1c3_14[2]==deltaXc1c3_14[3])&&(deltaXc2c4_14[2]==deltaXc2c4_14[3])) {
                                                    deltaXc1c3_14[0]=deltaXc1c3_14[2];
                                                    deltaXc2c4_14[0]=deltaXc2c4_14[2];
                                                    unsigned char STK14c1c3_3, indexSTKc1c3_14_3, STK14c2c4_3, indexSTKc2c4_14_3;
                                                    indexSTKc1c3_14_3 = ((deltaXc1c3_14[0]<<4)&0xF0) | ((Z_14[0][0]^Z_14[2][0])&0x0F);
                                                    indexSTKc2c4_14_3 = ((deltaXc2c4_14[0]<<4)&0xF0) | ((Z_14[1][0]^Z_14[3][0])&0x0F);
                                                    if((Sbox_4_y.find(indexSTKc1c3_14_3) != Sbox_4_y.end()) && (Sbox_4_y.find(indexSTKc2c4_14_3) != Sbox_4_y.end())) {
                                                        //cout <<hex<< (int)STK14_2<<" " <<(int)STK14_4<<" "<<(int)STK14_5<<endl;
                                                        vector<unsigned char> ttmpc1c3_14_3=Sbox_4_y[indexSTKc1c3_14_3];
                                                        vector<unsigned char> ttmpc2c4_14_3=Sbox_4_y[indexSTKc2c4_14_3];
                                                        for (int k7=0; k7<ttmpc1c3_14_3.size(); k7++)
                                                            for (int k8=0; k8<ttmpc2c4_14_3.size(); k8++) {
                                                                STK14c1c3_3 = sbox_4[ttmpc1c3_14_3[k7]]^Z_14[0][0];
                                                                STK14c2c4_3 = sbox_4[ttmpc2c4_14_3[k8]]^Z_14[1][0];
                                                                if (((STK14c1c3_3^STK14c2c4_3)==roundDK2[14][0][3])) {
                                                                    
                                                                    //cout << hex << (int)STK13c1c3_3 << " " << (int)STK13c2c4_3 << endl;
                                                                    unsigned char STK14_7;
                                                                    for (STK14_7=0; STK14_7<16; STK14_7++) {
                                                                        unsigned char X_13[4];
                                                                        //cout <<hex<< (int)X_13[0][3]<<" " <<(int)Z_13[0][1]<<" "<<(int)STK13_7<<" "<<(int)sbox_4_inv[Z_13[0][1]^STK13_7]<<endl;
                                                                        X_13[0]=sbox_4_inv[(X_14[0][3]^(sbox_4_inv[(Z_14[0][1]^STK14_7)]))];
                                                                        //cout << (int)X_12[0] << endl;
                                                                        //cout <<hex<< (int)X_13[1][3]<<" " <<(int)Z_13[1][1]<<" "<<(int)(STK13_7^roundDK1[13][1][3])<<" "<<(int)sbox_4_inv[Z_13[1][1]^STK13_7^roundDK1[13][1][3]]<<endl;
                                                                        X_13[1]=sbox_4_inv[(X_14[1][3]^(sbox_4_inv[(Z_14[1][1]^STK14_7^roundDK1[14][1][3])]))];
                                                                        //cout << (int)X_12[1] << endl;
                                                                        X_13[2]=sbox_4_inv[(X_14[2][3]^sbox_4_inv[(Z_14[2][1]^STK14_7^roundDK2[14][1][3])])];
                                                                        //cout << (int)X_12[2] << endl;
                                                                        X_13[3]=sbox_4_inv[(X_14[3][3]^sbox_4_inv[(Z_14[3][1]^STK14_7^roundDK1[14][1][3]^roundDK2[14][1][3])])];
                                                                        //cout << (int)X_12[3] << endl;
                                                                        //if (STK13_7==9)
                                                                            //cout << (int)X_12[0]<<" " <<(int)X_12[1]<<" "<<(int)X_12[2]<<" "<<(int)X_12[3]<<endl;
                                                                        if (((X_13[0]^X_13[2])==0x3)&&((X_13[1]^X_13[3])==0x3)) {
                                                                            //cout << (int)X_12[0]<<" " <<(int)X_12[1]<<" "<<(int)X_12[2]<<" "<<(int)X_12[3]<<endl;
                                                                            unsigned long long guesskey;
                                                                            guesskey=STK15c1c3_1;
                                                                            guesskey=(guesskey<<4)|STK15c1c3_3;
                                                                            guesskey=(guesskey<<4)|STK15c1c3_7;
                                                                            guesskey=(guesskey<<4)|STK15_2;
                                                                            guesskey=(guesskey<<4)|STK15_4;
                                                                            guesskey=(guesskey<<4)|STK15_5;
                                                                            guesskey=(guesskey<<4)|STK14c1c3_3;
                                                                            guesskey=(guesskey<<4)|STK14_7;
                                                                            for (int gk16=0; gk16<8; gk16++)
                                                                                guesskey=(guesskey<<4)|roundkey16[gk16>>2][gk16&0x3];
                                                                            //cout << (int)STK14_4<<" " <<(int)STK14_5<<" "<<(int)STK13c1c3_3<<" "<<(int)STK13_7<<endl;
                                                                            //cout << guesskey << endl;
                                                                            if((keycounter.find(guesskey) != keycounter.end()))
                                                                                keycounter[guesskey]++;
                                                                            else
                                                                                keycounter[guesskey]=1;
                                                                        }
                                                                    }
                                                                }
                                                        }
                                                    }
                                                }
                                            }
                                }
        }
    }
    return 0;
    
}

void keyrecovery(unsigned char c[4][8])
{
    //cout << "right quartet" << endl;
    unsigned char X_16[4][4][4]={0};
    unsigned char Y_16[4][4][4]={0};
    unsigned int i,j;
    for (int ci=0; ci<4; ci++)
    {
        for (int i=0; i<16; i++)
        {
            if(i&1)
                Y_16[ci][i>>2][i&0x3] = c[ci][i>>1]&0x0F;
            else
                Y_16[ci][i>>2][i&0x3] = (c[ci][i>>1]>>4)&0xF;
        }
        Y_16[ci][0][0] ^= (RC[16]&0xf);
        Y_16[ci][1][0] ^= ((RC[16]>>4)&0x3);
        Y_16[ci][2][0] ^= 0x2;
    }
    //display_matrix(Y_16[0],1);
    for (int ci=0; ci<4; ci++)
        for (int i=8; i<16; i++)
        {
            if(i&1)
                X_16[ci][i>>2][i&0x3] = sbox_4_inv[Y_16[ci][i>>2][i&0x3]];
            else
                X_16[ci][i>>2][i&0x3] = sbox_4_inv[Y_16[ci][i>>2][i&0x3]];
        }
    //display_matrix(X_16[0],1);
    unsigned char deltaXc1c3_16[4][4]={0};
    unsigned char deltaXc2c4_16[4][4]={0};
    for (int i=2; i<4; i++)
        for (int j=0; j<4; j++)
        {
            deltaXc1c3_16[i][j]=X_16[0][i][j]^X_16[2][i][j];
            deltaXc2c4_16[i][j]=X_16[1][i][j]^X_16[3][i][j];
        }
    deltaXc1c3_16[0][1] = deltaXc1c3_16[3][1];
    deltaXc1c3_16[1][2] = deltaXc1c3_16[3][2];
    deltaXc1c3_16[0][3] = deltaXc1c3_16[3][3];
    deltaXc1c3_16[1][3] = deltaXc1c3_16[2][3]^deltaXc1c3_16[3][3];
    
    deltaXc2c4_16[0][1] = deltaXc2c4_16[3][1];
    deltaXc2c4_16[1][2] = deltaXc2c4_16[3][2];
    deltaXc2c4_16[0][3] = deltaXc2c4_16[3][3];
    deltaXc2c4_16[1][3] = deltaXc2c4_16[2][3]^deltaXc2c4_16[3][3];
    //display_matrix(deltaXc1c3_16,1);
    //display_matrix(deltaXc2c4_16,1);
    unsigned char STK16c1c3_1, STK16c1c3_3, STK16c1c3_6, STK16c1c3_7;
    unsigned char indexSTKc1c3_16_1, indexSTKc1c3_16_3, indexSTKc1c3_16_6, indexSTKc1c3_16_7;
    unsigned char STK16c2c4_1, STK16c2c4_3, STK16c2c4_6, STK16c2c4_7;
    unsigned char indexSTKc2c4_16_1, indexSTKc2c4_16_3, indexSTKc2c4_16_6, indexSTKc2c4_16_7;
    indexSTKc1c3_16_1 = ((deltaXc1c3_16[0][1]<<4)&0xF0) | ((Y_16[0][0][1]^Y_16[2][0][1])&0x0F);
    indexSTKc1c3_16_3 = ((deltaXc1c3_16[0][3]<<4)&0xF0) | ((Y_16[0][0][3]^Y_16[2][0][3])&0x0F);
    indexSTKc1c3_16_6 = ((deltaXc1c3_16[1][2]<<4)&0xF0) | ((Y_16[0][1][2]^Y_16[2][1][2])&0x0F);
    indexSTKc1c3_16_7 = ((deltaXc1c3_16[1][3]<<4)&0xF0) | ((Y_16[0][1][3]^Y_16[2][1][3])&0x0F);
    indexSTKc2c4_16_1 = ((deltaXc2c4_16[0][1]<<4)&0xF0) | ((Y_16[1][0][1]^Y_16[3][0][1])&0x0F);
    indexSTKc2c4_16_3 = ((deltaXc2c4_16[0][3]<<4)&0xF0) | ((Y_16[1][0][3]^Y_16[3][0][3])&0x0F);
    indexSTKc2c4_16_6 = ((deltaXc2c4_16[1][2]<<4)&0xF0) | ((Y_16[1][1][2]^Y_16[3][1][2])&0x0F);
    indexSTKc2c4_16_7 = ((deltaXc2c4_16[1][3]<<4)&0xF0) | ((Y_16[1][1][3]^Y_16[3][1][3])&0x0F);
    //cout << hex << (int)indexSTKc2c4_16_1 << endl;
    if((Sbox_4_y.find(indexSTKc1c3_16_1) != Sbox_4_y.end()) && (Sbox_4_y.find(indexSTKc2c4_16_1) != Sbox_4_y.end()) && (Sbox_4_y.find(indexSTKc1c3_16_3) != Sbox_4_y.end()) && (Sbox_4_y.find(indexSTKc2c4_16_3) != Sbox_4_y.end()) && (Sbox_4_y.find(indexSTKc1c3_16_6) != Sbox_4_y.end()) && (Sbox_4_y.find(indexSTKc2c4_16_6) != Sbox_4_y.end()) && (Sbox_4_y.find(indexSTKc1c3_16_7) != Sbox_4_y.end()) && (Sbox_4_y.find(indexSTKc2c4_16_7) != Sbox_4_y.end())) {
        vector<unsigned char> ttmpc1c3_16_1=Sbox_4_y[indexSTKc1c3_16_1];
        vector<unsigned char> ttmpc2c4_16_1=Sbox_4_y[indexSTKc2c4_16_1];
        vector<unsigned char> ttmpc1c3_16_3=Sbox_4_y[indexSTKc1c3_16_3];
        vector<unsigned char> ttmpc2c4_16_3=Sbox_4_y[indexSTKc2c4_16_3];
        vector<unsigned char> ttmpc1c3_16_6=Sbox_4_y[indexSTKc1c3_16_6];
        vector<unsigned char> ttmpc2c4_16_6=Sbox_4_y[indexSTKc2c4_16_6];
        vector<unsigned char> ttmpc1c3_16_7=Sbox_4_y[indexSTKc1c3_16_7];
        vector<unsigned char> ttmpc2c4_16_7=Sbox_4_y[indexSTKc2c4_16_7];
        //for (int k1=0; k1<ttmpc2c4_14_7.size(); k1++)
            //cout << hex << (int)ttmpc2c4_16_7[k1] << endl;
        for (int k1=0; k1<ttmpc1c3_16_1.size(); k1++)
            for (int k2=0; k2<ttmpc2c4_16_1.size(); k2++)
                for (int k3=0; k3<ttmpc1c3_16_3.size(); k3++)
                    for (int k4=0; k4<ttmpc2c4_16_3.size(); k4++)
                        for (int k5=0; k5<ttmpc1c3_16_6.size(); k5++)
                            for (int k6=0; k6<ttmpc2c4_16_6.size(); k6++)
                                for (int k7=0; k7<ttmpc1c3_16_7.size(); k7++)
                                    for (int k8=0; k8<ttmpc2c4_16_7.size(); k8++) {
                                        STK16c1c3_1 = sbox_4[ttmpc1c3_16_1[k1]]^Y_16[0][0][1];
                                        STK16c2c4_1 = sbox_4[ttmpc2c4_16_1[k2]]^Y_16[1][0][1];
                                        STK16c1c3_3 = sbox_4[ttmpc1c3_16_3[k3]]^Y_16[0][0][3];
                                        STK16c2c4_3 = sbox_4[ttmpc2c4_16_3[k4]]^Y_16[1][0][3];
                                        STK16c1c3_6 = sbox_4[ttmpc1c3_16_6[k5]]^Y_16[0][1][2];
                                        STK16c2c4_6 = sbox_4[ttmpc2c4_16_6[k6]]^Y_16[1][1][2];
                                        STK16c1c3_7 = sbox_4[ttmpc1c3_16_7[k7]]^Y_16[0][1][3];
                                        STK16c2c4_7 = sbox_4[ttmpc2c4_16_7[k8]]^Y_16[1][1][3];
                                        //cout << hex << (int)sbox_4[ttmpc2c4_14_7[k6]] << " " << (int)Y_14[1][1][3] << endl;
                                        //cout << hex << (int)STK14c1c3_7 << " " << (int)STK14c2c4_7 << endl;
                                        if (((STK16c1c3_1^STK16c2c4_1)==roundDK2[16][0][1]) && ((STK16c1c3_3^STK16c2c4_3)==roundDK2[16][0][3]) &&((STK16c1c3_6^STK16c2c4_6)==roundDK2[16][1][2]) &&((STK16c1c3_7^STK16c2c4_7)==roundDK2[16][1][3])) {
                                            //cout << hex << (int)STK16c1c3_1 << " " << (int)STK16c2c4_1 << endl;
                                            //cout << hex << (int)STK16c1c3_3 << " " << (int)STK16c2c4_3 << endl;
                                            //cout << hex << (int)STK16c1c3_6 << " " << (int)STK16c2c4_6 << endl;
                                            //cout << hex << (int)STK16c1c3_7 << " " << (int)STK16c2c4_7 << endl;
                                            //cout << (int)roundkey16[1][1] <<endl;
                                            //cout << (int)roundDK2[16][1][1] <<endl;
                                            //display_matrix(Y_16[0],1);
                                            unsigned char STK16_0, STK16_2, STK16_4;
                                            for (STK16_2=0; STK16_2<16; STK16_2++)
                                                for (STK16_4=0; STK16_4<16; STK16_4++)
                                                    for (STK16_0=0; STK16_0<16; STK16_0++) {
                                                        roundkey16[0][0]=STK16_0;
                                                        roundkey16[0][1]=STK16c1c3_1;
                                                        roundkey16[0][2]=STK16_2;
                                                        roundkey16[0][3]=STK16c1c3_3;
                                                        roundkey16[1][0]=STK16_4;
                                                        roundkey16[1][2]=STK16c1c3_6;
                                                        roundkey16[1][3]=STK16c1c3_7;
                                                        //if((STK16_0==0x9)&&(STK16_2==0xd)&&(STK16_4==0x0))
                                                           //display_matrix(Y_16[0],1);
                                                        unsigned char Xt16[4][4][4];
                                                        for(i = 0; i <= 1; i++)
                                                            for(j = 0; j < 4; j++) {
                                                                Xt16[0][i][j]=Y_16[0][i][j] ^ roundkey16[i][j];
                                                                Xt16[1][i][j]=Y_16[1][i][j] ^ roundkey16[i][j]^roundDK1[16][i][j];
                                                                Xt16[2][i][j]=Y_16[2][i][j] ^ roundkey16[i][j]^roundDK2[16][i][j];
                                                                Xt16[3][i][j]=Y_16[3][i][j] ^ roundkey16[i][j]^roundDK1[16][i][j]^roundDK2[16][i][j];
                                                            }
                                                        for(i = 2; i <= 3; i++)
                                                            for(j = 0; j < 4; j++) {
                                                                Xt16[0][i][j]=Y_16[0][i][j];
                                                                Xt16[1][i][j]=Y_16[1][i][j];
                                                                Xt16[2][i][j]=Y_16[2][i][j];
                                                                Xt16[3][i][j]=Y_16[3][i][j];
                                                            }
                                                        //if((STK16_0==0x9)&&(STK16_2==0xd)&&(STK16_4==0x0))
                                                            //display_key(roundkey16,1);
                                                           //display_matrix(Xt16[0],1);
                                                        for(i = 0; i < 4; i++)
                                                            for(j = 0; j < 4; j++) {
                                                                Xt16[0][i][j] = sbox_4_inv[Xt16[0][i][j]];
                                                                Xt16[1][i][j] = sbox_4_inv[Xt16[1][i][j]];
                                                                Xt16[2][i][j] = sbox_4_inv[Xt16[2][i][j]];
                                                                Xt16[3][i][j] = sbox_4_inv[Xt16[3][i][j]];
                                                            }
                                                        MixColumn_inv(Xt16[0]);
                                                        MixColumn_inv(Xt16[1]);
                                                        MixColumn_inv(Xt16[2]);
                                                        MixColumn_inv(Xt16[3]);
                                                        ShiftRows_inv(Xt16[0]);
                                                        ShiftRows_inv(Xt16[1]);
                                                        ShiftRows_inv(Xt16[2]);
                                                        ShiftRows_inv(Xt16[3]);
                                                        unsigned char Z_15[4][8];
                                                        
                                                        for(i = 0; i < 8; i++) {
                                                            Z_15[0][i] = ((Xt16[0][(2*i)>>2][(2*i)&0x3] & 0xF) << 4) | (Xt16[0][(2*i+1)>>2][(2*i+1)&0x3] & 0xF);
                                                            Z_15[1][i] = ((Xt16[1][(2*i)>>2][(2*i)&0x3] & 0xF) << 4) | (Xt16[1][(2*i+1)>>2][(2*i+1)&0x3] & 0xF);
                                                            Z_15[2][i] = ((Xt16[2][(2*i)>>2][(2*i)&0x3] & 0xF) << 4) | (Xt16[2][(2*i+1)>>2][(2*i+1)&0x3] & 0xF);
                                                            Z_15[3][i] = ((Xt16[3][(2*i)>>2][(2*i)&0x3] & 0xF) << 4) | (Xt16[3][(2*i+1)>>2][(2*i+1)&0x3] & 0xF);
                                                            
                                                            keyrecovery15(Z_15);
                                                        }
                                                        
                                                    }
                                            
                                        }
                                    }
    }
}


int main() {
   int ver=1;
   unsigned int i,j,k,kk,kk1,kk2;
   unsigned int j1,j2,j3;
   unsigned char p1[8],p2[8],p3[8],p4[8];
   unsigned char c[4][8];
   unsigned char k1[24], k2[24], k3[24], k4[24];
   unsigned char dk1[24] = {00,00,00,00,00,00,0x10,00,00,00,00,00,00,00,0xc0,00};
   unsigned char dk2[24] = {00,00,0x01,00,00,00,00,00,00,00,0x07,00,00,00,00,00};
   unsigned char deltaX1[16]={11,14,9,4,3,7,8,5,6,2,12,0,10,15,1,13};

   map<unsigned long, vector<unsigned char> > S1map;
   map<unsigned long, vector<unsigned char> > S2map;
   map<unsigned long, vector<unsigned char> > Hmap;
   unsigned long index;
   //unsigned long long valuepair;

   clock_t START=clock();
   int r=17;
   generateSbox_4_y();
   srand((unsigned) time (NULL));
   // randomly choose k1
   for(i = 0; i < 16; i++) k1[i] = rand() & 0xff;
   // derive k2
   for(i = 0; i < 16; i++) k2[i] = k1[i]^dk1[i];
   // derive k3
   for(i = 0; i < 16; i++) k3[i] = k1[i]^dk2[i];
   // derive k4
   for(i = 0; i < 16; i++) k4[i] = k2[i]^dk2[i];
   generateRoundKey(k1,roundkeyCells_k1,ver,r);
   generateRoundKey(k2,roundkeyCells_k2,ver,r);
   generateRoundKey(k3,roundkeyCells_k3,ver,r);
   generateRoundKey(k4,roundkeyCells_k4,ver,r);
    
   generateRoundKey(dk1,roundDK1,ver,r);
   generateRoundKey(dk2,roundDK2,ver,r);
   
   unsigned long y=1024*1024*4;//2^22
   //generate S1
   for(i=0; i<y; i++){
       // randomly choose p1
       for(j = 0; j < 8; j++){
           p1[j] = rand() & 0xff;
           p2[j] = p1[j];
       }
       for(j = 0; j < 4096; j++){
           j1=j&0xF;
           j2=(j>>4)&0xF;
           j3=(j>>8)&0xF;
           p1[1]=((p1[1]&0x0f)|(j1<<4))&0xff;
           p2[1]=((p2[1]&0x0f)|(deltaX1[j1]<<4))&0xff;
           p1[2]=((p1[1]&0xf0)|j2)&0xff;
           p2[2]=((p2[1]&0xf0)|deltaX1[j2])&0xff;
           p1[4]=((p1[4]&0x0f)|(j3<<4))&0xff;
           p2[4]=((p2[4]&0x0f)|(deltaX1[j3]<<4))&0xff;
           ENC(p1, k1, ver, r); //c1 Z_16
           ENC(p2, k2, ver, r); //c2 Z_16
           index = geneindex(p1,p2);

           if(S1map.find(index) != S1map.end()) {
               vector<unsigned char> ttmp=S1map[index];
               for(k = 0; k < 8; k++)
                    ttmp.push_back(p1[k]);
               for(k = 0; k < 8; k++)
                    ttmp.push_back(p2[k]);
               S1map[index] = ttmp;
           } else {
               vector<unsigned char> ttmp;
               for(k = 0; k < 8; k++)
                    ttmp.push_back(p1[k]);
               for(k = 0; k < 8; k++)
                    ttmp.push_back(p2[k]);
               S1map[index] = ttmp;
           }
       }
   }
   //generate S2
   for(i=0; i<y; i++){
       // randomly choose p3
       for(j = 0; j < 8; j++){
           p3[j] = rand() & 0xff;
           p4[j] = p3[j];
       }
       for(j = 0; j < 4096; j++){
           j1=j&0xF;
           j2=(j>>4)&0xF;
           j3=(j>>8)&0xF;
           p3[1]=((p3[1]&0x0f)|(j1<<4))&0xff;
           p4[1]=((p4[1]&0x0f)|(deltaX1[j1]<<4))&0xff;
           p3[2]=((p3[1]&0xf0)|j2)&0xff;
           p4[2]=((p4[1]&0xf0)|deltaX1[j2])&0xff;
           p3[4]=((p3[4]&0x0f)|(j3<<4))&0xff;
           p3[4]=((p3[4]&0x0f)|(deltaX1[j3]<<4))&0xff;
           ENC(p3, k3, ver, r); //c1 Z_16
           ENC(p4, k4, ver, r); //c2 Z_16
           index = geneindex(p3,p4);
           if(S2map.find(index) != S2map.end()) {
               vector<unsigned char> ttmp=S2map[index];
               for(k = 0; k < 8; k++)
                    ttmp.push_back(p1[k]);
               for(k = 0; k < 8; k++)
                    ttmp.push_back(p2[k]);
               S2map[index] = ttmp;
           } else {
               vector<unsigned char> ttmp;
               for(k = 0; k < 8; k++)
                    ttmp.push_back(p1[k]);
               for(k = 0; k < 8; k++)
                    ttmp.push_back(p2[k]);
               S2map[index] = ttmp;
           }
       }
   }
   //generate H
   for (unsigned char gk=0; gk<16; gk++) {
        Hmap.clear();
        roundkey16[1][1]=gk;
        map<unsigned long, vector<unsigned char> >::iterator iter1;
        for(iter1 = S1map.begin(); iter1 != S1map.end(); iter1++) {
            unsigned long indexs1 = iter1->first;
            vector<unsigned char> ttmp1 = iter1->second;
            for (k=0; k<ttmp1.size(); k=k+16) {
                unsigned char c1hf=sbox_4_inv[(ttmp1[k+2]&0xF)^roundkey16[1][1]]^sbox_4_inv[(ttmp1[k+4]&0xF)]^sbox_4_inv[(ttmp1[k+6]&0xF)];
                unsigned char c2hf=sbox_4_inv[(ttmp1[k+10]&0xF)^roundkey16[1][1]^roundDK1[16][1][1]]^sbox_4_inv[(ttmp1[k+12]&0xF)]^sbox_4_inv[(ttmp1[k+14]&0xF)];
                indexs1 = (indexs1<<4)| c1hf;
                indexs1 = (indexs1<<4)| c2hf;
                if(Hmap.find(indexs1) != Hmap.end()) {
                    vector<unsigned char> ttmp12=Hmap[indexs1];
                    for(kk = 0; kk < 16; kk++)
                         ttmp12.push_back(ttmp1[k+kk]);
                    Hmap[indexs1] = ttmp12;
                } else {
                    vector<unsigned char> ttmp12;
                    for(kk = 0; kk < 16; kk++)
                         ttmp12.push_back(ttmp1[k+kk]);
                    Hmap[indexs1] = ttmp12;
                }
            }
        }
       
       map<unsigned long, vector<unsigned char> >::iterator iter2;
       for(iter2 = S2map.begin(); iter2 != S2map.end(); iter2++) {
           unsigned long indexs2 = iter2->first;
           vector<unsigned char> ttmp2 = iter2->second;
           for (k=0; k<ttmp2.size(); k=k+16) {
               unsigned char c3hf=sbox_4_inv[(ttmp2[k+2]&0xF)^roundkey16[1][1]]^sbox_4_inv[(ttmp2[k+4]&0xF)]^sbox_4_inv[(ttmp2[k+6]&0xF)];
               unsigned char c4hf=sbox_4_inv[(ttmp2[k+10]&0xF)^roundkey16[1][1]^roundDK1[16][1][1]]^sbox_4_inv[(ttmp2[k+12]&0xF)]^sbox_4_inv[(ttmp2[k+14]&0xF)];
               indexs2 = (indexs2<<4)| c3hf;
               indexs2 = (indexs2<<4)| c4hf;
               if(Hmap.find(indexs2) != Hmap.end()) {
                   for(kk = 0; kk < 8; kk++)
                       c[2][kk]=ttmp2[k+kk];
                   for(kk = 0; kk < 8; kk++)
                       c[3][kk]=ttmp2[k+kk+8];
                   vector<unsigned char> ttmp22=Hmap[indexs2];
                   for (kk1=0; kk1<ttmp22.size(); kk1=kk1+16) {
                       for(kk2 = 0; kk2 < 8; kk2++)
                           c[0][kk2]=ttmp22[kk1+kk2];
                       for(kk = 0; kk < 8; kk++)
                           c[1][kk2]=ttmp22[kk1+kk2+8];
                       keyrecovery(c);
                   }
               }
           }
       }
   }
    unsigned long long rightkey;
    rightkey = roundkeyCells_k1[15][0][1];
    rightkey = (rightkey<<4) | roundkeyCells_k1[15][0][3];
    rightkey = (rightkey<<4) | roundkeyCells_k1[15][1][3];
    rightkey = (rightkey<<4) | roundkeyCells_k1[15][0][2];
    rightkey = (rightkey<<4) | roundkeyCells_k1[15][1][0];
    rightkey = (rightkey<<4) | roundkeyCells_k1[15][1][1];
    rightkey = (rightkey<<4) | roundkeyCells_k1[14][0][3];
    rightkey = (rightkey<<4) | roundkeyCells_k1[14][1][3];
    for (int gk16=0; gk16<8; gk16++)
        rightkey=(rightkey<<4)|roundkeyCells_k1[16][gk16>>2][gk16&0x3];
    cout << keycounter[rightkey] << endl;
    if (keycounter[rightkey]>=1)
        cout << "The attack on 17-round skinny-64-128 succeed" << endl;
    
    
   clock_t FINISH=clock();
   double duration=(double)(FINISH-START)/CLOCKS_PER_SEC;
   cout << "Running time: "<<duration << endl;


   return 0;
}




