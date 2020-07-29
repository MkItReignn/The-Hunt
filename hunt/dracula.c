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
	// get current location
	PlaceId curr_loc = DvGetPlayerLocation(dv, PLAYER_DRACULA);

	// get hunter locations
	PlaceId hunter_locations[4];
	Player player;
	// get hunters location
	for(int i = 0, player = PLAYER_LORD_GODALMING; i < 4; i++, player++) {
		hunter_locations[i] = DvGetPlayerLocation(dv, player);
	}

	// stay away from person who is closest
	int *num_g = 0;
	int *num_s = 0;
	int *num_h = 0;
	int *num_m = 0;
	PlaceId *options_g = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, num_g);
	PlaceId *options_s = DvWhereCanTheyGo(dv, PLAYER_DR_SEWARD, num_s);
	PlaceId *options_h = DvWhereCanTheyGo(dv, PLAYER_VAN_HELSING, num_h);
	PlaceId *options_m = DvWhereCanTheyGo(dv, PLAYER_MINA_HARKER, num_m);

	// create array of all the places drac can reach via road
	int *roads = 0;
	PlaceId *best_options = DvWhereCanIGoByType(dv, true, false, roads);
	// remove all of the current locations
	for(int i = 0; i < 4; i++) {
		*best_options = removeLocationfromArray(best_options, hunter_locations[i], roads);
	}
	// remove all of the locations they can reach
	for(int i = 0; i < num_g; i++) {
		*best_options = removeLocationfromArray(best_options, options_g[i], roads);
	}
	for(int i = 0; i < num_s; i++) {
		*best_options = removeLocationfromArray(best_options, options_s[i], roads);
	}
	for(int i = 0; i < num_h; i++) {
		*best_options = removeLocationfromArray(best_options, options_h[i], roads);
	}
	for(int i = 0; i < num_m; i++) {
		*best_options = removeLocationfromArray(best_options, options_m[i], roads);
	}

	if(best_options[0] != NULL) {
		registerBestPlay(placeIdToAbbrev(best_options[0]), "Mwahahahaha");
		return;
	}

	// repeat same thing for boat the same for boat
	// create array of all the places drac can reach via road
	*roads = 0;
	*best_options = DvWhereCanIGoByType(dv, false, true, roads);
	// remove all of the current locations
	for(int i = 0; i < 4; i++) {
		*best_options = removeLocationfromArray(best_options, hunter_locations[i], roads);
	}
	// remove all of the locations they can reach
	for(int i = 0; i < num_g; i++) {
		*best_options = removeLocationfromArray(best_options, options_g[i], roads);
	}
	for(int i = 0; i < num_s; i++) {
		*best_options = removeLocationfromArray(best_options, options_s[i], roads);
	}
	for(int i = 0; i < num_h; i++) {
		*best_options = removeLocationfromArray(best_options, options_h[i], roads);
	}
	for(int i = 0; i < num_m; i++) {
		*best_options = removeLocationfromArray(best_options, options_m[i], roads);
	}

	if(best_options[0] != NULL) {
		registerBestPlay(placeIdToAbbrev(best_options[0]), "Mwahahahaha");
		return;
	}

	// else stay at curr location 
	// - to do 
	// - find locations that force teleport and use that to move around
	registerBestPlay(placeIdToAbbrev(curr_loc), "Mwahahahaha");
}

PlaceId *removeLocationfromArray(PlaceId *locations, PlaceId discard, int *size)
{
	int index = 0;
	// Get index
	while(locations[index] != discard) {
		index++;
		if(index >= *size) return locations; // early return if discard not in locations
	}
	// If only element in array set size to 0 and return NULL pointer
	if(*size == 1) {
		*size = 0;
		return NULL;
	}

	PlaceId *cut_location = malloc((*size - 1) * sizeof(PlaceId));
	// Shift elements to the left
	int k = 0;
	for(int i = 0; i < *size; i++) {
		if (i != index) {
			cut_location[k++] = locations[i];
		}
	}
	*size -= 1;
	free(locations);
	return cut_location;
} 
