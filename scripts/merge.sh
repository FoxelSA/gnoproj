  #!/bin/bash
  # merge EQR left and right tiles 
  # Apply conversion #
  # Remove extention #
  FILE=$(basename $1 _EQR-LEFT.$2)
  # Display message #
#  echo "Merge $3/${FILE}_EQR-LEFT.$2 and $3/${FILE}_EQR-RIGHT.$2 to $3/${FILE}_EQR.$2 "
  # merge image #
  convert  $3/${FILE}_EQR-LEFT.$2 $3/${FILE}_EQR-RIGHT.$2 +append  $3/${FILE}_EQR.$2 
