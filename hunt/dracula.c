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
	char *play = "HA";
	// always start at ZA
	if (DvGetRound(dv) == 0) {		
		registerBestPlay(play, "First Round");	
	} else  {
		play = placeIdToAbbrev(TpHotSpot(dv));
	}
	
	registerBestPlay(play, "other");
}
