#!/bin/bash

if [[ $MOKKO_WORKSPACE != $PWD ]]; then
   "not in a workspace"
   exit 1
fi

version=`ls -1 ../../release/main/ | tail -1`
version=${version/$'\n'/}

if [[ -z "$version" ]]; then
    echo "no plates in release"
    exit 1
fi

echo "found plate ${version}"

sudo -u prod mkdir -p ../../release/main/$version/render/2048x1080/jpg
sudo -u prod mkdir -p ../../release/main/$version/render/1024x540/jpg

mkdir -p exports

for style in footage plate none; do
    for res in 2048x1080 4096x2160 none; do
        input_plate_dir="../../release/main/$version/render/${res}/dpx"
        for f in ${input_plate_dir}/${MOKKO_SEQUENCE}_${MOKKO_SHOT}_${style}_main_v*_${res}.0001.dpx; do
            if [[ -f $f ]]; then
                plv=`echo ${f##*/} | egrep -o 'v[0-9]{3}'`
                input_plate="${input_plate_dir}/${MOKKO_SEQUENCE}_${MOKKO_SHOT}_${style}_main_${plv}_${res}.%04d.dpx"
                break
            fi
        done
        if [[ -n "$input_plate" ]]; then
            break;
        fi
    done
    if [[ -n "$input_plate" ]]; then
        break;
    fi
done

if [[ -z "$input_plate" ]]; then
    echo "no plate found"
    exit 1
fi

~/workspace/pug/configs/MokkoConfig/genundist2 --platePath $input_plate --outputFormat serena_2k --outputPath exports/${MOKKO_SEQUENCE}_${MOKKO_SHOT}_plate_main_${version}_2048x1080.%04d.jpg
~/workspace/pug/configs/MokkoConfig/genundist2 --platePath $input_plate --outputFormat serena_1k --outputPath exports/${MOKKO_SEQUENCE}_${MOKKO_SHOT}_plate_main_${version}_1024x540.%04d.jpg

sudo -u prod mv exports/${MOKKO_SEQUENCE}_${MOKKO_SHOT}_plate_main_${version}_2048x1080.* ../../release/main/${version}/render/2048x1080/jpg/.
sudo -u prod mv exports/${MOKKO_SEQUENCE}_${MOKKO_SHOT}_plate_main_${version}_1024x540.* ../../release/main/${version}/render/1024x540/jpg/.
