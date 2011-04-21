/*
 * consts.hpp
 *
 *  Created on: 20.04.2011
 *      Author: Sebastian
 */

#ifndef CONSTS_HPP_
#define CONSTS_HPP_


//TODO als const-singleton???


//Game.hpp:
#define MAXGAMEBOARDWIDTH 500
#define MAXGAMEBOARDHEIGHT 50
#define NOBODY -1

//Worm.hpp:
#define MAXENERGY 100
#define WORMJUMPSPEED_Y -500.0f
#define WORMMAXSPEED_X 175.0f
#define WORMACCELERATION 100.0f

//BLock.hpp:
#define BLOCKSIZE 20
							//AIR,NORMAL,SHOOTING,JUMPBOARD,INVISIBLE
#define BLOCKCOSTS 			{0   ,     5,      40,       10,       10}
#define BLOCKBOUNCING_X 	{0.3f,  0.3f,    0.0f,     0.9f,     0.3f}//Bouncingfactors...
#define BLOCKBOUNCING_Y 	{0.3f,  0.3f,    0.0f,     0.9f,     0.3f}//Bouncingfactors...

//Physics.hpp:
#define GRAVITY 981.0f
#define FRICTION 0.8f

//DATA:
#define _DIRDATA_ std::string("data")



#endif /* CONSTS_HPP_ */
