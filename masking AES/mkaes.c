#include"mkaes.h"

void FisherYatesShuffle(Array_info *Array){
	int i,j;
	int temp;
    /* Generate row array applied Fisher Yates Shuffle*/
	for(i=4-1; i>0; i--){
		j = rand()%i;
		temp = Array->row_array[i];
		Array->row_array[i] = Array->row_array[j];
		Array->row_array[j] = temp; 
	}
    /* Generate column array applied Fisher Yates Shuffle*/
    for(i=4+dummy_number/4-1; i>0; i--){
		j = rand()%i;
		temp = Array->col_array[i];
		Array->col_array[i] = Array->col_array[j];
		Array->col_array[j] = temp; 
	}
}

void settingArray(Array_info *Array){
    int i;
    for(i=0;i<4;i++){
        Array->col_array[i] = i;
        Array->row_array[i] = i;
    }

    for(i = 4; i<4+dummy_number/4; i++){
        Array->col_array[i] = i;
    }
}

void mkKeySchedule(MaskingValue *MV, MKAES *AES){
    int i;
    byte rcon[10] = { 0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36};
    byte temp[10] = {0,};
    byte temp2[4] = {0,};

    for(i=0; i<4;i++){
        AES->rndkey[0+i][0] = (AES->masterkey[0+4*i])^(MV->M)^(MV->M1_hat);
        AES->rndkey[0+i][1] = (AES->masterkey[1+4*i])^(MV->M)^(MV->M2_hat);
        AES->rndkey[0+i][2] = (AES->masterkey[2+4*i])^(MV->M)^(MV->M3_hat);
        AES->rndkey[0+i][3] = (AES->masterkey[3+4*i])^(MV->M)^(MV->M4_hat);
    }

    /* Generate next Keyword Wi+4, Wi+5, Wi+6, Wi+7 */
    for(i=0;i<Roundnum-1;i++){
        /* Generate next first keyword Wi+4 */
        temp2[0] = AES->rndkey[4*i+3][0] ^ MV->M1_hat;
        temp2[1] = AES->rndkey[4*i+3][1] ^ MV->M2_hat;
        temp2[2] = AES->rndkey[4*i+3][2] ^ MV->M3_hat;
        temp2[3] = AES->rndkey[4*i+3][3] ^ MV->M4_hat;

        AES->rndkey[4*i+4][3] = MV->Masking_S_BOX[temp2[0]] ^ AES->rndkey[4*i][3] ^ MV->M_hat;
        AES->rndkey[4*i+4][0] = MV->Masking_S_BOX[temp2[1]] ^ AES->rndkey[4*i][0] ^ MV->M_hat ^ rcon[i];
        AES->rndkey[4*i+4][1] = MV->Masking_S_BOX[temp2[2]] ^ AES->rndkey[4*i][1] ^ MV->M_hat;
        AES->rndkey[4*i+4][2] = MV->Masking_S_BOX[temp2[3]] ^ AES->rndkey[4*i][2] ^ MV->M_hat;

        /* Generate second and third keyword Wi+5, Wi+6*/
        temp[0] = AES->rndkey[4*i+4][0] ^ MV->M1_hat;
        temp[1] = AES->rndkey[4*i+4][1] ^ MV->M2_hat;
        temp[2] = AES->rndkey[4*i+4][2] ^ MV->M3_hat;
        temp[3] = AES->rndkey[4*i+4][3] ^ MV->M4_hat;

        AES->rndkey[4*i+5][0] = AES->rndkey[4*i+1][0] ^ MV->M ^ temp[0];
        AES->rndkey[4*i+5][1] = AES->rndkey[4*i+1][1] ^ MV->M ^ temp[1];
        AES->rndkey[4*i+5][2] = AES->rndkey[4*i+1][2] ^ MV->M ^ temp[2];
        AES->rndkey[4*i+5][3] = AES->rndkey[4*i+1][3] ^ MV->M ^ temp[3];

        temp[0] = AES->rndkey[4*i+5][0] ^ MV->M1_hat;
        temp[1] = AES->rndkey[4*i+5][1] ^ MV->M2_hat;
        temp[2] = AES->rndkey[4*i+5][2] ^ MV->M3_hat;
        temp[3] = AES->rndkey[4*i+5][3] ^ MV->M4_hat;

        AES->rndkey[4*i+6][0] = AES->rndkey[4*i+2][0] ^ MV->M ^ temp[0];
        AES->rndkey[4*i+6][1] = AES->rndkey[4*i+2][1] ^ MV->M ^ temp[1];
        AES->rndkey[4*i+6][2] = AES->rndkey[4*i+2][2] ^ MV->M ^ temp[2];
        AES->rndkey[4*i+6][3] = AES->rndkey[4*i+2][3] ^ MV->M ^ temp[3];

        /* Generate final keyword Wi+7 */
        temp[0] = AES->rndkey[4*i+6][0] ^ MV->M;
        temp[1] = AES->rndkey[4*i+6][1] ^ MV->M;
        temp[2] = AES->rndkey[4*i+6][2] ^ MV->M;
        temp[3] = AES->rndkey[4*i+6][3] ^ MV->M;

        AES->rndkey[4*i+7][0] = temp2[0] ^ temp[0];
        AES->rndkey[4*i+7][1] = temp2[1] ^ temp[1];
        AES->rndkey[4*i+7][2] = temp2[2] ^ temp[2];
        AES->rndkey[4*i+7][3] = temp2[3] ^ temp[3];
    }

    /* Generate final round key */
    temp2[0] = AES->rndkey[39][0] ^ MV->M1_hat;
    temp2[1] = AES->rndkey[39][1] ^ MV->M2_hat;
    temp2[2] = AES->rndkey[39][2] ^ MV->M3_hat;
    temp2[3] = AES->rndkey[39][3] ^ MV->M4_hat;

    AES->rndkey[40][3] = MV->Masking_S_BOX[temp2[0]] ^ AES->rndkey[36][3];
    AES->rndkey[40][0] = MV->Masking_S_BOX[temp2[1]] ^ AES->rndkey[36][0] ^ rcon[9];
    AES->rndkey[40][1] = MV->Masking_S_BOX[temp2[2]] ^ AES->rndkey[36][1];
    AES->rndkey[40][2] = MV->Masking_S_BOX[temp2[3]] ^ AES->rndkey[36][2];

    AES->rndkey[41][0] = AES->rndkey[40][0] ^ AES->rndkey[37][0];
    AES->rndkey[41][1] = AES->rndkey[40][1] ^ AES->rndkey[37][1];
    AES->rndkey[41][2] = AES->rndkey[40][2] ^ AES->rndkey[37][2];
    AES->rndkey[41][3] = AES->rndkey[40][3] ^ AES->rndkey[37][3];

    AES->rndkey[42][0] = AES->rndkey[41][0] ^ AES->rndkey[38][0] ^ MV->M1_hat;
    AES->rndkey[42][1] = AES->rndkey[41][1] ^ AES->rndkey[38][1] ^ MV->M2_hat;
    AES->rndkey[42][2] = AES->rndkey[41][2] ^ AES->rndkey[38][2] ^ MV->M3_hat;
    AES->rndkey[42][3] = AES->rndkey[41][3] ^ AES->rndkey[38][3] ^ MV->M4_hat;

    AES->rndkey[43][0] = temp2[0] ^ AES->rndkey[42][0];
    AES->rndkey[43][1] = temp2[1] ^ AES->rndkey[42][1];
    AES->rndkey[43][2] = temp2[2] ^ AES->rndkey[42][2];
    AES->rndkey[43][3] = temp2[3] ^ AES->rndkey[42][3];
    
}

