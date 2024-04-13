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
// A library for simulating D&D combat totally accurately.
//==============================================================================
// Copyright (C) 2024 CERN
// Licensed under the GNU Lesser General Public License (version 3 or later).
// Written by: Z. Wettersten (Mar 2024) for iCSC 2024.
//==============================================================================

#include "dndSim.h"

namespace dndSim{

    unsigned int genRNG( unsigned int size ){
        static std::mt19937 generator;
        std::uniform_int_distribution<int> distribution(0,size-1);
        return distribution(generator);
    }

    unsigned short int roll1d20(){
        static std::mt19937 generator;
        std::uniform_int_distribution<int> distribution(1,20);
        return distribution(generator);
    }

    unsigned short int roll2d20dl(){
        unsigned short int roll1 = roll1d20();
        unsigned short int roll2 = roll1d20();
        return std::max(roll1,roll2);
    }

    unsigned short int roll2d20dh(){
        unsigned short int roll1 = roll1d20();
        unsigned short int roll2 = roll1d20();
        return std::min(roll1,roll2);
    }

void character::setStats(std::vector<unsigned short int> statVec = {10,10,10,10,10,10}) {
    this->stats = {10,10,10,10,10,10};
    for( unsigned short int i = 0; i < stats.size(); i++ ){
        this->stats[i] = statVec[i];
    }
}

void character::setSaves(std::vector<unsigned short int> saveNames) {
    this->saves = {0,0,0,0,0,0};
    for( unsigned short int k = 0 ; k < 6 ; ++k ){
        this->saves[k] = ( stats[k] / 2 ) - 5; 
    }
    for( auto saveName : saveNames ){
        this->saves[saveName] += profBonus;
    }
}

void character::setAC(unsigned short int baseAc, bool includeDex) {
    this->ac = baseAc + unsigned( includeDex ) * (( stats[1] / 2 ) - 5);
}

void character::setProcBonus() {
    this->profBonus = ( lvlCR / 4 ) + 2;
}

void character::setSaveDC() {
    this->saveDC = 8 + this->atkBonus + this->profBonus;
}

character::character() {
    this->lvlCR = 1;
    setStats();
    setProcBonus();
    this->atkStat = 0;
    setSaves();
    setAC();
    this->causeSave = false;
    setSaveDC();
}

character::character(unsigned short int lvlCR, std::vector<unsigned short int> inputStats, bool causeSave, std::vector<unsigned short int> saveNames, unsigned short int atkStat, unsigned short int baseAc, bool includeDex) {
    this->lvlCR = lvlCR;
    setStats( inputStats );
    setProcBonus();
    this->atkStat = atkStat;
    setSaves( saveNames );
    setAC( baseAc, includeDex );
    setAtkBonus();
    this->causeSave = causeSave;
    setSaveDC();
}

unsigned short int character::getLvl() {
    return this->lvlCR;
}

std::vector<unsigned short int> character::getStats() {
    return this->stats;
}

std::vector<unsigned short int> character::getSaves() {
    return this->saves;
}

unsigned short int character::getProfBonus() {
    return this->profBonus;
}

unsigned short int character::getAC() {
    return this->ac;
}

bool character::attack(character& enemy) {
    if( this->causeSave ){
        return !(enemy.save( atkStat, saveDC ));
    } else {
        return (roll1d20() + this->atkBonus + this->profBonus >= enemy.getAC());
    }
    return false;
}

bool character::attack(std::shared_ptr<character> enemy) {
    if (!enemy) throw std::invalid_argument("Enemy is a nullptr.");
    return attack(*enemy);
}

bool character::save(unsigned short int saveStat, unsigned short int saveDC) {
    return (roll1d20() + saves[saveStat] >= saveDC);
}

void character::setAtkBonus() {
    this->atkBonus = (short)(stats[atkStat]/2) - 5;
}

void barbarian::initializeLvlStats() {
    lvlStats = {
        {16,14,14,8,12,10}, {18,14,14,8,12,10}, {18,14,16,8,12,10},
        {20,14,16,8,12,10}, {20,14,18,8,12,10}, {20,14,20,8,12,10}, {20,14,20,8,12,10}
    };
}
barbarian::barbarian() : character(1, {16,14,14,8,12,10}, false, {0,2}, 0, 10, true), rage(2) {
    initializeLvlStats();
    setStats(lvlStats[0]);
    setProcBonus();
    setSaves({0,2});
    setAtkBonus();
}
barbarian::barbarian(unsigned short int lvlCR, std::vector<unsigned short int> stats) : character(lvlCR, stats, false, {0,2},0, 10, true), rage(2) {
    initializeLvlStats();
    rage += unsigned(lvlCR > 8) + unsigned(lvlCR > 15);
    setStats(lvlStats[unsigned(lvlCR) / 4 + unsigned(lvlCR > 18)]);
    setProcBonus();
    setSaves({0,2});
    setAtkBonus();
}
barbarian::barbarian(int lvlCR, std::vector<unsigned short int> stats) : character(lvlCR, stats, false, {0,2}, 0, 10, true), rage(2) {
    initializeLvlStats();
    rage += unsigned(lvlCR > 8) + unsigned(lvlCR > 15);
    setStats(lvlStats[unsigned(lvlCR) / 4 + unsigned(lvlCR > 18)]);
    setProcBonus();
    setSaves({0,2});
    setAtkBonus();
}
void barbarian::setAC(unsigned short int baseAc, bool includeDex) {
    this->ac = baseAc + (unsigned short)(includeDex)*((stats[1] / 2) - 5) + ((stats[2] / 2) - 5);
}
bool barbarian::attack(character& enemy) {
    if (this->lvlCR == 1) {
        return (roll1d20() + this->atkBonus + this->profBonus + this->rage >= enemy.getAC());
    } else {
        return (roll2d20dl() + this->atkBonus + this->profBonus + this->rage >= enemy.getAC());
    }
}
bool barbarian::attack(std::shared_ptr<character> enemy) {
    if (!enemy) throw std::invalid_argument("Enemy is a nullptr.");
    return attack(*enemy);
}

void cleric::initializeLvlStats() {
    lvlStats = {
        {10,14,12,8,16,14}, {10,14,12,8,18,14}, {10,14,12,8,20,14},
        {10,16,12,8,20,14}, {10,16,12,8,20,14}, {10,16,12,8,20,14}, {10,16,12,8,20,14}
    };
}
cleric::cleric() : character(1, {10,14,12,8,16,14}, true, {4,5}, 4, 10, true), mediumArmorMaster(false) {
    initializeLvlStats();
    setStats(lvlStats[0]);
    setSaves({4,5});
    setProcBonus();
    setAtkBonus();
    setSaveDC();
    setAC();
}
cleric::cleric(unsigned short int lvlCR, std::vector<unsigned short int> stats) : character(lvlCR, stats, true, {4,5}, 4, 10, true), mediumArmorMaster(false) {
    initializeLvlStats();
    setStats(lvlStats[unsigned(lvlCR / 4) + unsigned(lvlCR > 18)]);
    mediumArmorMaster = lvlCR > 15;
    setProcBonus();
    setAtkBonus();
    setSaves({4,5});
    setSaveDC();
    setAC();
}
cleric::cleric(int lvlCR, std::vector<unsigned short int> stats) : character(lvlCR, stats, true, {4,5}, 4, 10, true), mediumArmorMaster(false) {
    initializeLvlStats();
    setStats(lvlStats[unsigned(lvlCR / 4) + unsigned(lvlCR > 18)]);
    mediumArmorMaster = lvlCR > 15;
    setProcBonus();
    setAtkBonus();
    setSaves({4,5});
    setSaveDC();
    setAC();
}
bool cleric::attack(character& enemy) {
    return !(enemy.save(4, saveDC));
}
bool cleric::attack(std::shared_ptr<character> enemy) {
    if (!enemy) throw std::invalid_argument("Enemy is a nullptr.");
    return attack(*enemy);
}
void cleric::setAC(unsigned short int baseAc, bool includeDex) {
    this->ac = baseAc + (unsigned short)(includeDex)*std::min(((stats[1] / 2) - 5), 2 + int(mediumArmorMaster));
}

void rogue::initializeLvlStats() {
    lvlStats = {
        {8,16,12,14,14,10}, {8,18,12,14,14,10}, {8,20,12,14,14,10}, 
        {8,20,12,14,14,10}, {8,20,12,14,14,10}, {8,20,12,14,14,10}, {8,20,12,14,14,10}
    };
}
rogue::rogue() : character(1, {8,16,12,14,14,10}, false, {1,3}, 1, 11, true) {
    initializeLvlStats();
    setStats(lvlStats[0]);
    setAC(11);
}
rogue::rogue(unsigned short int lvlCR, std::vector<unsigned short int> stats) : character(lvlCR, stats, false, {1,3}, 1, 11, true) {
    initializeLvlStats();
    setStats(lvlStats[unsigned(lvlCR / 4) + unsigned(lvlCR > 18)]);
    setProcBonus();
    setAtkBonus();
    setAC(11 + unsigned(lvlCR > 2));
    setSaves({1,3});
    if(lvlCR > 14) setSaves({1,3,4});
}
rogue::rogue(int lvlCR, std::vector<unsigned short int> stats) : character(lvlCR, stats, false, {1,3}, 1, 11, true) {
    initializeLvlStats();
    setStats(lvlStats[unsigned(lvlCR / 4) + unsigned(lvlCR > 18)]);
    setProcBonus();
    setAtkBonus();
    setAC(11 + unsigned(lvlCR > 2));
    setSaves({1,3});
    if(lvlCR > 14) setSaves({1,3,4});
}
bool rogue::attack(character& enemy) {
    return (roll1d20() + atkBonus + profBonus >= enemy.getAC());
}
bool rogue::attack(std::shared_ptr<character> enemy) {
    if (!enemy) throw std::invalid_argument("Enemy is a nullptr.");
    return attack(*enemy);
}

void wizard::initializeLvlStats() {
    lvlStats = {
        {8,14,10,16,14,12}, {8,14,10,18,14,12}, {8,14,10,20,14,12},
        {8,16,10,20,14,12}, {8,18,10,20,14,12}, {8,20,10,20,14,12}, {8,20,10,20,14,12}
    };
}
wizard::wizard() : character(1, {8,14,10,16,14,12}, false, {3,4}, 3, 10, true) {
    initializeLvlStats();
    setStats(lvlStats[0]);
    setProcBonus();
    setAtkBonus();
    setSaves({3,4});
    setSaveDC();
    setAC(10);
}
wizard::wizard(unsigned short int lvlCR, std::vector<unsigned short int> stats) : character(lvlCR, stats, false, {3,4}, 3, 10, true) {
    initializeLvlStats();
    setStats(lvlStats[(unsigned short)(lvlCR / 4) + (unsigned short)(lvlCR > 18)]);
    setProcBonus();
    setAtkBonus();
    setSaves({3,4});
    setSaveDC();
    setAC(10 + 3 * unsigned(lvlCR > 1));
}
wizard::wizard(int lvlCR, std::vector<unsigned short int> stats) : character(lvlCR, stats, false, {3,4}, 3, 10, true) {
    initializeLvlStats();
    setStats(lvlStats[(unsigned short)(lvlCR / 4) + (unsigned short)(lvlCR > 18)]);
    setProcBonus();
    setAtkBonus();
    setSaves({3,4});
    setSaveDC();
    setAC(10 + 3 * unsigned(lvlCR > 1));
}
bool wizard::attack(character& enemy) {
    return (roll1d20() + atkBonus + profBonus >= enemy.getAC());
}
bool wizard::attack(std::shared_ptr<character> enemy) {
    if (!enemy) throw std::invalid_argument("Enemy is a nullptr.");
    return attack(*enemy);
}

