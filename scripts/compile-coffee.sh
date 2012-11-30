#!/bin/bash

# allow * to expand to nothing
shopt -s nullglob

for coffeedir in $(find . -type d -name coffee); do
    if [ -n "$coffeedir/*.coffee" ]; then
        jsdir=${coffeedir%/*}/js
        if [ -n "$jsdir/*.js" ]; then
            rm -f $jsdir/*.js
        fi
        coffee -b -c -o $jsdir $coffeedir/*.coffee
    fi

    if [ -n "$jsdir/*.js" ]; then
        for jsfile in $jsdir/*.js; do
            sed -i -n '
            # if the first line copy the pattern to the hold buffer
            1h
            # if not the first line then append the pattern to the hold buffer
            1!H
            # if the last line then ...
            $ {
                    # copy from the hold to the pattern buffer
                    g
                    # replace /* .pragma blah */ with just .pragma blah
                    s=/[*]\(\s*[.]pragma.*\)[*]/=\1=g
                    # print
                    p
            }
            ' $jsfile
        done
    fi
done