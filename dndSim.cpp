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
        return ( roll1 > roll2 ) ? roll1 : roll2;
    }

    unsigned short int roll2d20dh(){
        unsigned short int roll1 = roll1d20();
        unsigned short int roll2 = roll1d20();
        return ( roll1 < roll2 ) ? roll1 : roll2;
    }

void character::setStats(std::vector<unsigned short int> stats = {10,10,10,10,10,10}) {
    for( unsigned short int i = 0; i < stats.size(); i++ ){
        this->stats[statNames[i]] = stats[i];
    }
}

void character::setSaves(std::vector<std::string> saveNames) {
    for( auto saveName : statNames ){
        this->saves[saveName] = ( stats[saveName] / 2 ) - 5; 
    }
    for( auto saveName : saveNames ){
        this->saves[saveName] += profBonus;
    }
}

void character::setAC(unsigned short int baseAc, bool includeDex) {
    this->ac = baseAc + unsigned( includeDex ) * (( stats["dex"] / 2 ) - 5);
}

void character::setProcBonus() {
    this->profBonus = ( lvlCR / 4 ) + 2;
}

void character::setSaveDC() {
    this->saveDC = 8 + (stats[atkStat] / 2) - 5 + profBonus;
}

character::character() {
    this->idName = "character";
    this->lvlCR = 1;
    setStats();
    setProcBonus();
    this->atkStat = "str";
    setSaves();
    setAC();
    this->causeSave = false;
    setSaveDC();
}

character::character(std::string idName, unsigned short int lvlCR, std::vector<unsigned short int> inputStats, bool causeSave, std::vector<std::string> saveNames, std::string atkStat, unsigned short int baseAc, bool includeDex) {
    this->idName = idName;
    this->lvlCR = lvlCR;
    setStats( inputStats );
    setProcBonus();
    this->atkStat = atkStat;
    setSaves( saveNames );
    setAC( baseAc, includeDex );
    this->causeSave = causeSave;
    setSaveDC();
}

character::character(std::string idName, unsigned short int lvlCR, std::map<std::string,unsigned short int> inputStats, std::vector<std::string> saveNames, bool causeSave, std::string atkStat, unsigned short int baseAc, bool includeDex) {
    this->idName = idName;
    this->lvlCR = lvlCR;
    this->stats = inputStats;
    setProcBonus();
    this->atkStat = atkStat;
    setSaves( saveNames );
    setAC( baseAc, includeDex );
    this->causeSave = causeSave;
    setSaveDC();
}

std::string character::getName() {
    return this->idName;
}

unsigned short int character::getLvl() {
    return this->lvlCR;
}

std::map<std::string, unsigned short int> character::getStats() {
    return this->stats;
}

std::map<std::string, unsigned short int> character::getSaves() {
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
        return enemy.save( atkStat, saveDC );
    } else {
        return (roll1d20() + stats[atkStat] + profBonus >= enemy.getAC());
    }
    return false;
}

bool character::attack(std::shared_ptr<character> enemy) {
    return attack(*enemy);
}

bool character::save(std::string saveStat, unsigned short int saveDC) {
    return (roll1d20() + stats[saveStat] >= saveDC);
}

void barbarian::initializeLvlStats() {
    lvlStats = {
        {16,14,14,8,12,10}, {18,14,14,8,12,10}, {18,14,16,8,12,10},
        {20,14,16,8,12,10}, {20,14,18,8,12,10}, {20,14,20,8,12,10}, {20,14,20,8,12,10}
    };
}
barbarian::barbarian() : character("barbarian", 1, {16,14,14,8,12,10}, false, {{"str","con"}}, "str", 10, true), rage(2) {
    initializeLvlStats();
    setStats(lvlStats[0]);
    setSaves({"str", "con"});
}
barbarian::barbarian(unsigned short int lvlCR, std::vector<unsigned short int> stats) : character("barbarian", lvlCR, stats, false, {{"str","con"}}, "str", 10, true), rage(2) {
    initializeLvlStats();
    rage += unsigned(lvlCR > 8) + unsigned(lvlCR > 15);
    setStats(lvlStats[unsigned(lvlCR) / 4 + unsigned(lvlCR > 18)]);
    setSaves({"str", "con"});
}
barbarian::barbarian(int lvlCR, std::vector<unsigned short int> stats) : character("barbarian", lvlCR, stats, false, {{"str","con"}}, "str", 10, true), rage(2) {
    initializeLvlStats();
    rage += unsigned(lvlCR > 8) + unsigned(lvlCR > 15);
    setStats(lvlStats[unsigned(lvlCR) / 4 + unsigned(lvlCR > 18)]);
    setSaves({"str", "con"});
}
void barbarian::setAC(unsigned short int baseAc, bool includeDex) {
    this->ac = baseAc + ((stats["dex"] / 2) - 5) + ((stats["con"] / 2) - 5);
}
bool barbarian::attack(character& enemy) {
    if (lvlCR == 1) {
        return (roll1d20() + stats["str"] + profBonus + rage >= enemy.getAC());
    } else {
        return (roll2d20dl() + stats["str"] + profBonus + rage >= enemy.getAC());
    }
}
bool barbarian::attack(std::shared_ptr<character> enemy) {
    return attack(*enemy);
}

