  #!/bin/bash
  # Convert image ($1 images path, $2 output directory $3 camera mac address, $4 mountpoint )
  # apply gnomonic projection #
  echo "merging left and right tiles"
  ./merge_master.sh  $1
  echo "project eqr images using gnomonic projection"
  find $1 -iname "*EQR.tiff" | parallel -j+0 --eta /home/foxel/src/gnoproj/scripts/gnoproj.sh {} $2 $3 $4
