#include "ai/SparseIntersectMap.hpp"
#include "common/misc.hpp"
#include "common/timer.hpp"
#include <ctime>
#include <cstdlib>

using namespace LM;
using namespace std;

#define RUN_TEST(t)               \
do {                              \
	uint64_t start = get_ticks(); \
	uint64_t end;                 \
	t;                            \
	end = get_ticks();            \
	cout << "Test " #t " took " << end - start << "ms" << endl; \
} while (0);

void test(const SparseIntersectMap::Intersect& c, const SparseIntersectMap::Intersect& e) {
	if (c.x != e.x || c.y != e.y) {
		BREAK();
		FATAL("Test failed");
	}
}

void test_incremental() {
	int size = 2048;
	SparseIntersectMap m(0, size*size);

	SparseIntersectMap::Intersect isect;
	SparseIntersectMap::Intersect itest;
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			isect.x = size - i;
			isect.y = size - j;
			m.set(i, j, 0, isect);
			m.get(i, j, 0, &itest);
			test(isect, itest);
		}
	}
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			isect.x = size - i;
			isect.y = size - j;
			m.get(i, j, 0, &itest);
			test(isect, itest);
		}
	}
}

void test_random(int size = 0x100000, int seed = 0) {
	SparseIntersectMap m(0, size);

	SparseIntersectMap::Intersect isect;
	SparseIntersectMap::Intersect itest;

	srand(seed);
	for (int i = 0; i < size; ++i) {
		isect.x = rand();
		isect.y = rand();
		m.set(rand(), rand(), 0, isect);
	}

	srand(seed);
	for (int i = 0; i < size; ++i) {
		isect.x = rand();
		isect.y = rand();
		m.get(rand(), rand(), 0, &itest);
		test(isect, itest);
	}
}

extern "C" int main(int argc, char* argv[]) {
	RUN_TEST(test_incremental());
	RUN_TEST(test_random(0x1000));
	RUN_TEST(test_random(0x10000));
	RUN_TEST(test_random(0x100000));
	RUN_TEST(test_random(0x200000));
	RUN_TEST(test_random(0x400000));

	return 0;
}
