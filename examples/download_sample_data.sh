#!/bin/bash

wget http://www.ipb.uni-bonn.de/html/projects/visual_place_recognition/small_relocalization.zip -O reloc.zip
unzip reloc.zip
rm reloc.zip 
mv small_relocalization/ data/