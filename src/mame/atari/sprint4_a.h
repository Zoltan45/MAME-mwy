// license:BSD-3-Clause
// copyright-holders:Derrick Renaud

/***************************************************************************

Atari Sprint 4 + Ultra Tank Audio

***************************************************************************/
#ifndef MAME_ATARI_SPRINT4_A_H
#define MAME_ATARI_SPRINT4_A_H

#pragma once

#include "sound/discrete.h"

/* discrete sound input nodes */
#define SPRINT4_MOTOR_DATA_1        NODE_01
#define SPRINT4_MOTOR_DATA_2        NODE_02
#define SPRINT4_MOTOR_DATA_3        NODE_03
#define SPRINT4_MOTOR_DATA_4        NODE_04
#define SPRINT4_SCREECH_EN_1        NODE_05
#define SPRINT4_SCREECH_EN_2        NODE_06
#define SPRINT4_SCREECH_EN_3        NODE_07
#define SPRINT4_SCREECH_EN_4        NODE_08
#define SPRINT4_BANG_DATA           NODE_09
#define SPRINT4_ATTRACT_EN          NODE_10

#define ULTRATNK_MOTOR_DATA_1       SPRINT4_MOTOR_DATA_1
#define ULTRATNK_MOTOR_DATA_2       SPRINT4_MOTOR_DATA_2
#define ULTRATNK_FIRE_EN_1          SPRINT4_SCREECH_EN_1
#define ULTRATNK_FIRE_EN_2          SPRINT4_SCREECH_EN_2
#define ULTRATNK_EXPLOSION_DATA     SPRINT4_BANG_DATA
#define ULTRATNK_ATTRACT_EN         SPRINT4_ATTRACT_EN

DISCRETE_SOUND_EXTERN( sprint4_discrete );
DISCRETE_SOUND_EXTERN( ultratnk_discrete );

#endif // MAME_ATARI_SPRINT4_A_H
