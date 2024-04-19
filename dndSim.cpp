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

unsigned short int character::getLvl() const
{
    return this->lvlCR;
}

std::vector<unsigned short int> character::getStats() const
{
    return this->stats;
}

std::vector<unsigned short int> character::getSaves() const
{
    return this->saves;
}

unsigned short int character::getProfBonus() const
{
    return this->profBonus;
}

unsigned short int character::getAC() const {
    return this->ac;
}

bool character::attack(character const& enemy, RNG::RNG_t& rng) const
{
    if( this->causeSave ){
        return !(enemy.save(atkStat, saveDC, rng));
    } else {
        return (RNG::roll1d20(rng) + this->atkBonus + this->profBonus >= enemy.getAC());
    }
    return false;
}

bool character::attack(barbarian const& enemy, RNG::RNG_t& rng)
{
    if( this->causeSave ){
        return !(enemy.save(atkStat, saveDC, rng));
    } else {
        if ( enemy.getLvl() == 1 )
            return (RNG::barb_roll1d20(rng) + this->atkBonus + this->profBonus >= enemy.getAC());
        else
            return (RNG::barb_roll2d20dl(rng) + this->atkBonus + this->profBonus >= enemy.getAC());
        }
    return false;
}

bool character::attack(cleric const& enemy, RNG::RNG_t& rng)
{
    if( this->causeSave ){
        return !(enemy.save(atkStat, saveDC, rng));
    } else {
        return (RNG::cler_roll1d20(rng) + this->atkBonus + this->profBonus >= enemy.getAC());
    }
    return false;
}

bool character::attack(rogue const& enemy, RNG::RNG_t& rng)
{
    if( this->causeSave ){
        return !(enemy.save(atkStat, saveDC, rng));
    } else {
        return (RNG::rog_roll1d20(rng) + this->atkBonus + this->profBonus >= enemy.getAC());
    }
    return false;
}

bool character::attack(wizard const& enemy, RNG::RNG_t& rng)
{
    if( this->causeSave ){
        return !(enemy.save(atkStat, saveDC, rng));
    } else {
        return (RNG::wiz_roll1d20(rng) + this->atkBonus + this->profBonus >= enemy.getAC());
    }
    return false;
}

bool character::save(unsigned short int saveStat, unsigned short int saveDC, RNG::RNG_t& rng) const
{
    return (RNG::roll1d20(rng) + saves[saveStat] >= saveDC);
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
bool barbarian::attack(character const& enemy, RNG::RNG_t& rng) const
{
    if (this->lvlCR == 1) {
        return (RNG::barb_roll1d20(rng) + this->atkBonus + this->profBonus + this->rage >= enemy.getAC());
    } else {
        return (RNG::barb_roll2d20dl(rng) + this->atkBonus + this->profBonus + this->rage >= enemy.getAC());
    }
}

bool barbarian::save(unsigned short int saveStat, unsigned short int saveDC, RNG::RNG_t& rng) const
{
    return (RNG::barb_roll1d20(rng) + saves[saveStat] + rage >= saveDC);
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
bool cleric::attack(character const& enemy, RNG::RNG_t& rng) const
{
    return !(enemy.save(4, saveDC, rng));
}

void cleric::setAC(unsigned short int baseAc, bool includeDex) {
    this->ac = baseAc + (unsigned short)(includeDex)*std::min(((stats[1] / 2) - 5), 2 + int(mediumArmorMaster));
}
bool cleric::save(unsigned short int saveStat, unsigned short int saveDC, RNG::RNG_t& rng) const
{
    return (RNG::cler_roll1d20(rng) + saves[saveStat] >= saveDC);
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
bool rogue::attack(character const& enemy, RNG::RNG_t& rng) const
{
    return (RNG::rog_roll1d20(rng) + atkBonus + profBonus >= enemy.getAC());
}

bool rogue::save(unsigned short int saveStat, unsigned short int saveDC, RNG::RNG_t& rng) const
{
    return (RNG::rog_roll1d20(rng) + saves[saveStat] >= saveDC);
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
bool wizard::attack(character const& enemy, RNG::RNG_t& rng) const
{
    return (RNG::wiz_roll1d20(rng) + atkBonus + profBonus >= enemy.getAC());
}

bool wizard::save(unsigned short int saveStat, unsigned short int saveDC, RNG::RNG_t& rng) const
{
    return (RNG::wiz_roll1d20(rng) + saves[saveStat] >= saveDC);
}

std::vector<barbarian> barbarian_premade(21);
std::vector<cleric> cleric_premade(21);
std::vector<rogue> rogue_premade(21);
std::vector<wizard> wizard_premade(21);

struct StaticInit {
    StaticInit()
    {
        for (int i = 0; i < barbarian_premade.size(); ++i) {
            barbarian_premade[i] = barbarian(i);
            cleric_premade[i] = cleric(i);
            rogue_premade[i] = rogue(i);
            wizard_premade[i] = wizard(i);
        }
    }
};
static StaticInit staticInit;

bool attack_barbarian(unsigned short int lvl, dndSim::npc const& npc, RNG::RNG_t& rng)
{
    return npc.attack(barbarian_premade[lvl], rng);
}

bool attack_cleric(unsigned short int lvl, dndSim::npc const& npc, RNG::RNG_t& rng)
{
    return npc.attack(cleric_premade[lvl], rng);
}

bool attack_rogue(unsigned short int lvl, dndSim::npc const& npc, RNG::RNG_t& rng)
{
    return npc.attack(rogue_premade[lvl], rng);
}

    bool attack_wizard(unsigned short int lvl, dndSim::npc const& npc, RNG::RNG_t& rng)
    {
        return npc.attack(wizard_premade[lvl], rng);
    }

    extern std::vector<std::vector<std::shared_ptr<dndSim::npc>>> monsters;
    extern std::vector<std::vector<std::shared_ptr<dndSim::npc>>> spell_monsters;
    extern std::vector<std::vector<std::shared_ptr<dndSim::npc>>> non_spell_monsters;

    dndSim::npc const& random_encounter_any(unsigned short int lvlCR, RNG::RNG_t& rng)
    {
        auto nr = RNG::genRNG(monsters[lvlCR - 1].size(), rng);
        return *monsters[lvlCR-1][nr];
    }

    dndSim::npc const& random_encounter_spellcaster(unsigned short int lvlCR, RNG::RNG_t& rng)
    {
        auto nr = RNG::genRNG(spell_monsters[lvlCR - 1].size(), rng);
        return *spell_monsters[lvlCR-1][nr];
    }

    dndSim::npc const& random_encounter_regular(unsigned short int lvlCR, RNG::RNG_t& rng)
    {
        auto nr = RNG::genRNG(non_spell_monsters[lvlCR - 1].size(), rng);
        return *non_spell_monsters[lvlCR-1][nr];
    }

    dndSim::npc const& random_encounter(int lvlCR, EncType type, RNG::RNG_t& rng)
    {
        if (lvlCR < 1 || lvlCR > 20) throw std::invalid_argument("Currently only CRs of integers 1 through 20 are implemented.");
        if (type == EncType::any)
            return random_encounter_any(lvlCR, rng);
        if (type == EncType::spellcaster)
            return random_encounter_spellcaster(lvlCR, rng);
        if (type == EncType::regular)
            return random_encounter_regular(lvlCR, rng);

        throw std::invalid_argument("Enemy type must be 'any', 'spellcaster', or 'regular'.");
    }
}
