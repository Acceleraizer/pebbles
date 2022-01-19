// #include <cstdio>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <cassert>

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
    int cur_stone = 2;
    int rem_ones;
    std::vector<std::vector<node*>> grid;
    std::vector<node*> unocc_w_val;
    std::vector<std::vector<std::vector<int>>> startcluster_sets = 
        std::vector<std::vector<std::vector<int>>>(9);
    std::vector<bool> stone_used;
    std::vector<std::vector<node*>> ones_list = {};
    int maxv = 1;
    // std::vector<std::vector<node*>> bestgrid;
    std::string log_path, result_path;


    // functions
    simstate(int grid_sz, int rem_ones, std::string path);
    ~simstate();

    std::vector<std::vector<int>> init_startcluster(int n);

    void place_tile(int r, int c, int v);
    void unplace_tile(int r, int c);
    void adjust(int r, int c, int dv);
    void place_cluster(int r, int c, int v, int index);
    void unplace_cluster(int r, int c);
    void place_ones(std::vector<int> &choice_arr);
    void unplace_ones(std::vector<int> &choice_arr);

    void try_place_stone();
    void try_place_ones();
    void try_place_cluster(int r, int c, int v);

    void step();
    void save_maximum();

    void write_blank(int dest);
    void write_state(int dest, bool app, int mode, std::vector<int> bounds);
    void write_unocc(int dest, int lvl, bool app);
    void write_stones_used(int dest, int mxm, bool app);
    void write_state_trimmed(int dest,  bool app, int mode);
};


