////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"

// #defines
#define MAX_LENGTH 100

// helper functions to determine what play to make
bool should_research(HunterView hv);
PlayerId camper(HunterView hv);
PlaceId move_to_dracula(HunterView hv);
PlaceId optimal_move(HunterView hv, PlaceId move);

void decideHunterMove(HunterView hv)
{
	char *play;
	char *message = "\0";

	Player curr_player = HvGetPlayer(hv);
	PlaceId curr_loc = HvGetPlayerLocation(hv, curr_player);

	// hunter will research
	if (should_research(hv)) {
		play = placeIdToAbbrev(curr_loc);
	// hunter will move to BUCHAREST to intercept dracula when he spawns
	} else if (curr_player == camper(hv)) {
		int path_length = 0;

		PlaceId *locs = HvGetShortestPathTo(hv, curr_player, BUCHAREST, &path_length);
		play = placeIdToAbbrev(locs[0]);
	} else {
		// move towards last known dracula location
		PlaceId next_move = move_to_dracula(hv);

		// looping to check if previous player has moved towards same city
		for (int i = 0; i < curr_player; i++) {
			if (HvGetPlayerLocation(hv, i) == next_move) {
				next_move = optimal_move(hv, next_move);
			}
		}
		play = placeIdToAbbrev(next_move);
	}

	registerBestPlay(play, message);
}


// if 3 rounds or more occur where we cannot see Dracula's location, return true
bool should_research(HunterView hv) {
	Round curr_round == HvGetRound(hv);
	int round = 0;

	// return false if we currently know Draculas location
	if (HvGetLastKnownDraculaLocation(hv, &round) != NOWHERE)) return false;
		
	// returning true if seen in last 3 rounds, else false
	if (curr_round - round >= 3) return true;
	else return false;
}

// return player which will move to Castle Dracula
PlayerId camper(HunterView hv) {
	Player curr_camper = PLAYER_LORD_GODALMING;
	int shortest_length = MAX_LENGTH;
	int curr_length = 0;

	// -1 as not including dracula
	for (int i = 0; i < NUM_PLAYERS - 1; i++) {
		HvGetShortestPathTo(hv, i, BUCHAREST, &curr_length);
		if (curr_length < shortest_length) {
			shortest_length = curr_length;
			curr_camper = i;
		}
	}
	return curr_camper;
}

// returns location that will move hunter closer to dracula
PlaceId move_to_dracula(HunterView hv) {
	int round = 0;
	int path_length = 0;
	Player curr_player = HvGetPlayer(hv);
	PlaceId curr_loc = HvGetPlayerLocation(hv, curr_player);

	// returning the first location that will move hunter to dracula
	PlaceId last_seen = HvGetLastKnownDraculaLocation(hv, &round);
	PlaceId *locs = HvGetShortestPathTo(hv, curr_player, last_seen, &path_length);

	return locs[0];
}

// move that prioritises rail->sea->road
PlaceId optimal_move(HunterView hv, PlaceId move) {
	int num_locs = 0;

	// preferably return rails
	PlaceId *locs = HvWhereCanIGoByType(hv, false, true , false, &num_locs);
	if (locs != NULL) {
		// if first location is not same as move, return first location, 
		// else return 2nd location if 2 or more locations exist
		if (locs[0] != move) return locs[0];
		if (num_locs > 1) return locs[1];	
	}

	// 2nd preference sea
	locs = HvWhereCanIGoByType(hv, false, false , true, &num_locs);
	if (locs != NULL) {
		// if first location is not same as move, return first location, 
		// else return 2nd location if 2 or more locations exist
		if (locs[0] != move) return locs[0];
		if (num_locs > 1) return locs[1];	
	}

	// last preference road
	locs = HvWhereCanIGo(hv, &num_locs);
	if (locs[0] != move) return locs[0];
	if (num_locs > 1) return locs[1];	

	// if absolutely no other option, go to move
	return move;
}