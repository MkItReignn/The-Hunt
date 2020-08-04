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

void decideHunterMove(HunterView hv)
{
	char *play;
	char *message = "\0";

	Player curr_player = HvGetPlayer(hv);
	PlaceId curr_loc = HvGetPlayerLocation(hv, curr_player);
	Round curr_round = HvGetRound(hv);

	int curr_health = HvGetScore(hv);
	// hunter has died
	if (curr_health <= 0) {
		play = (char *)placeIdToAbbrev(ST_JOSEPH_AND_ST_MARY);
	}
	// inital hunter locations
	if (curr_loc == NOWHERE) {
		if (curr_player == PLAYER_LORD_GODALMING) {
			play = (char *)placeIdToAbbrev(MILAN);
		} else if (curr_player == PLAYER_DR_SEWARD) {
			play = (char *)placeIdToAbbrev(SARAGOSSA);
		} else if (curr_player == PLAYER_VAN_HELSING) {
			play = (char *)placeIdToAbbrev(COLOGNE);
		} else {
			play = (char *)placeIdToAbbrev(BUCHAREST);
		}
		registerBestPlay(play, message);
		return;
	} 
	//printf("%d - camper\n", camper(hv));
	// hunter will research
	if (should_research(hv) && curr_round >= 6) {
		play = (char *)placeIdToAbbrev(curr_loc);
		// printf("research\n");
	// hunter closest to dracula becomes the main chaser
	} else if (curr_player == dracula_chaser(hv)) {
		PlaceId next_move =  move_to_dracula(hv);
		play = (char *)placeIdToAbbrev(next_move);
		// printf("drac chaser\n");
	// hunter is already at Bucharest, stay 
	} else if (curr_player == camper(hv) && curr_loc == BUCHAREST) {
		play = (char *)placeIdToAbbrev(BUCHAREST);
		// printf("at BC\n");
	// hunter will move to BUCHAREST to intercept dracula when he spawns
	} else if (curr_player == camper(hv)) {
		int path_length = 0;
		PlaceId *locs = HvGetShortestPathTo(hv, curr_player, BUCHAREST, &path_length);
		play = (char *)placeIdToAbbrev(locs[0]);
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
		play = (char *)placeIdToAbbrev(next_move);
		// printf("move drac\n");
	}
	registerBestPlay(play, message);
}


// if 3 rounds or more occur where we cannot see Dracula's location, return true
bool should_research(HunterView hv) {
	Round curr_round = HvGetRound(hv);
	int round = 0;

	// check location of dracula
	if (HvGetLastKnownDraculaLocation(hv, &round) != NOWHERE) {
		// returning true if seen in last 3 rounds, else false
		if (curr_round - round >= 3) {
			return true;
		} else {
			return false;
		} 
	} else {
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