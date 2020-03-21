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

#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

namespace core {

/**
 * @brief SHA256 hash function instance */
	typedef struct
	{
		uint32_t length[2]; /**< 64-bit input length */
		uint32_t h[8];      /**< Internal state */
		uint32_t w[80];	/**< Internal state */
		int hlen;		/**< Hash length in bytes */
	} hash256;

	/* Hash function */
	/**	@brief Initialise an instance of SHA256
	 *
		@param H an instance SHA256
	 */
	extern void HASH256_init(hash256 *H);
	/**	@brief Add a byte to the hash
	 *
		@param H an instance SHA256
		@param b byte to be included in hash
	 */
	extern void HASH256_process(hash256 *H, int b);
	/**	@brief Generate 32-byte hash
	 *
		@param H an instance SHA256
		@param h is the output 32-byte hash
	 */
	extern void HASH256_hash(hash256 *H, uint8_t *h);
}

#endif
