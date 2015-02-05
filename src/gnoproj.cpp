/*
 * gnoproj
 *
 * Copyright (c) 2013-2015 FOXEL SA - http://foxel.ch
 * Please read <http://foxel.ch/license> for more information.
 *
 *
 * Author(s):
 *
 *      Stéphane Flotron <s.flotron@foxel.ch>
 *
 * Contributor(s):
 *
 *      Luc Deschenaux <luc.deschenaux@foxel.ch>
 *
 *
 * This file is part of the FOXEL project <http://foxel.ch>.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Additional Terms:
 *
 *      You are required to preserve legal notices and author attributions in
 *      that material or in the Appropriate Legal Notices displayed by works
 *      containing it.
 *
 *      You are required to attribute the work as explained in the "Usage and
 *      Attribution" section of <http://foxel.ch/license>.
 */


#include "gnoproj.hpp"
#include "tools.hpp"
#include <cstring>

using namespace std;
using namespace cv;

/******************************************************************************
*
*   main function
*
*******************************************************************************
*/

int main(int argc, char** argv) {

    /* Usage branch */
    if ( argc != 4 || !strcmp( argv[1], "help" ) || !strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")  ) {
        /* Display help */
        printf( "Usage : %s <input_image>  <camera mac adress>  <mount point> [ <focal> ]\n\n",argv[0]);
        return 1;
    }

    // load inputs
    char* input_image_filename=argv[1]; // eqr image (input) filename
    std::string mac_address(argv[2]);  //mac adress
    std::string mount_point(argv[3]);  // mount point
    std::string input_image(input_image_filename);

    // check is a focal length is given, and update method if necessary
    int  normalizedFocal(0);  // gnomonic projection method. 0 elphel method (default), 1 with constant focal
    double focal = 0.0;       // focal length (in mm)
    double minFocal = 0.05 ;  // lower bound for focal length
    double maxFocal = 500.0;  // upper bound for focal length
    std::string inputFocal((argc==5)?argv[4]:"");

    // verify if input is present, and if yes, if it is consistant
    if(inputFocal.length())
    {
      focal  = atof(inputFocal.c_str());
      normalizedFocal = 1;

      // check input focal
      if( focal < minFocal || focal > maxFocal)
      {
        std::cerr << "Focal length is less than " << minFocal << " mm or bigger than " << maxFocal << " mm. ";
        std::cerr << "Input focal is " << inputFocal << endl;
        return 0;
      }
    }

    // do gnomonic projection
    const bool  bProjected = eqrToGnomonic (
          input_image,
          mount_point,
          mac_address,
          normalizedFocal,
          focal
    );

    return !bProjected;
}
