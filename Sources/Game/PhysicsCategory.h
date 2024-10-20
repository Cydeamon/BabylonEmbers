#ifndef GAMEPHYSICSCATEGORY_H
#define GAMEPHYSICSCATEGORY_H

enum GamePhysicsCategories
{
    PLAYER 		      = 0b0000000001,
    ENEMY  		      = 0b0000000010,
    GROUND 		      = 0b0000000100,
    TOWER_BRICK       = 0b0000001000,
    ARROW 		      = 0b0000010000,
    TOWER_TOP 	      = 0b0000100000,
    DEBRIS            = 0b0001000000,
    BODY              = 0b0010000000,
    BOMB              = 0b0100000000,
    MOLOTOV_PARTICLE  = 0b1000000000
};

#endif