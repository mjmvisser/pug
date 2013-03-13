#!/usr/bin/env python

import sys, os, subprocess, math, json
from optparse import OptionParser

def frange(limit1, limit2 = None, increment = 1.):
    """
    Range function that accepts floats (and integers).
    
    Usage:
    frange(-2, 2, 0.1)
    frange(10)
    frange(10, increment = 0.5)
    
    The returned value is an iterator.  Use list(frange) for a list.
    """
    
    if limit2 is None:
        limit2, limit1 = limit1, 0.
    else:
        limit1 = float(limit1)
    
    count = int(math.ceil(limit2 - limit1)/increment)
    return (limit1 + n*increment for n in range(count))

parser = OptionParser()
parser.add_option("", "--inputPath", dest="input_path", metavar="FILE")
parser.add_option("", "--outputPath", dest="output_path", metavar="FILE")
parser.add_option("", "--filmstrip", dest="film_strip", action="store_true", default=False)
parser.add_option("", "--firstFrame", dest="first_frame", type="float")
parser.add_option("", "--lastFrame", dest="last_frame", type="float")

(options, args) = parser.parse_args()

if not options.input_path or not options.output_path:
    parser.error("--srcPath and --destPath must be specified")

try:
    output_dir = os.path.dirname(options.output_path)
    if output_dir and not os.path.isdir(output_dir): 
        os.makedirs(output_dir)
except os.error:
    parser.error("unable to create directories for %s" % options.output_path)

if options.film_strip:
    if options.first_frame is None or options.last_frame is None:
        parser.error("--firstFrame and --lastFrame must be specified for --filmstrip")

    try:
        options.input_path % options.first_frame
    except TypeError:
        parser.error("--inputPath must be a sequence for --filmstrip") 
    
    args = ["convert"]
    
    # 3 frames per second
    num_frames = (options.last_frame - options.first_frame + 1) / 8
    num_frames = max(num_frames, 5)
    num_frames = min(num_frames, 100)
    step = (options.last_frame - options.first_frame + 1) / num_frames
    for frame in frange(options.first_frame, options.last_frame + 1, step):
        args += [options.input_path % frame, "-resize", "240x240"]
    args += ["+append", "-quality", "80", options.output_path]

    result = subprocess.call(args)
    
else:
    try:
        first_frame_path = options.input_path % options.first_frame
    except TypeError:
        first_frame_path = options.input_path
    
    result = subprocess.call(['convert', first_frame_path, "-resize", "240x240", "-quality", "80", options.output_path])

details = [{"element": {"path": options.output_path}}]

# pass output back
print "begin-json details"
print json.dumps(details, indent=4)
print "===="

sys.exit(result)