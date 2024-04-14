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

#ifndef RNG_H
#define RNG_H

#include <vector>
#include <random>

namespace RNG
{

    unsigned int genRNG( unsigned int size );
    unsigned short int roll1d20();
    unsigned short int barb_roll1d20();
    unsigned short int cler_roll1d20();
    unsigned short int rog_roll1d20();
    unsigned short int wiz_roll1d20();
    unsigned short int roll2d20dl();
    unsigned short int barb_roll2d20dl();
    unsigned short int roll2d20dh();

}

#endif