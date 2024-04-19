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

#include "rng.h"

namespace RNG
{

unsigned int genRNG(unsigned int size, RNG_t& rng)
{
    std::uniform_int_distribution<int> distribution(0, size - 1);
    return distribution(rng);
}

unsigned short int roll1d20(RNG_t& rng)
{
    std::uniform_int_distribution<int> distribution(1, 20);
    return distribution(rng);
}

unsigned short int barb_roll1d20(RNG_t& rng)
{
    std::uniform_int_distribution<int> distribution(1, 20);
    return distribution(rng);
}

unsigned short int cler_roll1d20(RNG_t& rng)
{
    std::uniform_int_distribution<int> distribution(1, 20);
    return distribution(rng);
}

unsigned short int rog_roll1d20(RNG_t& rng)
{
    std::uniform_int_distribution<int> distribution(1, 20);
    return distribution(rng);
}

unsigned short int wiz_roll1d20(RNG_t& rng)
{
    std::uniform_int_distribution<int> distribution(1, 20);
    return distribution(rng);
}

unsigned short int roll2d20dl(RNG_t& rng)
{
    unsigned short int roll1 = roll1d20(rng);
    unsigned short int roll2 = roll1d20(rng);
    return std::max(roll1, roll2);
}

unsigned short int barb_roll2d20dl(RNG_t& rng)
{
    unsigned short int roll1 = barb_roll1d20(rng);
    unsigned short int roll2 = barb_roll1d20(rng);
    return std::max(roll1, roll2);
}

unsigned short int roll2d20dh(RNG_t& rng)
{
    unsigned short int roll1 = roll1d20(rng);
    unsigned short int roll2 = roll1d20(rng);
    return std::min(roll1, roll2);
}
}