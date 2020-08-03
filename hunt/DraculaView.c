////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// DraculaView.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "DraculaView.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
// add your own #includes here
#include "utils.h"
#include "Queue.h"
#include "District.h"

#define RANDOM_PATH 0
#define MADRID_PATH 1
#define PRAGUE_PATH 2
#define VENICE_PATH 3
#define ENGCHA_PATH 4

struct draculaView {
	GameView gv;
	
	// for convenience
	PlaceId trailMoves[TRAIL_SIZE - 1];     // Dracula's last 5 moves in
	                                        // reverse order
	PlaceId trailLocations[TRAIL_SIZE - 1]; // Dracula's last 5 locations
	                                        // in reverse order
	int trailLength;	
	// personal additions

};

PlaceId DvWhereAmI(DraculaView dv);

static void fillTrail(DraculaView dv);
static bool canMoveTo(DraculaView dv, PlaceId location);

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

DraculaView DvNew(char *pastPlays, Message messages[])
{
	DraculaView dv = malloc(sizeof(*dv));
	if (dv == NULL) {
		fprintf(stderr, "Couldn't allocate DraculaView\n");
		exit(EXIT_FAILURE);
	}
	
	dv->gv = GvNew(pastPlays, messages);
	fillTrail(dv);
	return dv;
}

/**
 * For convenience, fills the trailMoves and trailLocations arrays in
 * the DraculaView struct with Dracula's last 5 moves/locations, and
 * sets trailLength to the number of moves stored (in case Dracula
 * hasn't made 5 moves yet)
 */
static void fillTrail(DraculaView dv) {
	int numMoves = TRAIL_SIZE - 1;
	int numLocations = TRAIL_SIZE - 1;
	
	bool canFreeMoves = false;
	bool canFreeLocations = false;
	
	PlaceId *moves = GvGetLastMoves(dv->gv, PLAYER_DRACULA, numMoves,
	                                &numMoves, &canFreeMoves);
	
	PlaceId *locations = GvGetLastLocations(dv->gv, PLAYER_DRACULA, numLocations,
	                                        &numLocations, &canFreeLocations);
	
	placesCopy(dv->trailMoves, moves, numMoves);
	placesCopy(dv->trailLocations, locations, numLocations);
	
	placesReverse(dv->trailMoves, numMoves);
	placesReverse(dv->trailLocations, numLocations);
	
	dv->trailLength = numMoves;
	if (canFreeMoves) free(moves);
	if (canFreeLocations) free(locations);
}

