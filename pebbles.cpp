// #include <cstdio>
#include "pebbles.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <queue>
#include <algorithm>
#include <cassert>
#include <unordered_set>
#include <bitset>


simstate::simstate(int grid_sz, int rem_ones, std::string path) {
    this->grid_sz = grid_sz;
    this->rem_ones = rem_ones;
    this->log_path = path + "_log.txt";
    this->result_path = path + ".txt";

    stone_used = std::vector<bool>(174*rem_ones, false);
    stone_used[1] = true;

    //initialize the ladder
    for (int i=0; i< 174*rem_ones; i++) {
        node* newnnode = new node(-1, -1);
        unocc_w_val.push_back(newnnode);
    }

    //initialize ones_list
    ones_list = std::vector<std::pair<int, std::vector<node*>>>(174*rem_ones, {0, std::vector<node*>(50, nullptr)});

    grid = std::vector<std::vector<node*>>(grid_sz, std::vector<node*>(grid_sz));
    for (int r=0; r<grid_sz; ++r) {
        for (int c=0; c<grid_sz; ++c) {
            grid[r][c] = new node(r, c);
            grid[r][c]->next = unocc_w_val[0]->next;
            grid[r][c]->prev = unocc_w_val[0];
            if (unocc_w_val[0]->next) unocc_w_val[0]->next->prev = grid[r][c];
            unocc_w_val[0]->next = grid[r][c];
        }
    }

    for (int i=2; i<9; ++i) {
        cluster_sets[i] = {};
        firstcluster_indices[i] = {};
        init_clusters(i);
    }   
}

simstate::~simstate() {
    for (auto r: grid) for (node* n: r) delete n;
    for (node* n: unocc_w_val) delete n;
}

bool next_combination (std::vector<int> &arr) {
    int i = arr.size() - 1;
    if (arr[i] == 0) {
        while (i && arr[i] == 0) {
            --i;
        }
        arr[i] = 0;
        arr[i+1] = 1;
    } else {
        int tl = 1;
        while (i>=0 && arr[i]) {
            ++tl;
            arr[i] = 0;
            --i;
        }
        while(i>=0 && arr[i] == 0) {
            --i;
        }
        if (i>=0) arr[i] = 0;
        else return false;
        ++i;
        while(tl) {
            arr[i] = 1;
            ++i;
            --tl;
        }
    }
    return true;
}

void print_combination(std::vector<int> &arr) {
    for (int i=0; i<arr.size(); ++i) {
        if (arr[i]) printf("%u ", i+1);
    }
    std::cout << std::endl;
}

void simstate::init_clusters(int n) {
    std::vector<int> start = std::vector<int>(8, 0);
    for (int i=0; i<n; ++i) start[i] = 1;
    
    do {
        // print_combination(start);
        int cluster = 0;
        for (int i=0; i<8; ++i) cluster += start[i]*(1<<i);
        if (std::find(cluster_sets[n].begin(), cluster_sets[n].end(), cluster) == cluster_sets[n].end()) {
            firstcluster_indices[n].push_back(cluster_sets[n].size());
            process_symmetries(n, cluster);
        }
        
    } while (next_combination(start));
}

void simstate::process_symmetries(int n, int cluster) {
    // std::cout << (n) << ": " <<  std::bitset<8>(cluster) << std::endl;
    std::unordered_set<int> syms;
    for(int q=0; q<4; ++q) {
        // std::cout << (n) << ":--- " <<  std::bitset<8>(cluster) << std::endl;
        syms.insert(cluster);
        cluster <<=2;
        int ovf = cluster - (cluster %256);
        cluster = cluster%256 + (ovf>>8);
    }
    int refl = 0;
    refl += ((cluster & (1)) << 2) + ((cluster & (1<<2)) >> 2)
            + ((cluster & (1<<7)) >> 4) + ((cluster & (1<<3)) << 4)
            + ((cluster & (1<<6)) >> 2) + ((cluster & (1<<4)) << 2)
            + (cluster & ((1<<1) + (1<<5)));
    for (int q=0; q<4; ++q) {
        // std::cout << (n) << ":||| " <<  std::bitset<8>(refl) << std::endl;
        syms.insert(refl);
        refl <<=2;
        int ovf = refl - (refl %256);
        refl = refl%256 + (ovf>>8);
    }
    for (int cl : syms) cluster_sets[n].push_back(cl);
}

