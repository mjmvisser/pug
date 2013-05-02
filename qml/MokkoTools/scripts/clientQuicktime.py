import re, os, sys, subprocess
from optparse import OptionParser
import json
import datetime

import nuke, nukescripts

codecs = ["DNxHD 115"]

usage = "usage: %prog [options]"
parser = OptionParser(usage=usage)
parser.add_option("", "--nukeTemplatePath", action="store")
parser.add_option("", "--inputPath", action="store")
parser.add_option("", "--fps", type="float", action="store")
parser.add_option("", "--frameStart", type="int", action="store")
parser.add_option("", "--frameEnd", type="int", action="store")
parser.add_option("", "--outputPath", action="store")
parser.add_option("", "--resolution", action="store")
parser.add_option("", "--codec", action="store", choices=codecs)
parser.add_option("", "--project", action="store")
parser.add_option("", "--sequence", action="store")
parser.add_option("", "--scene", action="store")
parser.add_option("", "--shot", action="store")
parser.add_option("", "--version", action="store")
parser.add_option("", "--notes", action="store")
parser.add_option("", "--artist", action="store")
parser.add_option("", "--onlyDump", action="store_true")

options, args = parser.parse_args()

for required_attr in ["nukeTemplatePath", "inputPath", "fps", "outputPath", "resolution", "codec"]:
    if not getattr(options, required_attr):
        parser.error("No --%s specified." % required_attr)

# read template
nuke.scriptReadFile(options.nukeTemplatePath)

input_frames = nuke.toNode("input_frames")
input_frames["file"].fromUserText(os.path.abspath(options.inputPath))
input_frames["first"].setValue(options.frameStart);
input_frames["last"].setValue(options.frameEnd);

input_project = nuke.toNode("input_project")
input_project["message"].setValue(options.project) 

input_sequence = nuke.toNode("input_sequence")
input_sequence["message"].setValue(options.sequence) 

input_scene = nuke.toNode("input_scene")
input_scene["message"].setValue(options.scene) 

input_shot = nuke.toNode("input_shot")
input_shot["message"].setValue(options.shot) 

input_version = nuke.toNode("input_version")
input_version["message"].setValue(options.version) 

input_notes = nuke.toNode("input_notes")
input_notes["message"].setValue(options.notes) 

#input_artist = nuke.toNode("input_artist")
#input_artist["message"].setValue(options.artist) 

input_filename = nuke.toNode("input_filename")
input_filename["message"].setValue(os.path.splitext(os.path.splitext(os.path.split(options.inputPath)[1])[0])[0])

input_date = nuke.toNode("input_date")
input_date["message"].setValue(datetime.date.today().isoformat())

output_format = nuke.toNode("output_format")
mobj = re.match(r"(\d+)x(\d+)", options.resolution)
if mobj:
    width = int(mobj.group(1))
    height = int(mobj.group(2))
    # given a resolution, find a matching format
    for f in nuke.formats():
        if f.width() == width and f.height() == height:
            # found it!
            output_format["format"].setValue(f)
            break
    else:
        parser.error("can't find matching resolution for " + options.resolution)
else:
    parser.error("can't parse resolution: " + options.resolution)
    
output_movie = nuke.toNode("output_movie")
output_movie["file"].fromUserText(os.path.abspath(options.outputPath))

ext = os.path.splitext(options.outputPath)[1][1:]
if ext == "mov":
    output_movie["format"].setValue("QuickTime / MOV (mov)")
else:
    parser.error("Unsupported output file type: " + ext)

if output_format.width() == 1920 and output_format.height() == 1080 and options.fps in (23.976, 24) and options.codec == "DNxHD 115":
    output_movie["file_type"].setValue("ffmpeg")
    output_movie["fps"].setValue(options.fps)
    output_movie["codec"].setValue("VC3/DNxHD")
    output_movie["bitrate"].setValue(120586240)
else:
    parser.error("Unsupported resolution/codec/fps combination: {width}x{height}/{codec}/{fps}".format(
                           width=output_format.width(),
                           height=output_format.height(),
                           codec=options.codec,
                           fps=options.fps))

if options.onlyDump:
    nuke.scriptSaveAs("/usr/tmp/dump.nk");
else:
    nuke.execute(output_movie, options.frameStart-1, options.frameEnd)
