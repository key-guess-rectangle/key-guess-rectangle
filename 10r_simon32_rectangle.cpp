#include <iostream>
#include<time.h>
#include<map>
#include <vector>
using namespace std;

typedef unsigned int uint32;
#define rotateleft32(x,l) (((x<<l) |( x>> (16-l))) & 0xFFFF)
#define rotateright32(x,l) (((x>>l) |( x<< (16-l))) & 0xFFFF)
#define TAKE_BIT(x, pos) (((x) >> (pos)) & 0x1)

#define COMPUTE_x5(XL, XR) (TAKE_BIT(XL, 5) ^ TAKE_BIT(XR, 7) ^ (TAKE_BIT(XR, 6) & TAKE_BIT(XR, 13)))
#define COMPUTE_x7(XL, XR) (TAKE_BIT(XL, 7) ^ TAKE_BIT(XR, 9) ^ (TAKE_BIT(XR, 8) & TAKE_BIT(XR, 15)))

const int z[5][62] = {
{1,1,1,1,1,0,1,0,0,0,1,0,0,1,0,1,0,1,1,0,0,0,0,1,1,1,0,0,1,1,0, 1,1,1,1,1,0,1,0,0,0,1,0,0,1,0,1,0,1,1,0,0,0,0,1,1,1,0,0,1,1,0},
{1,0,0,0,1,1,1,0,1,1,1,1,1,0,0,1,0,0,1,1,0,0,0,0,1,0,1,1,0,1,0, 1,0,0,0,1,1,1,0,1,1,1,1,1,0,0,1,0,0,1,1,0,0,0,0,1,0,1,1,0,1,0},
{1,0,1,0,1,1,1,1,0,1,1,1,0,0,0,0,0,0,1,1,0,1,0,0,1,0,0,1,1,0,0, 0,1,0,1,0,0,0,0,1,0,0,0,1,1,1,1,1,1,0,0,1,0,1,1,0,1,1,0,0,1,1},
{1,1,0,1,1,0,1,1,1,0,1,0,1,1,0,0,0,1,1,0,0,1,0,1,1,1,1,0,0,0,0, 0,0,1,0,0,1,0,0,0,1,0,1,0,0,1,1,1,0,0,1,1,0,1,0,0,0,0,1,1,1,1},
{1,1,0,1,0,0,0,1,1,1,1,0,0,1,1,0,1,0,1,1,0,1,1,0,0,0,1,0,0,0,0, 0,0,1,0,1,1,1,0,0,0,0,1,1,0,0,1,0,1,0,0,1,0,0,1,1,1,0,1,1,1,1}
};
const int c32 = (1<<16) - 4;

uint32 func32(uint32 x){
    uint32 y = rotateleft32(x,1);
    y &= rotateleft32(x,8);
    y ^= rotateleft32(x,2);
    return y;
}
void round32(uint32 in[2], uint32 out[2], uint32 k){//[0]left, [1]right
    out [1] = in [0];
    out[0] = in[1] ^ func32(in[0]) ^ k;
}

void round32_inv(uint32 in[2], uint32 out[2], uint32 k)
{
    out [0] = in [1];
    out[1] = in[0] ^ func32(in[1]) ^ k;
}

void key32(uint32 k[32]){
    for(int i=4; i < 32; ++i){
        k[i] = rotateright32(k[i-1], 3);
        k[i] ^= k[i-3];
        k[i] ^= rotateright32(k[i], 1);
        k[i] ^= c32 ^ z[0][i-4] ^ k[i-4];
    }
}
void ENC32(uint32 in[2],uint32 out[2], uint32 subkey[32], int rN=32)
{//The whole 32-round encryption of SIMON32/64
    uint32 tmp[2];
    tmp [0] = in[0];
    tmp [1] = in[1];
    for(int i=0; i<rN; ++i){
        round32(tmp, out, subkey[i]);
        tmp [0] = out [0];
        tmp [1] = out [1];
    }
}

void ENC(uint32 in[2],uint32 out[2], uint32 subkey[32], int rN)
{//The whole 32-round encryption of SIMON32/64
    uint32 tmp[2];
    tmp [0] = in[0];
    tmp [1] = in[1];
    for(int i=0; i<rN; ++i){
        round32(tmp, out, subkey[i]);
        tmp [0] = out [0];
        tmp [1] = out [1];
    }
}

