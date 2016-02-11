#!/usr/bin/env python

import sys
import os
import fnmatch

directory = os.path.abspath(sys.argv[1])

print """\
// AUTOMATICALLY GENERATED --- DO NOT EDIT
#include <string>
#include <map>

std::map<std::string, std::string> shader_map
{"""

#for fname in glob.glob(directory + '/*'):

for root, dirnames, filenames in os.walk(directory):
    relpath = os.path.relpath(root, directory)
    for fname in filenames:
        name = relpath + "/" + os.path.splitext(fname)[0]

        print "    {"
        print "        // %s" % (relpath + "/" + os.path.basename(fname))
        print "        \"%s\"" % (name) + ","
        print ""

        fp = open(root + "/" + fname)
        for line in fp:
            line = line.replace("\\", "\\\\").replace("\"", "\\\"")
            print '        "%s\\n"' % (line.rstrip())

        print "    }, "

print "};"
