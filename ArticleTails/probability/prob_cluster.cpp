#include<iostream>
#include<cmath>
using namespace std;

int sbox_4[16] = {12,6,9,0,1,10,2,11,3,8,5,13,4,14,7,15};
int sbox_8[256] = {0x65 , 0x4c , 0x6a , 0x42 , 0x4b , 0x63 , 0x43 , 0x6b , 0x55 , 0x75 , 0x5a , 0x7a , 0x53 , 0x73 , 0x5b , 0x7b ,0x35 , 0x8c , 0x3a , 0x81 , 0x89 , 0x33 , 0x80 , 0x3b , 0x95 , 0x25 , 0x98 , 0x2a , 0x90 , 0x23 , 0x99 , 0x2b ,0xe5 , 0xcc , 0xe8 , 0xc1 , 0xc9 , 0xe0 , 0xc0 , 0xe9 , 0xd5 , 0xf5 , 0xd8 , 0xf8 , 0xd0 , 0xf0 , 0xd9 , 0xf9 ,0xa5 , 0x1c , 0xa8 , 0x12 , 0x1b , 0xa0 , 0x13 , 0xa9 , 0x05 , 0xb5 , 0x0a , 0xb8 , 0x03 , 0xb0 , 0x0b , 0xb9 ,0x32 , 0x88 , 0x3c , 0x85 , 0x8d , 0x34 , 0x84 , 0x3d , 0x91 , 0x22 , 0x9c , 0x2c , 0x94 , 0x24 , 0x9d , 0x2d ,0x62 , 0x4a , 0x6c , 0x45 , 0x4d , 0x64 , 0x44 , 0x6d , 0x52 , 0x72 , 0x5c , 0x7c , 0x54 , 0x74 , 0x5d , 0x7d ,0xa1 , 0x1a , 0xac , 0x15 , 0x1d , 0xa4 , 0x14 , 0xad , 0x02 , 0xb1 , 0x0c , 0xbc , 0x04 , 0xb4 , 0x0d , 0xbd ,0xe1 , 0xc8 , 0xec , 0xc5 , 0xcd , 0xe4 , 0xc4 , 0xed , 0xd1 , 0xf1 , 0xdc , 0xfc , 0xd4 , 0xf4 , 0xdd , 0xfd ,0x36 , 0x8e , 0x38 , 0x82 , 0x8b , 0x30 , 0x83 , 0x39 , 0x96 , 0x26 , 0x9a , 0x28 , 0x93 , 0x20 , 0x9b , 0x29 ,0x66 , 0x4e , 0x68 , 0x41 , 0x49 , 0x60 , 0x40 , 0x69 , 0x56 , 0x76 , 0x58 , 0x78 , 0x50 , 0x70 , 0x59 , 0x79 ,0xa6 , 0x1e , 0xaa , 0x11 , 0x19 , 0xa3 , 0x10 , 0xab , 0x06 , 0xb6 , 0x08 , 0xba , 0x00 , 0xb3 , 0x09 , 0xbb ,0xe6 , 0xce , 0xea , 0xc2 , 0xcb , 0xe3 , 0xc3 , 0xeb , 0xd6 , 0xf6 , 0xda , 0xfa , 0xd3 , 0xf3 , 0xdb , 0xfb ,0x31 , 0x8a , 0x3e , 0x86 , 0x8f , 0x37 , 0x87 , 0x3f , 0x92 , 0x21 , 0x9e , 0x2e , 0x97 , 0x27 , 0x9f , 0x2f ,0x61 , 0x48 , 0x6e , 0x46 , 0x4f , 0x67 , 0x47 , 0x6f , 0x51 , 0x71 , 0x5e , 0x7e , 0x57 , 0x77 , 0x5f , 0x7f ,0xa2 , 0x18 , 0xae , 0x16 , 0x1f , 0xa7 , 0x17 , 0xaf , 0x01 , 0xb2 , 0x0e , 0xbe , 0x07 , 0xb7 , 0x0f , 0xbf ,0xe2 , 0xca , 0xee , 0xc6 , 0xcf , 0xe7 , 0xc7 , 0xef , 0xd2 , 0xf2 , 0xde , 0xfe , 0xd7 , 0xf7 , 0xdf , 0xff};

