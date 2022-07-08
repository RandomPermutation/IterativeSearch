#pragma once
#include "sboxInstances.h"
#include "diffusionInstances.h"
#include <string>
using std::string;

#define PRESENT 0
#define GIFT_64 1
#define RECTANGLE 2
#define KNOT_256 3
#define PUFFIN 4
#define TRIFLE 5
#define ICEBERG 6
#define GIFT_128 7
#define EPCBC_48 8
#define EPCBC_96 9
#define KNOT_384 10
#define KNOT_512 11
#define TANGRAM_128 12
#define TANGRAM_256 13

#define DDT 0
#define LAT2 1
#define LAT 2

#define IDENTICAL 0
#define ROTATIONAL 1

#define CIPHER PRESENT
#define TABLE DDT

#if CIPHER == GIFT_64
#define SBOX_SIZE 4
#define SBOX_NUM 16
#define SYMMETRY IDENTICAL
#define SBOX s_gift
#define DIFFUSION diffusion_gift_64
#define DIFFUSION_INV diffusion_inverse_gift_64
const string cipher_name = "gift64";

#elif CIPHER == PRESENT
#define SBOX_SIZE 4
#define SBOX_NUM 16
#define SYMMETRY IDENTICAL
#define SBOX s_present
#define DIFFUSION diffusion_present
#define DIFFUSION_INV diffusion_inverse_present
const string cipher_name = "present";

#elif CIPHER == RECTANGLE
#define SBOX_SIZE 4
#define SBOX_NUM 16
#define SYMMETRY ROTATIONAL
#define SBOX s_rectangle
#define DIFFUSION diffusion_rectangle
#define DIFFUSION_INV diffusion_inverse_rectangle
const string cipher_name = "rectangle";

#elif CIPHER == KNOT_256
#define SBOX_SIZE 4
#define SBOX_NUM 64
#define SYMMETRY ROTATIONAL
#define SBOX s_knot
#define DIFFUSION diffusion_knot_256
#define DIFFUSION_INV diffusion_inverse_knot_256
const string cipher_name = "knot256";
#elif CIPHER == KNOT_384
#define SBOX_SIZE 4
#define SBOX_NUM 96
#define SYMMETRY ROTATIONAL
#define SBOX s_knot
#define DIFFUSION diffusion_knot_384
#define DIFFUSION_INV diffusion_inverse_knot_384
const string cipher_name = "knot384";
#elif CIPHER == KNOT_512
#define SBOX_SIZE 4
#define SBOX_NUM 128
#define SYMMETRY ROTATIONAL
#define SBOX s_knot
#define DIFFUSION diffusion_knot_512
#define DIFFUSION_INV diffusion_inverse_knot_512
const string cipher_name = "knot512";
#elif CIPHER == TANGRAM_128
#define SBOX_SIZE 4
#define SBOX_NUM 32
#define SYMMETRY ROTATIONAL
#define SBOX s_tangram
#define DIFFUSION diffusion_tangram_128
#define DIFFUSION_INV diffusion_inverse_tangram_128
const string cipher_name = "tangram128";
#elif CIPHER == TANGRAM_256
#define SBOX_SIZE 4
#define SBOX_NUM 64
#define SYMMETRY ROTATIONAL
#define SBOX s_tangram
#define DIFFUSION diffusion_tangram_256
#define DIFFUSION_INV diffusion_inverse_tangram_256
const string cipher_name = "tangram256";

#elif CIPHER == PUFFIN
#define SBOX_SIZE 4
#define SBOX_NUM 16
#define SYMMETRY IDENTICAL
#define SBOX s_puffin
#define DIFFUSION diffusion_puffin
#define DIFFUSION_INV diffusion_inverse_puffin
const string cipher_name = "puffin";

#elif CIPHER == TRIFLE
#define SBOX_SIZE 4
#define SBOX_NUM 32
#define SYMMETRY IDENTICAL
#define SBOX s_trifle
#define DIFFUSION diffusion_trifle
#define DIFFUSION_INV diffusion_inverse_trifle
const string cipher_name = "trifle";


#elif CIPHER == ICEBERG
#define SBOX_SIZE 8
#define SBOX_NUM 8
#define SYMMETRY IDENTICAL
#define SBOX s_iceberg
#define DIFFUSION diffusion_iceberg
#define DIFFUSION_INV diffusion_inverse_iceberg
const string cipher_name = "iceberg";

#elif CIPHER == GIFT_128
#define SBOX_SIZE 4
#define SBOX_NUM 32
#define SYMMETRY IDENTICAL
#define SBOX s_gift
#define DIFFUSION diffusion_gift_128
#define DIFFUSION_INV diffusion_inverse_gift_128
const string cipher_name = "gift128";

#elif CIPHER == EPCBC_48
#define SBOX_SIZE 4
#define SBOX_NUM 12
#define SYMMETRY IDENTICAL
#define SBOX s_present
#define DIFFUSION diffusion_epcbc_48
#define DIFFUSION_INV diffusion_inverse_epcbc_48
const string cipher_name = "epcbc48";

#elif CIPHER == EPCBC_96
#define SBOX_SIZE 4
#define SBOX_NUM 24
#define SYMMETRY IDENTICAL
#define SBOX s_present
#define DIFFUSION diffusion_epcbc_96
#define DIFFUSION_INV diffusion_inverse_epcbc_96
const string cipher_name = "epcbc96";

#elif CIPHER == ASCON
#define SBOX_SIZE 5
#define SBOX_NUM 64
#define SYMMETRY ROTATIONAL
#define SBOX s_ascon
#define DIFFUSION diffusion_ascon
#define DIFFUSION_INV diffusion_inverse_ascon
const string cipher_name = "ascon";

#endif

#if TABLE==DDT
const string table_type = "ddt";
#elif TABLE==LAT2
const string table_type = "lat2";
#elif TABLE==LAT
const string table_type = "lat";
#endif

constexpr size_t WORD128_SIZE = ((SBOX_NUM - 1) >> 4) + 1;
constexpr size_t SBOX_CARD = 1 << SBOX_SIZE;
using Weight = float;
using Cost = double;