    std::map<unsigned short int, barbarian> barbarian_premade = {
        {1, barbarian(1)}, {2, barbarian(2)}, {3, barbarian(3)}, {4, barbarian(4)},
        {5, barbarian(5)}, {6, barbarian(6)}, {7, barbarian(7)}, {8, barbarian(8)},
        {9, barbarian(9)}, {10, barbarian(10)}, {11, barbarian(11)}, {12, barbarian(12)},
        {13, barbarian(13)}, {14, barbarian(14)}, {15, barbarian(15)}, {16, barbarian(16)},
        {17, barbarian(17)}, {18, barbarian(18)}, {19, barbarian(19)}, {20, barbarian(20)}
    };

    std::map<unsigned short int, cleric> cleric_premade = {
        {1, cleric(1)}, {2, cleric(2)}, {3, cleric(3)}, {4, cleric(4)},
        {5, cleric(5)}, {6, cleric(6)}, {7, cleric(7)}, {8, cleric(8)},
        {9, cleric(9)}, {10, cleric(10)}, {11, cleric(11)}, {12, cleric(12)},
        {13, cleric(13)}, {14, cleric(14)}, {15, cleric(15)}, {16, cleric(16)},
        {17, cleric(17)}, {18, cleric(18)}, {19, cleric(19)}, {20, cleric(20)}
    };

