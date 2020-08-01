//Test the hunter ai
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#include "hunter.h"
#include "HunterView.h"

int main(void) {
    
    printf("Testing hunter ai\n");

    char *trail = "GST.... SAO.... HZU.... MBB.... DZU.V.. ";
    Message messages[10] = "";

    HunterView hv = HvNew(trail, messages);
    decideHunterMove(HunterView hv);
}
