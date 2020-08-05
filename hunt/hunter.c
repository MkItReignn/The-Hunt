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
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #defines
#define MAX_LENGTH 100

// helper functions to determine what play to make
bool should_research(HunterView hv);
Player camper(HunterView hv);
PlaceId move_to_dracula(HunterView hv);
PlaceId optimal_move(HunterView hv, PlaceId move);
Player dracula_chaser(HunterView hv);
bool valid_move(HunterView hv, char *abbrev);
char *safety_net(HunterView hv, char *play, PlaceId curr_loc);

void decideHunterMove(HunterView hv)
{
	char *play;
	char *message = "\0";

	Player curr_player = HvGetPlayer(hv);
	PlaceId curr_loc = HvGetPlayerLocation(hv, curr_player);
	Round curr_round = HvGetRound(hv);

 	/* int round = 0;
	PlaceId drac = HvGetLastKnownDraculaLocation(hv, &round);
	printf("%d\n", drac);  
	
	PlaceId vamp = HvGetVampireLocation(hv);
	printf("%d\n", vamp); */

	// spawn locations
	if (curr_loc == NOWHERE) {
		if (curr_player == PLAYER_LORD_GODALMING) {
			play = placeIdToAbbrev(MILAN);
		} else if (curr_player == PLAYER_DR_SEWARD) {
			play = placeIdToAbbrev(SARAGOSSA);
		} else if (curr_player == PLAYER_VAN_HELSING) {
			play = placeIdToAbbrev(COLOGNE);
		} else {
			play = placeIdToAbbrev(BUCHAREST);
		}
		registerBestPlay(play, message);
		return;
	} 

	// hunter will research
	if (should_research(hv) && curr_round >= 6) {
		play = placeIdToAbbrev(curr_loc);
		registerBestPlay(play, message);
		return;
		// printf("research\n");
	}

	int round = 0;
	// dracula has not been revealed yet, go to optimal move
	if (HvGetLastKnownDraculaLocation(hv, &round) == NOWHERE) {
		play = placeIdToAbbrev(optimal_move(hv, curr_loc));
		// if program is messed up and our move is invalid, change 
		// it to first valid move so we don't get disqualified
		if (!valid_move(hv, play)) {
			play = safety_net(hv, play, curr_loc);
			// printf("ya dun messed up dee nice\n");
		}
		// printf("dracula not revealed\n");
		registerBestPlay(play, message);
		// printf("test\n");
		return;
	}


	if (curr_player == dracula_chaser(hv)) {
		PlaceId next_move =  move_to_dracula(hv);
		play = placeIdToAbbrev(next_move);
		// printf("drac chaser\n");
	// hunter is already at Bucharest, stay 
	} else if (curr_player == camper(hv) && curr_loc == BUCHAREST) {
		play = placeIdToAbbrev(BUCHAREST);
		// printf("at BC\n");
	// hunter will move to BUCHAREST to intercept dracula when he spawns
	} else if (curr_player == camper(hv)) {
		int path_length = 0;

		PlaceId *locs = HvGetShortestPathTo(hv, curr_player, BUCHAREST, &path_length);
		play = placeIdToAbbrev(locs[0]);
		// printf("going BC\n");
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
		// printf("move drac\n");
	}

	// if program is messed up and our move is invalid, change 
	// it to first valid move so we don't get disqualified
	if (!valid_move(hv, play)) {
		play = safety_net(hv, play, curr_loc);
		// printf("ya dun messed up a-a-ron\n");
	}
	registerBestPlay(play, message);
}


// if 3 rounds or more occur where we cannot see Dracula's location, return true
bool should_research(HunterView hv) {
	Round curr_round = HvGetRound(hv);
	int round = 0;

	// check location of dracula
	if (HvGetLastKnownDraculaLocation(hv, &round) != NOWHERE) {
		// return true if haven't seen dracula in at least 10 or more rounds
		if (curr_round - round >= 10) {
			// printf("1\n");
			return true;
		} else {
			// printf("2\n");
			return false;
		}
	} else {
		// printf("3\n");
		return true;
	}
}

// return player which will move to Castle Dracula
Player camper(HunterView hv) {
	Player curr_camper = PLAYER_LORD_GODALMING;
	int shortest_length = MAX_LENGTH;
	int curr_length = 0;

	// -1 as not including dracula
	for (int i = 0; i < NUM_PLAYERS - 1; i++) {
		HvGetShortestPathTo(hv, i, BUCHAREST, &curr_length);
		//printf("%d - dracula chaser\n", dracula_chaser(hv));
		if (curr_length < shortest_length && i != dracula_chaser(hv)) {
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
	// PlaceId curr_loc = HvGetPlayerLocation(hv, curr_player);

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

Player dracula_chaser(HunterView hv) {
	Player dracula_chaser = PLAYER_LORD_GODALMING;
	int shortest_length = MAX_LENGTH;
	int curr_length = 0;
	int round = 0;

	PlaceId last_seen = HvGetLastKnownDraculaLocation(hv, &round);

	// -1 as not including dracula
	for (int i = 0; i < NUM_PLAYERS - 1; i++) {
		HvGetShortestPathTo(hv, i, last_seen, &curr_length);
		if (curr_length < shortest_length) {
			shortest_length = curr_length;
			dracula_chaser = i;
		}
	}
	return dracula_chaser;
}

// checking if move is valid (checks going to a location adjacent to current location)
bool valid_move(HunterView hv, char *abbrev) {
	int num = 0;
	PlaceId *locs = HvWhereCanIGo(hv, &num);
	PlaceId place = placeAbbrevToId(abbrev);

	// looping through available locations and seeing if place is a valid place to go
	for (int i = 0; i < num; i++) {
		if (place == locs[i]) return true;
	}

	return false;
}

// if program is messed up and our move is invalid, change 
// it to first valid move so we don't get disqualified
char *safety_net(HunterView hv, char *play, PlaceId curr_loc) {
	int nums = 0;
	PlaceId *locs = HvWhereCanIGo(hv, &nums);
	if (curr_loc != locs[0])
		return placeIdToAbbrev(locs[0]);
	else 
		return placeIdToAbbrev(locs[1]);
}