    std::map<unsigned short int, rogue> rogue_premade = {
        {1, rogue(1)}, {2, rogue(2)}, {3, rogue(3)}, {4, rogue(4)},
        {5, rogue(5)}, {6, rogue(6)}, {7, rogue(7)}, {8, rogue(8)},
        {9, rogue(9)}, {10, rogue(10)}, {11, rogue(11)}, {12, rogue(12)},
        {13, rogue(13)}, {14, rogue(14)}, {15, rogue(15)}, {16, rogue(16)},
        {17, rogue(17)}, {18, rogue(18)}, {19, rogue(19)}, {20, rogue(20)}
    };

    std::map<unsigned short int, wizard> wizard_premade = {
        {1, wizard(1)}, {2, wizard(2)}, {3, wizard(3)}, {4, wizard(4)},
        {5, wizard(5)}, {6, wizard(6)}, {7, wizard(7)}, {8, wizard(8)},
        {9, wizard(9)}, {10, wizard(10)}, {11, wizard(11)}, {12, wizard(12)},
        {13, wizard(13)}, {14, wizard(14)}, {15, wizard(15)}, {16, wizard(16)},
        {17, wizard(17)}, {18, wizard(18)}, {19, wizard(19)}, {20, wizard(20)}
    };

#include "all_monsters.txt"

