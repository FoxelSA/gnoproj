  #!/bin/sh
  # $1 eqr image, $2 output directory $3 mac address  $4 mount point
  # project EQR using gnoproj
   /home/sflotron/foxel/git/gnoproj/build/gnoproj -i $1 -o $2 -m $3 -d $4
  #/home/foxel/src/gnoproj/build/gnoproj -i $1 -o $2 -m $3 -d $4 -f 9