void DvFree(DraculaView dv)
{
	GvFree(dv->gv);
	free(dv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round DvGetRound(DraculaView dv)
{
	return GvGetRound(dv->gv);
}

int DvGetScore(DraculaView dv)
{
	return GvGetScore(dv->gv);
}

int DvGetHealth(DraculaView dv, Player player)
{
	return GvGetHealth(dv->gv, player);
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	return GvGetPlayerLocation(dv->gv, player);
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
	return GvGetVampireLocation(dv->gv);
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	return GvGetTrapLocations(dv->gv, numTraps);
}

////////////////////////////////////////////////////////////////////////
// Making a Move

static void    addLocationMoves(DraculaView dv, PlaceId *moves,
                                int *numReturnedMoves);
static void    addDoubleBackMoves(DraculaView dv, PlaceId *moves,
                                  int *numReturnedMoves);
static void    addHideMoves(DraculaView dv, PlaceId *moves,
                            int *numReturnedMoves);
static bool    moveIsLegal(DraculaView dv, PlaceId move);
static bool    trailContains(DraculaView dv, PlaceId move);
static PlaceId resolveDoubleBack(DraculaView dv, PlaceId db);
static bool    canReach(DraculaView dv, PlaceId location);
static bool    trailContainsDoubleBack(DraculaView dv);
static bool    isDoubleBack(PlaceId move);

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	if (DvWhereAmI(dv) == NOWHERE) {
		*numReturnedMoves = 0;
		return NULL;
	}

	// There can't be more than NUM_REAL_PLACES
	// valid moves
	PlaceId *moves = malloc(NUM_REAL_PLACES * sizeof(PlaceId));
	assert(moves != NULL);
	
	*numReturnedMoves = 0;
	addLocationMoves(dv, moves, numReturnedMoves);
	addDoubleBackMoves(dv, moves, numReturnedMoves);
	addHideMoves(dv, moves, numReturnedMoves);
	return moves;
}

static void addLocationMoves(DraculaView dv, PlaceId *moves,
                             int *numReturnedMoves) {
	// Get the locations that Dracula can reach (we pass 1 as the round
	// number, since Dracula's movement doesn't depend on round number)
	int numLocs = 0;
	PlaceId *locs = GvGetReachable(dv->gv, PLAYER_DRACULA, 1,
	                               DvWhereAmI(dv), &numLocs);
	
	// For each location, check if it's a legal move, and add it to the
	// moves array if so
	for (int i = 0; i < numLocs; i++) {
		if (moveIsLegal(dv, locs[i])) {
			moves[(*numReturnedMoves)++] = locs[i];
		}
	}
	
	free(locs);
}

static void addDoubleBackMoves(DraculaView dv, PlaceId *moves,
                               int *numReturnedMoves) {
    
    // For each DOUBLE_BACK move, check it it's a legal move and add it
    // to the moves array if so
	for (PlaceId move = DOUBLE_BACK_1; move <= DOUBLE_BACK_5; move++) {
		if (moveIsLegal(dv, move)) {
			moves[(*numReturnedMoves)++] = move;
		}
	}
}

static void addHideMoves(DraculaView dv, PlaceId *moves,
                         int *numReturnedMoves) {
	if (moveIsLegal(dv, HIDE)) {
		moves[(*numReturnedMoves)++] = HIDE;
	}
}

static bool moveIsLegal(DraculaView dv, PlaceId move) {
	
	if (placeIsReal(move)) {
		return !trailContains(dv, move) && canReach(dv, move);
	
	} else if (isDoubleBack(move)) {
		PlaceId location = resolveDoubleBack(dv, move);
		return !trailContainsDoubleBack(dv) && canReach(dv, location);
		
	} else if (move == HIDE) {
		return !trailContains(dv, move) && !placeIsSea(DvWhereAmI(dv));
	
	} else {
		return false;
	}
}

static bool trailContains(DraculaView dv, PlaceId move) {
	return placesContains(dv->trailMoves, dv->trailLength, move);
}

static bool canReach(DraculaView dv, PlaceId location) {
	int numLocs = 0;
	PlaceId *places = GvGetReachable(dv->gv, PLAYER_DRACULA, 1,
	                                 DvWhereAmI(dv), &numLocs);
	bool result = placesContains(places, numLocs, location);
	free(places);
	return result;
}

/**
 * Resolves a DOUBLE_BACK move to a place
 */
static PlaceId resolveDoubleBack(DraculaView dv, PlaceId db) {
	// Get the position in the trail that the DOUBLE_BACK move refers to
	// DOUBLE_BACK_1 => 0, DOUBLE_BACK_2 => 1, etc.
	int pos = db - DOUBLE_BACK_1;
	
	// If the position in the trail is out of range, then return NOWHERE
	// otherwise return the location
	return (pos >= dv->trailLength ? NOWHERE : dv->trailLocations[pos]);
}

static bool trailContainsDoubleBack(DraculaView dv) {
	for (int i = 0; i < dv->trailLength; i++) {
		if (isDoubleBack(dv->trailMoves[i])) {
			return true;
		}
	}
	return false;
}

static bool isDoubleBack(PlaceId move) {
	return move >= DOUBLE_BACK_1 && move <= DOUBLE_BACK_5;
}

////////////////////////////////////////////////////////////////////////

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	return DvWhereCanIGoByType(dv, true, true, numReturnedLocs);
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	if (DvWhereAmI(dv) == NOWHERE) {
		*numReturnedLocs = 0;
		return NULL;
	}

	// Get an array of locations connected to Dracula's location
	// by the given transport methods
	int numReachable = 0;
	PlaceId *reachable = GvGetReachableByType(dv->gv, PLAYER_DRACULA, 1,
	                                          DvWhereAmI(dv), road, false,
	                                          boat, &numReachable);
	
	// Remove all of those locations that Dracula can't move to
	int i = 0;
	while (i < numReachable) {
		if (!canMoveTo(dv, reachable[i])) {
			// 'Remove' by replacing with the last location
			reachable[i] = reachable[numReachable - 1];
			numReachable--;
		} else {
			i++;
		}
	}
	
	*numReturnedLocs = numReachable;
	return reachable;
}

/**
 * Check if Dracula can move to the given location
 * Assumes the location is adjacent to Dracula's current location
 * and it's not a forbidden location
 */