void cleric::initializeLvlStats() {
    lvlStats = {
        {10,14,12,8,16,14}, {10,14,12,8,18,14}, {10,14,12,8,20,14},
        {10,16,12,8,20,14}, {10,16,12,8,20,14}, {10,16,12,8,20,14}, {10,16,12,8,20,14}
    };
}
cleric::cleric() : character("cleric", 1, {10,14,12,8,16,14}, true, {{"wis","cha"}}, "wis", 10, true), mediumArmorMaster(false) {
    initializeLvlStats();
    setStats(lvlStats[0]);
    setSaves({"wis","cha"});
    setSaveDC();
    setAC();
}
cleric::cleric(unsigned short int lvlCR, std::vector<unsigned short int> stats) : character("cleric", lvlCR, stats, true, {{"wis","cha"}}, "wis", 10, true), mediumArmorMaster(false) {
    initializeLvlStats();
    setStats(lvlStats[unsigned(lvlCR / 4) + unsigned(lvlCR > 18)]);
    mediumArmorMaster = lvlCR > 15;
    setSaves({"wis","cha"});
    setSaveDC();
    setAC();
}
cleric::cleric(int lvlCR, std::vector<unsigned short int> stats) : character("cleric", lvlCR, stats, true, {{"wis","cha"}}, "wis", 10, true), mediumArmorMaster(false) {
    initializeLvlStats();
    setStats(lvlStats[unsigned(lvlCR / 4) + unsigned(lvlCR > 18)]);
    mediumArmorMaster = lvlCR > 15;
    setSaves({"wis","cha"});
    setSaveDC();
    setAC();
}

void cleric::setAC(unsigned short int baseAc, bool includeDex) {
    this->ac = baseAc + std::min(((stats["dex"] / 2) - 5), 2 + int(mediumArmorMaster));
}

void rogue::initializeLvlStats() {
    lvlStats = {
        {8,16,12,14,14,10}, {8,18,12,14,14,10}, {8,20,12,14,14,10}, 
        {8,20,12,14,14,10}, {8,20,12,14,14,10}, {8,20,12,14,14,10}, {8,20,12,14,14,10}
    };
}
rogue::rogue() : character("rogue", 1, {8,16,12,14,14,10}, false, {{"dex","int"}}, "dex", 11, true) {
    initializeLvlStats();
    setStats(lvlStats[0]);
    setAC(11);
}
rogue::rogue(unsigned short int lvlCR, std::vector<unsigned short int> stats) : character("rogue", lvlCR, stats, false, {{"dex","int"}}, "dex", 11, true) {
    initializeLvlStats();
    setStats(lvlStats[unsigned(lvlCR / 4) + unsigned(lvlCR > 18)]);
    setAC(11 + unsigned(lvlCR > 2));
    if(lvlCR > 14) setSaves({"dex","int","wis"});
}
rogue::rogue(int lvlCR, std::vector<unsigned short int> stats) : character("rogue", lvlCR, stats, false, {{"dex","int"}}, "dex", 11, true) {
    initializeLvlStats();
    setStats(lvlStats[unsigned(lvlCR / 4) + unsigned(lvlCR > 18)]);
    setAC(11 + unsigned(lvlCR > 2));
    if(lvlCR > 14) setSaves({"dex","int","wis"});
}

void wizard::initializeLvlStats() {
    lvlStats = {
        {8,14,10,16,14,12}, {8,14,10,18,14,12}, {8,14,10,20,14,12},
        {8,16,10,20,14,12}, {8,18,10,20,14,12}, {8,20,10,20,14,12}, {8,20,10,20,14,12}
    };
}
wizard::wizard() : character("wizard", 1, {8,14,10,16,14,12}, false, {{"int","wis"}}, "int", 10, true) {
    initializeLvlStats();
    setStats(lvlStats[0]);
    setSaves({"int", "wis"});
    setSaveDC();
    setAC(10);
}
wizard::wizard(unsigned short int lvlCR, std::vector<unsigned short int> stats) : character("wizard", lvlCR, stats, false, {{"int", "wis"}}, "int", 10, true) {
    initializeLvlStats();
    setStats(lvlStats[(unsigned short)(lvlCR / 4) + (unsigned short)(lvlCR > 18)]);
    setSaves({"int", "wis"});
    setSaveDC();
    setAC(10 + 3 * unsigned(lvlCR > 1));
}
wizard::wizard(int lvlCR, std::vector<unsigned short int> stats) : character("wizard", lvlCR, stats, false, {{"int", "wis"}}, "int", 10, true) {
    initializeLvlStats();
    setStats(lvlStats[(unsigned short)(lvlCR / 4) + (unsigned short)(lvlCR > 18)]);
    setSaves({"int", "wis"});
    setSaveDC();
    setAC(10 + 3 * unsigned(lvlCR > 1));
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
        auto nr = dndSim::genRNG(monsters[lvlCR].size());
        return monsters[lvlCR][nr];
    }

    std::shared_ptr<dndSim::npc> random_encounter_spellcaster( unsigned short int lvlCR ){
        auto nr = dndSim::genRNG(spell_monsters[lvlCR].size());
        return spell_monsters[lvlCR][nr];
    }

    std::shared_ptr<dndSim::npc> random_encounter_regular( unsigned short int lvlCR ){
        auto nr = dndSim::genRNG(non_spell_monsters[lvlCR].size());
        return non_spell_monsters[lvlCR][nr];
    }

    std::shared_ptr<dndSim::npc> random_encounter( int lvlCR, std::string type ){
        if (lvlCR < 1 || lvlCR > 20) throw std::invalid_argument("Currently only CRs of integers 1 through 20 are implemented.");
        if (type != "any" || type != "spellcaster" || type != "regular" ) throw std::invalid_argument("Enemy type must be 'any', 'spellcaster', or 'regular'.");
        if (type == "any") return random_encounter_any(lvlCR);
        if (type == "spellcaster") return random_encounter_spellcaster(lvlCR);
        if (type == "regular") return random_encounter_regular(lvlCR);
        return nullptr;
    }

}