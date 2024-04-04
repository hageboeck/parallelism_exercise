//==============================================================================
//   _____ ___ ______      ______  _____ ________  ___
//  |_   _/ _ \|  _  \___  |  _  \/  ___|_   _|  \/  |
//    | |/ /_\ \ | | ( _ ) | | | |\ `--.  | | | .  . |
//    | ||  _  | | | / _ \/\ | | | `--. \ | | | |\/| |
//    | || | | | |/ / (_>  < |/ / /\__/ /_| |_| |  | |
//    \_/\_| |_/___/ \___/\/___/  \____/ \___/\_|  |_/
//                                                    
//==============================================================================
// TOTALLY ACCURATE D&D SIMULATOR
// A testing suite for simulating D&D combat totally accurately.
//==============================================================================
// Copyright (C) 2024 CERN
// Licensed under the GNU Lesser General Public License (version 3 or later).
// Written by: Z. Wettersten (Mar 2024) for iCSC 2024.
//==============================================================================

#include "dndSim.h"
#include <iostream>
#include <numeric>
#include <chrono>

void usage(){
    std::cout << "Welcome to the TAD&DSIM test suite!" << std::endl;
    std::cout << "This program tests the balance of our random encounters." << std::endl;
    std::cout << "Usage: ./testSuite [int n], where n is the number of battles you want to test per character level." << std::endl;
    std::cout << "Any arguments besides n will be ignored at runtime." << std::endl;
    std::cout << "Have fun!" << std::endl;
}

std::vector<unsigned short int> test_levels = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

std::vector<std::vector<std::vector<bool>>>& initializeHitVector(const int& n) {
    const int dim1 = 20;
    const int dim2 = 20;

    std::vector<std::vector<std::vector<bool>>> hitVector;

    // Resize the outer vector to have 'n' 2D vectors
    hitVector.resize(dim1);

    for (int i = 0; i < dim1; ++i) {
        // Resize each 2D vector to have 20 rows
        hitVector[i].resize(dim2);
        for (int j = 0; j < n; ++j) {
            // Resize each row to have 20 columns initialized to 'false'
            hitVector[i][j].resize(n, false);
        }
    }
    return hitVector;
}

int main(int argc, char* argv[]){
    if (argc < 2){
        usage();
        return 1;
    }
    int n = std::stoi(std::string(argv[1]));
    if (n < 1){
        usage();
        return 1;
    }
    
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    std::cout << "Testing dndSim..." << std::endl;

    auto t1 = high_resolution_clock::now();

    auto barbarian_hits = initializeHitVector(n);
    auto cleric_hits = initializeHitVector(n);
    auto rogue_hits = initializeHitVector(n);
    auto wizard_hits = initializeHitVector(n);

    for( size_t k = 0; k < n ; ++k){
        for ( auto lvlNPC : test_levels ){
            for( auto lvlPC : test_levels ){
                auto npc = dndSim::random_encounter(lvlNPC);
                barbarian_hits[lvlNPC-1][lvlPC-1][k] = dndSim::barbarian_premade[lvlPC].attack(npc);
                cleric_hits[lvlNPC-1][lvlPC-1][k] = dndSim::cleric_premade[lvlPC].attack(npc);
                rogue_hits[lvlNPC-1][lvlPC-1][k] = dndSim::rogue_premade[lvlPC].attack(npc);
                wizard_hits[lvlNPC-1][lvlPC-1][k] = dndSim::wizard_premade[lvlPC].attack(npc);
            }
        }
    }

    float barbarian_hit_rate[20][20];
    float cleric_hit_rate[20][20];
    float rogue_hit_rate[20][20];
    float wizard_hit_rate[20][20];

    for (auto lvlPC : test_levels){
        for(auto lvlNPC : test_levels){
            barbarian_hit_rate[lvlPC-1][lvlNPC-1] = static_cast<float>(std::accumulate(
                barbarian_hits[lvlNPC-1][lvlPC-1].begin(), barbarian_hits[lvlNPC-1][lvlPC-1].end(), 0))
                 / static_cast<float>(n);
            cleric_hit_rate[lvlPC-1][lvlNPC-1] = static_cast<float>(std::accumulate(
                cleric_hits[lvlNPC-1][lvlPC-1].begin(), cleric_hits[lvlNPC-1][lvlPC-1].end(), 0))
                 / static_cast<float>(n);
            rogue_hit_rate[lvlPC-1][lvlNPC-1] = static_cast<float>(std::accumulate(
                rogue_hits[lvlNPC-1][lvlPC-1].begin(), rogue_hits[lvlNPC-1][lvlPC-1].end(), 0))
                 / static_cast<float>(n);
            wizard_hit_rate[lvlPC-1][lvlNPC-1] = static_cast<float>(std::accumulate(
                wizard_hits[lvlNPC-1][lvlPC-1].begin(), wizard_hits[lvlNPC-1][lvlPC-1].end(), 0))
                 / static_cast<float>(n);
        }
    }

    auto t2 = high_resolution_clock::now();

    duration<double, std::milli> ms_double = t2 - t1;

    std::cout << "Done testing dndSim for " << n << " points per character and level." << std::endl;
    std::cout << "Time taken: " << ms_double.count() << " ms" << std::endl;
    return 0;
}