static bool canMoveTo(DraculaView dv, PlaceId location) {
	// If the location is not in the trail, Dracula can move there
	if (!trailContains(dv, location)) {
		return true;
	
	// If the location is in the trail, but there is no DOUBLE_BACK
	// in Dracula's trail, he can move there
	} else if (!trailContainsDoubleBack(dv)) {
		return true;
	
	// Otherwise, if the location is not Dracula's current location,
	// he can't move there
	} else if (location != DvWhereAmI(dv)) {
		return false;
	
	// Otherwise, if Dracula is able to HIDE, he can move there
	} else {
		return !trailContains(dv, HIDE) && !placeIsSea(location);
	}
}

////////////////////////////////////////////////////////////////////////

PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
	return DvWhereCanTheyGoByType(dv, player, true, true, true,
	                              numReturnedLocs);
}

PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	if (DvGetPlayerLocation(dv, player) == NOWHERE) {
		*numReturnedLocs = 0;
		return NULL;
	}
	
	if (player == PLAYER_DRACULA) {
		return DvWhereCanIGoByType(dv, road, boat, numReturnedLocs);
	} else {
		// The next move for all hunters is next round
		Round round = GvGetRound(dv->gv) + 1;
		return GvGetReachableByType(dv->gv, player, round,
		                            GvGetPlayerLocation(dv->gv, player),
		                            road, rail, boat, numReturnedLocs);
	}
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions
PlaceId DvWhereAmI(DraculaView dv);
PlaceId *DraculaBfs(DraculaView dv, Player dracula, PlaceId src, Round r, bool road, bool boat);
PlaceId *DvGetShortestPathTo(DraculaView dv, Player dracula, PlaceId dest,
                             int *pathLength, bool road, bool boat);
// static Round playerNextRound(DraculaView dv, Player player);
// TODO

PlaceId DvWhereAmI(DraculaView dv)
{
	return DvGetPlayerLocation(dv, PLAYER_DRACULA);
}

PlaceId *DvGetShortestPathTo(DraculaView dv, Player dracula, PlaceId dest,
                             int *pathLength, bool road, bool boat)
{
	Round r = DvGetRound(dv);
	PlaceId src = DvGetPlayerLocation(dv, dracula);
	PlaceId *pred = DraculaBfs(dv, dracula, src, r, road, boat);
	
	// One pass to get the path length
	int dist = 0;
	PlaceId curr = dest;
	while (curr != src) {
		dist++;
		curr = pred[curr];
	}
	
	PlaceId *path = malloc(dist * sizeof(PlaceId));
	// Another pass to copy the path in
	int i = dist - 1;
	curr = dest;
	while (curr != src) {
		path[i] = curr;
		curr = pred[curr];
		i--;
	}
	
	free(pred);
	*pathLength = dist;
	return path;
}

PlaceId *DraculaBfs(DraculaView dv, Player dracula, PlaceId src, Round r, bool road, bool boat) 
{
	PlaceId *pred = malloc(NUM_REAL_PLACES * sizeof(PlaceId));
	placesFill(pred, NUM_REAL_PLACES, -1);
	pred[src] = src;
	
	Queue q1 = QueueNew(); // current round locations
	Queue q2 = QueueNew(); // next round locations
	
	QueueEnqueue(q1, src);
	while (!(QueueIsEmpty(q1) && QueueIsEmpty(q2))) {
		PlaceId curr = QueueDequeue(q1);
		int numReachable = 0;
		PlaceId *reachable = GvGetReachableByType(dv->gv, PLAYER_DRACULA, r, curr, road, false, boat, &numReachable);
		// printf("Here\n");
		for (int i = 0; i < numReachable; i++) {
			if (pred[reachable[i]] == -1) {
				pred[reachable[i]] = curr;
				QueueEnqueue(q2, reachable[i]);
			}
		}
		free(reachable);
		
		// When we've exhausted the current round's locations, advance
		// to the next round and swap the queues (so the next round's
		// locations becomes the current round's locations)
		if (QueueIsEmpty(q1)) {
			Queue tmp = q1; q1 = q2; q2 = tmp; // swap queues
			r++;
		}
	}
	
	QueueDrop(q1);
	QueueDrop(q2);
	return pred;
}



///////////////////////////////////////////////////////////////////////////////
///////////// GET NEXT MOVE
PlaceId TpHotSpot(DraculaView dv);
PlaceId TpGetToHead(DraculaView dv, PlaceId head);
PlaceId TpGetToTail(PlaceId lastMove, int district);
PlaceId DvGetLastMove(DraculaView dv);
PlaceId TpMoveToCity(DraculaView dv, PlaceId move); 

