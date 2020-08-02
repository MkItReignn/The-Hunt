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
	// always start at Toulouse - TO
	if (DvGetRound(dv) == 0) {
		registerBestPlay("TO", " ");
	}

	// move down to GR and teleport back to CD
	// will make a funciton for this later
	else if (DvGetRound(dv)  == 1) {
		registerBestPlay("SR", " ");
	} else if (DvGetRound(dv)  == 2) {
		registerBestPlay("MA", " ");
	} else if (DvGetRound(dv)  == 3) {
		registerBestPlay("AL", " ");
	} else if (DvGetRound(dv)  == 4) {
		registerBestPlay("GR", " ");
	} else if (DvGetRound(dv)  == 5) {
		registerBestPlay("CA", " ");
	} else if (DvGetRound(dv)  == 6) {
		registerBestPlay("D1", " ");
	} else if (DvGetRound(dv)  == 7) {
		registerBestPlay("HI", " ");
	}


	
	
	registerBestPlay("CD", "Mwahahahaha");
}
