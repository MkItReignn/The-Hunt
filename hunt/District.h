#include <stdbool.h>

typedef struct district {
	PlaceId    id;
	int  District;
    bool path;
    bool head;
} District;

// Returns which district a city is in, districts are defined
// with respect to the relevant teleport hotspot located to it
int DtWhichDistrict(PlaceId pid);

// Returns whether a location is the head of the path that
// triggers a teleport
bool DtIsHead(PlaceId pid);

// Returns whether a location is the on the path that
// triggers a teleport (need to avoid these before reaching head)
bool DtIsOnPath(PlaceId pid);
