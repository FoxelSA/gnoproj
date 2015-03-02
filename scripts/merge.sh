  #!/bin/bash
  # merge EQR left and right tiles 
  # Apply conversion #
  # Remove extention #
  FILE=$(basename $1 _EQR-LEFT.$2)
  # Display message #
  #echo "Merge ${FILE}_EQR-LEFT.$2 and ${FILE}_EQR-RIGHT.$2 to ${FILE}_EQR.$2 "
  # merge image #
  convert  ${FILE}_EQR-LEFT.$2 ${FILE}_EQR-RIGHT.$2 +append  ${FILE}_EQR.$2 
