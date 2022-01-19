// #include <cstdio>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_map>


struct node {
    int val = 0;
    int pressure = 0;
    int onesscore = 0;
    node* prev;
    node* next;

    int r,c;

    node(int r, int c) {this->r = r; this->c = c;}
};


struct simstate {
    int grid_sz;
    int maxv = 1;
    // std::vector<std::vector<int>> grid;
    // std::vector<std::vector<int>> newly_accessible_1s;
    // std::unordered_map<int, std::vector<std::pair<int,int>>> unocc_w_val;

    int rem_ones;
    std::vector<std::vector<node*>> grid;
    std::vector<node*> newly_acsble_1s;
    std::vector<node*> unocc_w_val;
    std::vector<std::vector<std::vector<int>>> startcluster_sets = 
        std::vector<std::vector<std::vector<int>>>(9);

    simstate(int grid_sz, int rem_ones);
    ~simstate();

    std::vector<std::vector<int>> init_startcluster(int n);

    void place_tile(int r, int c, int v);
    void adjust(int r, int c, int v);
    void place_cluster(int r, int c, int v, int index);

    void print_state(std::string fn);
};


simstate::simstate(int grid_sz, int rem_ones) {
    this->grid_sz = grid_sz;
    this->rem_ones = rem_ones;

    grid = std::vector<std::vector<node*>>(grid_sz, std::vector<node*>(grid_sz));
    for (int r=0; r<grid_sz; ++r) {
        for (int c=0; c<grid_sz; ++c) {
            grid[r][c] = new node(r, c);
        }
    }

    for (int i=2; i<9; ++i) {
        startcluster_sets[i] = init_startcluster(i);
    }
}

simstate::~simstate() {
    for (auto r: grid) for (node* n: r) delete n;
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
// Updates the pressure in neighboring cells. 
// Creates a new list of possible 1-positions if there are 1s remaining.
void simstate::place_tile(int r, int c, int v) {
    int diff = v - grid[r][c]->val;
    grid[r][c]->val = v;
    for (int rp = r-1; rp <= r+1; ++rp) {
        for (int cp = c-1; cp <= c+1; ++cp) {
            if (!(rp == r && cp == c) && !grid[rp][cp]->val) {
                adjust(rp, cp, grid[rp][cp]->val + diff);
            }
        }
    }

    newly_acsble_1s = {};
    if (!rem_ones) return;

    for (int rp = r-2; rp <= r+2; ++rp) {
        for (int cp = c-2; cp <= c+2; ++cp) {
            if (rp == r-2 || rp == r+2 || cp == c-2 || cp == c+2) {
                if (v) ++grid[rp][cp]->onesscore;
                else --grid[rp][cp]->onesscore;

                if (grid[rp][cp]->onesscore == 1) newly_acsble_1s.push_back(grid[rp][cp]);
            }
        }
    }
}

// Always inserts at the head of list (becomes the node pointed to by unocc_w_val[i]->next)
void simstate::adjust(int r, int c, int v) {
    if (grid[r][c]->next) {
        grid[r][c]->prev->next = grid[r][c]->next;
        grid[r][c]->next->prev = grid[r][c]->prev;
    } else {
        grid[r][c]->prev->next = nullptr;
    }

    grid[r][c]->next = unocc_w_val[v]->next;
    if (grid[r][c]->next) grid[r][c]->next->prev = grid[r][c];
    unocc_w_val[v]-> next = grid[r][c];
    grid[r][c]->prev = unocc_w_val[v];
    grid[r][c]->val = v;
}





// printing for visualization
void simstate::print_state(std::string fn) {
    std::ofstream outfile;
    outfile.open(fn);
    int digits = ceil(log10(maxv))+1;
    outfile << std::setw(digits);

    std::string horizontal_border = std::string((size_t)(digits+1)*grid_sz+1, '_');

    outfile << horizontal_border << '\n';
    for (int r=0; r<grid_sz; ++r) {
        outfile << '|';
        for (int c=0; c<grid_sz; ++c) {
            outfile << grid[r][c]->val << '|';
        }
        outfile << '\n' << horizontal_border << '\n';
    }
}



// ==================





void take_a_step(int cur, int ones) {
    // A FEW OPTIONS
    // -- place tile cur
    // -- if 'ones > 0', place isolated 1 in the fresh outer border

}

// void take_step_cluster_available(int &highscore, std::vector<std::) {

// }


void simstate::place_cluster(int r, int c, int v, int index) {
    std::vector<int> cluster = startcluster_sets[v][index];
    int i = 0;
    for (int rp=0; rp<3; ++rp) {
        for (int cp=0; cp<3; ++cp) {
            grid[r+rp][c+cp]->val = cluster[i];
            ++i;
        }
    }
}





int main() {
    std::string out = "out.txt";
    int n_pebbles = 2;
    int rad = ceil(log2f64(174*n_pebbles));

    // printf("%u\n", rad);


    int grid_sz = (2*rad + 1)*n_pebbles *2 +1;

    std::vector<std::vector<std::vector<int>>> startcluster_sets = std::vector<std::vector<std::vector<int>>>(9);
    // for (int i=2; i<9; ++i) {
    //     startcluster_sets[i] = iter_startcluster(i);
    // }
    // for (auto all: startcluster_sets) if (all.size()) for (auto elt: all) {
    //     int i=0;
    //     for (int r=0; r<3; ++r) {
    //         for (int c=0; c<3; ++c) {
    //             if (elt[i]) std::cout << elt[i];
    //             else std::cout << ".";
    //             ++i;
    //         } 
    //         // std::cout << std::endl;
    //     }std::cout << std::endl;
    // }


    //testing 
    simstate state = simstate(10, n_pebbles);
    state.place_cluster(3, 3, 2, 0);
    state.print_state(out);


    return 0;
}