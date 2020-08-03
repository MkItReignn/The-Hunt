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
#include <stdio.h>

void decideDraculaMove(DraculaView dv)
{	
	char *play;
	// always start at Toulouse - TO
	if (DvGetRound(dv) == 0) {		
		registerBestPlay("TO", "First Round");	
	} else if (DvNumberOfTeleport(dv) % 2 == 0) { // first TP hotspot
			/**
			 * need to replace DvWhereAmI with a function that finds out if MA has been travelled to
			 */
			if (DvWhereAmI(dv) != MADRID) { // if you are not at madrid
				int path_length = 0;
				PlaceId *shortest = DvGetShortestPathTo(dv, PLAYER_DRACULA, MADRID, &path_length);		
				printf("Shortest path from curr location: %s\n", placeIdToAbbrev(DvWhereAmI(dv)));
				// for (int i = 0; i < path_length; i++) {
				// 	printf("%d %s\n", shortest[i], placeIdToAbbrev(shortest[i]));
				// }
				play = (char *)placeIdToAbbrev(shortest[0]);
			} else if (lastMoveDoubleBack(dv)) {
				play = "HI";
			} 
			
			else {
				play = (char *)placeIdToAbbrev(TpSequence(DvWhereAmI(dv), 0));
			}
	
				
		
	} else { // second TP hotspot -> DvNumberOfTeleport(dv) % 2 != 0

	}
	


	
	
	registerBestPlay(play, "other");
}
