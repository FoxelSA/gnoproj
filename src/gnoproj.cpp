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

#include "tools.hpp"
#include "../lib/stlplus3/filesystemSimplified/file_system.hpp"
#include "../lib/cmdLine/cmdLine.h"
#include <cstring>

using namespace std;
using namespace cv;

/******************************************************************************
*
*   main function
*
*******************************************************************************
*/

/*! \file gnoproj.cpp
* \author Stephane Flotron <s.flotron@foxel.ch>
*
* software main function
*/

/*! \mainpage gnoproj
* \section gnoproj
*
* Gnomonic reprojection software for equirectangular images from ElphelPHG cameras
*
* \section Documentation
*
* Documentation can be consulted on the [wiki](https://github.com/FoxelSA/gnoproj/wiki).
*
* \section Copyright
*
* Copyright (c) 2013-2014 FOXEL SA - [http://foxel.ch](http://foxel.ch)<br />
* This program is part of the FOXEL project <[http://foxel.ch](http://foxel.ch)>.
*
* Please read the [COPYRIGHT.md](COPYRIGHT.md) file for more information.
*
* \section License
*
* This program is licensed under the terms of the
* [GNU Affero General Public License v3](http://www.gnu.org/licenses/agpl.html)
* (GNU AGPL), with two additional terms. The content is licensed under the terms
* of the [Creative Commons Attribution-ShareAlike 4.0 International](http://creativecommons.org/licenses/by-sa/4.0/)
* (CC BY-SA) license.
*
* You must read <[http://foxel.ch/license](http://foxel.ch/license)> for more
*information about our Licensing terms and our Usage and Attribution guidelines.
*
*/

/*! \brief Main software function
*
* This function takes a sensor as input and load all calibration
* needed for gnomonic projection.
*
* \param input_image   Name of the EQR image you want to project
* \param output_directory  Complete path of the output directory where you want to put your images
* \param mac_address   Mac address of the elphel camera that take the photo
* \param mount_point   Mount point of the camera folder on your machine
* \param focal         (optionnal) Focal length in mm that you want to use
*                      for gnomonic projection with constant focal
*
* \return 0 if all was well, 1 in other cases.
*/

int main(int argc, char** argv) {

    CmdLine cmd;

    // load inputs
    std::string input_image=""; // eqr image (input) filename
    std::string output_directory=""; // output directory
    std::string mac_address="";  //mac adress
    std::string mount_point="";  // mount point

    // check is a focal length is given, and update method if necessary
    int  normalizedFocal(0);  // gnomonic projection method. 0 elphel method (default), 1 with constant focal
    double focal = 0.0;       // focal length (in mm)
    double minFocal = 0.05 ;  // lower bound for focal length
    double maxFocal = 500.0;  // upper bound for focal length

    cmd.add( make_option('i', input_image, "inputEQRImage") );
    cmd.add( make_option('o', output_directory, "outputDirectory") );
    cmd.add( make_option('m', mac_address, "macAddress") );
    cmd.add( make_option('d', mount_point, "mountPoint") );
    cmd.add( make_option('f', focal, "focal") );

    try {
      if (argc == 1) throw std::string("Invalid command line parameter.");
      cmd.process(argc, argv);
    } catch(const std::string& s) {
      std::cerr << "Usage: " << argv[0] << '\n'
      << "[-i|--inputEQRImage]\n"
      << "[-m]--macAddress\n"
      << "[-o|--outputDirectory]\n"
      << "[-d]--mountPoint]\n"
      << "[-f|--focal] (in mm)\n"
      << std::endl;

      std::cerr << s << std::endl;
      return EXIT_FAILURE;
    }

    // verify if input is present, and if yes, if it is consistant
    if(focal > 0.0)
    {
      normalizedFocal = 1;

      // check input focal
      if( focal < minFocal || focal > maxFocal)
      {
        std::cerr << "Focal length is less than " << minFocal << " mm or bigger than " << maxFocal << " mm. ";
        std::cerr << "Input focal is " << focal << endl;
        return EXIT_FAILURE;
      }
    }

    // check if image dir exists
    if ( !stlplus::file_exists( input_image ) )
    {
      std::cerr << "\nThe input image doesn't exist" << std::endl;
      return EXIT_FAILURE;
    }
    else
    {
      // check if output dir is given
      if (output_directory.empty())
      {
        std::cerr << "\nInvalid output directory" << std::endl;
        return EXIT_FAILURE;
      }
      else
      {
        // if output dir is empty, create it
        if ( !stlplus::folder_exists( output_directory ) )
        {
          if( !stlplus::folder_create ( output_directory ) )
          {
            std::cerr << "\nCannot create output directory" << std::endl;
            return EXIT_FAILURE;
          }
          else
          {
            // check if mac address is given
            if( mac_address.empty() )
            {
              std::cerr << "\n No mac address given " << std::endl;
              return EXIT_FAILURE;
            }
            else
            {
              // check if mount point is given
              if( mount_point.empty() )
              {
                std::cerr << "\n No mount point given " << std::endl;
                return EXIT_FAILURE;
              }
            }
          }
        }
        else
        {
            // check if mac address is given
            if( mac_address.empty() )
            {
              std::cerr << "\n No mac address given " << std::endl;
              return EXIT_FAILURE;
            }
            else
            {
              // check if mount point is given
              if( mount_point.empty() )
              {
                std::cerr << "\n No mount point given " << std::endl;
                return EXIT_FAILURE;
              }
            }

          }

      }

    }

    // do gnomonic projection
    const bool  bProjected = eqrToGnomonic (
          input_image,
          output_directory,
          mount_point,
          mac_address,
          normalizedFocal,
          focal
    );

    return !bProjected;
}
