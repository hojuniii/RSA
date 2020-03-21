/*
   Copyright (C) 2019 MIRACL UK Ltd.

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as
	published by the Free Software Foundation, either version 3 of the
	License, or (at your option) any later version.


	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.

	 https://www.gnu.org/licenses/agpl-3.0.en.html

	You should have received a copy of the GNU Affero General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   You can be released from the requirements of the license by purchasing
   a commercial license. Buying such a license is mandatory as soon as you
   develop commercial activities involving the MIRACL Core Crypto SDK
   without disclosing the source code of your own applications, or shipping
   the MIRACL Core Crypto SDK with a closed source product.
*/

/*
 * Implementation of the Secure Hashing Algorithm (SHA-256/384/512 and SHA3)
 *
 * Generates a message digest. It should be impossible to come
 * come up with two messages that hash to the same value ("collision free").
 *
 * For use with byte-oriented messages only. Could/Should be speeded
 * up by unwinding loops in HASH_transform(), and assembly patches.
 */

#include "core.h"

using namespace core;

#define H0_256 0x6A09E667L
#define H1_256 0xBB67AE85L
#define H2_256 0x3C6EF372L
#define H3_256 0xA54FF53AL
#define H4_256 0x510E527FL
#define H5_256 0x9B05688CL
#define H6_256 0x1F83D9ABL
#define H7_256 0x5BE0CD19L

static const uint32_t K_256[64] =
{
	0x428a2f98L, 0x71374491L, 0xb5c0fbcfL, 0xe9b5dba5L, 0x3956c25bL, 0x59f111f1L, 0x923f82a4L, 0xab1c5ed5L,
	0xd807aa98L, 0x12835b01L, 0x243185beL, 0x550c7dc3L, 0x72be5d74L, 0x80deb1feL, 0x9bdc06a7L, 0xc19bf174L,
	0xe49b69c1L, 0xefbe4786L, 0x0fc19dc6L, 0x240ca1ccL, 0x2de92c6fL, 0x4a7484aaL, 0x5cb0a9dcL, 0x76f988daL,
	0x983e5152L, 0xa831c66dL, 0xb00327c8L, 0xbf597fc7L, 0xc6e00bf3L, 0xd5a79147L, 0x06ca6351L, 0x14292967L,
	0x27b70a85L, 0x2e1b2138L, 0x4d2c6dfcL, 0x53380d13L, 0x650a7354L, 0x766a0abbL, 0x81c2c92eL, 0x92722c85L,
	0xa2bfe8a1L, 0xa81a664bL, 0xc24b8b70L, 0xc76c51a3L, 0xd192e819L, 0xd6990624L, 0xf40e3585L, 0x106aa070L,
	0x19a4c116L, 0x1e376c08L, 0x2748774cL, 0x34b0bcb5L, 0x391c0cb3L, 0x4ed8aa4aL, 0x5b9cca4fL, 0x682e6ff3L,
	0x748f82eeL, 0x78a5636fL, 0x84c87814L, 0x8cc70208L, 0x90befffaL, 0xa4506cebL, 0xbef9a3f7L, 0xc67178f2L
};

#define PAD  0x80
#define ZERO 0

/* functions */

#define S(m,n,x) (((x)>>n) | ((x)<<(m-n)))
#define R(n,x) ((x)>>n)

#define Ch(x,y,z)  ((x&y)^(~(x)&z))
#define Maj(x,y,z) ((x&y)^(x&z)^(y&z))
#define Sig0_256(x)    (S(32,2,x)^S(32,13,x)^S(32,22,x))
#define Sig1_256(x)    (S(32,6,x)^S(32,11,x)^S(32,25,x))
#define theta0_256(x)  (S(32,7,x)^S(32,18,x)^R(3,x))
#define theta1_256(x)  (S(32,17,x)^S(32,19,x)^R(10,x))