// Function that decides the next best move, based on the current location of
// dracula, he moves towards one of four cities using routes that force him
// to teleport to CD. 
// Note: there is a chance that no "good" move can be made i.e. we are forced
// to enter the route that forces teleport in an incorrect order. In this event
// our function is not able to recover
PlaceId TpHotSpot(DraculaView dv) 
{
	// move from current location to tp hot spot location dependent on which sequence to employ
	PlaceId curr_loc = DvGetPlayerLocation(dv, PLAYER_DRACULA);
	int district = DtWhichDistrict(curr_loc);
	PlaceId next = NOWHERE;

	// Note: "tail" refers to the endpoint i.e. place we teleport, and "head"
	// refers to the starting point of predetermined routes that force teleporting,
	// "path" refers to that cities that are on the abovementioned route which
	// need to be avoided prior to getting to the head
	switch (district)
	{
	case RANDOM_PATH:
		// to do 
		break;
	case MADRID_PATH:
		if (!DtIsOnPath(curr_loc)) next = TpGetToHead(dv, MADRID);
		else next = TpGetToTail(DvGetLastMove(dv), district);
		break;
	case PRAGUE_PATH:
		if (!DtIsOnPath(curr_loc)) next = TpGetToHead(dv, PRAGUE);
		else next = TpGetToTail(DvGetLastMove(dv), district);
		break;
	case VENICE_PATH:
		if (!DtIsOnPath(curr_loc)) next = TpGetToHead(dv, VENICE);
		else next = TpGetToTail(DvGetLastMove(dv), district);
		break;
	case ENGCHA_PATH:
		if (!DtIsOnPath(curr_loc)) next = TpGetToHead(dv, ENGLISH_CHANNEL);
		else next = TpGetToTail(DvGetLastMove(dv), district);
		break;
	default:
		break;
	}

	// Get all of the valid moves we can make
	int *num_valid_locs = 0;
	PlaceId *valid_locs = DvWhereCanIGo(dv, num_valid_locs);
	// If TpGetToHead fails it returns NOWHERE, in which case make a random move
	// need better solution
	if(next == NOWHERE) return valid_locs[0]; 

	// converts a move like HIDE into the actual city, used for comparison
	PlaceId city_loc = MoveToCity(dv, next); 
	for(int i = 0; i < num_valid_locs; i++) {
		if(city_loc == valid_locs[i]) return next;
	}

	// if this happens then we are kinda screwed for the rest of the game
	// need a new back up for this return, maybe store in the struct
	return valid_locs[0];
}


// Function that decides the next best move to get dracula to the "head" of the
// teleport route, "head" is defined as the starting city of the route that forces
// teleporting
PlaceId TpGetToHead(DraculaView dv, PlaceId head)
{
 	int path_length = 0;
	PlaceId *path_road = DvGetShortestPathTo(dv, PLAYER_DRACULA, head, &path_length, true, false);
	PlaceId *path_any = DvGetShortestPathTo(dv, PLAYER_DRACULA, head, &path_length, true, true);

	// make sure not to enter the path from somewhere thats not the head
	if(!DtIsOnPath(path_road[0])) return path_road[0];
	if(!DtIsOnPath(path_any[0])) return path_any[0];

	// go anywhere thats not on the path, prioritise road over sea
	PlaceId *valid = DvWhereCanIGo(dv, &path_length);
	PlaceId valid_boat = NOWHERE;
	for(int i = 0; i < path_length; i++) {
		if(!DtIsOnPath(valid[i])) {
			if(!placeIsSea(valid[i])) {
				// return the first valid location thats not at sea
				return valid[i];
			} else {
				// store sea location incase no land is found
				valid_boat = valid[i];
			}
		}
	}

	// no valid boat was found and nowhere is returned, we are forced to do 
	// an early entry into the path which means we are kinda screwed, need solution
	return valid_boat;
}

