#ifndef SCANNER_H
#define SCANNER_H

Token getNumericToken( FILE *source, char c );
Token scanner( FILE *source );

#endif