#define Sig0_512(x)    (S(64,28,x)^S(64,34,x)^S(64,39,x))
#define Sig1_512(x)    (S(64,14,x)^S(64,18,x)^S(64,41,x))
#define theta0_512(x)  (S(64,1,x)^S(64,8,x)^R(7,x))
#define theta1_512(x)  (S(64,19,x)^S(64,61,x)^R(6,x))


/* SU= 72 */
static void HASH256_transform(hash256 *sh)
{
	/* basic transformation step */
	uint32_t a, b, c, d, e, f, g, h, t1, t2;
	int j;
	for (j = 16; j < 64; j++)
		sh->w[j] = theta1_256(sh->w[j - 2]) + sh->w[j - 7] + theta0_256(sh->w[j - 15]) + sh->w[j - 16];

	a = sh->h[0];
	b = sh->h[1];
	c = sh->h[2];
	d = sh->h[3];
	e = sh->h[4];
	f = sh->h[5];
	g = sh->h[6];
	h = sh->h[7];

	for (j = 0; j < 64; j++)
	{
		/* 64 times - mush it up */
		t1 = h + Sig1_256(e) + Ch(e, f, g) + K_256[j] + sh->w[j];
		t2 = Sig0_256(a) + Maj(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	sh->h[0] += a;
	sh->h[1] += b;
	sh->h[2] += c;
	sh->h[3] += d;
	sh->h[4] += e;
	sh->h[5] += f;
	sh->h[6] += g;
	sh->h[7] += h;
}

/* Initialise Hash function */
void core::HASH256_init(hash256 *sh)
{
	/* re-initialise */
	int i;
	for (i = 0; i < 64; i++) sh->w[i] = 0L;
	sh->length[0] = sh->length[1] = 0L;
	sh->h[0] = H0_256;
	sh->h[1] = H1_256;
	sh->h[2] = H2_256;
	sh->h[3] = H3_256;
	sh->h[4] = H4_256;
	sh->h[5] = H5_256;
	sh->h[6] = H6_256;
	sh->h[7] = H7_256;

	sh->hlen = 32;
}

/* process a single byte */
void core::HASH256_process(hash256 *sh, int byt)
{
	/* process the next message byte */
	int cnt;
	cnt = (int)((sh->length[0] / 32) % 16);

	sh->w[cnt] <<= 8;
	sh->w[cnt] |= (uint32_t)(byt & 0xFF);

	sh->length[0] += 8;
	if (sh->length[0] == 0L)
	{
		sh->length[1]++;
		sh->length[0] = 0L;
	}
	if ((sh->length[0] % 512) == 0) HASH256_transform(sh);
}

/* SU= 24 */
/* Generate 32-byte Hash */
void core::HASH256_hash(hash256 *sh, uint8_t *digest)
{
	/* pad message and finish - supply digest */
	int i;
	uint32_t len0, len1;
	len0 = sh->length[0];
	len1 = sh->length[1];
	HASH256_process(sh, PAD);
	while ((sh->length[0] % 512) != 448) HASH256_process(sh, ZERO);
	sh->w[14] = len1;
	sh->w[15] = len0;
	HASH256_transform(sh);
	for (i = 0; i < sh->hlen; i++)
	{
		/* convert to bytes */
		digest[i] = (sh->h[i / 4] >> (8 * (3 - i % 4))) & 0xffL;
	}
	HASH256_init(sh);
}


/* test program: should produce digest

256 bit

248d6a61 d20638b8 e5c02693 0c3e6039 a33ce459 64ff2167 f6ecedd4 19db06c1
*/
/*
#include <stdio.h>

char test256[]="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";

int main()
{
	char digest[100];
	int i;

	hash256 sh256;

	HASH256_init(&sh256);
	for (i=0;test256[i]!=0;i++) HASH256_process(&sh256,test256[i]);
	HASH256_hash(&sh256,digest);
	for (i=0;i<32;i++) printf("%02x",(unsigned char)digest[i]);
	printf("\n");

	return 0;
}
*/