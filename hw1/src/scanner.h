#ifndef SCANNER_H
#define SCANNER_H

#include "type.h"

Token getNumericToken( FILE *source, char c );
Token scanner( FILE *source );

#endif