void AddRoundKey(MKAES *AES, byte *state, int col, Array_info *Array, int round){
    byte position[4][4] = {
        {0x00, 0x05, 0x0A, 0x0F},
        {0x04, 0x09, 0x0E, 0x03},
        {0x08, 0x0D, 0x02, 0x07},
        {0x0C, 0x01, 0x06, 0x0B}
    };

    state[position[col][Array->row_array[0]]] ^= AES->rndkey[4*round + (col+Array->row_array[0])%4][Array->row_array[0]];
    state[position[col][Array->row_array[1]]] ^= AES->rndkey[4*round + (col+Array->row_array[1])%4][Array->row_array[1]];
    state[position[col][Array->row_array[2]]] ^= AES->rndkey[4*round + (col+Array->row_array[2])%4][Array->row_array[2]];
    state[position[col][Array->row_array[3]]] ^= AES->rndkey[4*round + (col+Array->row_array[3])%4][Array->row_array[3]];

}

void SubBytes(MaskingValue *MV, byte *state, Array_info *Array, int col){
    byte position[4][4] = {
        {0x00, 0x05, 0x0A, 0x0F},
        {0x04, 0x09, 0x0E, 0x03},
        {0x08, 0x0D, 0x02, 0x07},
        {0x0C, 0x01, 0x06, 0x0B}
    };

    state[position[col][Array->row_array[0]]] = MV->Masking_S_BOX[state[position[col][Array->row_array[0]]]];
    state[position[col][Array->row_array[1]]] = MV->Masking_S_BOX[state[position[col][Array->row_array[1]]]];
    state[position[col][Array->row_array[2]]] = MV->Masking_S_BOX[state[position[col][Array->row_array[2]]]];
    state[position[col][Array->row_array[3]]] = MV->Masking_S_BOX[state[position[col][Array->row_array[3]]]];
}

