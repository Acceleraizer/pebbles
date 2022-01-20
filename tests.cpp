#include "pebbles.h"
#include "math.h"
#include "iostream"


// ===== SOME TEST CASES

void test() {
    std::string out = "testlogs/test";
    int n_pebbles = 2;
    int rad = ceil(log2f64(174*n_pebbles));

    // printf("%u\n", rad);

    int grid_sz = (2*rad + 1)*n_pebbles *2 +1;

    simstate state = simstate(10, n_pebbles, out);
    state.place_cluster(3, 3, 2, 6);
    state.write_blank(TO_LOG);
    state.write_state(TO_LOG);
    state.write_state(TO_LOG, true, MODE_PRES);
    state.write_unocc(TO_LOG, 3, true);
    state.write_unocc(TO_LOG, 4, true);


    // place a stone
    state.place_tile(4,3,3);
    state.write_state(TO_LOG, true);
    state.write_state(TO_LOG, true, MODE_PRES);
    state.write_unocc(TO_LOG, 3, true);
    state.write_unocc(TO_LOG, 4, true);

    // place another stone
    state.place_tile(4,2,4);
    state.write_state(TO_LOG, true);
    state.write_state(TO_LOG, true, MODE_PRES);
    state.write_unocc(TO_LOG, 3, true);
    state.write_unocc(TO_LOG, 4, true);

    // undo placing the stones
    state.unplace_tile(4,2);
    state.write_state(TO_LOG, true);
    state.write_state(TO_LOG, true, MODE_PRES);
    state.write_unocc(TO_LOG, 3, true);
    state.write_unocc(TO_LOG, 4, true);

    state.unplace_tile(4,3);
    state.write_state(TO_LOG, true);
    state.write_state(TO_LOG, true, MODE_PRES);
    state.write_unocc(TO_LOG, 3, true);
    state.write_unocc(TO_LOG, 4, true);
}

void cluster_test() {
    std::string path = "testlogs/cluster_test";
    int n_pebbles = 2;
    int rad = ceil(log2f64(174*n_pebbles));
    int grid_sz = 21;
    simstate state = simstate(grid_sz, n_pebbles, path);
    state.place_cluster(3,3,2,6);
    state.write_blank(TO_LOG);
    state.write_state(TO_LOG);
    state.write_state(TO_LOG, true, MODE_PRES);
    state.place_tile(4,3,3);
    state.write_state(TO_LOG, true);
    state.write_state(TO_LOG, true, MODE_PRES);
    state.unplace_tile(4,3);
    state.write_state(TO_LOG, true);
    state.write_state(TO_LOG, true, MODE_PRES);
    state.unplace_cluster(3,3,2, 6);
    state.write_state(TO_LOG, true);
    state.write_state(TO_LOG, true, MODE_PRES);
}

void simple_loop() {
    std::string path = "testlogs/simple_loop";
    int n_pebbles = 2;
    int rad = ceil(log2f64(174*n_pebbles));
    int grid_sz = 21;
    simstate state = simstate(grid_sz, n_pebbles, path);
    state.write_blank(TO_LOG);
    state.write_blank(TO_RESULT);
    state.try_place_firstcluster(10,10,2);
}

void test_ones() {
    std::string path = "testlogs/test_ones";
    int n_pebbles = 4;
    int grid_sz = 10;
    simstate state = simstate(grid_sz, n_pebbles, path);
    state.write_blank(TO_LOG);
    state.write_possible_ones(TO_LOG);
    state.place_cluster(4, 4, 2, 0);
    // state.write_state(TO_LOG);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 1);

    std::vector<int> choice_arr = {1};
    state.place_ones(choice_arr);
    // state.write_state(TO_LOG);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 1);

    state.place_tile(4,6,3);
    // state.write_state(TO_LOG);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 1);

    state.place_tile(3,6,4);
    // state.write_state(TO_LOG);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 1);

    state.unplace_tile(3,6);
    // state.write_state(TO_LOG);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 1);

    state.unplace_tile(4,6);
    // state.write_state(TO_LOG);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 1);

    state.unplace_ones(choice_arr);
    // state.write_state(TO_LOG);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 1);
}



void three_stones_debug() {
    std::string path = "testlogs/three_stones_debug";
    int n_pebbles = 3;
    int rad = ceil(log2f64(174*n_pebbles));
    int grid_sz = 14;
    simstate state = simstate(grid_sz, n_pebbles, path);
    state.write_blank(TO_LOG);
    state.place_cluster(6,4,2,state.firstcluster_indices[2][3]);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 3);

    state.place_tile(6,5,3);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 4);

    state.place_tile(5,6,4);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 5);

    state.place_tile(5,7,5);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 6);

    state.place_tile(7,4,6);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 7);

    state.place_tile(8,3,7);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 8);

    state.place_tile(9,3,8);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 9);

    state.place_tile(4,6,9);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 10);

    std::vector<int> choice_arr = {1,0,0,0,0};
    state.place_ones(choice_arr);
    state.write_state(TO_LOG);
    state.write_unocc(TO_LOG, 10);

    state.place_tile(3,5,10);
    state.maxv = 10;
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 11);

    state.place_tile(2,5,11);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 12);

    state.place_tile(1,4,12);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 13);

    state.place_tile(2,3,13);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 14);

    state.place_tile(3,3,14);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 15);

    state.place_tile(8,2,15);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 16);

    state.place_tile(9,4,16);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 17);

    state.place_tile(9,5,17);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 18);

    state.place_tile(4,7,18);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 19);
    
    state.place_tile(8,6,19);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 20);

    state.place_tile(7,7,20);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 21);

    state.place_tile(2,6,21);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 22);

    state.place_tile(7,2,22);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 23);

    state.place_tile(4,8,23);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 24);

    state.place_tile(4,4,24);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 25);

    state.place_tile(6,8,25);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 26);

    state.place_tile(1,3,26);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 27);

    state.place_tile(3,2,27);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 28);

    state.place_tile(6,3,28);
    state.write_possible_ones(TO_LOG);
    state.write_unocc(TO_LOG, 29);
}


void run_test_suite() {
    test();
    cluster_test();
    simple_loop();
    test_ones();
}


int main() {
    run_test_suite();
}