void DEC(uint32 in[2],uint32 out[2], uint32 subkey[32], int rN)
{//The whole 32-round encryption of SIMON32/64
    uint32 tmp[2];
    tmp [0] = out[0];
    tmp [1] = out[1];
    for(int i=rN-1; i>=0; --i){
        round32_inv(tmp, in, subkey[i]);
        tmp [0] = in [0];
        tmp [1] = in [1];
    }
}

int boomerang(uint32 din[2],uint32 dout[2], uint32 subkey[32], int rN, int N)
{
    int i, j, t, num=0;
    uint32 p1[2],p2[2],p3[2],p4[2];
    uint32 c1[2],c2[2],c3[2],c4[2];
    bool flag;
    
    for (t=0; t<N; t++){
        // randomly choose p1
        for(i = 0; i < 2; i++) p1[i] = rand() & 0xffff;
        // derive p2
        for(i = 0; i < 2; i++) p2[i] = p1[i]^din[i];
        ENC(p1, c1, subkey, rN);
        ENC(p2, c2, subkey, rN);
        
        // derive c3
        for(i = 0; i < 2; i++) c3[i] = c1[i]^dout[i];
        // derive c4
        for(i = 0; i < 2; i++) c4[i] = c2[i]^dout[i];
        DEC(p3, c3, subkey, rN);
        DEC(p4, c4, subkey, rN);
        flag = 1;
        for(i = 0; i < 2; i++)
            if ((p3[i]^p4[i]) != din[i])
                flag = 0;
        if (flag) {num ++; }
    }
    return num;
}

void test_dis()
{
    uint32 din[2]={0,16384};
    uint32 dout[2]={16384,0};
    uint32 subkey[32]={0};
    
    int sum=0;
    for(int k = 0; k < 10; k++) {
        for(int m = 0; m < 4; m++)
            subkey[m] = (uint32)random() & 0xffff;
        key32(subkey);
        sum +=boomerang(din, dout, subkey, 6, 1000);
    }
    cout << sum << endl;
    
}