void MixColumns(byte *state, Array_info *Array, int col){
    byte temp[4];
    byte position[4][4] = {
        {0x00, 0x05, 0x0A, 0x0F},
        {0x04, 0x09, 0x0E, 0x03},
        {0x08, 0x0D, 0x02, 0x07},
        {0x0C, 0x01, 0x06, 0x0B}
    };

    temp[Array->row_array[0]] = xtimes(state[position[col][Array->row_array[0]]]) 
                                ^ (xtimes(state[position[col][((Array->row_array[0])+1)%4]]) ^ state[position[col][(Array->row_array[0]+1)%4]]) 
                                ^ state[position[col][((Array->row_array[0])+2)%4]] 
                                ^ state[position[col][((Array->row_array[0])+3)%4]];

    temp[Array->row_array[1]] = xtimes(state[position[col][Array->row_array[1]]]) 
                                ^ (xtimes(state[position[col][(Array->row_array[1]+1)%4]]) ^ state[position[col][(Array->row_array[1]+1)%4]]) 
                                ^ state[position[col][(Array->row_array[1]+2)%4]] 
                                ^ state[position[col][(Array->row_array[1]+3)%4]];
    temp[Array->row_array[2]] = xtimes(state[position[col][Array->row_array[2]]]) 
                                ^ (xtimes(state[position[col][(Array->row_array[2]+1)%4]]) ^ state[position[col][(Array->row_array[2]+1)%4]]) 
                                ^ state[position[col][(Array->row_array[2]+2)%4]] 
                                ^ state[position[col][(Array->row_array[2]+3)%4]];
    temp[Array->row_array[3]] = xtimes(state[position[col][Array->row_array[3]]]) 
                                ^ (xtimes(state[position[col][(Array->row_array[3]+1)%4]]) ^ state[position[col][(Array->row_array[3]+1)%4]]) 
                                ^ state[position[col][(Array->row_array[3]+2)%4]] 
                                ^ state[position[col][(Array->row_array[3]+3)%4]];

    state[position[col][Array->row_array[0]]] = temp[Array->row_array[0]];
    state[position[col][Array->row_array[1]]] = temp[Array->row_array[1]];
    state[position[col][Array->row_array[2]]] = temp[Array->row_array[2]];
    state[position[col][Array->row_array[3]]] = temp[Array->row_array[3]];
}

void SubBytes2(MaskingValue *MV, byte *state, Array_info *Array, int col){

    state[Array->row_array[0] + 4*col] = MV->Masking_S_BOX[state[Array->row_array[0] + 4*col]];

    state[Array->row_array[1] + 4*col] = MV->Masking_S_BOX[state[Array->row_array[1] + 4*col]];

    state[Array->row_array[2] + 4*col] = MV->Masking_S_BOX[state[Array->row_array[2] + 4*col]];

    state[Array->row_array[3] + 4*col] = MV->Masking_S_BOX[state[Array->row_array[3] + 4*col]];

}

