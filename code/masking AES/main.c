#include"mkaes.h"

/**
 * @brief Master key setting	
 * @param masterkey	    (input) masterkey of MKAES
 * @param masterkey 	(output)masterkey to input user of MKAES 
 */
void settingkey(MKAES *AES){
    for(int i=0;i<16;i++){
        AES->masterkey[i] = i;
    }
}

/**
 * @brief Plaintxt setting	
 * @param Plaintxt	    (input) Plaintxt of MKAES
 * @param Plaintxt 	    (output)Plaintxt to input user of MKAES 
 */
void settingPlaintxt(MKAES *AES){
    for(int i=0; i<16;i++){
        AES->Plaintxt[i] = (i<<4)^i;
    }
}

/**
 * @brief State setting	
 * @param State 	    (input) State of MKAES
 * @param txt    	    (input) Plaintxt or Ciphertxt
 * @param State 	    (output)State to input user of MKAES 
 */
void settingState(MaskingValue *MV, MKAES *AES, byte *txt){
    for(int i=0;i<4;i++){
        AES->State[4*i + 0] = txt[4*i+0] ^ MV->M1_hat;
        AES->State[4*i + 1] = txt[4*i+1] ^ MV->M2_hat;
        AES->State[4*i + 2] = txt[4*i+2] ^ MV->M3_hat;
        AES->State[4*i + 3] = txt[4*i+3] ^ MV->M4_hat;
    }
    if(dummy_number != 0){
        srand( (unsigned)time(NULL));
        for(int i=0;i<dummy_number;i++){
            AES->dummy_State[i] = rand()%0xff;
        }
    }
}

/**
 * @brief Show all roundkey applied masking value	
 * @param rndkey	    (input) roundkey of MKAES
 */
void ShowRoundKey(MKAES *AES){
    printf("================== show all roundkey ==================");
    for(int i=0;i<=Roundnum;i++){
        printf("\nround = %d \n", i);
        for(int j=0;j<4;j++){
            printf("|%02x|%02x|%02x|%02x| ", AES->rndkey[4*i+j][0],
                                             AES->rndkey[4*i+j][1], 
                                             AES->rndkey[4*i+j][2], 
                                             AES->rndkey[4*i+j][3]);
        }
    }
    printf("\n");
}

/**
 * @brief Show State and dummy State	
 * @param State	        (input) State and dummy State
 */
void ShowState(MKAES *AES){
    printf("Stats =                             dummy State = \n");
    /* show all state */
    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            printf("%02x || ", AES->State[4*j + i]);
        }
        /* show all dummy state */
        if(dummy_number != 0){
            printf("             ");
            for(int j=0;j<dummy_number/4; j++)
                printf("%02x || ", AES->dummy_State[i + 4*j]);
        }
        else{
            printf("             not exist dummy state");
        }
        printf("\n");

    }
    printf("\n");
}

/**
 * @brief Show Shuffled position using Fisher Yates Shuffle	
 * @param position        (input) position
 */
void ShowArrayPosition(Array_info *Array){
    int i;
    printf("Current Shuffling column array = ");
    for(i=0;i<4+dummy_number/4;i++){
        printf("%d || ", Array->col_array[i]);
    }
    printf("\n");
    printf("Current Shuffling row array    = ");
    for(i=0;i<4;i++){
        printf("%d || ", Array->row_array[i]);
    }
    printf("\n");
}

int main(){
    MaskingValue MV;
    MKAES AES;
    MKAES *AES_ptr;
    Array_info Array;

    AES_ptr = &AES;

    /* Generate Masking Value */
    GenMaskingValue(&MV);
    // IdentityMaskingValue(&MV);
    GenMaskingSbox(&MV);   
    
    /* Show Masking Value */
    Show_all_MaskingValue(&MV);
    Show_MaskingSbox(&MV);

    /* test Masking S box such that S'(x^m) = S(x) ^ m */
    // TestMaskingSbox(&MV);

    /* Generate Masking roundkey */
    settingkey(&AES);
    mkKeySchedule(&MV, &AES);
    ShowRoundKey(&AES);

    /* Generate Plaintxt and State */
    settingPlaintxt(&AES);

    /* Plaintxt ^ M1_hat, M2_hat, M3_hat and M4_hat */
    settingState(&MV ,&AES, AES_ptr->Plaintxt);
    // ShowState(&AES);

    /* Generate Array information about columns and rows */
    printf("================== Original Array ==================\n");
    settingArray(&Array);
    ShowArrayPosition(&Array);

    /* Generate Array information applied Fisher Yates Shuffle */
    FisherYatesShuffle(&Array);
    printf("================== Array applied Shuffle ==================\n");
    ShowArrayPosition(&Array);
 
    /* Print dummy state information */
    printf("================== information of dummy state ==================\n");
    printf("number of columns = %d\n", dummy_number/4);
    printf("dummy State = \n");
    if(dummy_number == 0){
        printf("not exist dummy state \n");
    }
    else{
        for(int i=0;i<4;i++){
            for(int j=0;j<dummy_number/4; j++)
                printf("%02x || ", AES.dummy_State[i + 4*j]);
            printf("\n");
        }
    }


    printf("================== PlainText ===================\n");
    ShowState(&AES);

    /* Encryption start */
    printf("================== round = 1 ===================\n");
    Randomized_Zone1(&Array, AES_ptr->State, &AES, &MV);
    ShiftRows(AES_ptr->State);
    ShowState(&AES);
    settingArray(&Array);
    for(int i=1; i<9; i++){
        Round(&Array, AES_ptr->State,&AES, &MV, i);
        printf("================== round = %d ===================\n", i+1);
        ShowState(&AES);
    }
    printf("================== round = 10 ===================\n");
    Randomized_Zone2(&Array, AES_ptr->State, &AES, &MV);
    ShiftRows(AES_ptr->State);
    ShowState(&AES);

}