int main(int argc, const char * argv[]) {
    // insert code here...
    srand(time(NULL));
    uint32 p1[2],p2[2],p3[2],p4[2];
    uint32 c1[2],c2[2],c3[2],c4[2];
    uint32 din[2]={0,16384};
    uint32 dout[2]={16384,0};
    uint32 subkey[32]={0};
    
    double rate=0;
    for (int t=0; t<100; t++) {
        for(int m = 0; m < 4; m++) //random choose the master key
            subkey[m] = (uint32)random() & 0xffff;
        key32(subkey);
        
        int y=1<<18;
        map<unsigned long, vector<uint32>> finalout;
        for (int i=0; i<y; i++) {
            p1[0]=(uint32)random() & 0xffff;
            p1[1]=(uint32)random() & 0xffff;

            p2[0]=p1[0]^din[1];
            p2[1]=func32(p1[0]) ^ p1[1] ^ func32(p1[0]^din[1]);
            
            ENC(p1, c1, subkey, 10);
            ENC(p2, c2, subkey, 10);
            

            uint32 tmpc;
            tmpc = c1[0];
            c1[0]=c1[1];
            c1[1]=tmpc^func32(c1[1]);
            tmpc = c2[0];
            c2[0]=c2[1];
            c2[1]=tmpc^func32(c2[1]);
            unsigned long index1 = (c1[0] & 0xbe7c)<<16 | (c1[1] & 0x7FbF);
            unsigned long index2 = (c2[0] & 0xbe7c)<<16 | (c2[1] & 0x7FbF);
            unsigned long index = index1 << 32 | index2;
            
            if(finalout.find(index) != finalout.end())
            {
                vector<uint32> ttmp = finalout[index];
                ttmp.push_back(c1[0]);
                ttmp.push_back(c1[1]);
                ttmp.push_back(c2[0]);
                ttmp.push_back(c2[1]);
                finalout[index] = ttmp;
            } else {
                vector<uint32> ttmp;
                ttmp.push_back(c1[0]);
                ttmp.push_back(c1[1]);
                ttmp.push_back(c2[0]);
                ttmp.push_back(c2[1]);
                finalout[index] = ttmp;
            }
        }
        int keycounter[1<<15]={0};
        map<unsigned long, vector<uint32>>::iterator iter;
        for(iter = finalout.begin(); iter != finalout.end(); iter++) {
            unsigned long index = iter->first ^ 0x0004000100040001; //X9
            if(finalout.find(index) != finalout.end()) {
                //cout << "Find!\n";
                vector<uint32> ttmp1 = iter->second;
                vector<uint32> ttmp2 = finalout[index];
                for (int i=0; i<ttmp1.size(); i=i+4) {
                    for (int j=0; j<ttmp2.size(); j=j+4) {
                        for (uint32 k=0; k<(1<<12); k++) {
                            uint32 key9 = (TAKE_BIT(k,11)<<15)|(TAKE_BIT(k,10)<<14)|(TAKE_BIT(k,9)<<13)|(TAKE_BIT(k,8)<<12)|(TAKE_BIT(k,7)<<9)|(TAKE_BIT(k,6)<<8)|(TAKE_BIT(k,5)<<7)|(TAKE_BIT(k,4)<<6)|(TAKE_BIT(k,3)<<5)|(TAKE_BIT(k,2)<<4)|(TAKE_BIT(k,1)<<3)|(TAKE_BIT(k,0)<<0);
                            uint32 c1x8[2], c2x8[2], c3x8[2], c4x8[2];
                            c1x8[0]=ttmp1[i+1]^key9;
                            c1x8[1]=ttmp1[i]^func32(ttmp1[i+1]^key9);
                            c2x8[0]=ttmp1[i+3]^key9;
                            c2x8[1]=ttmp1[i+2]^func32(ttmp1[i+3]^key9);
                            c3x8[0]=ttmp2[j+1]^key9;
                            c3x8[1]=ttmp2[j]^func32(ttmp2[j+1]^key9);
                            c4x8[0]=ttmp2[j+3]^key9;
                            c4x8[1]=ttmp2[j+2]^func32(ttmp2[j+3]^key9);
                            if ((((c1x8[1]^c3x8[1])&0x4183)==0x4000) && (((c2x8[1]^c4x8[1])&0x4183)==0x4000)) {
                                for (uint32 k2=0; k2<8; k2++) {
                                    uint32 key8 = (TAKE_BIT(k2,2)<<14)|(TAKE_BIT(k2,1)<<7)|(TAKE_BIT(k2,0)<<5);
                                    //uint32 key8 = (TAKE_BIT(k2,2)<<9)|(TAKE_BIT(k2,1)<<7);
                                    uint32 c1x7[2], c2x7[2], c3x7[2], c4x7[2];
                                    c1x7[1]=c1x8[0]^func32(c1x8[1]^key8);
                                    c2x7[1]=c2x8[0]^func32(c2x8[1]^key8);
                                    c3x7[1]=c3x8[0]^func32(c3x8[1]^key8);
                                    c4x7[1]=c4x8[0]^func32(c4x8[1]^key8);
                                    if ((((c1x7[1]^c3x7[1])&0x8040)==0x0000) && (((c2x7[1]^c4x7[1])&0x8040)==0x0000)) {
                                        uint32 indexkey=k<<3|k2;
                                        keycounter[indexkey]++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        uint32 rightkey = (TAKE_BIT(subkey[9],15)<<14)|(TAKE_BIT(subkey[9],14)<<13)|(TAKE_BIT(subkey[9],13)<<12)|(TAKE_BIT(subkey[9],12)<<11)|(TAKE_BIT(subkey[9],9)<<10)|(TAKE_BIT(subkey[9],8)<<9)|(TAKE_BIT(subkey[9],7)<<8)|(TAKE_BIT(subkey[9],6)<<7)|(TAKE_BIT(subkey[9],5)<<6)|(TAKE_BIT(subkey[9],4)<<5)|(TAKE_BIT(subkey[9],3)<<4)|(TAKE_BIT(subkey[9],0)<<3)|(TAKE_BIT(subkey[8],14)<<2)|(TAKE_BIT(subkey[8],7)<<1)|TAKE_BIT(subkey[8],5);
        int num=0;
        int h=5;
        //cout << rightkey <<endl;
        //cout <<keycounter[rightkey]<< endl;
        for (int i=0; i<(1<<15); i++) {
            //if (keycounter[i] != 0)
                //cout <<keycounter[i]<< endl;
            if (keycounter[rightkey]>=keycounter[i])
                num++;
        }

        if (num >= pow(2,h)) {
            rate = rate+1;
            //cout << "Success" << t << ": The right key counter is in the first 2^{15-h} counters" <<endl;;
        }
    }
    
    cout << "Success probability:" << (rate/100) << endl;
    return 0;
}
