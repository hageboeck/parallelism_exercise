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
#include <vector>
#include <random>
#include <algorithm>
#include <memory>
#include <stdexcept>

namespace dndSim{

    const std::vector<std::string> statNames = {"str", "dex", "con", "int", "wis", "cha"};

    unsigned short int roll1d20();
    unsigned short int roll2d20dl();
    unsigned short int roll2d20dh();

    class character;
    class barbarian;
    class cleric;
    class rogue;
    class wizard;
    using npc = character;

    class character {
    protected:
        std::string idName;
        unsigned short int lvlCR = 0;
        std::map<std::string, unsigned short int> stats;
        std::map<std::string, unsigned short int> saves;
        std::string atkStat = "str";
        unsigned short int profBonus = 2;
        unsigned short int ac = 10;
        bool causeSave = false;
        unsigned short int saveDC = 10;
        void setStats(std::vector<unsigned short int> stats);
        void setSaves(std::vector<std::string> saveNames = {});
        virtual void setAC(unsigned short int baseAc = 10, bool includeDex = true);
        void setProcBonus();
        void setSaveDC();
    public:
        character();
        character(std::string idName, unsigned short int lvlCR, std::vector<unsigned short int> stats = {10,10,10,10,10,10}, bool causeSave = false, std::vector<std::string> saveNames = {}, std::string atkStat = "str", unsigned short int baseAc = 10, bool includeDex = false);
        character(std::string idName, unsigned short int lvlCR, std::map<std::string,unsigned short int> stats, std::vector<std::string> saveNames = {}, bool causeSave = false, std::string atkStat = "str", unsigned short int baseAc = 10, bool includeDex = false);
        std::string getName();
        unsigned short int getLvl();
        std::map<std::string, unsigned short int> getStats();
        std::map<std::string, unsigned short int> getSaves();
        unsigned short int getProfBonus();
        unsigned short int getAC();
        virtual bool attack(character& enemy);
        virtual bool attack(std::shared_ptr<character> enemy);
        virtual bool save(std::string saveStat, unsigned short int saveDC);
    };


    class barbarian : public character {
        std::vector<std::vector<unsigned short int>> lvlStats;
        unsigned short int rage;
    public:
        barbarian();
        barbarian(unsigned short int lvlCR, std::vector<unsigned short int> stats = {16,14,14,8,12,10});
        barbarian(int lvlCR, std::vector<unsigned short int> stats = {16,14,14,8,12,10});
        bool attack(character& enemy) override;
        bool attack(std::shared_ptr<character> enemy) override;
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
        bool attack( std::shared_ptr<character> enemy );
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
    private:
        void initializeLvlStats(); 
    };

    class wizard : public character {
        std::vector<std::vector<unsigned short int>> lvlStats;
    public:
        wizard();
        wizard(unsigned short int lvlCR, std::vector<unsigned short int> stats = {8,14,10,16,14,12});
        wizard(int lvlCR, std::vector<unsigned short int> stats = {8,14,10,16,14,12});
    private:
        void initializeLvlStats(); 
    };


    std::shared_ptr<npc> random_encounter( int lvlCR, std::string type = "any" );

    extern std::map<unsigned short int, barbarian> barbarian_premade;
    extern std::map<unsigned short int, cleric> cleric_premade;
    extern std::map<unsigned short int, rogue> rogue_premade;
    extern std::map<unsigned short int, wizard> wizard_premade;

}

#endif // DND_SIM_H