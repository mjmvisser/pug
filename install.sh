#!/bin/bash

install_dir="/prod/tools/pipeline/pug"

if [ -d "$install_dir" ]; then
    if [ -d "${install_dir}.old" ]; then
        rm -rf "${install_dir}.old"
    fi
    
    mv "$install_dir" "${install_dir}.old"
fi;


mkdir -p "$install_dir"

rsync -a --exclude=".svn" imports qml bin doc $install_dir/ 