// Updates the value of the tile node at (r,c). 
// Updates the level in neighboring cells. 
// Creates a new list of possible 1-positions if there are 1s remaining.
void simstate::place_tile(int r, int c, int v) {
    grid[r][c]->val = v;
    for (int rp = r-1; rp <= r+1; ++rp) {
        for (int cp = c-1; cp <= c+1; ++cp) {
            if ((rp - r) || (cp - c)) {
                adjust(rp, cp, v);
                // std::cout << rp << " " << cp  << " : " << v << std::endl;
            }
        }
    }
    stone_used[v] = true;
    while (stone_used[cur_stone]) ++cur_stone;

    ++depth;
    int ones_i = 0;
    for (int rp = r-2; rp <= r+2; ++rp) {
        for (int cp = c-2; cp <= c+2; ++cp) {
            if (rp == r-2 || rp == r+2 || cp == c-2 || cp == c+2) {
                ++grid[rp][cp]->onesscore;
                if (grid[rp][cp]->onesscore == 1 && !(grid[rp][cp]->val)) {
                    ones_list[depth].second[ones_i] = grid[rp][cp];
                    ++ones_i;
                } 
            }
        }
    }
    // ones_list.push_back(newly_acsble_1s);
    ones_list[depth].first = ones_i;
}

void simstate::unplace_tile(int r, int c) {
    int v = grid[r][c]->val;
    for (int rp = r+1; rp >= r-1; --rp) {
        for (int cp = c+1; cp >= c-1; --cp) {
            if ((rp - r) || (cp - c)) {
                adjust(rp, cp, -v);
                // std::cout << rp << " " << cp  << std::endl;
            }
        }
    }
    for (int rp = r-2; rp <= r+2; ++rp) {
        for (int cp = c-2; cp <= c+2; ++cp) {
            if (rp == r-2 || rp == r+2 || cp == c-2 || cp == c+2) {
                --grid[rp][cp]->onesscore;
            }
        }
    }
    --depth;
    stone_used[v] = false;
    grid[r][c]->val = 0;
    while (!stone_used[cur_stone-1]) --cur_stone;
}

// If v>0 (placing a tile), always inserts at the head of list (becomes the node pointed to by unocc_w_val[i]->next)
// Else, restores the node back to its previous position based on its history
// Updates the level
void simstate::adjust(int r, int c, int dv) {
    if (!dv) return;
    if (grid[r][c]->next) {
        grid[r][c]->prev->next = grid[r][c]->next;
        grid[r][c]->next->prev = grid[r][c]->prev;
    } else {
        grid[r][c]->prev->next = nullptr;
    }
    grid[r][c]->level += dv;
    int v = grid[r][c]->level;

    if (dv > 0) {
        grid[r][c]->parent_history.push_back(grid[r][c]->prev);
        grid[r][c]->next = unocc_w_val[v]->next;
        if (grid[r][c]->next) grid[r][c]->next->prev = grid[r][c];
        unocc_w_val[v]-> next = grid[r][c];
        grid[r][c]->prev = unocc_w_val[v];
    }
    else if (dv < 0) {
        grid[r][c]->prev = grid[r][c]->parent_history.back();
        grid[r][c]->next = grid[r][c]->prev->next;
        grid[r][c]->prev->next = grid[r][c];
        if (grid[r][c]->next) grid[r][c]->next->prev = grid[r][c];
        grid[r][c]->parent_history.pop_back();
    }
    
}

// Converts between cluster encoding to offset from top-left
std::pair<int, int> code_to_coord(int index) {
    switch (index) {
    case 0: return {0,0};
    case 1: return {0,1};
    case 2: return {0,2};
    case 3: return {1,2};
    case 4: return {2,2};
    case 5: return {2,1};
    case 6: return {2,0};
    case 7: return {1,0};
    }
    assert((false));
}

