#!/usr/bin/env python

import sys, os, subprocess, math, json
from optparse import OptionParser

parser = OptionParser()
parser.add_option("", "--inputPath", dest="input_path", metavar="FILE")
parser.add_option("", "--outputPath", dest="output_path", metavar="FILE")
parser.add_option("", "--format", dest="format", metavar="FORMAT")
parser.add_option("", "--firstFrame", dest="first_frame", type="int")
parser.add_option("", "--fps", dest="fps", type="int")

(options, args) = parser.parse_args()

if not options.input_path or not options.output_path:
    parser.error("--inputPath and --outputPath must be specified")

if not options.fps:
    parser.error("--fps must be specified")

if not options.format:
    parser.error("--format mp4 or webm must be specified")

if options.first_frame is None:
    parser.error("--firstFrame must be specified")

try:
    output_dir = os.path.dirname(options.output_path)
    if output_dir and not os.path.isdir(output_dir): 
        os.makedirs(output_dir)
except os.error:
    parser.error("unable to create directories for %s" % options.output_path)

args = ["/prod/tools/common/ffmpeg-1.2/bin/ffmpeg", "-y"]
args += ["-f", "image2", "-start_number", str(options.first_frame), "-i", options.input_path]

if options.format == "mp4":
    args += ["-vcodec", "libx264", "-pix_fmt", "yuv420p", "-vf", "scale=trunc((a*oh)/2)*2:720", "-g", "30", "-b:v", "2000k", "-vprofile", "high", "-bf", "0"]
    #args += ["-i", options.audio_input]
    #args += ["-strict", "experimental", "-acodec", "aac", "-ab", "160k", "-ac", "2"]
    args += ["-f", "mp4", "-r", str(options.fps), options.output_path]

elif options.format == "webm":
    args += ["-pix_fmt", "yuv420p", "-vcodec", "libvpx", "-vf", "scale=trunc((a*oh)/2)*2:720", "-g", "30", "-b:v", "2000k", "-vpre", "720p", "-quality", "good", "-cpu-used", "0", "-qmin", "10", "-qmax", "42"]
    #args += ["-i", options.audio_input]
    #args += ["-acodec", "libvorbis", "-aq", "60", "-ac", "2"]
    args += ["-f", "webm", "-r", str(options.fps), options.output_path]

print " ".join(args)

result = subprocess.call(args)

sys.exit(result)