    std::shared_ptr<dndSim::npc> random_encounter_any( unsigned short int lvlCR ){
        auto nr = dndSim::genRNG(monsters[lvlCR-1].size());
        return monsters[lvlCR-1][nr];
    }

    std::shared_ptr<dndSim::npc> random_encounter_spellcaster( unsigned short int lvlCR ){
        auto nr = dndSim::genRNG(spell_monsters[lvlCR-1].size());
        return spell_monsters[lvlCR-1][nr];
    }

    std::shared_ptr<dndSim::npc> random_encounter_regular( unsigned short int lvlCR ){
        auto nr = dndSim::genRNG(non_spell_monsters[lvlCR-1].size());
        return non_spell_monsters[lvlCR-1][nr];
    }

    std::shared_ptr<dndSim::npc> random_encounter( int lvlCR, std::string type ){
        if (lvlCR < 1 || lvlCR > 20) throw std::invalid_argument("Currently only CRs of integers 1 through 20 are implemented.");
        if (!(type == "any" || type == "spellcaster" || type == "regular" )) throw std::invalid_argument("Enemy type must be 'any', 'spellcaster', or 'regular'.");
        if (type == "any") return random_encounter_any(lvlCR);
        if (type == "spellcaster") return random_encounter_spellcaster(lvlCR);
        if (type == "regular") return random_encounter_regular(lvlCR);
        return nullptr;
    }

}