#pragma once
#include <stdint.h>

void diffusion_present(uint8_t* in, uint8_t* out);
void diffusion_gift_64(uint8_t* in, uint8_t* out);
void diffusion_gift_128(uint8_t* in, uint8_t* out);
void diffusion_puffin(uint8_t* in, uint8_t* out);
void diffusion_epcbc_48(uint8_t* in, uint8_t* out);
void diffusion_epcbc_96(uint8_t* in, uint8_t* out);
void diffusion_iceberg(uint8_t* in, uint8_t* out);
void diffusion_trifle(uint8_t* in, uint8_t* out);
void diffusion_rectangle(uint8_t* in, uint8_t* out);
void diffusion_knot_256(uint8_t* in, uint8_t* out);
void diffusion_knot_384(uint8_t* in, uint8_t* out);
void diffusion_knot_512(uint8_t* in, uint8_t* out);
void diffusion_tangram_128(uint8_t* in, uint8_t* out);
void diffusion_tangram_256(uint8_t* in, uint8_t* out);

void diffusion_inverse_present(uint8_t* in, uint8_t* out);
void diffusion_inverse_gift_64(uint8_t* in, uint8_t* out);
void diffusion_inverse_gift_128(uint8_t* in, uint8_t* out);
void diffusion_inverse_puffin(uint8_t* in, uint8_t* out);
void diffusion_inverse_epcbc_48(uint8_t* in, uint8_t* out);
void diffusion_inverse_epcbc_96(uint8_t* in, uint8_t* out);
void diffusion_inverse_iceberg(uint8_t* in, uint8_t* out);
void diffusion_inverse_trifle(uint8_t* in, uint8_t* out);
void diffusion_inverse_rectangle(uint8_t* in, uint8_t* out);
void diffusion_inverse_knot_256(uint8_t* in, uint8_t* out);
void diffusion_inverse_knot_384(uint8_t* in, uint8_t* out);
void diffusion_inverse_knot_512(uint8_t* in, uint8_t* out);
void diffusion_inverse_tangram_128(uint8_t* in, uint8_t* out);
void diffusion_inverse_tangram_256(uint8_t* in, uint8_t* out);