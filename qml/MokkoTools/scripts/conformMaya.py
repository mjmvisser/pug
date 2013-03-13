#!/usr/bin/env mayapy
import os, sys, traceback, re, json
from optparse import OptionParser

# safely exit on uncaught exceptions
def uncaught_exceptions(ex_cls, ex, tb):
    traceback.print_exception(ex_cls, ex, tb)
    os._exit(1)

sys.excepthook = uncaught_exceptions
  
parser = OptionParser(usage="%prog: SCENE OUTPUT")

options, args = parser.parse_args()

if len(args) != 2:
    parser.error("invalid arguments")

scene = args[0]
output = args[1]

if "MOKKO_WORK_TYPE" in os.environ:
    if "MOKKO_DEPARTMENT" in os.environ and os.environ["MOKKO_WORK_TYPE"] == "asset":
        if os.environ["MOKKO_DEPARTMENT"] == "shaded":
            from mokmaya.scenefile import ShadedAssetConformer as Conformer
        elif os.environ["MOKKO_DEPARTMENT"] == "rig" and os.environ["MOKKO_ASSET_TYPE"] == "lightset":
            from mokmaya.scenefile import LightsetAssetConformer as Conformer
        elif "MOKKO_DEPARTMENT" in os.environ and os.environ["MOKKO_DEPARTMENT"] == "rig":
            from mokmaya.scenefile import RigAssetConformer as Conformer
        else: 
            from mokmaya.scenefile import AssetConformer as Conformer
    elif os.environ["MOKKO_WORK_TYPE"] == "shot":
        from mokmaya.scenefile import ShotConformer as Conformer
    else:
        print "Don't know how to conform work type {$1}".format(os.environ["MOKKO_WORK_TYPE"])
        os._exit(1)
else:
    print "MOKKO_WORK_TYPE is not set."
    os._exit(1)

import pymel.core as pm
pm.openFile(scene, loadReferenceDepth="none", force=True)

conformer = Conformer()
conformer.conform()

pm.saveAs(output, force=True)

details = [{"element": {"path": output}}]

# pass outputs back
print "begin-json details"
print json.dumps(details, indent=4)
print "===="

# flush stdout, as os._exit does not do so
sys.stdout.flush()

os._exit(0)
