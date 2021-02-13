#include"masking.h"
#include"mkaes.h"

void MixColumns_GenMasking(MaskingValue *MV) {
	byte tmp[4];

	MV->M1_hat = xtimes(MV->M1) ^ (xtimes(MV->M2) ^ MV->M2) ^ MV->M3 ^ MV->M4;
	MV->M2_hat = xtimes(MV->M2) ^ (xtimes(MV->M3) ^ MV->M3) ^ MV->M4 ^ MV->M1;
	MV->M3_hat = xtimes(MV->M3) ^ (xtimes(MV->M4) ^ MV->M4) ^ MV->M1 ^ MV->M2;
	MV->M4_hat = xtimes(MV->M4) ^ (xtimes(MV->M1) ^ MV->M1) ^ MV->M2 ^ MV->M3;
}

void Show_all_MaskingValue(MaskingValue *MV){
    printf("================== masking value check ==================\n");
    printf("M = %02x, M' = %02x", MV->M, MV->M_hat);

    printf("\nM1     = %02x", MV->M1);
    printf("\nM2     = %02x", MV->M2);
    printf("\nM3     = %02x", MV->M3);
    printf("\nM4     = %02x", MV->M4);

    printf("\nM1_hat = %02x", MV->M1_hat);
    printf("\nM2_hat = %02x", MV->M2_hat);
    printf("\nM3_hat = %02x", MV->M3_hat);
    printf("\nM4_hat = %02x", MV->M4_hat);
    printf("\n");
}

void Show_MaskingSbox(MaskingValue *MV){
    int count = 0;
    printf("================== masking SBOX ==================\n");
    for(int i=0; i<0x100; i++){
        printf(" %02x |", MV->Masking_S_BOX[i]);
        count ++;
        if(count == 16){
            printf("\n");
            count = 0;
        }
    }
}

void GenMaskingValue(MaskingValue *MV){
    
    srand( (unsigned)time(NULL));
    MV->M = rand()%0xff;
    MV->M_hat = rand()%0xff;

    MV->M1 = rand()%0xff;
    MV->M2 = rand()%0xff;
    MV->M3 = rand()%0xff;
    MV->M4 = rand()%0xff;

    MixColumns_GenMasking(MV);

}

void IdentityMaskingValue(MaskingValue *MV){
    MV->M = 0;
    MV->M_hat = 0;
    MV->M1 = 0;
    MV->M2 = 0;
    MV->M3 = 0;
    MV->M4 = 0;

    MixColumns_GenMasking(MV);
}

void GenMaskingSbox(MaskingValue *MV){
    for(int x = 0; x<0x100; x++){
       MV->Masking_S_BOX[x^MV->M] = S_BOX[x]^MV->M_hat;
    }
}

void TestMaskingSbox(MaskingValue *MV){
    byte a,b;
    for(int x=0; x<0x100; x++){
        a = S_BOX[x]^MV->M_hat;
        b = MV->Masking_S_BOX[x^MV->M];   
        if(a == b){
            printf("S_BOX[%02x]^%02x          = %02x\n",x,MV->M_hat, a);
            printf("Masking_S_BOX[%02x^%02x]  = %02x\n",x,MV->M,b);
            printf("=============True===============\n");
        } 
    }
}
