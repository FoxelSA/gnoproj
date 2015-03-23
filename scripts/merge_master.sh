 #!/bin/bash
  # merge eqr tiles#
  echo "Processing..."
  find $1 -iname "*LEFT.tiff" | parallel -j+0 --eta --gnu /home/foxel/src/gnoproj/scripts/merge.sh {} tiff $1
