#!/bin/bash

exec $NUKE_EXEC -t ${PIPELINE_QTENGINE2:-$HOME/workspace/pipelinetools/qtengine2}/qtdistort.py "$@"
