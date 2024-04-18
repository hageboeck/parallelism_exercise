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
#include <numeric>
#include <chrono>
#include <functional>
#include <fstream>

void usage(){
    std::cout << "Welcome to the TAD&DSIM test suite!" << std::endl;
    std::cout << "This program tests the balance of our random encounters." << std::endl;
    std::cout << "Usage: ./testSuite [int n], where n is the number of battles you want to test per character level." << std::endl;
    std::cout << "Any arguments besides n will be ignored at runtime." << std::endl;
    std::cout << "Have fun!" << std::endl;
}

const std::vector<unsigned short int> test_levels = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

std::shared_ptr<std::vector<std::vector<std::vector<bool>>>> initializeHitVector(int n) {
    const int dim1 = test_levels.size();
    const int dim2 = test_levels.size();
    auto hitVector = std::make_shared<std::vector<std::vector<std::vector<bool>>>>(dim1, std::vector<std::vector<bool>>(dim2, std::vector<bool>(n, false)));
    return hitVector;
}

void plotAsciiHeatmap(float data[20][20]) {
    for (int i = 19; i > 0; --i) {
        std::cout << i+1 << " ";
        if(i < 9) std::cout << " ";
        for (int j = 0; j < 20; ++j) {
            // Simple thresholding to represent different heat levels
            char heat = ' ';
            if (data[i][j] > 0.75) heat = '#';
            else if (data[i][j] > 0.5) heat = '+';
            else if (data[i][j] > 0.25) heat = '-';
            std::cout << heat << " ";
        }
        std::cout << std::endl;
    }
    for (int j = 0; j < 20; ++j) {
        std::cout << " " << j+1;
    }
    std::cout << std::endl;
}
// Simplify some of the main by adding an alias for the attack function
using attack = std::function<bool(unsigned short int, dndSim::npc const &)>;

