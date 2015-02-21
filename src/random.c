/*
 * Copyright 2015 Evan Buswell
 * Borrowed with minimal changes from
 * https://github.com/divfor/mt_rand/blob/master/mtrand.c
 *
 * Copyright 1997--2004, Makoto Matsumoto, Takuji Nishimura, and Eric Landry;
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. The names of its contributors may not be used to endorse or promote
 * products derived from this software without specific prior written
 * permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Any feedback is very welcome.
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
 * email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
 *
 * Reference: M. Matsumoto and T. Nishimura, "Mersenne Twister: A
 * 623-Dimensionally Equidistributed Uniform Pseudo-Random Number Generator",
 * ACM Transactions on Modeling and Computer Simulation, Vol. 8, No. 1,
 * January 1998, pp 3--30.
 */

#include <stdint.h>
#include <stdatomic.h>
#include <math.h>
#include <stddef.h>
#include "sonicmaths/random.h"
#include <stdlib.h>

/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A	0x9908b0df /* constant vector a */
#define UPPER_MASK	0x80000000 /* most significant w-r bits */
#define LOWER_MASK	0x7fffffff /* least significant r bits */

static uint32_t x[N] = {
	0xd42e637d,	0xaa2a216a,
	0x4208f4b6,	0xd224c506,
	0xda17956d,	0x14a487bc,
	0x2dc27db7,	0x9ba06177,
	0x2778232b,	0x206cd8c8,
	0x65831626,	0x285f3431,
	0x6afec325,	0xf2533a30,
	0x94c39fc8,	0x15d9a917,
	0xb99ce0b4,	0x037aaf89,
	0xbca24dbe,	0xd3b1cbce,
	0xac78e554,	0x8dfbc604,
	0x179d127d,	0x7f477fdb,
	0x78438806,	0x8fe806fe,
	0xfb459587,	0x0c343c5f,
	0x82a40825,	0x6127c3f5,
	0x963884c5,	0x916f0dc6,
	0xb853dd71,	0x493d4ca7,
	0xa552726b,	0xe0c81da6,
	0xc4d7654b,	0x40473941,
	0x3535c786,	0x905c7385,
	0xc154ebe4,	0x21d55318,
	0x3ea7ad41,	0x9f142c04,
	0xe80cb06d,	0xd5c8ec02,
	0x1e32882c,	0x0a5e5543,
	0xe0cbac13,	0xc60e61cf,
	0x06d2fcd8,	0xe50a6747,
	0x705d3507,	0x99c13d45,
	0xbd677ced,	0x11b173ca,
	0xb0879eaa,	0xe7bcec15,
	0x2c3092d8,	0x5bd2798f,
	0xf237569f,	0x431a9e39,
	0x393fef65,	0xdd98abb1,
	0x62a83b3d,	0x3de94324,
	0x4e796299,	0xa2f5b7e3,
	0xf5e4ab58,	0x66f2c4b7,
	0x8d1d330e,	0x536e1656,
	0xab123b3a,	0x6371e4da,
	0xe6e4567c,	0x9f170fab,
	0xf6ac32a4,	0x1a55f478,
	0x5769f634,	0x53c3d0e1,
	0x4acdf811,	0x93273a15,
	0xdb292c43,	0xf00a980d,
	0x80bc282d,	0xb60aa435,
	0x60d62ee3,	0x6244e508,
	0xb1b90610,	0x55189b61,
	0x94dbbf4c,	0xe661707c,
	0xc771e09b,	0x0ecb7d1c,
	0xb9248761,	0x7b6e0bbc,
	0x508162c8,	0xc9f37334,
	0x094e18da,	0x32a58228,
	0x01efe2ac,	0x8419c5f1,
	0xb48851e8,	0x195eee40,
	0xa4d681fd,	0xc8d7cfe3,
	0xbe1afbf5,	0x636d8165,
	0x0171e841,	0x43b5e2b0,
	0x399c7623,	0x46ea4564,
	0xa53c2139,	0xe45adfac,
	0x6ae0e636,	0x5a4b4aa9,
	0xb7f6ffc8,	0x52e42221,
	0xfe31bc6a,	0x8ca342a9,
	0x3ef92fbd,	0xe7688b68,
	0x436c2c65,	0xe0b7d981,
	0x326a9afc,	0x9e19df06,
	0x7cc3e3cd,	0xfc1516fa,
	0x6baf08de,	0x788b4566,
	0x49acb610,	0x5eb15698,
	0xa050edf3,	0x5740da05,
	0xee6a465b,	0xe47424ae,
	0xbba6f8fa,	0x807310e6,
	0x85f59d28,	0x7f96ed66,
	0xc60ca70c,	0x46bd4d75,
	0xdadba321,	0x326ccc9d,
	0xdd1125e4,	0x0bcf4236,
	0x1849f17d,	0x0f9f2101,
	0x89fccfae,	0xa614efc2,
	0xe14c4da9,	0x88b90c62,
	0x9d1ec5b2,	0xcfb8bb04,
	0x1aa66f65,	0xe478f0b2,
	0xbf3fff92,	0xdf6408a7,
	0x33dc6d05,	0x5fba0264,
	0xea8de842,	0xd00ba17c,
	0x2e1204ed,	0xab50513a,
	0x8b59eaf0,	0xa17b3554,
	0x384f4491,	0x8e87f3b2,
	0xe0c36ecd,	0x9daca457,
	0x9817fc7a,	0x4f627830,
	0x0f0e3ccb,	0x16f766e7,
	0x51d71a53,	0xc088eede,
	0xc8df5c98,	0x4b6449f3,
	0x5f6c1dcd,	0x34863fb7,
	0x45cf5bb8,	0xa3549367,
	0x0c6e3328,	0x35b0a563,
	0xdbb9457d,	0xbb7fb85a,
	0xc3a8bfc1,	0x8f1a99dd,
	0x6c9c46b8,	0x59dd2a98,
	0x0e1a26ad,	0xc8e7f9d8,
	0x2f50f3a8,	0xd4dd6159,
	0xdc3ead81,	0xc6ffe576,
	0xa6ba3997,	0x1b7958d9,
	0xbac7f8ef,	0xdb369bd8,
	0x7bb761af,	0x83b3d43f,
	0x67ad21f9,	0xf8d4eee5,
	0xb4a35abf,	0xe41520b3,
	0x51aa3b37,	0xecca35d7,
	0x13bc2dbf,	0x04a1323a,
	0x31c65629,	0xbbf2f222,
	0xefe8703a,	0xa5e29e04,
	0x904c2477,	0x8f48e1d5,
	0xb1b83569,	0xb92f099c,
	0xe71513dc,	0x4944a5f6,
	0xc8c3aeaa,	0x4d5832cf,
	0xc72efdf4,	0x20810798,
	0x1a29a472,	0xf5d041b9,
	0xcde71969,	0xef886df0,
	0x0ee5567b,	0xefca9c1d,
	0xdf8fef6c,	0x2c9bbfea,
	0xce61e3fe,	0x255b86bb,
	0xcfa7243a,	0x5e3021d1,
	0x881ffc7a,	0xbc96a54e,
	0x5a3d1f99,	0x2fc76d44,
	0x873c31ea,	0xe3e69b75,
	0xacef1939,	0xc353c351,
	0x26441e64,	0xde80d1a7,
	0xa331cc04,	0x0d231a71,
	0xd705e1f3,	0x01ee539b,
	0x8dcc5886,	0x82c4a3d6,
	0x98dd78a0,	0x1406646a,
	0x1b2a9ca3,	0xd1d2aa79,
	0xdae71b24,	0x4ef1818c,
	0xd94c7241,	0x0bcac41e,
	0xf5aa9b9d,	0x674bd967,
	0xe63ecbf1,	0xe101b717,
	0x0e74d267,	0x1eedaef2,
	0x95d9005c,	0xb3650221,
	0xa721a69e,	0x9c8ad3d0,
	0x1d92fd46,	0xdcb8b02c,
	0xe8159598,	0x5c583e39,
	0x22f413a7,	0x14e302ef,
	0xf145179b,	0x50886938,
	0xfc244381,	0x26c97c80,
	0x5bdde0d2,	0x4bceb536,
	0xdf7608cc,	0xf63f34e7,
	0x1d8bd8b0,	0x859031f2,
	0xc371b3d8,	0x56506dec,
	0x50dd6ca4,	0xf2b15c10,
	0x9bbd67db,	0x451eb0aa,
	0xbeb50332,	0x3583e94f,
	0xe740e2e4,	0x3f4c7219,
	0xe1864e7c,	0xa1a1271f,
	0x0700544a,	0xa4aa4fac,
	0x221a1780,	0x4a322d7b,
	0xe11b71ed,	0x06acf3d6,
	0x1f7ace4c,	0xcb1edc74,
	0xea612d47,	0x07438557,
	0x91fda543,	0x40e37935,
	0x8510deef,	0xaa32a25b,
	0x93d8df5e,	0xda19bfcf,
	0x03cb1897,	0x0d999bd8,
	0xfdce2ab4,	0x82a79be8,
	0x50b751be,	0x756e838c,
	0x6975f6e2,	0x151c8073,
	0xf050898c,	0xc836b135,
	0x8d976cca,	0xfa912547,
	0xb347c0ea,	0xaef6f2a8,
	0x27ff17e0,	0x71d60e54,
	0xc075cb4c,	0x9af3318a,
	0x889ec6d2,	0x08a11b50,
	0x46bb560c,	0xb6fdd200,
	0x5fac1695,	0xa39f584b,
	0xce385511,	0x2d97dd4b,
	0x369090e2,	0xf089f7a1,
	0x8a51bcbf,	0xf6c6ad14,
	0xc0b1538c,	0xee4ac85e,
	0x7d7f71ec,	0x61eb3004,
	0x168dfd75,	0xe0c3c100,
	0x58c54cb0,	0xcdd8f51c,
	0x2ee0ceaf,	0x2fb91222,
	0x2aa8dc41,	0x8040174d,
	0x8fa55d87,	0xe4d36f02,
	0x43352d56,	0xc909e0e0,
	0x2a42c88a,	0x0b611d64,
	0x9ae44dec,	0x71ee4eff,
	0xd4e579b7,	0x8834381e,
	0x26037bda,	0x19af1f00,
	0xb8e95497,	0xfbae5e73,
	0xa5db45a3,	0xac21e538,
	0x0c4b80c4,	0x069edc55,
	0xb9107b49,	0x9f5e9d46,
	0x907aab99,	0xabd35a6c,
	0x2c53d151,	0x38a3b5ce,
	0xfbf28f21,	0x8b7e1c5c,
	0xf2db0341,	0x8c38d7cd,
	0xf0eef7e1,	0xc7998dd3,
	0xee87ae3c,	0x01aa2710,
	0xe636b30b,	0xc21492f1,
	0xcb33b13a,	0xe277424e,
	0xfda45d2a,	0x71d856e9,
	0xf87f31fe,	0x05966034,
	0xe0c8e7f3,	0xdabe432a,
	0xf3b95aad,	0xa72659b5,
	0x93825c98,	0xe7484eb6,
	0xd7fe0007,	0x53173e26,
	0x8ad2b63c,	0xa4711e15,
	0x6ad27643,	0x4dc45d9e,
	0x072bd481,	0x7cd0611f,
	0xc68115ac,	0xcaf48312,
	0x4c408be4,	0xeb96a037,
	0xdab92140,	0x609ed739,
	0x179d5160,	0x1b12bb43,
	0xb30d7362,	0x7a489899,
	0xeacb64d1,	0x55c0de68,
	0x3cd5cf33,	0x6593e161,
	0x975561a8,	0x3b6cd454,
	0xb7859baf,	0xeac7209c,
	0x0e0c5e4b,	0x9dc4750c,
	0xa01731a4,	0x11890ac4,
	0x10bf826a,	0xa87f01f2,
	0x9551c3db,	0xdb9f94e7,
	0xedeba944,	0x654d9ec3,
	0x47ad88d6,	0xc9adbae9,
	0x16de4cfb,	0x2d2398b5,
	0x32d61071,	0x853f2d4c,
	0x97e1368a,	0x390ceb17,
	0xd5f8522e,	0x4de33bc5,
	0x4ae5a29a,	0xca0fd7a8,
	0xf64538f3,	0x7fb01821,
	0x28e310ce,	0x1e78160c,
	0xe3fe107b,	0x9064a6fd,
	0xe0997769,	0xcd98ed78,
	0x7161caeb,	0xfa864c05,
	0x0595bd4f,	0x4dbafe3a,
	0x35df88ae,	0x124e871d,
	0xf6b257af,	0x0d5d0e79,
	0x232a6961,	0x6d9c7ea0,
	0x6768e561,	0x588d6bf9,
	0x3f0a98ac,	0x65a27c81,
	0xf9f1eb15,	0x96170138,
	0xb3a71d69,	0x9d5cff37,
	0x4737f431,	0x6d23d137,
	0x40d99632,	0x081641c3,
	0xb952341b,	0xd36b4d85,
	0x87e7a059,	0xef7b76a7,
	0xf5317da7,	0x97aa015b,
	0xf414d12c,	0x9a59db13,
	0x518a4108,	0xdfe6e91f,
	0x7decbebf,	0x0a19c1ef,
	0x8b0e80f9,	0x51883e8c,
	0x01210b72,	0x368e9937,
	0x95355709,	0x206bda0d,
	0x458f679f,	0xf1e207b2,
	0x8bea25f1,	0x3386f3fe,
	0x28f7c60f,	0xaab0aac9,
	0x7f4a9eb5,	0xad61f766,
	0x3d4d1139,	0x25e282a3,
	0x0083b75e,	0x1823d480,
	0xef163a73,	0x6ec724d0,
	0x377dde18,	0x471858cf,
	0x8d1112f2,	0x653ef484,
	0xbac6f3e4,	0xfe1ec30e,
	0x2be91e64,	0xaa25ddfd,
	0x4b915973,	0x7535fbe9,
	0xc91995f8,	0x3be39250,
	0xce42f992,	0x8eb44f51,
	0x2e37f081,	0x88abb0a2,
	0xfe464e58,	0x5965647c,
	0x650d894c,	0x30db8c10,
	0xe6530a88,	0x7b119a2c,
	0xd888d0a5,	0xd93e99af,
	0x57411fc4,	0x269a6fa4,
	0x6e0563e6,	0x7c816429,
	0x1231ef2e,	0xd074303e,
	0x54db906d,	0xb09b796e,
	0x5ce980c4,	0x9dc7cd60,
	0x42fa6cdb,	0xda35a245,
	0x28d8e29d,	0xf1131650,
	0x8bd6fe3a,	0xc362b519,
	0xe31cfd88,	0x5d79235a,
	0xe5473a77,	0x62838a6d,
	0x3174ab61,	0x79b1ad2a,
	0x0bab5d39,	0x2ca3d83c,
	0xda812680,	0xe95b28e1,
	0xac64a726,	0xb0c3deb4,
	0x8f60c1c8,	0xe6ce5830,
	0x86b45cf8,	0xf6952511,
	0x20c4d0e6,	0xc995cc8a,
	0xc06965d4,	0xce693adf,
	0xd895c5bb,	0x00937c0f,
	0x1efb5b18,	0x5f53a227,
	0x9d45fe4e,	0xb7d32492,
	0xa349d885,	0x4c03459b,
	0xc538c3ec,	0x353057b6,
	0x47c6d870,	0x811ad81c,
	0x9ae0ef81,	0xac53b4e1,
	0x71ae0315,	0xa6746848,
	0x4b4561eb,	0xa3d24744,
	0x6b700278,	0xb8c06fcc,
	0x24049c8c,	0x45b1fbcc,
	0xcfb277cd,	0x90f13be4,
	0x30e24011,	0x2909bb04,
	0xd50d7ef2,	0xeb7af88a,
	0xa50f8b93,	0xc9f81a7f,
	0xc397c0f8,	0xc955e285,
	0x0ce059dd,	0xb7ed3abb,
	0x1412c637,	0x7e9103ef,
	0xb3f447ef,	0x249a4533,
	0xe500cb36,	0x372a0f05,
	0xa6d21e3d,	0x32aecd5c,
	0x64036438,	0x3f312269,
	0xf7836f42,	0xf468f647,
	0x425ab772,	0xd1c6bb32,
	0x308e90fb,	0xb4895c4a,
	0x070d3196,	0x37062a7e
};/* the array for the state vector */

