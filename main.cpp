//12151378 남호준 RSA 최종 과제
#include "hash.hpp"
#include <iostream>
#include <random>
#include <tuple>
#include <stdlib.h>
using namespace std;

#define Prime 0
#define Composite 1

//1. gcd
int gcd(int a, int b);

//2. Extended Euclid for mul_inverse
tuple<int, int, int> extended_euclid(int a, int b);
int m_inv(int a, int n);

//3.exp
long long exp(long long a, long long b, long long m);


//4. miller_rabin for Prime Test
bool miller_rabin(long long n, int s);
bool test(long long a, long long n);

//5. extract least significant 30bit
unsigned int shortHash(uint8_t *digest);

int main()
{
	int p, q;
	int e;
	int d;
	char m[65];
	long long n;
	long long phi_n;
	uint8_t digest[32];
	uint8_t temp[4];

	//input values
	cin >> p >> q >> e >> m;

	//Is p and q both prime number?
	if (miller_rabin(p, 20) || miller_rabin(q, 20)) {
		cout << -1 << endl;
		return 0;
	}

	n = p * q;
	phi_n = (p - 1) * (q - 1);

	//Is phi_n and e in the Multiplicative inverse relationship?
	if (gcd(phi_n, e) != 1) {
		cout << -2 << endl;
		return 0;
	}

	//Key generation for d
	d = m_inv(e, phi_n);
	cout << d << endl;

	// msg -> cryptographic hash function -> output bit sequence
	HASH256(m, digest);

	//output bit sequence -> shortHash -> H_m(30bit)
	int H_m = shortHash(digest);

	//sign generation S = H_m ^ d mod n
	long long S = exp(H_m, d, n);
	cout << S << endl;

	//Is S ^ e mod n == H_m mod n ? -> verification success!
	if (exp(S, e, n) == (H_m % n)) {
		cout << 1 << endl;
	}
	else {
		cout << -3 << endl;
	}

	return 0;
}

int gcd(int a, int b) {
	if (a < b) {
		int temp = a;
		a = b;
		b = temp;
	}

	if (a == b)
		return a;
	else if (b == 0)
		return a;
	else
		return gcd(b, a % b);
}

tuple<int, int, int> extended_euclid(int a, int b) {
	int x_1, y_1, r_1;
	int x_2, y_2, r_2;
	int x_t, y_t, r_t;

	int q;

	if (a == b)
		return make_tuple(1, 0, a);
	else if (b == 0)
		return make_tuple(1, 0, a);
	else {
		x_1 = 1;
		y_1 = 0;
		r_1 = a;

		x_2 = 0;
		y_2 = 1;
		r_2 = b;

		while (r_2 != 0) {

			q = r_1 / r_2;

			r_t = r_1 - q * r_2;
			x_t = x_1 - q * x_2;
			y_t = y_1 - q * y_2;

			x_1 = x_2;
			y_1 = y_2;
			r_1 = r_2;

			x_2 = x_t;
			y_2 = y_t;
			r_2 = r_t;
		}

		return make_tuple(x_1, y_1, r_1);
	}
}

long long exp(long long a, long long b, long long m) {
	long long val = 1;
	a = a % m;

	while (b > 0) {
		if (b & 1)
			val = (val * a) % m;
		a = a * a % m;
		b >>= 1;
	}
	return val;
}



bool miller_rabin(long long n, int s) {
	if (n == 2)
		return Prime;
	else if ((n % 2) == 0)
		return Composite;


	for (int i = 0; i < s; i++) {
		int a = rand();
		a = a % (n - 1) + 1;
		if (test(a, n) == true)
			return Composite;
	}
	return Prime;
}

bool test(long long a, long long n) {
	long long temp = n - 1;
	long long u = 0;
	long long t = 0;

	while (temp % 2 == 0) {
		t += 1;
		temp = temp / 2;
	}

	u = temp;
	t = t;

	long long *x = new long long[t + 1];
	x[0] = exp(a, u, n);

	for (int i = 1; i < t + 1; i++)
	{
		x[i] = exp(x[i - 1], 2, n);
		if ((x[i] == 1) && (x[i - 1] != 1) && (x[i - 1] != n - 1)) {
			return Composite;
		}
	}

	if (x[t] != 1)
		return Composite;

	return Prime;
}

int m_inv(int a, int n) {
	tuple<int, int, int> temp = extended_euclid(n, a % n);

	int x = get<0>(temp);
	int y = get<1>(temp);
	int r = get<2>(temp);

	if (r != 1) {
		cout << "No multiplecative inverse" << endl;
		return 0;
	}
	else {
		if (y < 0) {
			y += n;
		}
		return y % n;
	}

}

unsigned int shortHash(uint8_t *digest) {
	uint8_t temp[4];
	unsigned int result = 0;

	temp[3] = (int)digest[28] & 0x3f; //하위 6비트 추출
	temp[2] = (int)digest[29];
	temp[1] = (int)digest[30];
	temp[0] = (int)digest[31];

	return *((unsigned int*)temp);
}