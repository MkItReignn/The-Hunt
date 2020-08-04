#include "Places.h"
#include "District.h"
#include <stdbool.h>

District DISTRICT[] =
{
	{ADRIATIC_SEA,           3, false, false},
	{ALICANTE,               1, true, false},   // path
	{AMSTERDAM,              4, false, false},
	{ATHENS,                 3, false, false},
	{ATLANTIC_OCEAN,         1, false, false},
	{BARCELONA,              1, false, false},
	{BARI,                   3, false, false},
	{BAY_OF_BISCAY,          1, false, false},
	{BELGRADE,               3, false, false},
	{BERLIN,                 2, true, false},   // path
	{BLACK_SEA,              3, false, false},
	{BORDEAUX,               1, false, false},
	{BRUSSELS,               4, false, false},
	{BUCHAREST,              3, false, false},
	{BUDAPEST,               2, false, false},
	{CADIZ,                  1, true, false},   // path
	{CAGLIARI,               1, false, false},
	{CASTLE_DRACULA,         0, false, false},  // neutral
	{CLERMONT_FERRAND,       1, false, false},
	{COLOGNE,                2, false, false},
	{CONSTANTA,              3, false, false},
	{DUBLIN,                 1, false, false},
	{EDINBURGH,              1, false, false},
	{ENGLISH_CHANNEL,        4, true, true},    // head
	{FLORENCE,               3, true, false},   // path
	{FRANKFURT,              2, false, false},
	{GALATZ,                 3, false, false}, 
	{GALWAY,                 1, false, false},
	{GENEVA,                 0, false, false},  // neutral
	{GENOA,                  3, true, false},   // path
	{GRANADA,                1, true, false},   // path
	{HAMBURG,                2, true, false},   // path
	{IONIAN_SEA,             3, false, false},
	{IRISH_SEA,              1, false, false},
	{KLAUSENBURG,            0, false, false},  // neutral
	{LE_HAVRE,               4, false, false},
	{LEIPZIG,                2, true, false},   // path
	{LISBON,                 1, false, false},
	{LIVERPOOL,              1, false, false},
	{LONDON,                 4, true, false},   // path
	{MADRID,                 1, true, true},    // head
	{MANCHESTER,             1, false, false},
	{MARSEILLES,             1, false, false},
	{MEDITERRANEAN_SEA,      1, false, false},
	{MILAN,                  3, false, false},
	{MUNICH,                 0, false, false},  // neutral
	{NANTES,                 4, false, false},
	{NAPLES,                 3, false, false},
	{NORTH_SEA,              3, false, false},
	{NUREMBURG,              2, false, false},
	{PARIS,                  4, false, false},
	{PLYMOUTH,               4, true, false},   // path
	{PRAGUE,                 2, true, true},    // head
	{ROME,                   3, true, false},   // path
	{SALONICA,               3, false, false},
	{SANTANDER,              1, false, false},
	{SARAGOSSA,              1, false, false},
	{SARAJEVO,               3, false, false},
	{SOFIA,                  3, false, false},
	{ST_JOSEPH_AND_ST_MARY, -1, false, false},
	{STRASBOURG,             0, false, false},  // neutral
	{SWANSEA,                1, false, false},
	{SZEGED,                 0, false, false},  // neutral
	{TOULOUSE,               1, false, false},
	{TYRRHENIAN_SEA,         1, false, false},
	{VALONA,                 3, false, false},
	{VARNA,                  3, false, false},
	{VENICE,                 3, true, true},    // head
	{VIENNA,                 2, false, false},
	{ZAGREB,                 0, false, false},  // neutral
	{ZURICH,                 3, false, false},
};

int DtWhichDistrict(PlaceId pid) 
{
	if (placeIsReal(pid)) {
		return DISTRICT[pid].District;
	}
	return 0;
}

bool DtIsHead(PlaceId pid) 
{
	if (placeIsReal(pid)) {
		return DISTRICT[pid].head;
	}
	return false;
}

bool DtIsOnPath(PlaceId pid) 
{
	if (placeIsReal(pid)) {
		return DISTRICT[pid].path;
	}
	return false;
}