#ifndef MAIN_H
#define MAIN_H

#include <vector>
#include <iostream>
#include <chrono>

#define MODE_VAL  0
#define MODE_PRES 1
#define TO_LOG    32
#define TO_RESULT 33

struct node {
    int val = 0;
    int level = 0;
    int onesscore = 0;
    node* prev = nullptr;
    node* next = nullptr;
    std::vector<node*> parent_history;

    int r,c;

    node(int r, int c) {this->r = r; this->c = c;}
};


struct simstate {
    int grid_sz;
    int rem_ones;
    std::vector<std::vector<node*>> grid;
    std::vector<node*> unocc_w_val;
    // 8-bit number, clockwise numbering
    std::vector<std::vector<int>> cluster_sets = 
        std::vector<std::vector<int>>(9);
    // same as above, but unique up to symmetries
    std::vector<std::vector<int>> firstcluster_indices = 
        std::vector<std::vector<int>>(9);
    std::vector<bool> stone_used;
    std::vector<std::vector<node*>> ones_list = {};
    int maxv = 1;
    std::string log_path, result_path;

    int cur_stone = 1; // Next stone to place.

    // functions
    simstate(int grid_sz, int rem_ones, std::string path);
    ~simstate();

    void init_clusters(int n);
    void process_symmetries(int n, int cluster);

    void place_tile(int r, int c, int v);
    void unplace_tile(int r, int c);
    void adjust(int r, int c, int dv);
    void place_cluster(int r, int c, int v, int index);
    void unplace_cluster(int r, int c, int v, int index);
    void place_ones(std::vector<int> &choice_arr);
    void unplace_ones(std::vector<int> &choice_arr);

    void try_place_stone();
    void try_place_ones();
    void try_place_cluster(int r, int c, int v);
    void try_place_firstcluster(int r, int c, int v);

    void step();
    void save_maximum(bool newrecord);

    void write_blank(int dest);
    void write_state(int dest, bool app=true, int mode=MODE_VAL, std::vector<int> bounds={});
    void write_unocc(int dest, int lvl, bool app=true);
    void write_stones_used(int dest, int mxm, bool app=true);
    void write_state_trimmed(int dest, bool app=true, int mode=MODE_VAL);
    void write_possible_ones(int dest, bool app=true);
};


void n_stones_no_multiple_clusters(int n_pebbles);
void print_time_elapsed(std::string pretext, std::chrono::_V2::system_clock::time_point &start_time);


#endif