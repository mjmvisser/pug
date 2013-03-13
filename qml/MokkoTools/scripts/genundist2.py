import re, os, sys, subprocess
from optparse import OptionParser
import json

import nuke, nukescripts

usage = "usage: %prog [options]"
parser = OptionParser(usage=usage)
parser.add_option("", "--nukeNodePath", action="store")
parser.add_option("", "--platePath", action="store")
parser.add_option("", "--outputFormat", action="store")
parser.add_option("", "--outputPath", action="store")

options, args = parser.parse_args()

for required_attr in ["platePath", "outputFormat", "outputPath"]:
    if not getattr(options, required_attr):
        parser.error("No --%s specified." % required_attr)

plate_path = options.platePath
nuke_node_path = options.nukeNodePath
output_format = options.outputFormat
output_path = options.outputPath

if nuke_node_path is not None and not output_format.endswith("_und"):
    parser.error("nukeNodePath specified, but outputFormat is not _und")
elif not nuke_node_path and output_format.endswith("_und"):
    parser.error("no nukeNodePath specified, but outputFormat is _und")
    
# use Nuke-style instead of hashes
plate_path = nukescripts.replaceHashes(plate_path)
plate_path = os.path.abspath(plate_path)

# (*&#(*&@# Nuke doesn't detect the frame range, so we have to do it ourselves
first = 1
last = 1
for f in range(0, 2000):
    if os.path.exists(plate_path % f):
        if f < first:
            first = f
        if f > last:
            last = f

if nuke_node_path:
    # read undistort node
    nuke.scriptReadFile(nuke_node_path)
    tde_node = nuke.root().nodes()[0]

# read the input
read_plate = nuke.nodes.Read(name="plate_in")
read_plate["file"].fromUserText(plate_path)
read_plate["first"].setValue(first)
read_plate["last"].setValue(last)
read_plate["colorspace"].setValue("Cineon")
last_node = read_plate

input_format = read_plate.format().name()

if input_format.endswith("_und"):
    parser.error("input plate is already padded")

if output_format.replace("_und", "") != input_format:
    # need to scale to non-padded output format
    reformat_scale = nuke.nodes.Reformat(format=output_format.replace("_und", ""), resize="width")
    reformat_scale.setInput(0, last_node)
    last_node = reformat_scale
    
# we want black outside the image area, not streaks
black_outside = nuke.nodes.BlackOutside(name="black_outside")
black_outside.setInput(0, last_node)
last_node = black_outside

if nuke_node_path:
    # wire in the undistort node
    tde_node["direction"].setValue("undistort")
    tde_node.setInput(0, last_node)
    last_node = tde_node

    # perform the pad
    reformat_full = nuke.nodes.Reformat(format=output_format, resize="none")
    reformat_full.setInput(0, last_node)
    last_node = reformat_full

# resize to new res
reformat_output = nuke.nodes.Reformat(format=output_format, resize="width")
reformat_output.setInput(0, last_node)
last_node = reformat_output

write_output = nuke.nodes.Write(file=output_path, name="write_output")
write_output.setInput(0, last_node)

nuke.executeMultiple([write_output], [(first, last, 1)])

details = [{"element": {"path": output_path},
            "context": {"FORMAT": output_format}}]

# pass outputs back
print "begin-json details"
print json.dumps(details, indent=4)
print "===="

