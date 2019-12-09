#!/bin/sh
mogrify -interpolate Nearest -filter point -resize 1280x $@
optipng -o7 $@
