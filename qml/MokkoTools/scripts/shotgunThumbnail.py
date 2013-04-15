#!/usr/bin/env python

import sys, os, subprocess, math, json
from optparse import OptionParser

parser = OptionParser()
parser.add_option("", "--inputPath", dest="input_path", metavar="FILE")
parser.add_option("", "--outputPath", dest="output_path", metavar="FILE")
parser.add_option("", "--thumbnail", dest="filmstrip", action="store_false", default=False)
parser.add_option("", "--filmstrip", dest="filmstrip", action="store_true", default=False)
parser.add_option("", "--firstFrame", dest="first_frame", type="int")
parser.add_option("", "--lastFrame", dest="last_frame", type="int")

(options, args) = parser.parse_args()

if not options.input_path or not options.output_path:
    parser.error("--srcPath and --destPath must be specified")

try:
    output_dir = os.path.dirname(options.output_path)
    if output_dir and not os.path.isdir(output_dir): 
        os.makedirs(output_dir)
except os.error:
    parser.error("unable to create directories for %s" % options.output_path)

if options.filmstrip:
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
    for frame in range(options.first_frame, options.last_frame + 1, step):
        args += [options.input_path % frame, "-resize", "240x240"]
    args += ["+append", "-quality", "80", options.output_path]

    result = subprocess.call(args)
    
else:
    if options.first_frame and options.last_frame:
        # halfway through sequence
        image_path = options.input_path % ((options.first_frame + options.last_frame)/2)
    elif options.first_frame:
        image_path = options.input_path % options.first_frame
    elif options.last_frame:
        image_path = options.input_path % options.last_frame
    else:
        image_path = options.input_path
    
    result = subprocess.call(['convert', image_path, "-resize", "240x240", "-quality", "80", options.output_path])

sys.exit(result)