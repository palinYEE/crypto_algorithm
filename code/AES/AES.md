# AES

1977년 미국의 연방 정보처리 표준 규격(FIPS)으로 블록 암호 DES(Data Encryption Standard)가 채택되었다. DES는 56비트의 짧은 키 길이를 가졌고, 컴퓨터 성능이 증가함에 따라 현실적인 공격이 가능해졌다.    
[DES 암호 해독 가능 시간]
* DES Challenge 1 : 96일
* DES Challenge 2 : 41일
* DES Challenge 3 : 22시간 15분 

1997년 NIST는 경쟁방식에 의한 표준화를 통해 DES를 대체할 새로운 암호 알고리즘을 공모했는데, 최종적으로 선택된 알고리즘이 바로 AES(Advanced Encryption Standard)이다. 
AES가 선택되어진 큰 이유는 고속 구현이 가능하고 다양한 플랫폼에서 적용이 가능하며, 다른 후보 알고리즘보다 안전성이 우수하기 때문이다. 

## 알고리즘 구현 전 AES의 규격과 특징

AES 알고리즘은 다음과 같은 규격을 가지고 있다. 
<center>

|          | 마스터 키 길이 | 라운드 키 길이  | 입력 비트 길이 | 라운드 수    | 
|:--------:|:----------:|:------------:|:-----------:|:---------:|
| AES-128  |   128 비트  |   128 비트 | 128 비트 | 10 |
| AES-192  | 192 비트 | 128 비트 | 128 비트 | 12 |
| AES-256  | 256 비트 | 128 비트 | 128비트 | 14 |

</center>

그리고 AES를 구현하는데 가장 중요한 것이 바로 `state` 와 `유한체 연산` 이다. 

* `state`   
    `state`는 암호화와 복호화 과정에서 사용되는 2차원 행렬이다. 16 바이트 입력 값이   
    ![byte](/images/AES/16byte.png)
    라고 하면 이를 `state`로 변환할 때 다음과 같다.    
    ![state](/images/AES/state.png)

* `유한체 연산`   
    AES 알고리즘은 $\{0,1\}^8$ 에서의 연산을 기본으로 하고 있으며, 이는 기약다항식이 `0x11b`인 `GF(2^8)` 유한체 안에서의 연산으로 간주한다. 유한체에서의 연산 중 덧셈과 곱셈을 사용한다. 

------------

## 암호화

AES 알고리즘의 암호화 과정은 다음과 같다. 
```
Require: 128-bit Plaintext, 128-bit or 192-bit or 256 bit key
Ensure : 128-bit Ciphertext
==============================================================
1. {rk_0, rk_1, ..., rk_{Nr}} <-- KeySchedule(key)
2. state <-- Plaintext
3. state <-- addroundkey(state, rk_0)
4. for round = 1 to Nr - 1 do :
5.      state <-- subbytes(state)
6.      state <-- shiftrows(state)
7.      state <-- mixcolumns(state)
8.      state <-- addroundkey(state, rk_{round})
9. endfor
10.      state <-- subbytes(state)
11.      state <-- shiftrows(state)
12.      state <-- addroundkey(state, rk_{Nr})
13. Ciphertext <-- state
14. return Ciphertext
```

이제 각각의 함수에 대해서 한번 살펴보자. 

### subbytes()

`subbytes()`는 간단하게 `sbox` 라는 비선형 테이블을 통해서 값을 치환하는 함수이다.
<center>

![sbox](/images/AES/sbox.png)
</center>

간단한 예시로 `0x1b`를 치환해보자. 그러면 다음과 같다. 
<center>
subbytes(0x1b) = 0xaf
</center>

그러면 다음과 같은 궁금을 가질 수 있다. 

Q. sbox는 어떻게 생성되는 건가요??   
A. 이에 대해서는 `GF(2^8)` 에서의 역원을 구한 다름에 규격서에 있는 아핀 변환 수식을 통해서 변환한 값들을 테이블로 만들면 sbox가 된다. 자세한 과정과 예시를 원한다면 메일을 보내주시면 됩니다.    
<center>

<alwns28@naver.com>

</center>

### shiftrows()
`shiftrows()` 는 행을 오른쪽으로 옮기는 함수이다. 첫번째 행은 0번, 두번째 행은 1번, 세번째 행은 2번, 4번째 행은 3번 오른쪽으로 로테이션 한다. 

<center>

![shiftrows](/images/AES/shiftrows.png)

</center>

### mixcolumns()

