#!/bin/bash

echo exposure
read exposure_end # is notified of exposure end
read image imagename  # $image will hold "image" string
echo rename $imagename /foo/bar/%f  # renames acquired image to /foo/bar/{standard image pattern}
read new_name # reads new image name to $new_name

