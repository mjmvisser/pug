#!/usr/bin/env python
import sys
import subprocess
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-s", "--srcPath", dest="srcPath", metavar="FILE")
parser.add_option("-d", "--destPath", dest="destPath", metavar="FILE")
parser.add_option("-m", "--mode", dest="mode", metavar="copy|move|hardlink|symlink", default="copy")

(options, args) = parser.parse_args()

if not options.srcPath or not options.destPath:
    parser.error("--srcPath and --destPath must be specified")
    
if options.mode == "copy":
    command = ["cp"]
elif options.mode == "move":
    command = ["mv"]
elif options.mode == "hardlink":
    command = ["ln"]
elif options.mode == "symlink":
    command = ["ln", "-s"]
else:
    parser.error("don't understand mode '%s'" % options.mode)
    
command.append(options.srcPath)
command.append(options.destPath)

result = subprocess.call(command)

sys.exit(result)