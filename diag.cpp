#include "pebbles.h"
#include <cassert>
#include <set>


std::pair<int, int> od2_disp(int index, int r=0, int c=0) {
    if (0 <= index < 4) return {r-1+index, c-1};
    else if (4 <= index < 8) return {r+3, c+index-5};
    else if (8 <= index < 12) return {r+11-index, c+3};
    else if (12 <= index < 16) return {r-1, c+15-index};
    else if (16 <= index < 22) return {r-18+index, c-2};
    else if (22 <= index < 28) return {r+4, c+index-24};
    else if (28 <= index < 34) return {r+32-index, c+4};
    else if (34 <= index < 40) return {r-2, c+38-index};
    assert((false));
}

// both operations
long long int r180(long long int code) {
    long long int inner = code % (1L<<16);
    long long int outer = code - inner;
    long long int tmp;
    inner <<= 8;
    tmp = inner % (1L<<16);
    tmp = (inner-tmp) >> 16;
    inner = tmp + (inner % (1L<<16));
    outer <<= 12;
    tmp = outer % (1L<<40);
    tmp = (outer-tmp) >> 24;
    outer = tmp + (outer % (1L<<40));
    return inner + outer;
} 

long long int refl(long long int code) {
    long long int inner = code % (1L<<16);
    long long int outer = code - inner;
    long long int tmp = inner;
    inner = tmp & (1L+(1L<<8));
    for (int i=1; i<=7; ++i) {
        inner += ((tmp & (1L<<i)) << (16-2*i)) + ((tmp & (1L<<(16L-i))) >> (16-2*i));
    }
    tmp = outer;
    outer = tmp & ((1L<<16) + (1L<<28));
    for (int i=17; i<=27; ++i) {
        inner += ((tmp & (1L<<i)) << (56-2*i)) + ((tmp & (1L<<(56L-i))) >> (56-2*i));
    }
    return inner+outer;
}


void pl(long long int code) {
    int i = 0;
    while (code) {
        if (code%2) {
            std::cerr << i << " ";
        }
        ++i;
        code >>= 1;
    }
    std::cerr << "-- ";
}

long long int ctc (std::vector<int> &arr) {
    long long int code = 0;
    for (int i=0; i<arr.size(); ++i) {
        code += arr[i]*(1L<<i);
    }
    return code;
}


void opti_diag_2(int n_pebbles) {
    std::string path = "testlogs/" + std::to_string(n_pebbles) + "_stones_diag2";
    int rad = 10;
    int grid_sz= (2*n_pebbles+1)*rad;
    simstate state = simstate(grid_sz, n_pebbles, path);
    // state.write_blank(TO_LOG);
    state.write_blank(TO_RESULT);

    state.place_cluster(grid_sz/2, grid_sz/2, 2, state.firstcluster_indices[2][3]);
    // ##1
    // #2#
    // 1##
    state.write_state_trimmed(TO_LOG);

    std::cerr << "Computing " << n_pebbles << "-stone case" << std::endl;
    auto totalclock = std::chrono::high_resolution_clock::now();
    auto clock = std::chrono::high_resolution_clock::now();
    state.place_tile(grid_sz/2, grid_sz/2+1, 3);
    state.step();
    state.unplace_tile(grid_sz/2, grid_sz/2+1);
    print_time_elapsed("1231 config : ", clock);

    std::set<long long int> unique;
    for (int n=1; n<=n_pebbles-2; ++n) {
        std::vector<int> choices = std::vector<int>(40,0);
        for (int j=0; j<n; ++j) choices[j] = 1;
        do {
            long long int code = ctc(choices);
            if (unique.count(code)) continue;
            unique.insert(code);
            unique.insert(r180(code));
            unique.insert(refl(code));
            unique.insert(r180(refl(code)));
            pl(code);
            std::vector<std::pair<int, int>> ones;
            for (int i=0; i<40; ++i) {
                if (choices[i]) {
                    ones.push_back(od2_disp(i, grid_sz/2, grid_sz/2));
                }
            }
            state.try_place_fixedones(ones);
            print_time_elapsed("Time : ", clock);
        } while (next_combination(40, choices));
    }

    print_time_elapsed("Total time elapsed for this case: ", totalclock);
}



int main(int argc, char* argv[]) {
    if (! (argc == 2)) return 1;
    int np = atoi(argv[1]);
    if (!np) return 1;

    opti_diag_2(np);
}

