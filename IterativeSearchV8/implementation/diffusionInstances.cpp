#include "../include/diffusionInstances.h"

#include <string.h>

#define bitpermutation(sbn,sn,bptable)	\
uint8_t mask = 1 << (sbn - 1);			\
memset(out,0,sn);						\
for (int i = 0; i < sn; i++) {			\
for (int j = 0; j < sbn; j++) {			\
	int out_index = bptable[j + sbn * i];									\
	out[out_index / sbn] ^= ((in[i] << j) & mask) >> (out_index % sbn);	\
}}
#define bitpermutation_inverse(sbn,sn,bptable)	\
int bptable_inverse[sbn*sn];					\
for(int i=0;i<sbn*sn;i++) bptable_inverse[bptable[i]]=i;	\
uint8_t mask = 1 << (sbn - 1);					\
memset(out,0,sn);								\
for (int i = 0; i < sn; i++) {					\
for (int j = 0; j < sbn; j++) {					\
	int out_index = bptable_inverse[j + sbn * i];						\
	out[out_index / sbn] ^= ((in[i] << j) & mask) >> (out_index % sbn);	\
}}
#define bitpermutation_rowlshift(sbn,sn,stable)	\
uint8_t mask = 1;								\
memset(out,0,sn);								\
for (int i = 0; i < sn; i++) {					\
for (int j = 0; j < sbn; j++) {					\
	out[(sn + i - stable[j]) % sn] ^= in[i] & (mask << j);	\
}}
#define bitpermutation_rowrshift(sbn,sn,stable)	\
uint8_t mask = 1;								\
memset(out,0,sn);								\
for (int i = 0; i < sn; i++) {					\
for (int j = 0; j < sbn; j++) {					\
	out[(sn + i + stable[j]) % sn] ^= in[i] & (mask << j);	\
}}
#define bitxor_rowrshift(sbn,sn,stable,shiftn)	\
uint8_t mask = 1 << (sbn - 1);								\
memset(out,0,sn);								\
int c=0;\
for (int j = 0; j < sbn; j++) {					\
	for (int i = 0; i < sn; i++) {					\
		for (int k=0;k<shiftn[j];k++){		\
			out[(sn + i + stable[c+k]) % sn] ^= in[i] & (mask >> j);	\
		}\
	}\
	c+=shiftn[j];\
}


int bptable_present[64]= 
{ 0,16,32,48,1,17,33,49,2,18,34,50,3,19,35,51,
4,20,36,52,5,21,37,53,6,22,38,54,7,23,39,55,
8,24,40,56,9,25,41,57,10,26,42,58,11,27,43,59,
12,28,44,60,13,29,45,61,14,30,46,62,15,31,47,63 };
void diffusion_present(uint8_t * in, uint8_t * out) { bitpermutation(4, 16, bptable_present) }
void diffusion_inverse_present(uint8_t* in, uint8_t* out) { bitpermutation_inverse(4, 16, bptable_present) }

int bptable_gift_64[64] =
/*{ 0,17,34,51,48,1,18,35,32,49,2,19,16,33,50,3,
4,21,38,55,52,5,22,39,36,53,6,23,20,37,54,7,
8,25,42,59,56,9,26,43,40,57,10,27,24,41,58,11,
12,29,46,63,60,13,30,47,44,61,14,31,28,45,62,15 };*/
{48, 1, 18, 35, 32, 49, 2, 19, 16, 33, 50, 3, 0, 17, 34, 51,
52, 5, 22, 39, 36, 53, 6, 23, 20, 37, 54, 7, 4, 21, 38, 55,
56, 9, 26, 43, 40, 57, 10, 27, 24, 41, 58, 11, 8, 25, 42, 59,
60, 13, 30, 47, 44, 61, 14, 31, 28, 45, 62, 15, 12, 29, 46, 63};
void diffusion_gift_64(uint8_t* in, uint8_t* out) { bitpermutation(4, 16, bptable_gift_64) }
void diffusion_inverse_gift_64(uint8_t* in, uint8_t* out) { bitpermutation_inverse(4, 16, bptable_gift_64) }