// Function that decides the next best move to get dracula to the "tail" of the
// teleport route, "tail" is defined as the city dracula is forced to teleport from
PlaceId TpGetToTail(PlaceId lastMove, int district) {
	// perhaps i should call DvWhereCanIGo to check if I can't go anywhere, and if that is the case return the move TP

	if (district == MADRID_PATH) {
		switch (lastMove)
		{
		case MADRID:
			return ALICANTE;
			break;
		case ALICANTE:
			return GRANADA;
			break;
		case CADIZ:
			return DOUBLE_BACK_2;	
			break;	
		case DOUBLE_BACK_2:
			return HIDE;
			break;
		case HIDE:
			return TELEPORT;
			break;
		default:
			break;
		}
	} 
	else if (district == PRAGUE_PATH) {
		switch (lastMove)
		{
		case PRAGUE:
			return BERLIN;
			break;
		case BERLIN:
			return LEIPZIG;
			break;
		case LEIPZIG:
			return HAMBURG;
			break;
		case HAMBURG:
			return DOUBLE_BACK_3;
			break;
		case DOUBLE_BACK_3:
			return HIDE;
			break;
		case HIDE:
			return TELEPORT;
			break;
		default:
			break;
		}
	} else if (district == VENICE_PATH) {
		switch (lastMove)
		{
			case VENICE:
				return GENOA;
				break;
			case GENOA:
				return FLORENCE;
			case FLORENCE:
				return ROME;
				break;
			case ROME:
				return DOUBLE_BACK_2;
				break;
			case DOUBLE_BACK_2:
				return HIDE;
				break;
			case HIDE:
				return TELEPORT;
				break;
			default:
				break;
		}
	}  else if (district == ENGCHA_PATH) {
		switch (lastMove)
		{
			case ENGLISH_CHANNEL:
				return PLYMOUTH;
				break;
			case PLYMOUTH:
				return LONDON;
			case LONDON:
				return DOUBLE_BACK_2;
				break;
			case DOUBLE_BACK_2:
				return HIDE;
				break;
			case HIDE:
				return TELEPORT;
				break;
			default:
				break;
		}
	}
	return NOWHERE;
}

// Function that retrieves draculas last move, used in TpGetToTail as it lets us
// keep track of which city is visited next as these routes are predetermined and
// fixed
PlaceId DvGetLastMove(DraculaView dv)
{
	int numReturn = 0;
	bool canFree = false;
	PlaceId *lastMove = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 1, &numReturn, &canFree);
	return lastMove[0];
}

// Function that find the cities that dracula will go to if he uses HIDE or DOUBLE_BACK
PlaceId TpMoveToCity(DraculaView dv, PlaceId move)
{
	// Currently does not account for the first 5 turns i.e. when numReturn < 5
	// shouldn't be necessary as it should just be TO->SR->MA for the first 2 moves

	// IMPORTANT: if by chance it goes to AL instead of MA for some reason 
	// then our code breaks lol
	int numReturn = 0;
	bool canFree = false;
	PlaceId *trail = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 5, &numReturn, &canFree);

	if(move == HIDE) {
		return trail[4];
	} else if(move == DOUBLE_BACK_1) {
		return trail[4];	
	} else if(move == DOUBLE_BACK_2) {
		return trail[3];	
	} else if(move == DOUBLE_BACK_3) {
		return trail[2];
	} else if(move == DOUBLE_BACK_4) {
		return trail[1];
	} else if(move == DOUBLE_BACK_5) {
		return trail[0];	
	}

	return move;
}

/*
bool lastMoveDoubleBack(DraculaView dv) { // replaced by DvGetLastMove
	int numReturn = 0;
	bool canFree = false;
	PlaceId *DB = GvGetLastMoves(dv->gv, PLAYER_DRACULA, 1, &numReturn, &canFree);
	return DB[0] == DOUBLE_BACK_1 || DB[0] == DOUBLE_BACK_2 || DB[0] == DOUBLE_BACK_3 || DB[0] == DOUBLE_BACK_4 || DB[0] == DOUBLE_BACK_5;
}
*/


/**
 * see how many times vampire teleported
 */
/*
int DvNumberOfTeleport(DraculaView dv) 
{
	int numTp = 0;
	int numReturn = 0;
	bool canFree = false;
	PlaceId *location = GvGetMoveHistory(dv->gv, PLAYER_DRACULA, &numReturn, &canFree);
	// printf("\n\n");
	for (int i = 0; i < numReturn; i++) {
		
		// printf("%s\n", placeIdToAbbrev(location[i]));
		if (location[i] == TELEPORT) {
			numTp++;
		}
	}
	// printf("\n\n");
	return numTp;
}
*/

/*
bool atHotSpot(DraculaView dv, int sq) 
{
	int numReturn = 0;
	bool can_free = false;
	PlaceId *trail = GvGetLastLocations(dv->gv, PLAYER_DRACULA, TRAIL_SIZE, &numReturn, &can_free);


	for (int i = 0; i < numReturn; i++) {
		if (trail[i] == MADRID && sq == 0) {
			return true;
		} else if (trail[i] == PRAGUE && sq == 1) {
			return true;
		} else if (trail[i] == ROME && sq == 2) {
			return true;
		}
	}
	return false;
}
*/
