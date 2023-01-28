#!/usr/bin/awk -f
BEGIN {
	print "/* AUTOMATICALLY GENERATED */" > "screens.h"
	print "/* AUTOMATICALLY GENERATED */" 
	print "#include <stdlib.h>"
	print "#include \"screens.h\""
}
BEGINFILE {
	sub(".raw$", "", FILENAME)
	print "extern const char *" FILENAME "[];" >> "screens.h"
	print "extern const int " FILENAME "_cnt;" >> "screens.h"
	print "const char *" FILENAME "[] = {"
}
{
	print "\"" $0 "\","
}
ENDFILE {
	print "NULL};"
	print "const int " FILENAME "_cnt = " FNR ";"
}