// Handles placing clusters
void simstate::place_cluster(int r, int c, int v, int index) {
    int cluster = cluster_sets[v][index];
    for (int i=0; i<8; ++i) {
        std::pair<int, int> coords = code_to_coord(i);
        int rp=coords.first, cp=coords.second;
        if (cluster & (1<<i)) {
            grid[r+rp][c+cp]->val = 1;
            for (int dr=-1; dr<=1; ++dr) {
                for (int dc=-1; dc<=1; ++dc) {
                    if (dr || dc) {
                        adjust(r+rp+dr, c+cp+dc,1);
                    }
                }
            }
        }
        ++grid[r+rp][c+cp]->onesscore;
    }
    grid[r+1][c+1]->val = v;
    for (int dr=-1; dr<=1; ++dr) {
        for (int dc=-1; dc<=1; ++dc) {
            if (dr || dc) {
                adjust(r+1+dr, c+1+dc, v);
            }
        }
    }

    stone_used[v] = true;
    while (stone_used[cur_stone]) ++cur_stone;
    rem_ones -= v;

     ++depth;
    int ones_i = 0;
    for (int rp=r-2; rp<r+5; ++rp) {
        for (int cp=c-2; cp<c+5; ++cp) {
            ++grid[rp][cp]->onesscore;
            if (grid[rp][cp]->onesscore == 1 && !(grid[rp][cp]->val)) {
                ones_list[depth].second[ones_i] = grid[rp][cp];
                ++ones_i;
            }
        }
    }
    
    ones_list[depth].first = ones_i;
}

void simstate::unplace_cluster(int r, int c, int v, int index) {
    // ones_list.pop_back();
    int cluster = cluster_sets[v][index];
    grid[r+1][c+1]->val = 0;
    for (int dr=1; dr>=-1; --dr) {
        for (int dc=1; dc>=-1; --dc) {
            if (dr || dc) {
                adjust(r+1+dr, c+1+dc, -v);
            }
        }
    }
    for (int i=7; i>=0; --i) {
        std::pair<int, int> coords = code_to_coord(i);
        int rp=coords.first, cp=coords.second;
        if (cluster & (1<<i)) {
            grid[r+rp][c+cp]->val = 0;
            for (int dr=1; dr>=-1; --dr) {
                for (int dc=1; dc>=-1; --dc) {
                    if (dr || dc) {
                        adjust(r+rp+dr, c+cp+dc, -1);
                    }
                }
            }
        }
        --grid[r+rp][c+cp]->onesscore;
    }
    
    --depth;
    stone_used[v] = false;
    rem_ones += v;

    for (int rp=r-2; rp<r+5; ++rp) {
        for (int cp=c-2; cp<c+5; ++cp) {
            if(!(r<=rp && rp<r+3 && c<=cp && cp<c+3)) {
                --grid[rp][cp]->onesscore;
            }
        }
    }
}


void simstate::place_ones(std::vector<int> &choice_arr) {
    std::unordered_set<uintptr_t> update_set;
    for (int i = 0; i < choice_arr.size(); ++i) {
        if (choice_arr[i]) {
            node* n = ones_list[depth].second[i];
            assert((!n->val));
            n->val = 1;
            --rem_ones;
            for (int dr = -1; dr <= 1; ++dr) {
                for (int dc = -1; dc <= 1; ++dc) {
                    // if (dr || dc) 
                    adjust(dr+n->r, dc+n->c, 1);
                }
            }
            for (int dr = -2; dr <= 2; ++dr) {
                for (int dc = -2; dc <= 2; ++dc) {
                    update_set.insert((uintptr_t) grid[dr+n->r][dc+n->c]);
                }
            }
        }
    }

    ++depth;
    int ones_i = 0;
    for (auto p: update_set) {
        node* n = (node*) p;
        ++n->onesscore;
        if (n->onesscore == 1 && !n->val) {
            ones_list[depth].second[ones_i] = n;
            ++ones_i;
        }
    }
    ones_list[depth].first = ones_i;
    
}


void simstate::unplace_ones(std::vector<int> &choice_arr) {
    // ones_list.pop_back();
    --depth;
    std::unordered_set<uintptr_t> update_set;
    for (int i = choice_arr.size()-1; i >= 0; --i) {
        if (choice_arr[i]) {
            node* n = ones_list[depth].second[i];
            assert((n->val == 1));
            n->val = 0;
            ++rem_ones;
            for (int dr = 1; dr >= -1; --dr) {
                for (int dc = 1; dc >= -1; --dc) {
                    // if (dr || dc) 
                    adjust(dr+n->r, dc+n->c, -1);
                }
            }
            for (int dr = 2; dr >= -2; --dr) {
                for (int dc = 2; dc >= -2; --dc) {
                    update_set.insert((uintptr_t) grid[dr+n->r][dc+n->c]);
                }
            }
        }
    }
    
    for (auto p: update_set) {
        node* n = (node*) p;
        --n->onesscore;
    }
    
}


