import re, os, sys, subprocess
from optparse import OptionParser
import json

import nuke, nukescripts

def find_format(resolution):
    mobj = re.match(r"(\d+)x(\d+)", resolution)
    if mobj:
        width = int(mobj.group(1))
        height = int(mobj.group(2))
        # given a resolution, find a matching format
        for f in nuke.formats():
            if f.width() == width and f.height() == height:
                # found it!
                return f

    return None

print sys.argv

usage = "usage: %prog [options]"
parser = OptionParser(usage=usage)
parser.add_option("", "--nukeNodePath", action="store")
parser.add_option("", "--inputPath", action="store")
parser.add_option("", "--mode", action="store", default="und", help="und or dist")
parser.add_option("", "--inputResolution", action="store")
parser.add_option("", "--outputResolution", action="store")
parser.add_option("", "--outputPath", action="store")
parser.add_option("", "--frameStart", type="int", action="store")
parser.add_option("", "--frameEnd", type="int", action="store")
parser.add_option("", "--onlyDump", action="store_true")

options, args = parser.parse_args()

for required_attr in ["inputPath", "inputResolution", "outputPath", "outputResolution", "frameStart", "frameEnd"]:
    if getattr(options, required_attr) is None:
        parser.error("No --%s specified." % required_attr)

input_path = options.inputPath
mode = options.mode
nuke_node_path = options.nukeNodePath
input_resolution = options.inputResolution
output_resolution = options.outputResolution
output_path = options.outputPath
first = options.frameStart
last = options.frameEnd
onlyDump = options.onlyDump

# use Nuke-style instead of hashes
input_path = nukescripts.replaceHashes(input_path)
input_path = os.path.abspath(input_path)

if nuke_node_path:
    # read undistort node
    nuke.scriptReadFile(nuke_node_path)
    tde_node = nuke.root().nodes()[0]

# read the input
read_plate = nuke.nodes.Read(name="plate_in")
read_plate["file"].fromUserText(input_path)
read_plate["first"].setValue(first)
read_plate["last"].setValue(last)
read_plate["colorspace"].setValue("Cineon")
last_node = read_plate

input_format = find_format(input_resolution)
if not input_format:
    parser.error("Can't find matching format for inputRresolution %s" % input_resolution)

output_format = find_format(output_resolution)
if not output_format:
    parser.error("Can't find matching format for outputResolution %s" % output_resolution)

# scale to input format
reformat_scale = nuke.nodes.Reformat(format=input_format.name(), resize="fit")
reformat_scale.setInput(0, last_node)
last_node = reformat_scale
    
# we want black outside the image area, not streaks
black_outside = nuke.nodes.BlackOutside(name="black_outside")
black_outside.setInput(0, last_node)
last_node = black_outside

if nuke_node_path:
    # wire in the undistort node
    tde_node["direction"].setValue("undistort" if mode == "und" else "distort")
    tde_node.setInput(0, last_node)
    last_node = tde_node

    # perform the pad
    reformat_full = nuke.nodes.Reformat(format=output_format.name(), resize="none")
    reformat_full.setInput(0, last_node)
    last_node = reformat_full

# resize to new res
# reformat_output = nuke.nodes.Reformat(format=output_format.name(), resize="width")
# reformat_output.setInput(0, last_node)
# last_node = reformat_output

write_output = nuke.nodes.Write(file=output_path, name="write_output")
write_output.setInput(0, last_node)

# make sure the output dir exists
if not os.path.exists(os.path.dirname(output_path)):
    os.makedirs(os.path.dirname(output_path))

if onlyDump:
    nuke.scriptSaveAs("/usr/tmp/dump.nk");
else:
    nuke.executeMultiple([write_output], [(first, last, 1)])