`mixcolumns()` 는 다음과 같은 행렬 연산 하는 함수이다. (아래 그림은 `state`의 한개의 열 기준)
<center>

![mixcolumns](/images/AES/mixcolumns.png)

</center>

위 수식에서 01, 03, 02, 01 에 대한 곱셈 연산을 해야 한다. 이때 우리는 구현 관점에서 다음과 같은 생각을 할 수 있다.

Q. 연산 함수를 구현할때 각각의 함수를 다 만들어야 하나?   
A. 그렇지 않다. 우리는 `xtimes()` 라는 함수 하나를 정의함으로써 위 연산을 모두 수행할 것이다.  

![xtimes](/images/AES/xtimes.png)

물론 `xtimes`를 사용하지 않고 `mixcolumns`를 사용할 수 있다. 하지만 이를 사용한 것과 사용하지 않은 것에 대한 연산량은 나름 차이가 있다. 

<center>

|`mixcolumns` 구현 방식|`GF(2^8)` 곱| `xtimes` | Xor|
|:------------------:|:----------:|:--------:|:----:|
|행렬 곱|16|0|12|
|`xtimes` 이용|0|4|15|  
</center>

### addroundkey()

`addroundkey()`는 키 스케줄 과정을 통해 만들어진 라운드 키를 비트별로 더하는 연산이다. 

<center>

![addroundkey](/images/AES/addroundkey.png)
</center>

이러한 함수들을 거치고 마지막에 `state`를 16 바이트로 변환하여 Ciphertext로 반환하게 된다.

![ciphertext](/images/AES/ciphertext.png)

-------
## 복호화

AES 알고리즘의 복호화 과정은 다음과 같다. 
```
Require: 128-bit Ciphertext, 128-bit or 192-bit or 256 bit key
Ensure : 128-bit Plaintext
==============================================================
1. {rk_0, rk_1, ..., rk_{Nr}} <-- KeySchedule(key)
2. state <-- Ciphertext
3. state <-- addroundkey(state, rk_0)
4. for round = Nr - 1 to 1 do :
5.      state <-- inverse_shiftrows(state)
6.      state <-- inverse_subbytes(state)
7.      state <-- addroundkey(state, rk_{round})
8.      state <-- inverse_mixcolumns(state)
9. endfor
10.      state <-- inverse_shiftrows(state)
11.      state <-- inverse_subbytes(state)
12.      state <-- addroundkey(state, rk_{Nr})
13. Plaintext <-- state
14. return Plaintext
```

복호화 알고리즘은 암호화 알고리즘의 반대이다. 그래서 우리는 `inverse_subbytes` 와 `inverse_mixcolumns`를 살펴보자. 

### inverse_subbytes()

`inverse_subbytes` 는 `subbytes`와 비슷하게 inverse_sbox 를 통해서 값을 치환하는 함수이다. 

![inverse_sbox](/images/AES/inverse_sbox.png)


### inverse_mixcolumns()

`inverse_mixcolumns`는 `mixcolumns`에서 사용한 행렬의 역행렬을 사용한다. 

![inverse_mixcolumns](/images/AES/inverse_mixcolumns.png)

여기서 다음과 같은 생각을 할 수 있다.    
Q. `inverse_mixcolumns`에서도 `xtimes`를 사용할 수 있을까?   
A. 사용 가능하다. 예를 들어 다음과 같이 사용할 수 있다. 
![inverse_mixcolumns_xtimes](/images/AES/inverse_mix_xtimes.png)

--------

## 키 스케줄

키 스케줄은 입력되는 키의 길이에 따라서 다음과 같이 출력된다.   
* 입력 키 길이 : 128 비트 --> 마스터 키를 포함한 44개의 32비트 값
* 입력 키 길이 : 192 비트 --> 마스터 키를 포함한 52개의 32비트 값
* 입력 키 길이 : 256 비트 --> 마스터 키를 포함한 60개의 32비트 값

키 스케줄 과정을 그림으로 표현하면 다음과 같다. (AES-128 기준)

![keySchedule](/images/AES/keyschedule.PNG)

AES-192의 키스케줄 과정은 32비트 6개 단위로 위 그림 과정을 진행한다. AES-256은 32비트 8개 단위로 위 그림 과정을 진행하는데 중간에 rk_i 에서 i mod 8 != 0 이고 i mod 4 = 0 이면 SubWord 함수를 진행한다. 

---------
# Reference

* Joan Daemen and Vincent Rijmen.AES Proposal: Rijndael. 1999