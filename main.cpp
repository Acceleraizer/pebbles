#include "pebbles.h"


int main(int argc, char* argv[]) {
    // run_test_suite();
    // three_stones_debug();

    if (! (argc == 2 || argc == 3)) return 1;
    int minn = 2;
    int maxn;
    if (argc == 2) {
        maxn = atoi(argv[1]);
        if (!maxn) return 1;
    }
    if (argc == 3) {
        minn = atoi(argv[1]);
        if (!minn) return 1;
        maxn = atoi(argv[2]);
        if (!maxn) return 1;
    }

    auto clock = std::chrono::high_resolution_clock::now();
    for (int n=minn; n<=maxn; ++n) {
        std::cerr << "Computing " << n << "-stone case" << std::endl;
        n_stones_no_multiple_clusters(n);
        print_time_elapsed("Total time elapsed for this case: ", clock);
    }

}