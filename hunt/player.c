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
	char *pastPlays = /* "GMI.... SSR.... HCO.... MBC.... DC?.V.. GVE.... SAL.... "
					"HMR.... MBC.... DC?T... GAS.... SMS.... HGO.... MBC.... DC?T... "
					"GBI.... SAL.... HMI...."; */
					"GMI.... SSR.... HCO.... MBC.... DLV.V.. GVE.... SAL.... HMR.... MBC.... DMNT... GAS.... SMS.... HGO.... MBC.... DLOT... GAS.... STS.... HVE.... MBC.... DEC.... GAS.... SIO.... HBD.... MBC.... DLET... GAS.... SBS.... HVE.... MBC.... DNAT... GAS.... SBS.... HVE.... MBC.... DBOT.V. GAS.... SBS.... HVE.... MBC.... DSRT.M. GAS.... SBS.... HVE.... MBC.... DMAT.M. GAS.... SBS.... HVE.... MBC.... DCAT... GAS.... SBS.... HVE.... MBC.... DAO..M. GAS.... SBS.... HVE.... MBC.... DMS..M. GAS.... SBS.... HVE.... MBC.... DTS..M. GAS.... SBS.... HVE.... MBC.... DIO..M. GAS.... SBS.... HVE.... MBC.... DVAT.M. GAS.... SBS.... HVE.... MBC.... DSOT.M. GAS.... SBS.... HVE.... MBC.... DBET... GAS.... SBS.... HVE.... MBC.... DKLT... GAS.... SBS.... HVE.... MBC.... DCDT... GAS.... SCN.... HVI.... MBC.... DGAT... GAS.... SGATD.. HBD.... MBC.... DCNT.M. GAS.... SGA.... HBET... MBC.... DBS..M. GAS.... SGA.... HBE.... MBC.... DIO.... GAS.... SGA.... HBE.... MBC.... DTS..M. GAS.... SGA.... HBE.... MBC.... DROT.M. GAS.... SGA.... HBE.... MBC.... DNPT... GAS.... SGA.... HBE.... MBC.... DBI.VM. GAS.... SGA.... HBE.... MBC.... DAS.... GAS.... SGA.... HBE.... MBC.... DIO.... GAS.... SGA.... HBE.... MBC.... DTS.... GAS.... SGA.... HBE.... MBC.... DROT.M. GAS.... SGA.... HBE.... MBC.... DNPT.M. GAS.... SGA.... HBE.... MBC.... DBIT.V. GAS.... SGA.... HBE.... MBC.... DAS.... GAS.... SGA.... HBE.... MBC.... DIO.... GAS.... SGA.... HBE.... MBC.... DTS.... GAS.... SGA.... HBE.... MBC.... DROT.M. GAS.... SGA.... HBE.... MBC.... DNPT.M. GAS.... SGA.... HBE.... MBC.... DBIT.M. GAS.... SGA.... HBE.... MBC.... DAS.... GAS.... SGA.... HBE.... MBC.... DIO.... GAS.... SGA.... HBE.... MBC.... DTS.... GAS.... SGA.... HBE.... MBC.... DROT.M. GAS.... SGA.... HBE.... MBC.... DNPT.M. GAS.... SGA.... HBE.... MBC.... DBIT.M. GAS.... SGA.... HBE.... MBC.... DAS.... GAS.... SGA.... HBE.... MBC.... DIO.... GAS.... SGA.... HBE.... MBC.... DVAT... GAS.... SGA.... HBE.... MBC.... DATT.M. GAS.... SGA.... HBE.... MBC.... DD1T.M. GAS.... SGA.... HBE.... MBC.... DHIT.M. GAS.... SGA.... HBE.... MBC.... DTPT... GAS.... SGA.... HKL.... MBC.... DKL.V.. GAS.... SGA.... HKLVD.. MBC.... DBET.M. GAS.... SGA.... HKL.... MBC.... DSJT.M. GAS.... SGA.... HKL.... MBC.... DVAT.M. GAS.... SGA.... HKL.... MBC.... DSAT.M. GAS.... SGA.... HKL.... MBC.... DSOT.M. GAS.... SGA.... HKL.... MBC....";
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