void MixColumns2(byte *state, Array_info *Array, int col){
    byte temp[4];
    byte position[4][4] = {
        {0x00, 0x01, 0x02, 0x03},
        {0x04, 0x05, 0x06, 0x07},
        {0x08, 0x09, 0x0A, 0x0B},
        {0x0C, 0x0D, 0x0E, 0x0F}
    };

    temp[Array->row_array[0]] = xtimes(state[position[col][Array->row_array[0]]]) 
                                ^ (xtimes(state[position[col][((Array->row_array[0])+1)%4]]) ^ state[position[col][(Array->row_array[0]+1)%4]]) 
                                ^ state[position[col][((Array->row_array[0])+2)%4]] 
                                ^ state[position[col][((Array->row_array[0])+3)%4]];

    temp[Array->row_array[1]] = xtimes(state[position[col][Array->row_array[1]]]) 
                                ^ (xtimes(state[position[col][(Array->row_array[1]+1)%4]]) ^ state[position[col][(Array->row_array[1]+1)%4]]) 
                                ^ state[position[col][(Array->row_array[1]+2)%4]] 
                                ^ state[position[col][(Array->row_array[1]+3)%4]];
    temp[Array->row_array[2]] = xtimes(state[position[col][Array->row_array[2]]]) 
                                ^ (xtimes(state[position[col][(Array->row_array[2]+1)%4]]) ^ state[position[col][(Array->row_array[2]+1)%4]]) 
                                ^ state[position[col][(Array->row_array[2]+2)%4]] 
                                ^ state[position[col][(Array->row_array[2]+3)%4]];
    temp[Array->row_array[3]] = xtimes(state[position[col][Array->row_array[3]]]) 
                                ^ (xtimes(state[position[col][(Array->row_array[3]+1)%4]]) ^ state[position[col][(Array->row_array[3]+1)%4]]) 
                                ^ state[position[col][(Array->row_array[3]+2)%4]] 
                                ^ state[position[col][(Array->row_array[3]+3)%4]];

    state[position[col][Array->row_array[0]]] = temp[Array->row_array[0]];
    state[position[col][Array->row_array[1]]] = temp[Array->row_array[1]];
    state[position[col][Array->row_array[2]]] = temp[Array->row_array[2]];
    state[position[col][Array->row_array[3]]] = temp[Array->row_array[3]];
}

void AddRoundKey2(MKAES *AES, byte *state, int col, Array_info *Array, int round){
    state[Array->row_array[0] + 4*col] ^= AES->rndkey[4*round + col][Array->row_array[0]];
    state[Array->row_array[1] + 4*col] ^= AES->rndkey[4*round + col][Array->row_array[1]];
    state[Array->row_array[2] + 4*col] ^= AES->rndkey[4*round + col][Array->row_array[2]];
    state[Array->row_array[3] + 4*col] ^= AES->rndkey[4*round + col][Array->row_array[3]];
}

void KeyAppliedInvShift(MaskingValue *MV, MKAES *AES){
    byte temp;
    /* Correct Mask to M_hat before use fo Key */
    AES->rndkey[40][0] ^= MV->M1_hat ^ MV->M;
    AES->rndkey[40][1] ^= MV->M2_hat ^ MV->M;
    AES->rndkey[40][2] ^= MV->M3_hat ^ MV->M;
    AES->rndkey[40][3] ^= MV->M4_hat ^ MV->M;

    /* Correct Mask to M_hat before use fo Key */
    AES->rndkey[42][0] ^= MV->M;
    AES->rndkey[42][1] ^= MV->M;
    AES->rndkey[42][2] ^= MV->M;
    AES->rndkey[42][3] ^= MV->M;

    /* Generate final key applied inverse ShiftRows */
    temp = AES->rndkey[43][1];
    AES->rndkey[43][1] = AES->rndkey[42][1];
    AES->rndkey[42][1] = AES->rndkey[41][1];
    AES->rndkey[41][1] = AES->rndkey[40][1];
    AES->rndkey[40][1] = temp;

    temp = AES->rndkey[40][2];
    AES->rndkey[40][2] = AES->rndkey[42][2];
    AES->rndkey[42][2] = temp;
    temp = AES->rndkey[41][2];
    AES->rndkey[41][2] = AES->rndkey[43][2];
    AES->rndkey[43][2] = temp;

    temp = AES->rndkey[40][3];
    AES->rndkey[40][3] = AES->rndkey[41][3];
    AES->rndkey[41][3] = AES->rndkey[42][3];
    AES->rndkey[42][3] = AES->rndkey[43][3];
    AES->rndkey[43][3] = temp;
}

