/*
 * @file    masking.h
 * @brief   function set about masking value using Masking AES
 * @author  Yoon Yeong jin
 * @date    2020.4.26.
 */

#ifndef MASKING_H
#define MASKING_H

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<memory.h>

typedef unsigned char byte;

/* Masking Value Storage */
typedef struct 
{
    byte M;
    byte M_hat;
    byte M1;
    byte M2;
    byte M3;
    byte M4;
    byte M1_hat;
    byte M2_hat;
    byte M3_hat;
    byte M4_hat;

    byte Masking_S_BOX[256];

}MaskingValue;

/**
 * @brief Generate Masking Value M, M_hat, M1, M2, M3, M4 etc.
 * @param MaskingValue	(input) struct MaskingValue
 * @param MaskingValue  (output) Masking Value M, M_hat, M1, M2, M3, M4 etc.
 */
void GenMaskingValue(MaskingValue *MV);

/**
 * @brief Generate all zero Masking Value M, M_hat, M1, M2, M3, M4 etc.
 * @param MaskingValue	(input) struct MaskingValue
 * @param MaskingValue  (output) Masking Value M, M_hat, M1, M2, M3, M4 etc.
 */
void IdentityMaskingValue(MaskingValue *MV);

/**
 * @brief Show all masking value
 * @param MaskingValue	(input) struct MaskingValue
 */
void Show_all_MaskingValue(MaskingValue *MV);

/**
 * @brief Show MaskingSbox
 * @param MaskingValue	(input) Masking_S_BOX
 */
void Show_MaskingSbox(MaskingValue *MV);

/**
 * @brief test MaskingSbox
 * @param MaskingValue	(input) Masking_S_BOX
 */
void TestMaskingSbox(MaskingValue *MV);

/**
 * @brief Generate Masking S-BOX
 * @param MaskingValue	(input) Masking value M and M', S box
 * @param Masking_S_BOX	(output) Masking S box such that MaskingSBOX(x^M) = SBOX(X)^M')
 */
void GenMaskingSbox(MaskingValue *MV);

#endif