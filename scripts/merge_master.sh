 #!/bin/bash
  # merge eqr tiles#
  echo "Processing..."
  find $1 -iname "*LEFT.tiff" | parallel -j+0 --eta /home/foxel/src/gnoproj/scripts/merge.sh {} tiff
