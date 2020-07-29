////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"

void decideDraculaMove(DraculaView dv)
{
	// get all places dracula can go
	int *num_roads = 0;
	int *num_boat = 0;
	PlaceId *options_road = DvWhereCanIGoByType(dv, true, false, num_roads);
	PlaceId *options_boat = DvWhereCanIGoByType(dv, false, true, num_boat);

	PlaceId hunter_locations[4];
	Player player;
	// get hunters location
	for(int i = 0, player = PLAYER_LORD_GODALMING; i < 4; i++, player++) {
		hunter_locations[i] = DvGetPlayerLocation(dv, player);
	}

	PlaceId best_place = options_road[1];

	registerBestPlay(placeIdToAbbrev(best_place), "Mwahahahaha");
}
