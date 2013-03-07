import sys
import traceback
import re
import json
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
    
if not "{lod}" in args[1]:
    parser.error("OUTPUT must contain {lod}")

sys.stderr.write("inputs are " + str(options) + " " + str(args))

import pymel.core as pm

pm.openFile(args[0], loadReferenceDepth="none", force=True)

details = []
for transform in pm.listTransforms():
    try:
        lod = transform.resolution_export.get()
        if len(lod):
            path = args[1].format(lod=lod)
            pm.showHidden(transform)
            pm.select(transform)
            # unparent, otherwise the top node is also exported
            pm.parent(transform, world=True)
            pm.exportSelected(path, preserveReferences=True, force=True)
            details.append({"element": {"path": path},
                            "context": {"LOD": lod}})
    except AttributeError:
        pass

# pass outputs back
print "begin-json details"
print json.dumps(details, indent=4)
print "===="

# flush stdout, as os._exit does not do so
sys.stdout.flush()

os._exit(0)