// ================== main loops
void print_time_elapsed(std::string pretext, std::chrono::_V2::system_clock::time_point &start_time) {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now-start_time);
    std::cerr << pretext << std::fixed << std::setprecision(3) << (float) duration.count() /1000.0 << "s" << std::endl;
    start_time = now;
}

void simstate::try_place_stone() {
    // write_state(true);
    // write_stones_used(TO_LOG, 50, true);
    // write_possible_ones(TO_LOG, true);
    // std::cerr << "STONE: " << cur_stone << std::endl;
    // int old_cur_stone = cur_stone;
    // maxv = std::max(maxv, cur_stone);
    // while(stone_used[cur_stone]) ++cur_stone;
    node* cur = unocc_w_val[cur_stone]->next;
    
    if (!cur) {
        if (cur_stone-1 >= maxv) save_maximum(cur_stone-1>maxv);
        return;
    }
    while (cur) {
        if (!cur->val) {
            place_tile(cur->r, cur->c, cur_stone);
            step();
            unplace_tile(cur->r, cur->c);
        } 
        cur = cur -> next;
    }
}


void simstate::try_place_ones() {
    int n_options = ones_list[depth].first;
    // std::cerr << "CHOICES: " << n_options << std::endl;
    // write_possible_ones(TO_LOG, true);
    // if (!rem_ones || !n_options) return;

    for (int cnt=1; cnt<=rem_ones; ++cnt) {
        std::vector<int> choice_arr = std::vector<int>(n_options, 0);
        for (int i=0; i<cnt; ++i) choice_arr[i] = 1;
        do {
            // print_combination(choice_arr);
            place_ones(choice_arr);
            step();
            unplace_ones(choice_arr);
        } while (next_combination(choice_arr));
    }
}

void simstate::try_place_cluster(int r, int c, int v) {
    for (int i=0; i<cluster_sets[v].size(); ++i) {
        place_cluster(r, c, v, i);
        step();
        unplace_cluster(r, c, v, i);
    }
}

void simstate::try_place_firstcluster(int r, int c, int v) {
    auto clock = std::chrono::high_resolution_clock::now();
    for (int i: firstcluster_indices[v]) {
        std::cerr << std::bitset<8>(cluster_sets[v][i]);
        place_cluster(r, c, v, i);
        step();
        unplace_cluster(r, c, v, i);
        std::cerr << " --- ";
        print_time_elapsed("Time elapsed from last time point: ", clock);
    }
}



void simstate::step() {
    // A FEW OPTIONS
    // -- place tile cur
    // -- if 'ones > 0', place isolated 1 in the fresh outer border

    // NOT DONE: placing cluster
    // write_stones_used(TO_LOG, 50, true);
    // if (cur_stone >= 8) {
    //     write_possible_ones(TO_LOG);
    // }
    if (rem_ones && ones_list[depth].first) try_place_ones();
    // write_unocc(TO_LOG, cur_stone);
    try_place_stone();
    
    
}


void n_stones_no_multiple_clusters(int n_pebbles) {
    std::string path = "testlogs/" + std::to_string(n_pebbles) + "_stones" ;
    int rad = ceil(log2f64(174*n_pebbles));
    int grid_sz = (2*n_pebbles+1)*rad;
    simstate state = simstate(grid_sz, n_pebbles, path);
    state.write_blank(TO_LOG);
    state.write_blank(TO_RESULT);
    for (int s=2; s<=n_pebbles; ++s) {
        state.try_place_firstcluster(grid_sz/2, grid_sz/2, s);
    }
}