int main(int argc, char* argv[]){
    if (argc < 2){
        usage();
        return 1;
    }
    std::string nArg = argv[1];
    int n = std::stoi(argv[1]);
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

    // Define the types of attacks of the pre-built characters
    attack barbarian = [](unsigned short int lvl, dndSim::npc const & npc){
        return dndSim::barbarian_premade[lvl].attack(npc);
    };
    attack cleric = [](unsigned short int lvl, dndSim::npc const& npc) {
        return dndSim::cleric_premade[lvl].attack(npc);
    };
    attack rogue = [](unsigned short int lvl, dndSim::npc const& npc) {
        return dndSim::rogue_premade[lvl].attack(npc);
    };
    attack wizard = [](unsigned short int lvl, dndSim::npc const& npc) {
        return dndSim::wizard_premade[lvl].attack(npc);
    };

    // Define the types of attacks against the pre-built characters
    attack barbarian_defend = [](unsigned short int lvl, dndSim::npc const& npc) {
        return dndSim::attack_barbarian(lvl, npc);
    };
    attack cleric_defend = [](unsigned short int lvl, dndSim::npc const& npc) {
        return dndSim::attack_cleric(lvl, npc);
    };
    attack rogue_defend = [](unsigned short int lvl, dndSim::npc const& npc) {
        return dndSim::attack_rogue(lvl, npc);
    };
    attack wizard_defend = [](unsigned short int lvl, dndSim::npc const& npc) {
        return dndSim::attack_wizard(lvl, npc);
    };

    // Initialize the hit vectors for each character type
    auto barbarian_hits = initializeHitVector(n);
    auto cleric_hits = initializeHitVector(n);
    auto rogue_hits = initializeHitVector(n);
    auto wizard_hits = initializeHitVector(n);
    auto barbarian_def = initializeHitVector(n);
    auto cleric_def = initializeHitVector(n);
    auto rogue_def = initializeHitVector(n);
    auto wizard_def = initializeHitVector(n);
    // Create a vector of the pre-built characters and their attack functions
    auto pre_builds = std::vector<attack>(4);
    pre_builds[0] = barbarian;
    pre_builds[1] = cleric;
    pre_builds[2] = rogue;
    pre_builds[3] = wizard;
    // Create a vector of the pre-built characters and their defend functions
    auto pre_builds_defend = std::vector<attack>(4);
    pre_builds_defend[0] = barbarian_defend;
    pre_builds_defend[1] = cleric_defend;
    pre_builds_defend[2] = rogue_defend;
    pre_builds_defend[3] = wizard_defend;
    // Create a vector of the hit vectors for each character class
    auto hits = std::vector<std::shared_ptr<std::vector<std::vector<std::vector<bool>>> > >(4);
    hits[0] = barbarian_hits;
    hits[1] = cleric_hits;
    hits[2] = rogue_hits;
    hits[3] = wizard_hits;
    auto def = std::vector<std::shared_ptr<std::vector<std::vector<std::vector<bool>>> > >(4);
    def[0] = barbarian_def;
    def[1] = cleric_def;
    def[2] = rogue_def;
    def[3] = wizard_def;

    // Run the simulation for each character class and level
    // The actual loop order is pretty irrelevant, so long as we get all the combinations
    // Just don't mess up the indices
    // The outer loop is for the number of battles to simulate
    // Couldn't this be done in parallel?
    // --- Not with this loop order, maybe if we moved the n-loop in?
    // I don't think our random number generator is vectorized, so we'd have to do it manually
    // ...Have you tried it?
    // ...No
    for( int k = 0; k < n ; ++k){
        // The next loop is for the enemy levels
        for ( auto lvlNPC : test_levels ){
            // The next loop is for the character classes
            // Maybe we could multithread here?
            // Well, multithreading here would only use up to four threads
            // Would it? The inner loop could be split across multiple threads
            // Do we need to do that manually?
            // Nah, I'm pretty sure OMP can handle that
            // But wait, if it's just unfolding the loop, couldn't we just multithread the entire loop?
            for ( auto l = 0 ; l < 4 ; ++l ){
                // The innermost loop is for the player character levels
                for( auto lvlPC : test_levels ){
                    auto const & npc = dndSim::random_encounter(lvlNPC, dndSim::EncType::any);
                    hits[l]->at(lvlNPC - 1)[lvlPC - 1][k] = pre_builds[l](lvlPC, npc);
                }
            }
        }
    }
    for( int k = 0; k < n ; ++k){
        for ( auto lvlNPC : test_levels ){
            for ( auto l = 0 ; l < 4 ; ++l ){
                for( auto lvlPC : test_levels ){
                    auto const & npc = dndSim::random_encounter(lvlNPC, dndSim::EncType::any);
                    def[l]->at(lvlNPC - 1)[lvlPC - 1][k] = pre_builds_defend[l](lvlPC, npc);
                }
            }
        }
    }

    // Initialise the actual hit rate matrices
    float barbarian_hit_rate[20][20];
    float cleric_hit_rate[20][20];
    float rogue_hit_rate[20][20];
    float wizard_hit_rate[20][20];
    std::vector<float(*)[20]> PC_hit_rate = {barbarian_hit_rate, cleric_hit_rate, rogue_hit_rate, wizard_hit_rate};

    // Initialize the hit rate matrices against each character
    float barbarian_def_rate[20][20];
    float cleric_def_rate[20][20];
    float rogue_def_rate[20][20];
    float wizard_def_rate[20][20];
    std::vector<float(*)[20]> NPC_hit_rate = {barbarian_def_rate, cleric_def_rate, rogue_def_rate, wizard_def_rate};


    // Calculate the hit rates
    // Here, the loop order is PC lvl > NPC lvl > PC class
    for (auto lvlPC : test_levels){
        for(auto lvlNPC : test_levels){
            for (int l = 0; l < 4; ++l){
                PC_hit_rate[l][lvlNPC-1][lvlPC-1] = static_cast<float>(std::accumulate(
                    hits[l]->at(lvlNPC-1)[lvlPC-1].begin(), hits[l]->at(lvlNPC-1)[lvlPC-1].end(), 0))
                     / static_cast<float>(n);
            }
        }
    }
    for (auto lvlPC : test_levels){
        for(auto lvlNPC : test_levels){
            for (int l = 0; l < 4; ++l){
                NPC_hit_rate[l][lvlNPC-1][lvlPC-1] = static_cast<float>(std::accumulate(
                    def[l]->at(lvlNPC-1)[lvlPC-1].begin(), def[l]->at(lvlNPC-1)[lvlPC-1].end(), 0))
                     / static_cast<float>(n);
            }
        }
    }

    auto t2 = high_resolution_clock::now();

    duration<double, std::milli> ms_double = t2 - t1;

    auto atk_filnames = std::vector<std::string>{"barbarian_NPC_hit_rate.csv", "cleric_NPC_hit_rate.csv", "rogue_NPC_hit_rate.csv", "wizard_NPC_hit_rate.csv"};
    auto def_filnames = std::vector<std::string>{"NPC_barbarian_hit_rate.csv", "NPC_cleric_hit_rate.csv", "NPC_rogue_hit_rate.csv", "NPC_wizard_hit_rate.csv"};

    // Export the hit rates to CSV files for plotting
    for (int l = 0; l < 4; ++l){
        std::ofstream file(atk_filnames[l]);
        for (int i = 0; i < 20; ++i){
            for (int j = 0; j < 20; ++j){
                file << PC_hit_rate[l][i][j] << ",";
            }
            file << std::endl;
        }
        file.close();
    }
    for (int l = 0; l < 4; ++l){
        std::ofstream file(def_filnames[l]);
        for (int i = 0; i < 20; ++i){
            for (int j = 0; j < 20; ++j){
                file << NPC_hit_rate[l][i][j] << ",";
            }
            file << std::endl;
        }
        file.close();
    }

    // std::cout << "BARBARIAN" << std::endl;
    // plotAsciiHeatmap(PC_hit_rate[0]);
    // std::cout << "CLERIC" << std::endl;
    // plotAsciiHeatmap(PC_hit_rate[1]);
    // std::cout << "ROGUE" << std::endl;
    // plotAsciiHeatmap(PC_hit_rate[2]);
    // std::cout << "WIZARD" << std::endl;
    // plotAsciiHeatmap(PC_hit_rate[3]);
    // std::cout << std::endl;

    std::cout << "Done testing dndSim for " << n << " points per character and level." << std::endl;
    std::cout << "Time taken: " << ms_double.count() << " ms" << std::endl;

    return 0;
}