static atomic_int n;

float *fixed_gaussian = NULL;
float *fixed_uniform = NULL;

void smrand_seed(uint32_t s) {
	int i;
	atomic_store_explicit(&n, 0, memory_order_release);
	x[0] = s;
	for (i = 1; i < N; i++) {
		x[i] = (0x6c078965 * (x[i - 1] ^ (x[i - 1] >> 30)) + i);
	}
}

int smrand_init_fixed_gaussian(size_t len) {
	float *new_fixed_gaussian = malloc(sizeof(float) * len);
	if (new_fixed_gaussian == NULL) {
		return -1;
	}
	if (fixed_gaussian != NULL) {
		free(fixed_gaussian);
	}
	fixed_gaussian = new_fixed_gaussian;
	while (len--)
	{
		fixed_gaussian[len] = smrand_gaussian();
	}
	return 0;
}

int smrand_init_fixed_uniform(size_t len) {
	float *new_fixed_uniform = malloc(sizeof(float) * len);
	if (new_fixed_uniform == NULL) {
		return -1;
	}
	if (fixed_uniform != NULL) {
		free(fixed_uniform);
	}
	fixed_uniform = new_fixed_uniform;
	while (len--)
	{
		fixed_uniform[len] = smrand_uniform();
	}
	return 0;
}