void ShiftRows(byte *state){
    byte temp;
    /* row = 1 */
    temp = *(state+1);
    *(state+1) = *(state+5);
    *(state+5) = *(state+9);
    *(state+9) = *(state+13);
    *(state+13) = temp;

    /* row = 2 */
    temp = *(state+2);
    *(state+2) = *(state+10);
    *(state+10) = temp;
    temp = *(state+6);
    *(state+6) = *(state+14);
    *(state+14) = temp;

    /* row = 3 */
    temp = *(state+15);
    *(state+15) = *(state+11);
    *(state+11) = *(state+7);
    *(state+7) = *(state+3);
    *(state+3) = temp;
}

void M_hat2Mi(MaskingValue *MV, byte *state, int col){
    byte position[4][4] = {
        {0x00, 0x05, 0x0A, 0x0F},
        {0x04, 0x09, 0x0E, 0x03},
        {0x08, 0x0D, 0x02, 0x07},
        {0x0C, 0x01, 0x06, 0x0B}
    };

    state[position[col][0]] ^= MV->M_hat ^ MV->M1;
    state[position[col][1]] ^= MV->M_hat ^ MV->M2;
    state[position[col][2]] ^= MV->M_hat ^ MV->M3;
    state[position[col][3]] ^= MV->M_hat ^ MV->M4;
}

void Randomized_Zone1(Array_info *Array, byte *state, MKAES *AES, MaskingValue *MV){
    int col;
    for(col=0;col<4 + dummy_number/4 ;col++){
        /* dummy State operation */
        if(Array->col_array[col] > 3){
            AddRoundKey(AES, AES->dummy_State, Array->col_array[col]%4, Array, 0);
            SubBytes(MV, AES->dummy_State, Array, Array->col_array[col]%4);
            M_hat2Mi(MV, AES->dummy_State,Array->col_array[col]%4);
            MixColumns(AES->dummy_State, Array, Array->col_array[col]%4);
        }
        /* State operation */
        else{
            AddRoundKey(AES, state, Array->col_array[col], Array, 0);
            SubBytes(MV, state, Array, Array->col_array[col]);
            M_hat2Mi(MV, state,Array->col_array[col]);
            MixColumns(state, Array, Array->col_array[col]);
        }
    }

}

void Round(Array_info *Array, byte *state, MKAES *AES, MaskingValue *MV, int round){
    int col;
    for(col=0; col<4; col++){
            AddRoundKey(AES, state, Array->col_array[col], Array, round);
            SubBytes(MV, state, Array, Array->col_array[col]);
            if(round != 8){
                M_hat2Mi(MV, state,Array->col_array[col]);
                MixColumns(state, Array, Array->col_array[col]);
            }
                
    }
    ShiftRows(state);
    if( round == 8){
        for(col=0; col<4; col++){
            M_hat2Mi(MV, state,Array->col_array[col]);
        }
    }

}

void Randomized_Zone2(Array_info *Array, byte *state, MKAES *AES, MaskingValue *MV){
    int col;
    KeyAppliedInvShift(MV, AES);
    for(col=0;col<4 + dummy_number/4 ;col++){
        /* dummy State operation */
        if(Array->col_array[col] > 3){
            MixColumns2(AES->dummy_State, Array, Array->col_array[col]%4);
            AddRoundKey2(AES, AES->dummy_State, Array->col_array[col]%4, Array, 9);
            SubBytes2(MV, AES->dummy_State, Array, Array->col_array[col]%4);
            AddRoundKey2(AES, state, Array->col_array[col], Array, 10);
        }
        /* State operation */
        else{
            MixColumns2(state, Array, Array->col_array[col]);
            AddRoundKey2(AES, state, Array->col_array[col], Array, 9);
            SubBytes2(MV, state, Array, Array->col_array[col]);
            AddRoundKey2(AES, state, Array->col_array[col], Array, 10);
        }
    }
}
