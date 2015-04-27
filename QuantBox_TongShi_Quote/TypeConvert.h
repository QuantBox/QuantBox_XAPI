#pragma once

#include <stdlib.h>
#include "Stockdrv.h"
#include "../include/ApiStruct.h"

ExchangeType Market_2_ExchangeType(WORD In);
char* Market_2_ExchangeID(WORD In);
char* OldSymbol_2_NewSymbol(char* In, WORD In2);

