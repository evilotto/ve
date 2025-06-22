#!/usr/bin/awk -f
BEGIN {
	print "/* AUTOMATICALLY GENERATED */" > "screens.h"
	print "/* AUTOMATICALLY GENERATED */" 
	print "#include <stdlib.h>"
	print "#include \"screens.h\""
	CF = ""
}
{
    if (CF != FILENAME && CF != "") {
	    # endfile
		print "NULL};"
		print "const int " FILENAME "_cnt = " FNR ";"
	}
    if (CF != FILENAME) {
	    # beginfile
		sub(".raw$", "", FILENAME)
		print "extern const char *" FILENAME "[];" >> "screens.h"
		print "extern const int " FILENAME "_cnt;" >> "screens.h"
		print "const char *" FILENAME "[] = {"
	}
	CF = FILENAME
}
{
	print "\"" $0 "\","
}
END {
	print "NULL};"
	print "const int " FILENAME "_cnt = " FNR ";"
}
