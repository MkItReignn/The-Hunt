////////////////////////////////////////////////////////////////////////
// Runs a player's game turn ...
//
// Can  produce  either a hunter player or a Dracula player depending on
// the setting of the I_AM_DRACULA #define
//
// This  is  a  dummy  version of the real player.c used when you submit
// your AIs. It is provided so that you can test whether  your  code  is
// likely to compile ...
//
// Note that this is used to drive both hunter and Dracula AIs. It first
// creates an appropriate view, and then invokes the relevant decideMove
// function,  which  should  use the registerBestPlay() function to send
// the move back.
//
// The real player.c applies a timeout, and will halt your  AI  after  a
// fixed  amount of time if it doesn 't finish first. The last move that
// your AI registers (using the registerBestPlay() function) will be the
// one used by the game engine. This version of player.c won't stop your
// decideMove function if it goes into an infinite loop. Sort  that  out
// before you submit.
//
// Based on the program by David Collien, written in 2012
//
// 2017-12-04	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v1.2	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v1.3	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#ifdef I_AM_DRACULA
# include "dracula.h"
# include "DraculaView.h"
#else
# include "hunter.h"
# include "HunterView.h"
#endif

// Moves given by registerBestPlay are this long (including terminator)
#define MOVE_SIZE 3

// The minimum static globals I can get away with
static char latestPlay[MOVE_SIZE] = "";
static char latestMessage[MESSAGE_SIZE] = "";

// A pseudo-generic interface, which defines
// - a type `View',
// - functions `ViewNew', `decideMove', `ViewFree',
// - a trail `xtrail', and a message buffer `xmsgs'.
#ifdef I_AM_DRACULA

typedef DraculaView View;

# define ViewNew DvNew
# define decideMove decideDraculaMove
# define ViewFree DvFree
// "GZA.... SED.... HZU.... MZU.... DTO.V.. GZA.... SED.... HZU.... MZU.... DTP.... GZA.... SED.... HZU.... MZU...."

/**
 * "GZA.... SED.... HZU.... MZU.... DTO.V.. "
	"GZA.... SED.... HZU.... MZU.... DAL.... "
	"GZA.... SED.... HZU.... MZU.... DGR.... "
	"GZA.... SED.... HZU.... MZU.... DCA.... "
	"GZA.... SED.... HZU.... MZU.... DD1....";
 */
# define xPastPlays "GZA.... SED.... HZU.... MZU.... DTO.V.. "
# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

# define xPastPlays "GZA.... SED.... HZU...."
# define xMsgs { "", "", "" }

#endif

int main(void)
{
	char *pastPlays = "GMI.... SSR.... HCO.... MBC.... DZA.V.. GFL.... SAL.... HBR.... MBE.... DMUT... GGE.... SBA.... HHA.... MSO.... DVIT... GFL.... SMS.... HBR.... MBE.... DD1T... GGO.... SAL.... HFR.... MBC.... DBDT... GMI.... SBA.... HBR.... MBE.... DHIT... GMI.... SBA.... HBR.... MBE.... DZAT.V. GMUT... SMS.... HLI.... MBC.... DSJT... GMI.... SMR.... HMU.... MVITT.. DBET... GMU.... SMI.... HVI.... MBDTT.. DSOT... GVE.... SMU.... HBD.... MKL.... DSAT... GBD.... SVI.... HKL.... MBET... DIO.... GKL.... SBD.... HBE.... MJM.... DAS..M. GBE.... SBC.... HBC.... MJM.... DBI.VM. GBC.... SBE.... HBC.... MJM.... DNPT... GBE.... SBC.... HCN.... MJM.... DROT.M. GBC.... SBE.... HCN.... MJM.... DFLT.M. GBE.... SBC.... HBE.... MJM.... DGOT... GBE.... SBC.... HBE.... MJM.... DMRT... GSA.... SCN.... HBC.... MSZ.... DCFT.V. GIO.... SBS.... HBD.... MVE.... DBOT.M. GAS.... SIO.... HVE.... MAS.... DBB..M. GAS.... SIO.... HVE.... MAS.... DAO..M. GVE.... SAS.... HFL.... MBI.... DCAT.M. GFL.... SVE.... HGE.... MBI.... DGRT.M. GGO.... SFL.... HGE.... MRO.... DALT.M. GGO.... SFL.... HGE.... MRO.... DMA.VM. GGE.... SGO.... HCF.... MFL.... DLST... GCF.... SMR.... HBO.... MGE.... DAO.... GBO.... SBU.... HALT... MMR.... DCAT.M. GBA.... SSR.... HGRT... MMS.... DD2.... GAL.... SSR.... HGR.... MMS.... DBB.... GGR.... SAL.... HGR.... MMS.... DBOT.V. GAL.... SBA.... HGR.... MMS.... DNAT.M. GBA.... SAL.... HGR.... MMS.... DCFT... GBA.... SAL.... HCAT... MAO.... DGET... GMS.... SGR.... HAO.... MCA.... DPAT... GAO.... SCA.... HAO.... MAO.... DBUT... GCA.... SAO.... HBB.... MAO.... DAMT.M. GCA.... SAO.... HBB.... MAO.... DCO.VM. GAO.... SBB.... HNA.... MAO.... DFRT.M. GBB.... SNA.... HBB.... MAO.... DLIT.M. GNA.... SBB.... HAO.... MBB.... DBRT.M. GNA.... SBB.... HAO.... MBB.... DHAT.M. GLE.... SBO.... HEC.... MNA.... DD2T.M. GBU.... SBA.... HLE.... MNA....";
					
	Message msgs[] = xMsgs;

	View state = ViewNew(pastPlays, msgs);
	decideMove(state);
	ViewFree(state);

	printf("Move: %s, Message: %s\n", latestPlay, latestMessage);
	return EXIT_SUCCESS;
}

// Saves characters from play (and appends a terminator)
// and saves characters from message (and appends a terminator)
void registerBestPlay(char *play, Message message)
{
	strncpy(latestPlay, play, MOVE_SIZE - 1);
	latestPlay[MOVE_SIZE - 1] = '\0';

	strncpy(latestMessage, message, MESSAGE_SIZE - 1);
	latestMessage[MESSAGE_SIZE - 1] = '\0';
}
