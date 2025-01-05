完整多项式模板 (乘除、指对、分治、求值、插值) (线性递推更新版)
```cpp
/*
namespace poly_base;
namespace poly;
namespace CDQ_NTT;
namespace poly_evaluation;
namespace poly_interpolation;
namespace linear_recur;
namespace linear_recur_single;
namespace miscellaneous; 
*/ 

// This is a NEW polynomial template for C++11
#include <bits/stdc++.h>
#define EB emplace_back
#define lg2 std::__lg

typedef unsigned long long u64;
const int N = 530000, mod = 998244353, iv2 = (mod + 1) / 2, unity = 31;
typedef int vec[N], *pvec;
typedef std::pair <int, int> pr;
typedef std::vector <int> vector;

vec inv, fact, finv;

inline int min(const int x, const int y) {return x < y ? x : y;}
inline int max(const int x, const int y) {return x < y ? y : x;}
inline int & reduce(int &x) {return x += x >> 31 & mod;}
inline int & neg(int &x) {return x = (!x - 1) & (mod - x);}
u64 PowerMod(u64 a, int n, u64 c = 1) {for (; n; n >>= 1, a = a * a % mod) if (n & 1) c = c * a % mod; return c;}

// 变换基础
namespace poly_base {
	int l, n; u64 iv; vec w2;

	void init(int n = N, bool dont_calc_factorials = true) {
		int i, t;
		for (inv[1] = 1, i = 2; i < n; ++i) inv[i] = u64(mod - mod / i) * inv[mod % i] % mod;
		if (!dont_calc_factorials) for (*finv = *fact = i = 1; i < n; ++i) fact[i] = (u64)fact[i - 1] * i % mod, finv[i] = (u64)finv[i - 1] * inv[i] % mod;
		t = min(n > 1 ? lg2(n - 1) : 0, 21),
		*w2 = 1, w2[1 << t] = PowerMod(unity, 1 << (21 - t));
		for (i = t; i; --i) w2[1 << (i - 1)] = (u64)w2[1 << i] * w2[1 << i] % mod;
		for (i = 1; i < n; ++i) w2[i] = (u64)w2[i & (i - 1)] * w2[i & -i] % mod;
	}

	inline void NTT_init(int len) {n = 1 << (l = len), iv = mod - (mod - 1) / n;}

	void DIF(int *a) {
		int i, *j, *k, len = n >> 1, R, *o;
		for (i = 0; i < l; ++i, len >>= 1)
			for (j = a, o = w2; j != a + n; j += len << 1, ++o)
				for (k = j; k != j + len; ++k)
					R = (u64)*o * k[len] % mod, reduce(k[len] = *k - R), reduce(*k += R - mod);
	}

	void DIT(int *a) {
		int i, *j, *k, len = 1, R, *o;
		for (i = 0; i < l; ++i, len <<= 1)
			for (j = a, o = w2; j != a + n; j += len << 1, ++o)
				for (k = j; k != j + len; ++k)
					reduce(R = *k + k[len] - mod), k[len] = u64(*k - k[len] + mod) * *o % mod, *k = R;
	}

	inline void DNTT(int *a) {DIF(a);}
	inline void IDNTT(int *a) {
		DIT(a), std::reverse(a + 1, a + n);
		for (int i = 0; i < n; ++i) a[i] = a[i] * iv % mod;
	}

	inline void DIF(int *a, int *b) {memcpy(b, a, n << 2), DIF(b);}
	inline void DIT(int *a, int *b) {memcpy(b, a, n << 2), DIT(b);}
	inline void DNTT(int *a, int *b) {memcpy(b, a, n << 2), DNTT(b);}
	inline void IDNTT(int *a, int *b) {memcpy(b, a, n << 2), IDNTT(b);}
}

// 多项式初等函数
namespace poly {
	using namespace poly_base;

	vec B1, B2, B3, B4, B5, B6;

	// Multiplication (use one buffer, 3-dft of length 2n)
	void mul(int deg, pvec a, pvec b, pvec c) {
		if (!deg) {*c = (u64)*a * *b % mod; return;}
		NTT_init(lg2(deg) + 1), DNTT(a, c), DNTT(b, B1);
		for (int i = 0; i < n; ++i) c[i] = (u64)c[i] * B1[i] % mod;
		IDNTT(c);
	}

	// Inversion (use three buffers, 5-dft)
	void inv(int deg, pvec a, pvec b) {
		int i, len; assert(*a);
		if (*b = PowerMod(*a, mod - 2), deg <= 1) return;
		memset(b + 1, 0, i = 8 << lg2(deg - 1)), memset(B1, 0, i), *B1 = *a;

		for (len = 0; 1 << len < deg; ++len) {
			NTT_init(len + 1);

			memcpy(B1 + (n >> 1), a + (n >> 1), n << 1), DIF(b, B2), DIF(B1, B3);
			for (i = 0; i < n; ++i) B3[i] = (u64)B3[i] * B2[i] % mod; DIT(B3);
			for (i = n >> 1; i < n; ++i) B3[i] = B3[n - i] * iv % mod;

			memset(B3, 0, n << 1), DIF(B3);
			for (i = 0; i < n; ++i) B3[i] = (u64)B3[i] * B2[i] % mod; DIT(B3);
			for (i = n >> 1; i < n; ++i) b[i] = B3[n - i] * (mod - iv) % mod;
		}
	}

	// Division and Modulo Operation (use five buffers)
	void div_mod(int A, int B, pvec a, pvec b, pvec q, pvec r) {
		if (A < B) {memcpy(r, a, (A + 1) << 2), memset(r + (A + 1), 0, (B - A) << 2); return;}
		int Q = A - B, i, l_ = Q ? lg2(Q) + 1 : 0; NTT_init(l_);
		for (i = 0; i <= Q && i <= B; ++i) B4[i] = b[B - i];
		memset(B4 + i, 0, (n - i) << 2), inv(i = Q + 1, B4, B5);

		std::reverse_copy(a + B, a + (A + 1), B4), NTT_init(++l_),
		memset(B4 + i, 0, (n - i) << 2), memset(B5 + i, 0, (n - i) << 2),
		mul(2 * Q, B4, B5, q), std::reverse(q, q + (Q + 1)),
		memset(q + i, 0, (n - i) << 2);

		if (!B) return;
		NTT_init(lg2(2 * B - 1) + 1);
		for (i = 0; i <= Q && i < B; ++i) B2[i] = b[i], B3[i] = q[i];
		memset(B2 + i, 0, (n - i) << 2), memset(B3 + i, 0, (n - i) << 2),
		mul(2 * (B - 1), B2, B3, r), memset(r + i, 0, (n - i) << 2);
		for (i = 0; i < B; ++i) reduce(r[i] = a[i] - r[i]);
	}

	// Multiplication with std::vector (use two buffers, 3-dft)
	void mul(vector &a, vector &b, vector &ret) {
		int A = a.size() - 1, B = b.size() - 1;
		if (!(A || B)) {ret.EB((u64)a[0] * b[0] % mod); return;}
		NTT_init(lg2(A + B) + 1),
		memcpy(B1, a.data(), (A + 1) << 2), memset(B1 + (A + 1), 0, (n - A - 1) << 2),
		memcpy(B2, b.data(), (B + 1) << 2), memset(B2 + (B + 1), 0, (n - B - 1) << 2),
		DNTT(B1), DNTT(B2);
		for (int i = 0; i < n; ++i) B1[i] = (u64)B1[i] * B2[i] % mod;
		IDNTT(B1), ret.assign(B1, B1 + (A + B + 1));
	}

	// Differential
	void diff(int deg, pvec a, pvec b) {for (int i = 1; i <= deg; ++i) b[i - 1] = (u64)a[i] * i % mod;}

	// Integral
	void intg(int deg, pvec a, pvec b, int constant = 0) {for (int i = deg; i; --i) b[i] = (u64)a[i - 1] * ::inv[i] % mod; *b = constant;}

	// f'[x] / f[x] (use four buffers, 6.5-dft)
	void dif_quo(int deg, pvec a, pvec b) {
		assert(*a);
		if (deg <= 1) {*b = PowerMod(*a, mod - 2, a[1]); return;}

		int i, len = lg2(deg - 1);
		inv((deg + 1) / 2, a, B4), NTT_init(len + 1),
		memset(B4 + (n >> 1), 0, n << 1), DIF(B4, B2),

		diff(deg, a, B1), memcpy(B3, B1, n << 1),
		memset(B3 + (n >> 1), 0, n << 1), DIF(B3);

		for (i = 0; i < n; ++i) B3[i] = (u64)B3[i] * B2[i] % mod;
		DIT(B3, b), *b = *b * iv % mod;
		for (i = 1; i < n >> 1; ++i) b[i] = b[n - i] * iv % mod;
		memset(b + (n >> 1), 0, n << 1);

		DIF(b, B4), DIF(a, B3);
		for (i = 0; i < n; ++i) B3[i] = (u64)B3[i] * B4[i] % mod; DIT(B3);
		for (i = n >> 1; i < n; ++i) B3[i] = (B3[n - i] * iv + mod - B1[i]) % mod;

		memset(B3, 0, n << 1), DIF(B3);
		for (i = 0; i < n; ++i) B3[i] = (u64)B3[i] * B2[i] % mod; DIT(B3);
		for (i = n >> 1; i < n; ++i) b[i] = B3[n - i] * (mod - iv) % mod;
	}

	// Logarithm (use DifQuo)
	inline void ln(int deg, pvec a, pvec b) {assert(*a == 1), --deg ? (dif_quo(deg, a, b), intg(deg, b, b)) : void(*b = 0);}

	// Exponentiation (use six buffers, 12-dft)
	// WARNING : this implementation of exponentiation is SLOWER than the CDQ_NTT ver.
	void exp(int deg, pvec a, pvec b) {
		int i, len; pvec c = B6; assert(!*a);
		if (*b = 1, deg <= 1) return;
		if (b[1] = a[1], deg == 2) return;

		memset(b + 2, 0, i = 8 << lg2(deg - 1)), memset(c, 0, i), memset(B1, 0, i),
		*c = 1, neg(c[1] = b[1]);

		for (len = 1; 1 << len < deg; ++len) {
			NTT_init(len + 1);

			DIF(c, B2), DIF(b, B3);
			for (i = 0; i < n; ++i) B4[i] = (u64)B3[i] * B2[i] % mod; DIT(B4);
			for (i = n >> 1; i < n; ++i) B4[i] = B4[n - i] * iv % mod;

			memset(B4, 0, n << 1), DIF(B4);
			for (i = 0; i < n; ++i) B4[i] = (u64)B4[i] * B2[i] % mod; DIT(B4);
			for (i = n >> 1; i < n; ++i) B4[i] = B4[n - i] * (mod - iv) % mod;

			memcpy(B4, c, n << 1), DIF(B4);
			diff(n >> 1, b, B1), DIF(B1, B5);
			for (i = 0; i < n; ++i) B4[i] = (u64)B4[i] * B5[i] % mod; DIT(B4);
			for (i = n >> 1; i < n; ++i) reduce(B5[i] = (a[i] + B4[n - i + 1] * (mod - iv) % mod * ::inv[i]) % mod);

			memset(B5, 0, n << 1), DIF(B5);
			for (i = 0; i < n; ++i) B5[i] = (u64)B5[i] * B3[i] % mod; DIT(B5);
			for (i = n >> 1; i < n; ++i) b[i] = B5[n - i] * iv % mod;

			if (2 << len >= deg) return;

			DIF(b, B3);
			for (i = 0; i < n; ++i) B3[i] = (u64)B3[i] * B2[i] % mod; DIT(B3);
			for (i = n >> 1; i < n; ++i) B3[i] = B3[n - i] * iv % mod;

			memset(B3, 0, n << 1), DIF(B3);
			for (i = 0; i < n; ++i) B3[i] = (u64)B3[i] * B2[i] % mod; DIT(B3);
			for (i = n >> 1; i < n; ++i) c[i] = B3[n - i] * (mod - iv) % mod;
		}
	}
}

// 分治多项式技巧
namespace CDQ_NTT {
	using namespace poly_base;

	int lim;
	vec f, g, C1;
	int fn[N * 2], gn[N * 2];

	inline void register_g(pvec g) {for (int i = 1; 1 << (i - 1) <= lim; ++i) NTT_init(i), DIF(g, gn + (1 << i));}

	// Standard CDQ-NTT Algorithm, type `UK`
	void solve(int L, int w) {
		int i, R = L + (1 << w), M;
		if (!w) {
			// something depend on problem
			return;
		}
		solve(L, w - 1);
		if ((M = (1 << (w - 1)) + L) > lim) return;
		NTT_init(w);
		pvec ga = gn + (1 << w);
		memcpy(C1, f + L, 2 << w), memset(C1 + (1 << (w - 1)), 0, 2 << w),
		DIF(C1);
		for (i = 0; i < n; ++i) C1[i] = (u64)C1[i] * ga[i] % mod;
		DIT(C1);
		for (i = M; i < R; ++i) f[i] = (f[i] + C1[n - (i - L)] * iv) % mod;
		solve(M, w - 1);
	}

	void solve(int L, int w) {
		int i, R = L + (1 << w), M;
		if (!w) {
			// something depend on problem
			return;
		}
		solve(L, w - 1);
		if ((M = (1 << (w - 1)) + L) > lim) return;
		NTT_init(w);
		if (L) {
			pvec fa = fn + (1 << w), ga = gn + (1 << w);
			memcpy(C1, f + L, 2 << w), memset(C1 + (1 << (w - 1)), 0, 2 << w),
			memcpy(C2, g + L, 2 << w), memset(C2 + (1 << (w - 1)), 0, 2 << w),
			DIF(C1), DIF(C2);
			for (i = 0; i < n; ++i) C1[i] = ((u64)C1[i] * ga[i] + (u64)C2[i] * fa[i]) % mod;
			DIT(C1);
			for (i = M; i < R; ++i) f[i] = (f[i] + C1[n - (i - L)] * iv) % mod;
		} else {
			memcpy(C1, f, 2 << w), memset(C1 + M, 0, 2 << w),
			memcpy(C2, g, 2 << w), memset(C2 + M, 0, 2 << w),
			DIF(C1), DIF(C2),
			memcpy(fn + (1 << (w - 1)), C1, 2 << w),
			memcpy(gn + (1 << (w - 1)), C2, 2 << w);
			for (i = 0; i < n; ++i) C1[i] = (u64)C1[i] * C2[i] % mod;
			DIT(C1);
			for (i = M; i < R; ++i) f[i] = (f[i] + C1[n - i] * iv) % mod;
		}
		solve(M, w - 1);
	}
}

// 多点求值
namespace poly_evaluation {
	using namespace poly_base;

	int cnt = 0, lc[N], rc[N];
	vec Prd_, E1, E2, E3;
	vector g[N], tmp_;

	int solve(int L, int R) {
		if (L + 1 == R) return L;
		int M = (L + R) / 2, id = cnt++, lp = solve(L, M), rp = solve(M, R);
		return poly::mul(g[lp], g[rp], g[id]), lc[id] = lp, rc[id] = rp, id;
	}

	void recursion(int id, int L, int R, const vector &poly) {
		if (L + 1 == R) return tmp_.EB(poly.back());
		int i, n = poly.size() - 1, M = (L + R) / 2, lp = lc[id], rp = rc[id],
			dl = min(n, g[lp].size() - 1), dr = min(n, g[rp].size() - 1);
		if (L + 2 == R) return
			tmp_.EB((poly[n] + (u64)poly[n - 1] * g[rp].back()) % mod),
			tmp_.EB((poly[n] + (u64)poly[n - 1] * g[lp].back()) % mod);

		vector ly, ry; ly.reserve(dl + 1), ry.reserve(dr + 1);
		NTT_init(lg2(dl + dr) + 1);
		memcpy(E1, poly.data(), (n + 1) << 2), DIF(E1, E2), memset(E1, 0, (n + 1) << 2);

		memcpy(E1, g[rp].data(), (dr + 1) << 2), DIF(E1, E3), memset(E1, 0, (dr + 1) << 2);
		for (i = 0; i < poly::n; ++i) E3[i] = (u64)E3[i] * E2[i] % mod;
		DIT(E3), std::reverse(E3 + 1, E3 + poly::n);
		for (i = n - dl; i <= n; ++i) ly.EB(E3[i] * iv % mod);

		memcpy(E1, g[lp].data(), (dl + 1) << 2), DIF(E1, E3), memset(E1, 0, (dl + 1) << 2);
		for (i = 0; i < poly::n; ++i) E3[i] = (u64)E3[i] * E2[i] % mod;
		DIT(E3), std::reverse(E3 + 1, E3 + poly::n);
		for (i = n - dr; i <= n; ++i) ry.EB(E3[i] * iv % mod);

		recursion(lp, L, M, ly), recursion(rp, M, R, ry);
	}

	vector emain(int n, pvec f, const vector &pts) {
		int i, id, m = pts.size(), q;
		if (!m) return vector();
		if (!n) return vector(m, *f);
		for (i = 0; i < m; ++i) g[i].clear(), g[i].EB(1), g[i].EB(neg(q = pts[i]));

		id = solve(0, cnt = m), memcpy(Prd_, g[id].data(), (m + 1) << 2);
		poly::inv(n + 1, Prd_, E2), memset(Prd_, 0, (m + 1) << 2);
		if (n > 0) memset(E2 + (n + 1), 0, (poly::n - n - 1) << 2);

		std::reverse_copy(f, f + (n + 1), E1), poly::mul(2 * n, E1, E2, E3),
		memset(E1, 0, (n + 1) << 2), memset(E2, 0, (n + 1) << 2);

		return tmp_.clear(), tmp_.reserve(m), recursion(id, 0, m, vector(E3 + max(n - m + 1, 0), E3 + (n + 1))), tmp_;
	}
}

// 快速插值
namespace poly_interpolation {
	using namespace poly_evaluation;

	vec I1, I2, I3, I4, I5;
	pvec iresult;

	void irecursion(int id, int L, int R) {
		if (L + 1 == R) return;
		int i, M = (L + R) / 2, lp = lc[id], rp = rc[id];
		irecursion(lp, L, M), irecursion(rp, M, R);
		NTT_init(lg2(R - L - 1) + 1);
		memcpy(I1, iresult + L, (M - L) << 2), memset(I1 + (M - L), 0, (poly::n - (M - L)) << 2);
		memcpy(I2, iresult + M, (R - M) << 2), memset(I2 + (R - M), 0, (poly::n - (R - M)) << 2);
		memcpy(I3, g[lp].data(), (M - L + 1) << 2), memset(I3 + (M - L + 1), 0, (poly::n - (M - L + 1)) << 2);
		memcpy(I4, g[rp].data(), (R - M + 1) << 2), memset(I4 + (R - M + 1), 0, (poly::n - (R - M + 1)) << 2);
		DIF(I1), DIF(I2), DIF(I3), DIF(I4);
		for (i = 0; i < n; ++i) I1[i] = ((u64)I1[i] * I4[i] + (u64)I2[i] * I3[i]) % mod;
		DIT(I1), std::reverse(I1 + 1, I1 + n);
		for (i = 0; i < R - L; ++i) iresult[L + i] = I1[i] * iv % mod;
	}

	void imain(int n, pr *pts, pvec ret) {
		int i, id, q;
		assert(n > 0);
		for (i = 0; i < n; ++i) g[i].clear(), g[i].EB(1), g[i].EB(neg(q = pts[i].first));

		id = solve(0, cnt = n), memcpy(Prd_, g[id].data(), (n + 1) << 2);
		poly::inv(n, Prd_, E2), memset(Prd_, 0, n << 2);
		if (n > 1) memset(E2 + n, 0, (poly::n - n) << 2);

		for (i = 0; i < n; ++i) E1[i] = u64(n - i) * g[id][i] % mod;
		poly::mul(2 * (n - 1), E1, E2, E3),
		memset(E1, 0, n << 2), memset(E2, 0, n << 2);

		tmp_.clear(), tmp_.reserve(n), recursion(id, 0, n, vector(E3, E3 + n));
		for (i = 0; i < n; ++i) ret[i] = PowerMod(tmp_[i], mod - 2, pts[i].second);
		iresult = ret, irecursion(id, 0, n), std::reverse(ret, ret + n);
	}
}

// 常系数线性齐次递推式 - Fiduccia
namespace linear_recur {
	using namespace poly_base;

	int ld;
	vec f, g, L1, L2, L3;

	void __builtin_divmod(int d) {
		int i; NTT_init(ld);
		std::reverse_copy(f + d, f + 2 * d, L3);
		memset(L3 + d, 0, (n - d) << 2);
		DNTT(L3);
		for (i = 0; i < n; ++i) L3[i] = (u64)L3[i] * L2[i] % mod;
		IDNTT(L3);
		std::reverse(L3, L3 + d), memset(L3 + d, 0, (n - d) << 2);
		DNTT(L3);
		for (i = 0; i < n; ++i) L3[i] = (u64)L3[i] * L1[i] % mod;
		IDNTT(L3);
		for (i = 0; i < d; ++i) reduce(f[i] -= L3[i]);
		memset(f + d, 0, d << 2);
	}

	void solve(int Q, int d) {
		int i, df = 1, z = lg2(Q); bool alive = false;
		assert(d > 0), ld = lg2(2 * d - 1) + 1;
		if (d == 1) alive = true, *f = 1;

		NTT_init(ld),
		std::reverse_copy(g + 1, g + (d + 1), L1),
		memset(L1 + d, 0, (n - d) << 2),
		poly::inv(d, L1, L2);

		NTT_init(ld),
		memset(L2 + d, 0, (n - d) << 2),
		memcpy(L1, g, d << 2),
		memset(L1 + d, 0, (n - d) << 2),
		DIF(L1), DIF(L2);

		for (; --z >= 0; ) {
			df = df << 1 | (Q >> z & 1);
			if (df < d) continue;
			if (!alive) alive = true, f[df >> 1] = 1;
			NTT_init(ld), DNTT(f);
			for (i = 0; i < n; ++i) f[i] = (u64)f[i] * f[i] % mod;
			IDNTT(f);
			if (df & 1) std::copy_backward(f, f + (2 * d - 1), f + 2 * d), *f = 0;
			__builtin_divmod(d);
		}
	}
}

// 常系数线性齐次递推式 - 单点
namespace linear_recur_single {
	using namespace poly;

	int ld;
	vec f, g, L1, L2, L3, L4;

	int get(int Q, int d) {
		if (Q < d) return f[Q];

		int i, j, ret = 0, halves = 0;
		assert(d > 0), ld = lg2(2 * d - 1) + 1,

		mul(2 * d - 1, f, g, L1), memset(L1 + d, 0, (n - d) << 2),
		memcpy(L2, g, (d + 1) << 2), memset(L2 + (d + 1), 0, (n - d - 1) << 2);

		for (; Q >= d; Q >>= 1) {
			NTT_init(ld), DIF(L1, L4), DIF(L2, L3),
			NTT_init(ld - 1), ++halves;
			if (Q & 1) {
				for (j = i = 0; i < n; ++i, j += 2)
					L4[i] = ((u64)L4[j] * L3[j + 1] + u64(mod - L3[j]) * L4[j + 1]) % mod;
				for (i = 0; i < n; ++i) L4[i] = (u64)L4[i] * w2[i] % mod;
				IDNTT(L4), L4[n] = *L4, memcpy(L1, L4 + 1, d << 2);
			} else {
				for (j = i = 0; i < n; ++i, j += 2)
					L4[i] = ((u64)L4[j] * L3[j + 1] + (u64)L3[j] * L4[j + 1]) % mod;
				IDNTT(L4, L1);
			}
			for (j = i = 0; i < n; ++i, j += 2) L3[i] = (u64)L3[j] * L3[j + 1] % mod;
			IDNTT(L3, L2);
			if (*L2 != 1) L2[d] = (*L2 ? *L2 : mod) - 1, *L2 = 1;
		}

		poly::inv(Q + 1, L2, L3);
		for (i = 0; i <= Q; ++i) ret = (ret + (u64)L1[i] * L3[Q - i]) % mod;
		return PowerMod(iv2, halves, ret);
	}
}

// 杂项
namespace miscellaneous {
	using namespace poly_base;

	vec M1, M2, M3, M4;
	vec Ma[200];

	// calculate b(x) = a(x + c), poly_base::init need 'false'
	void translate(int deg, pvec a, int c, pvec b) {
		if (!deg) {*b = *a; return;}
		int i; u64 pc = 1; NTT_init(lg2(deg) + 2);
		for (i = 0; i <= deg; ++i) M1[i] = (u64)a[i] * fact[i] % mod;
		memset(M1 + i, 0, (n - i) << 2);
		for (i = deg; i >= 0; --i, pc = pc * c % mod) M2[i] = finv[deg - i] * pc % mod;
		DIF(M1), DIF(M2);
		for (i = 0; i < n; ++i) M1[i] = (u64)M1[i] * M2[i] % mod;
		DIT(M1);
		for (i = 0; i <= deg; ++i) b[i] = M1[n - deg - i] * iv % mod * finv[i] % mod;
	}

	// compute a(1), a(z), a(z^2), ..., a(z^(len-1)) to b
	void czt(int deg, int len, int z, pvec a, pvec b) {
		int i; u64 iz = PowerMod(z, mod - 2);
		if (!len) return;
		if (!deg || !z) return std::fill(b, b + len, *a);
		NTT_init(lg2(deg + len - 1) + 1);
		M1[1] = *M1 = M3[1] = *M3 = 1;
		for (i = 2; i < deg + len; ++i) M1[i] = M1[i - 1] * (u64)z % mod, M3[i] = M3[i - 1] * iz % mod;
		for (i = 2; i < deg + len; ++i) M1[i] = (u64)M1[i] * M1[i - 1] % mod, M3[i] = (u64)M3[i] * M3[i - 1] % mod;
		memset(M1 + i, 0, (n - i) << 2);
		for (i = 0; i <= deg; ++i) M2[deg - i] = (u64)M3[i] * a[i] % mod;
		memset(M2 + i, 0, (n - i) << 2),
		DIF(M1), DIF(M2);
		for (i = 0; i < n; ++i) M1[i] = (u64)M1[i] * M2[i] % mod;
		DIT(M1);
		for (i = 0; i < len; ++i) b[i] = M1[n - deg - i] * iv % mod * M3[i] % mod;
	}

	inline int __builtin_inner(int len, pvec a, pvec b) {
		int i = 0; u64 ans = 0;
		#define term(k) (u64)a[i + k] * b[i + k]
		for (; i + 15 < len; i += 16) {
			ans = (ans + term(0) + term(1) + term(2) + term(3)
					   + term(4) + term(5) + term(6) + term(7)
					   + term(8) + term(9) + term(10) + term(11)
					   + term(12) + term(13) + term(14) + term(15)
				) % mod;
		}
		#undef term
		for (; i < len; ++i) ans += (u64)a[i] * b[i];
		return ans % mod;
	}

	// compositional inverse
	void cinv(int deg, pvec a, pvec b) {
		assert(!*a), *b = 0;
		if (--deg <= 0) return;
		int i, j, k, d, B, *c;

		NTT_init(lg2(2 * deg - 1)),
		memcpy(M1, a + 1, deg << 2), memset(M1 + deg, 0, (n - deg) << 2),
		poly::inv(deg, M1, M2),

		NTT_init(lg2(2 * deg - 1) + 1),
		memset(M2 + deg, 0, (n - deg) << 2),

		DIF(M2, M1), B = sqrt(deg),
		memset(*Ma, 0, n << 2), **Ma = 1;
		for (j = 1; j <= B; ++j) {
			DIF(Ma[j - 1], M3), c = Ma[j];
			for (i = 0; i < n; ++i) M3[i] = (u64)M3[i] * M1[i] % mod;
			DIT(M3), *c = *M3 * iv % mod;
			for (i = 1; i < deg; ++i) c[i] = M3[n - i] * iv % mod;
			memset(c + i, 0, (n - i) << 2);
		}
		DIF(Ma[B], M1), memset(M3, 0, n << 2), *M3 = M4[deg - 1] = 1;

		for (d = 0; ; ) {
			for (k = 0; k < B && d < deg; ++k, ++d)
				b[d + 1] = (u64)__builtin_inner(d + 1, Ma[k + 1], M4 + (deg - d - 1)) * inv[d + 1] % mod;
			if (d >= deg) break;
			DIF(M3);
			for (i = 0; i < n; ++i) M3[i] = (u64)M3[i] * M1[i] % mod;
			DIT(M3), *M3 = *M3 * iv % mod;
			for (i = 1; i < deg; ++i) M3[i] = M3[n - i] * iv % mod;
			memset(M3 + i, 0, (n - i) << 2),
			std::reverse_copy(M3, M3 + deg, M4);
		}
	}
}

```