int bptable_gift_128[128] =
/*{ 0,33,66,99,96,1,34,67,64,97,2,35,32,65,98,3,
4,37,70,103,100,5,38,71,68,101,6,39,36,69,102,7,
8,41,74,107,104,9,42,75,72,105,10,43,40,73,106,11,
12,45,78,111,108,13,46,79,76,109,14,47,44,77,110,15,
16,49,82,115,112,17,50,83,80,113,18,51,48,81,114,19,
20,53,86,119,116,21,54,87,84,117,22,55,52,85,118,23,
24,57,90,123,120,25,58,91,88,121,26,59,56,89,122,27,
28,61,94,127,124,29,62,95,92,125,30,63,60,93,126,31 };*/
{96, 1, 34, 67, 64, 97, 2, 35, 32, 65, 98, 3, 0, 33, 66, 99, 
100, 5, 38, 71, 68, 101, 6, 39, 36, 69, 102, 7, 4, 37, 70, 103,
104, 9, 42, 75, 72, 105, 10, 43, 40, 73, 106, 11, 8, 41, 74, 107,
108, 13, 46, 79, 76, 109, 14, 47, 44, 77, 110, 15, 12, 45, 78, 111,
112, 17, 50, 83, 80, 113, 18, 51, 48, 81, 114, 19, 16, 49, 82, 115,
116, 21, 54, 87, 84, 117, 22, 55, 52, 85, 118, 23, 20, 53, 86, 119,
120, 25, 58, 91, 88, 121, 26, 59, 56, 89, 122, 27, 24, 57, 90, 123,
124, 29, 62, 95, 92, 125, 30, 63, 60, 93, 126, 31, 28, 61, 94, 127
};

void diffusion_gift_128(uint8_t* in, uint8_t* out) { bitpermutation(4, 32, bptable_gift_128) }
void diffusion_inverse_gift_128(uint8_t* in, uint8_t* out) { bitpermutation_inverse(4, 32, bptable_gift_128) }

int bptable_puffin[64] = 
{ 12, 1, 59, 49, 50, 26, 9, 35,
24, 6, 31, 60, 0, 48, 46, 18,
33, 52, 15, 21, 56, 19, 47, 40,
8, 51, 5, 30, 61, 29, 27, 10,
36, 16, 57, 7, 32, 43, 45, 58,
23, 54, 62, 37, 55, 38, 14, 22,
13, 3, 4, 25, 17, 53, 41, 44,
20, 34, 39, 2, 11, 28, 42, 63 };
void diffusion_puffin(uint8_t* in, uint8_t* out) { bitpermutation(4, 16, bptable_puffin) }
void diffusion_inverse_puffin(uint8_t* in, uint8_t* out) { bitpermutation_inverse(4, 16, bptable_puffin) }

int bptable_iceberg[64] =
{ 0,12,23,25,38,42,53,59,22,9,26,32,1,47,51,61,
24,37,18,41,55,58,8,2,16,3,10,27,33,46,48,62,
11,28,60,49,36,17,4,43,50,19,5,39,56,45,29,13,
30,35,40,14,57,6,54,20,44,52,21,7,34,15,31,63 };
void diffusion_iceberg(uint8_t* in, uint8_t* out) { bitpermutation(8, 8, bptable_iceberg) }
void diffusion_inverse_iceberg(uint8_t* in, uint8_t* out) { bitpermutation_inverse(8, 8, bptable_iceberg) }

int bptable_trifle[128] =
{ 0,32,64,96,1,33,65,97,2,34,66,98,3,35,67,99,
4,36,68,100,5,37,69,101,6,38,70,102,7,39,71,103,
8,40,72,104,9,41,73,105,10,42,74,106,11,43,75,107,
12,44,76,108,13,45,77,109,14,46,78,110,15,47,79,111,
16,48,80,112,17,49,81,113,18,50,82,114,19,51,83,115,
20,52,84,116,21,53,85,117,22,54,86,118,23,55,87,119,
24,56,88,120,25,57,89,121,26,58,90,122,27,59,91,123,
28,60,92,124,29,61,93,125,30,62,94,126,31,63,95,127 };
void diffusion_trifle(uint8_t* in, uint8_t* out) { bitpermutation(4, 32, bptable_trifle) }
void diffusion_inverse_trifle(uint8_t* in, uint8_t* out) { bitpermutation_inverse(4, 32, bptable_trifle) }

