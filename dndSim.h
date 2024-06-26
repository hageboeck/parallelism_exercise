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

#ifndef DND_SIM_H
#define DND_SIM_H

#include <string>
#include <map>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <iostream>
#include "rng.h"

namespace dndSim{

    const std::map<std::string,unsigned short int> statNames = {{"str",0}, {"dex",1}, {"con",2}, {"int",3}, {"wis",4}, {"cha",5}};

    class character;
    class barbarian;
    class cleric;
    class rogue;
    class wizard;
    using npc = character;

    class character {
    protected:
        unsigned short int lvlCR = 0;
        std::vector<unsigned short int> stats;
        std::vector<unsigned short int> saves;
        unsigned short int atkStat = 0;
        void setAtkBonus();
        unsigned short int profBonus = 2;
        short int atkBonus = 0;
        unsigned short int ac = 10;
        bool causeSave = false;
        unsigned short int saveDC = 10;
        void setStats(std::vector<unsigned short int> stats);
        void setSaves(std::vector<unsigned short int> saveNames = {});
        virtual void setAC(unsigned short int baseAc = 10, bool includeDex = true);
        void setProcBonus();
        void setSaveDC();
    public:
        character();
        character(unsigned short int lvlCR, std::vector<unsigned short int> stats = {10,10,10,10,10,10}, bool causeSave = false, std::vector<unsigned short int> saveNames = {}, unsigned short int atkStat = 0, unsigned short int baseAc = 10, bool includeDex = false);
        virtual ~character() = default;
        unsigned short int getLvl() const;
        std::vector<unsigned short int> getStats() const;
        std::vector<unsigned short int> getSaves() const;
        unsigned short int getProfBonus() const;
        unsigned short int getAC() const;
        virtual bool attack(character const& enemy, RNG::RNG_t& rng) const;
        bool attack(barbarian const& enemy, RNG::RNG_t& rng);
        bool attack(cleric const& enemy, RNG::RNG_t& rng);
        bool attack(rogue const& enemy, RNG::RNG_t& rng);
        bool attack(wizard const& enemy, RNG::RNG_t& rng);
        virtual bool save(unsigned short int saveStat, unsigned short int saveDC, RNG::RNG_t& rng) const;
    };

    class barbarian : public character {
        std::vector<std::vector<unsigned short int>> lvlStats;
        unsigned short int rage;
    public:
        barbarian();
        barbarian(unsigned short int lvlCR, std::vector<unsigned short int> stats = {16,14,14,8,12,10});
        barbarian(int lvlCR, std::vector<unsigned short int> stats = {16,14,14,8,12,10});
        bool attack(character const& enemy, RNG::RNG_t& rng) const override;
        bool save(unsigned short int saveStat, unsigned short int saveDC, RNG::RNG_t& rng) const override;

    protected:
        void setAC(unsigned short int baseAc = 10, bool includeDex = true) override;
    private:
        void initializeLvlStats();
    };

    class cleric : public character {
        std::vector<std::vector<unsigned short int>> lvlStats;
        bool mediumArmorMaster;
    public:
        cleric();
        cleric(unsigned short int lvlCR, std::vector<unsigned short int> stats = {10,14,12,8,16,14});
        cleric(int lvlCR, std::vector<unsigned short int> stats = {10,14,12,8,16,14});
        bool attack(character const& enemy, RNG::RNG_t& rng) const override;
        bool save(unsigned short int saveStat, unsigned short int saveDC, RNG::RNG_t& rng) const override;

    protected:
        void setAC(unsigned short int baseAc = 13, bool includeDex = true) override;
    private:
        void initializeLvlStats(); // Helper function for lvlStats initialization
    };

    class rogue : public character {
        std::vector<std::vector<unsigned short int>> lvlStats;
    public:
        rogue();
        rogue(unsigned short int lvlCR, std::vector<unsigned short int> stats = {8,16,12,14,14,10});
        rogue(int lvlCR, std::vector<unsigned short int> stats = {8,16,12,14,14,10});
        bool attack(character const& enemy, RNG::RNG_t& rng) const override;
        bool save(unsigned short int saveStat, unsigned short int saveDC, RNG::RNG_t& rng) const override;

    private:
        void initializeLvlStats();
    };

    class wizard : public character {
        std::vector<std::vector<unsigned short int>> lvlStats;
    public:
        wizard();
        wizard(unsigned short int lvlCR, std::vector<unsigned short int> stats = {8,14,10,16,14,12});
        wizard(int lvlCR, std::vector<unsigned short int> stats = {8,14,10,16,14,12});
        bool attack(character const& enemy, RNG::RNG_t& rng) const override;
        bool save(unsigned short int saveStat, unsigned short int saveDC, RNG::RNG_t& rng) const override;

    private:
        void initializeLvlStats();
    };

    bool attack_barbarian(unsigned short int lvl, npc const& npc, RNG::RNG_t& rng);
    bool attack_cleric(unsigned short int lvl, npc const& npc, RNG::RNG_t& rng);
    bool attack_rogue(unsigned short int lvl, dndSim::npc const& npc, RNG::RNG_t& rng);
    bool attack_wizard(unsigned short int lvl, dndSim::npc const& npc, RNG::RNG_t& rng);

    enum class EncType { any, spellcaster, regular, unknown };
    npc const& random_encounter(int lvlCR, EncType type, RNG::RNG_t& rng);

    extern std::vector<barbarian> barbarian_premade;
    extern std::vector<cleric> cleric_premade;
    extern std::vector<rogue> rogue_premade;
    extern std::vector<wizard> wizard_premade;
}

#endif // DND_SIM_H