int ddt4[16][16]={0};
int ddt8[256][256]={0};
int TWEAKEY_P[16] = {9,15,8,13,10,14,12,11,0,1,2,3,4,5,6,7};
void DDT4()
{
   int in, out, m0;
   for (in=0; in<16; in++)
   {
       for (m0=0; m0<16; m0++)
       {
           out = sbox_4[m0] ^ sbox_4[m0^in];
           ddt4[in][out] ++;
       }
   }
}
void DDT8()
{
   int in, out, m0;
   for (in=0; in<256; in++)
   {
       for (m0=0; m0<256; m0++)
       {
           out = sbox_8[m0] ^ sbox_8[m0^in];
           ddt8[in][out] ++;
       }
   }
}

void skinny_128_384_p()
{
    long double prob=0;
    long double clprob=0;

    for(int u=1; u<256; u++) {
        if(ddt8[32][u]>0 && ddt8[u][91]>0){
            for(int v=1; v<256; v++) {
                if(ddt8[91][v]>0 && ddt8[v][192]>0) {
                    for(int w1=1; w1<256; w1++) {
                        if(ddt8[v][w1]>0 && ddt8[w1][4]>0) {
                            for(int w2=1; w2<256; w2++) {
                                if(ddt8[v][w2]>0 && ddt8[w2][4]>0) {
                                
                                    prob = ddt8[32][u]*ddt8[u][91];
                                    prob *= ddt8[91][v]*ddt8[v][192];
                                    prob *= ddt8[v][w1]*ddt8[v][w2];
                                    prob *= ddt8[w1][4]*ddt8[w1][4]*ddt8[w1][4];
                                    prob *= ddt8[w2][4]*ddt8[w2][4]*ddt8[w2][4];
                                    prob *= ddt8[192][4]*ddt8[192][4];
                            
                                    clprob += prob*prob;
                                
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    cout << "The probability of (R0-R10)-round for 23-round skinny_128_384:"<<endl;
    cout << "p="<<log(sqrt(clprob))/log(2.0)-14*8 << endl;
}

void skinny_128_384_q()
{
    long double prob=0;
    long double clprob=0;

    for(int w=1; w<256; w++) {
        if(ddt8[2][w]>0&&ddt8[66][w]>0){
            for(int x=1; x<256; x++) {
                if(ddt8[w][x]>0 && ddt8[x][80]>0) {
                   prob = ddt8[2][w]*ddt8[2][w]*ddt8[66][88]*ddt8[66][w];
                   prob *= ddt8[w][x]*ddt8[x][80];
                   clprob += prob*prob;

                }
            }
        }
    }
    
    cout << "The probability of (R14-R22)-round for 23-round skinny_128_384:"<<endl;
    cout << "q="<<log(sqrt(clprob))/log(2.0)-6*8 << endl;
}

void skinny_64_192_p()
{
    long double prob=0;
    long double clprob=0;
    
    for(int k11=1; k11<16; k11++) {
        if(ddt4[1][k11]>0) {
            for(int k12=1; k12<16; k12++) {
                if(ddt4[1][k12]>0 && ddt4[k12][11]>0) {
                    if(ddt4[k11^9][11]>0 ){
                        prob = ddt4[1][k11]*ddt4[1][k12]*ddt4[1][8];
                        prob *= ddt4[k12][11];
                        prob *= ddt4[k11^9][11]*ddt4[11][14]*ddt4[5][2];
                        prob *= ddt4[1][9]*ddt4[1][8];
                        clprob += prob*prob;
                    }
                }
            }
        }
    }
    cout << "The probability of r0=10 round for 22-round skinny_64_192:"<<endl;
    cout << "p="<<log(sqrt(clprob))/log(2.0)-36<< endl;
}

/*void skinny_64_128_p()
{
    long double prob=0;
    long double clprob=0;
    for(int k11=1; k11<16; k11++) {
        if(ddt4[5][k11]>0) {
            for(int k12=1; k12<256; k12++){
                if(ddt4[5][k12]>0){
                    prob = ddt4[2][5];
                    prob *= ddt4[5][k11]*ddt4[5][k12];
                    prob *= ddt4[k11^12][6]*ddt4[k12][6];
                    prob *= ddt4[6][7];
                    prob *= ddt4[13][15];
                    clprob += prob*prob;
                }
            }
        }
    }
    cout << "The probability of r0=8 round for skinny_64_128:"<<endl;
    cout << "p="<<log(sqrt(clprob))/log(2.0)-28 << endl;
}*/

void skinny_128_256_p_19r()
{
    long double prob=0;
    long double clprob=0;
    for(int u=1; u<256; u++) {
        if(ddt8[203][u]>0 && ddt8[u][88]>0){
            for(int v=1; v<256; v++) {
                if(ddt8[176][v]>0 && ddt8[v][208]>0) {
                    for(int w1=1; w1<256; w1++) {
                        if(ddt8[v][w1]>0 && ddt8[w1][4]>0) {
                            for(int w2=1; w2<256; w2++) {
                                if(ddt8[v][w2]>0 && ddt8[w2][4]>0) {
                                
                                    prob = ddt8[203][u]*ddt8[203][u]*ddt8[203][u]*ddt8[u][88];
                                    prob *= ddt8[176][v]*ddt8[v][208];
                                    prob *= ddt8[v][w1]*ddt8[v][w2];
                                    prob *= ddt8[w1][4]*ddt8[w1][4]*ddt8[w1][4];
                                    prob *= ddt8[w2][4]*ddt8[w2][4]*ddt8[w2][4];
                                    prob *= ddt8[208][4]*ddt8[208][4];
                            
                                    clprob += prob*prob;
                                
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    cout << "The probability of (R0-R8)-round for 19-round skinny_128_256:"<<endl;
    cout << "p="<<log(sqrt(clprob))/log(2.0)-16*8 << endl;
}
void skinny_128_256_q_19r()
{
    long double prob=0;
    long double clprob=0;
    
    for(int x=1; x<256; x++) {
        if(ddt8[32][x]>0 && ddt8[x][192]>0) {
            prob = ddt8[32][x]*ddt8[x][192];
            clprob += prob*prob;
        }
    }
 
    cout << "The probability of (R13-R18)-round for 19-round skinny_128_256:"<<endl;
    cout << "q="<<log(sqrt(clprob))/log(2.0)-2*8 << endl;
}

void skinny_128_256_p_18r()
{
    long double prob=0;
    long double clprob=0;
             
    prob = ddt8[2][8];
    prob *= ddt8[16][64];
    clprob += prob*prob;
    
    cout << "The probability of (R0-R5)-round for 18-round skinny_128_256:"<<endl;
    cout << "p="<<log(sqrt(clprob))/log(2.0)-2*8 << endl;
}
void skinny_128_256_q_18r()
{
    long double prob=0;
    long double clprob=0;
    
    for(int u=1; u<256; u++) {
        if(ddt8[u][155]>0){
            for(int v=1; v<256; v++) {
                if(ddt8[v][u]>0) {
                    for(int w1=1; w1<256; w1++) {
                        if(ddt8[w1][v]>0 && ddt8[48][w1]>0&& ddt8[128][w1]>0&& ddt8[160][w1]>0) {
                            for(int w2=1; w2<256; w2++) {
                                if(ddt8[w2][v]>0 && ddt8[48][w2]>0) {
                                    for(int w3=1; w3<256; w3++) {
                                        if(ddt8[w3][v]>0 && ddt8[w3][86]>0&& ddt8[48][w3]>0&& ddt8[16][w3]>0) {
                                    prob = ddt8[u][155]*ddt8[v][u];
                                    prob *= ddt8[w1][v]*ddt8[w2][v]*ddt8[w3][v]*ddt8[w3][86];
                                    prob *= ddt8[48][w1]*ddt8[128][w1]*ddt8[160][w1];
                                    prob *= ddt8[48][w2];
                                    prob *= ddt8[16][w3] * ddt8[48][w3];
                                   
                            
                                    clprob += prob*prob;
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
 
    cout << "The probability of (R10-R17)-round for 18-round skinny_128_256:"<<endl;
    cout << "q="<<log(sqrt(clprob))/log(2.0)-12*8 << endl;
}

int main()
{
    DDT4();
    DDT8();
    skinny_128_384_p();
    skinny_128_384_q();
    skinny_64_192_p();
    skinny_128_256_p_19r();
    skinny_128_256_q_19r();
    skinny_128_256_p_18r();
    skinny_128_256_q_18r();
    return 0;
}