int bptable_epcbc_48[48]=
{ 0, 12, 24, 36, 1, 13, 25, 37,
2, 14, 26, 38, 3, 15, 27, 39,
4, 16, 28, 40, 5, 17, 29, 41,
6, 18, 30, 42, 7, 19, 31, 43,
8, 20, 32, 44, 9, 21, 33, 45,
10, 22, 34, 46, 11, 23, 35, 47 };
void diffusion_epcbc_48(uint8_t* in, uint8_t* out) { bitpermutation(4, 12, bptable_epcbc_48) }
void diffusion_inverse_epcbc_48(uint8_t* in, uint8_t* out) { bitpermutation_inverse(4, 12, bptable_epcbc_48) }
int bptable_epcbc_96[96]=
{ 0, 24, 48, 72, 1, 25, 49, 73, 2, 26, 50, 74, 3, 27, 51, 75, 4, 28, 52, 76, 5, 29
, 53, 77, 6, 30, 54, 78, 7, 31, 55, 79, 8, 32, 56, 80, 9, 33, 57, 81, 10, 34, 58
, 82, 11, 35, 59, 83, 12, 36, 60, 84, 13, 37, 61, 85, 14, 38, 62, 86, 15, 39, 63
, 87, 16, 40, 64, 88, 17, 41, 65, 89, 18, 42, 66, 90, 19, 43, 67, 91, 20, 44, 68
, 92, 21, 45, 69, 93, 22, 46, 70, 94, 23, 47, 71, 95 };
void diffusion_epcbc_96(uint8_t* in, uint8_t* out) { bitpermutation(4, 24, bptable_epcbc_96) }
void diffusion_inverse_epcbc_96(uint8_t* in, uint8_t* out) { bitpermutation_inverse(4, 24, bptable_epcbc_96) }

int shift_table_knot256[] = { 0,1,8,25 };
void diffusion_knot_256(uint8_t* in, uint8_t* out) { bitpermutation_rowlshift(4, 64, shift_table_knot256); }
void diffusion_inverse_knot_256(uint8_t* in, uint8_t* out) { bitpermutation_rowrshift(4, 64, shift_table_knot256); }
int shift_table_knot384[] = { 0,1,8,55 };
void diffusion_knot_384(uint8_t* in, uint8_t* out) { bitpermutation_rowlshift(4, 96, shift_table_knot384); }
void diffusion_inverse_knot_384(uint8_t* in, uint8_t* out) { bitpermutation_rowrshift(4, 96, shift_table_knot384); }
int shift_table_knot512[] = { 0,1,16,25 };
void diffusion_knot_512(uint8_t* in, uint8_t* out) { bitpermutation_rowlshift(4, 128, shift_table_knot512); }
void diffusion_inverse_knot_512(uint8_t* in, uint8_t* out) { bitpermutation_rowrshift(4, 128, shift_table_knot512); }
int shift_table_tangram128[] = { 0,1,8,11 };
void diffusion_tangram_128(uint8_t* in, uint8_t* out) { bitpermutation_rowlshift(4, 32, shift_table_tangram128); }
void diffusion_inverse_tangram_128(uint8_t* in, uint8_t* out) { bitpermutation_rowrshift(4, 32, shift_table_tangram128); }
int shift_table_tangram256[] = { 0,1,8,41 };
void diffusion_tangram_256(uint8_t* in, uint8_t* out) { bitpermutation_rowlshift(4, 64, shift_table_tangram256); }
void diffusion_inverse_tangram_256(uint8_t* in, uint8_t* out) { bitpermutation_rowrshift(4, 64, shift_table_tangram256); }


int shift_table_rectangle[] = { 0,1,12,13 };
void diffusion_rectangle(uint8_t* in, uint8_t* out) { bitpermutation_rowlshift(4, 16, shift_table_rectangle); }
void diffusion_inverse_rectangle(uint8_t* in, uint8_t* out) { bitpermutation_rowrshift(4, 16, shift_table_rectangle); }