#!/bin/bash
cd shaders
# glslc shader.vert -o vert.spv
# glslc shader.frag -o frag.spv
# for all .vert or .frag files, compile them to "name".vert/frag.spv
for file in *.vert *.frag
do
    glslc $file -o $file.spv
done