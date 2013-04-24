import re, os, sys, subprocess
from optparse import OptionParser
import json

import nuke, nukescripts

usage = "usage: %prog [options]"
parser = OptionParser(usage=usage)
parser.add_option("", "--nukeTemplatePath", action="store")
parser.add_option("", "--vignettePath", action="store")
parser.add_option("", "--outputPath", action="store")
parser.add_option("", "--outputFormat", action="store")
parser.add_option("", "--project", action="store")
parser.add_option("", "--shot", action="store")
parser.add_option("", "--version", action="store")
parser.add_option("", "--length", action="store")
parser.add_option("", "--notes", action="store")
parser.add_option("", "--artist", action="store")
parser.add_option("", "--onlyDump", action="store_true")

options, args = parser.parse_args()

for required_attr in ["nukeTemplatePath", "vignettePath", "outputPath", "outputFormat", "project", "shot", "version", "length", "notes", "artist"]:
    if not getattr(options, required_attr):
        parser.error("No --%s specified." % required_attr)
    
# read template
nuke.scriptReadFile(options.nukeTemplatePath)

vignette = nuke.toNode("vignette")
vignette["file"].fromUserText(os.path.abspath(options.vignettePath))

input_project = nuke.toNode("input_project")
input_project["message"].setValue(options.project) 

input_shot = nuke.toNode("input_shot")
input_shot["message"].setValue(options.shot) 

input_version = nuke.toNode("input_version")
input_version["message"].setValue(options.version) 

input_length = nuke.toNode("input_length")
input_length["message"].setValue(options.length) 

input_notes = nuke.toNode("input_notes")
input_notes["message"].setValue(options.notes) 

input_artist = nuke.toNode("input_artist")
input_artist["message"].setValue(options.artist) 

final_format = nuke.toNode("final_format")
final_format["format"].setValue(options.outputFormat)
    
output = nuke.toNode("output")
output["file"].fromUserText(os.path.abspath(options.outputPath))

if options.onlyDump:
    nuke.scriptSaveAs("/usr/tmp/dump.nk");
else:
    nuke.execute(output, 1, 1)