float smrand_fixed_uniform(size_t i) {
	return fixed_uniform[i];
}

float smrand_fixed_gaussian(size_t i) {
	return fixed_gaussian[i];
}

/* generates a random number on the interval [0,0xffffffff] */
static inline uint32_t smrand_do() {
	register uint32_t y, i, j;
	i = atomic_fetch_add_explicit(&n, 1, memory_order_acq_rel);
	if (i >= N) {
		int dummy = i + 1;
		i -= N;
		atomic_compare_exchange_strong_explicit(&n, &dummy, i + 1,
							memory_order_acq_rel,
							memory_order_relaxed);
	}
	j = (i + 1) % N;
	/* Twisted feedback */
	y = x[i] = x[i < N - M ? i + M : i + M - N]
	           ^ (((x[i] & UPPER_MASK) | (x[j] & LOWER_MASK)) >> 1)
	           ^ ((~(x[j] & 1) + 1) & MATRIX_A);
	/* Temper */
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);
	return y;
}

uint32_t smrand() {
	return smrand_do();
}

#define SIGNMASK  0x80000000
#define EXPMASK   0x7f800000
#define NMASK     0x007fffff
#define GMASK     0x00400000
#define EXPOFFSET 126
#define FSIZE     32
#define NSIZE     23

/* generates a random number on the interval (-1,1). */
static inline float smrand_uniform_do() {
	union {
		float f;
		uint32_t i;
	} ret;
	uint32_t extra;
	uint32_t exp;
	uint32_t mask;
	ret.i = smrand_do();
	mask = ret.i & (SIGNMASK | GMASK);
	exp = 0;
	extra = (ret.i & EXPMASK) >> NSIZE;
	ret.i &= (NMASK ^ GMASK);
	for (;;) {
		if (ret.i & GMASK) {
			ret.i ^= mask | ((EXPOFFSET - exp) << NSIZE);
			return ret.f;
		}
		exp++;
		if (exp > EXPOFFSET) {
			ret.i ^= mask;
			return ret.f;
		}
		if ((exp - (FSIZE - NSIZE - 1 /* 1 for the sign */))
		    % 32 == 0) {
			extra = (uint32_t) smrand_do();
		}
		ret.i <<= 1;
		ret.i |= extra & 1;
		extra >>= 1;
	}
}

float smrand_uniform() {
	return smrand_uniform_do();
}

atomic_uint_fast32_t gaussian_extra = ATOMIC_VAR_INIT(0x7FC00000);

float smrand_gaussian() {
	float s, u1, u2, a;
       	union {
		int32_t i;
		float f;
	} b;
	b.i = atomic_exchange_explicit(&gaussian_extra, 0x7FC00000,
				       memory_order_acq_rel);
	if(b.i != 0x7FC00000) {
		return b.f;
	}
	do {
		u1 = smrand_uniform_do();
		u2 = smrand_uniform_do();
		s = u1 * u1 + u2 * u2;
	} while(s >= 1);
	s = sqrtf(-2 * logf(s) / s);
	a = s * u1;
	b.f = s * u2;
	atomic_store_explicit(&gaussian_extra, b.i,
			      memory_order_release);
	return a;
}