// printing for visualization
void simstate::write_state(int dest, bool app, int mode, std::vector<int> bounds) {
    std::string path = dest == TO_LOG ? log_path : result_path;
    std::ofstream of;
    if (app) of.open(path, std::ios::out | std::ios::app);
    else of.open(path);
    if (bounds.empty()) bounds = {0, grid_sz, 0, grid_sz};

    int digits = ceil(log10(maxv))+1;
    std::string horizontal_border = std::string((size_t)(digits+1)*(bounds[3]-bounds[2])+1, '-');

    if (mode == MODE_VAL) of << "values\n";
    if (mode == MODE_PRES) of << "level\n";
    of << horizontal_border << '\n';
    for (int r=bounds[0]; r<bounds[1]; ++r) {
        of << '|';
        for (int c=bounds[2]; c<bounds[3]; ++c) {
            if (mode == MODE_VAL) {
                of << std::setw(digits);
                if (grid[r][c]->val) of << grid[r][c]->val;
                else of << ' ';
                of << '|';
            }
            if (mode == MODE_PRES) {
                of << std::setw(digits);
                if ( grid[r][c]->level) of << grid[r][c]->level;
                else of << ' ';
                of << '|';
            }
        }
        of << '\n' << horizontal_border << '\n';
    }
    of.close();
}

void simstate::write_unocc(int dest, int lvl, bool app) {
    std::string path = dest == TO_LOG ? log_path : result_path;std::ofstream of;
    if (app) of.open(path, std::ios::out | std::ios::app);
    else of.open(path);

    of << "LEVEL = " << lvl << ": ";
    node* cur = unocc_w_val[lvl]->next;
    while (cur) {
        assert((cur->level == lvl));
        if (!cur->val) of << "(" << cur->r << ", " << cur->c << ") ";
        cur = cur->next;
    }
    of << '\n';
    of.close();
}

void simstate::write_stones_used(int dest, int mxm, bool app) {
    std::string path = dest == TO_LOG ? log_path : result_path;
    std::ofstream of;
    if (app) of.open(path, std::ios::out | std::ios::app);
    else of.open(path);
    of << "STONES USED : ";
    for (int i=2; i<=mxm; ++i) {
        if (stone_used[i]) of << i << " ";
    }
    of << '\n';
    of.close();
}

void simstate::write_blank(int dest) {
    std::string path = dest == TO_LOG ? log_path : result_path;
    std::ofstream of;
    of.open(path, std::ios::trunc);
    of.close();
}

void simstate::write_state_trimmed(int dest, bool app, int mode) {
    int l = grid[0].size();
    int r = 0;
    int u = grid.size();
    int d = 0;
    for (int y=0; y<grid.size(); ++y) {
        for (int x=0; x<grid[0].size(); ++x) {
            if (grid[y][x]->val>0) {
                l = std::min(l, x);
                r = std::max(r, x+1);
                u = std::min(u, y);
                d = std::max(d, y+1);
            }
        }
    }
    // std::cerr << u << d << l << r << std::endl;
    write_state(dest, app, mode, {u,d,l,r});
}

void simstate::write_possible_ones(int dest, bool app) {
    std::string path = dest == TO_LOG ? log_path : result_path;
    std::ofstream of;
    if (app) of.open(path, std::ios::out | std::ios::app);
    else of.open(path);
    of << "POSSIBLE ONES : \n";
    if (ones_list.empty()) {
        of.close();
        return;
    }

    int digits = ceil(log10(maxv))+1;
    std::string horizontal_border = std::string((size_t)(digits+1)*grid_sz+1, '-');
    of << horizontal_border << '\n';
    for (int r=0; r<grid_sz; ++r) {
        of << '|';
        for (int c=0; c<grid_sz; ++c) {
            of << std::setw(digits);
            std::vector<node*> vec = ones_list[depth].second;;
            if (grid[r][c]->val) of << grid[r][c]->val;
            else if (std::find(vec.begin(), vec.begin() + ones_list[depth].first, grid[r][c]) != (vec.begin() + ones_list[depth].first)) of << 'x';
            else of << ' ';
            of << '|';
        }
        of << '\n' << horizontal_border << '\n';
    }
    of.close();
}

void simstate::save_maximum(bool newrecord) {
    maxv = cur_stone-1;
    if (newrecord) {
        std::ofstream of;
        of.open(result_path);
        of << "CURRENT BEST : " << maxv << "\n";
        of.close();
    }
    write_state_trimmed(TO_RESULT);
}