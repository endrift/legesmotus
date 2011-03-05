#include "ai/SparseIntersectMap.hpp"
#include "common/misc.hpp"

using namespace LM;
using namespace std;

void test(const SparseIntersectMap::Intersect& c, const SparseIntersectMap::Intersect& e) {
	if (c.x != e.x || c.y != e.y) {
		BREAK();
		FATAL("Test failed");
	}
}

void test_incremental() {
	int size = 2048;
	SparseIntersectMap m(0, 0x400000);

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

extern "C" int main(int argc, char* argv[]) {
	test_incremental();

	return 0;
}