simstate::simstate(int grid_sz, int rem_ones, std::string path) {
    this->grid_sz = grid_sz;
    this->rem_ones = rem_ones;
    this->log_path = path + "_log.txt";
    this->result_path = path + ".txt";

    stone_used = std::vector<bool>(174*rem_ones, false);

    //initialize the ladder
    for (int i=0; i< 174*rem_ones; i++) {
        node* newnnode = new node(-1, -1);
        unocc_w_val.push_back(newnnode);
    }

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
        startcluster_sets[i] = init_startcluster(i);
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

std::vector<std::vector<int>> simstate::init_startcluster(int n) {
    std::vector<std::vector<int>> possible;

    std::vector<int> start = std::vector<int>(8, 0);
    for (int i=0; i<n; ++i) start[i] = 1;
    
    do {
        // print_combination(start);
        start.insert(start.begin()+4, n);
        possible.push_back(start);
        start.erase(start.begin()+4);
    } while (next_combination(start));
    return possible;
}


// Updates the value of the tile node at (r,c). 
// Updates the level in neighboring cells. 
// Creates a new list of possible 1-positions if there are 1s remaining.
void simstate::place_tile(int r, int c, int v) {
    grid[r][c]->val = v;
    for (int rp = r-1; rp <= r+1; ++rp) {
        for (int cp = c-1; cp <= c+1; ++cp) {
            if (!(rp == r && cp == c)) {
                adjust(rp, cp, v);
                // std::cout << rp << " " << cp  << " : " << v << std::endl;
            }
        }
    }
    stone_used[v] = true;

    std::vector<node*> newly_acsble_1s = {};
    if (!rem_ones) {
        ones_list.push_back(newly_acsble_1s);
        return;
    }

    for (int rp = r-2; rp <= r+2; ++rp) {
        for (int cp = c-2; cp <= c+2; ++cp) {
            if (rp == r-2 || rp == r+2 || cp == c-2 || cp == c+2) {
                if (v) ++grid[rp][cp]->onesscore;
                else --grid[rp][cp]->onesscore;

                if (grid[rp][cp]->onesscore == 1) newly_acsble_1s.push_back(grid[rp][cp]);
            }
        }
    }
    ones_list.push_back(newly_acsble_1s);
}

void simstate::unplace_tile(int r, int c) {
    for (int rp = r+1; rp >= r-1; --rp) {
        for (int cp = c+1; cp >= c-1; --cp) {
            if (!(rp == r && cp == c)) {
                adjust(rp, cp, -grid[r][c]->val);
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
    stone_used[grid[r][c]->val] = false;
    grid[r][c]->val = 0;
    ones_list.pop_back();
}

// Always inserts at the head of list (becomes the node pointed to by unocc_w_val[i]->next)
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

// TODO: UNDO PLACING CLUSTERS
// Handles placing clusters
void simstate::place_cluster(int r, int c, int v, int index) {
    std::vector<int> cluster = startcluster_sets[v][index];
    int i = 0;
    for (int rp=0; rp<3; ++rp) {
        for (int cp=0; cp<3; ++cp) {
            grid[r+rp][c+cp]->val = cluster[i];
            // Update levels
            for (int dr=-1; dr<=1; ++dr) {
                for (int dc=-1; dc<=1; ++dc) {
                    if (dr || dc) {
                        adjust(r+rp+dr, c+cp+dc, cluster[i]);
                    }
                }
            }
            ++i;
        }
    }
    stone_used[v] = true;
    rem_ones -= v;
}

void simstate::unplace_cluster(int r, int c) {
    int v = grid[r+1][c+1]->val;
    for (int rp=2; rp>=0; --rp) {
        for (int cp=2; cp>=0; --cp) {
            
            // Update levels
            for (int dr=1; dr>=-1; --dr) {
                for (int dc=1; dc>=-1; --dc) {
                    if (dr || dc) {
                        adjust(r+rp+dr, c+cp+dc, -grid[r+rp][c+cp]->val);
                    }
                }
            }
            grid[r+rp][c+cp]->val = 0;
        }
    }
    stone_used[v] = false;
    rem_ones += v;
}



// printing for visualization
void simstate::write_state(int dest, bool app=false, int mode=MODE_VAL, std::vector<int> bounds={}) {
    std::string path = dest == TO_LOG ? log_path : result_path;
    std::ofstream of;
    if (app) of.open(path, std::ios::out | std::ios::app);
    else of.open(path);
    if (bounds.empty()) bounds = {0, grid_sz, 0, grid_sz};

    int digits = ceil(log10(maxv))+1;
    std::string horizontal_border = std::string((size_t)(digits+1)*(bounds[3]-bounds[2])+1, '_');

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

void simstate::write_unocc(int dest, int lvl, bool app=false) {
    std::string path = dest == TO_LOG ? log_path : result_path;std::ofstream of;
    if (app) of.open(path, std::ios::out | std::ios::app);
    else of.open(path);

    of << "LEVEL = " << lvl << ": ";
    node* cur = unocc_w_val[lvl]->next;
    while (cur) {
        // assert((cur->level == lvl));
        of << "(" << cur->r << ", " << cur->c << ") ";
        cur = cur->next;
    }
    of << '\n';
    of.close();
}

void simstate::write_stones_used(int dest, int mxm, bool app=false) {
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

void simstate::write_state_trimmed(int dest, bool app=false, int mode=MODE_VAL) {
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
    std::cerr << u << d << l << r << std::endl;
    write_state(dest, app, mode, {u,d,l,r});
}

void simstate::save_maximum() {
    maxv = cur_stone-1;
    write_state_trimmed(TO_RESULT, true);
    std::ofstream of;
    of.open(result_path, std::ios::app);
    of << "CURRENT BEST\n";
    of.close();
}


// ================== main loops

void simstate::try_place_stone() {
    // write_state(true);
    write_stones_used(TO_LOG, 50, true);
    int old_cur_stone = cur_stone;
    while(stone_used[cur_stone]) ++cur_stone;
    node* cur = unocc_w_val[cur_stone]->next;
    
    if (!cur) {
        if (cur_stone-1 >= maxv) save_maximum();
        cur_stone = old_cur_stone;
        return;
    }

    // write_unocc(cur_stone, true);
    
    while (cur) {
        if (!cur->val) {
            place_tile(cur->r, cur->c, cur_stone);
            step();
            unplace_tile(cur->r, cur->c);
        } 
        cur = cur -> next;
    }
    cur_stone = old_cur_stone;
}


void simstate::try_place_ones() {
    int n_options = ones_list.back().size();
    // if (!rem_ones || !n_options) return;

    for (int cnt=1; cnt<=rem_ones; ++cnt) {
        std::vector<int> choice_arr = std::vector<int>(n_options, 0);
        for (int i=0; i<cnt; ++i) choice_arr[i] = 1;
        do {
            // place_ones(choice_arr);
            step();
            // unplace_ones(choice_arr);
        } while (next_combination(choice_arr));
    }
}

void simstate::try_place_cluster(int r, int c, int v) {
    for (int i=0; i<
    startcluster_sets[v].size()
    ; ++i) {
        place_cluster(r, c, v, i);
        step();
        unplace_cluster(r, c);
    }
}


void simstate::step() {
    // A FEW OPTIONS
    // -- place tile cur
    // -- if 'ones > 0', place isolated 1 in the fresh outer border
    //if (rem_ones && ones_list.back().size()) try_place_ones();
    try_place_stone();
}



// ===== SOME TEST CASES

void test() {
    std::string out = "testlogs/test";
    int n_pebbles = 2;
    int rad = ceil(log2f64(174*n_pebbles));

    // printf("%u\n", rad);

    int grid_sz = (2*rad + 1)*n_pebbles *2 +1;

    simstate state = simstate(10, n_pebbles, out);
    state.place_cluster(3, 3, 2, 6);
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
    state.write_state(TO_LOG);
    state.write_state(TO_LOG, true, MODE_PRES);
    state.place_tile(4,3,3);
    state.write_state(TO_LOG, true);
    state.write_state(TO_LOG, true, MODE_PRES);
    state.unplace_tile(4,3);
    state.write_state(TO_LOG, true);
    state.write_state(TO_LOG, true, MODE_PRES);
    state.unplace_cluster(3,3);
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
    state.try_place_cluster(10,10,2);
}


int main() {
    test();
    cluster_test();
    simple_loop();
}