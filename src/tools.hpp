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

  /*! \file tools.hpp
   * \author Stephane Flotron <s.flotron@foxel.ch>
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

#ifndef TOOLS_HPP_
#define TOOLS_HPP_

#include <iostream>
#include <iomanip>
#include <gnomonic-all.h>
#include <fastcal-all.h>
#include <inter-all.h>
#include <ctype.h>
#include <unistd.h>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <cstring>

using namespace std;
using namespace cv;

/******************************************************************************
* sensorData
*****************************************************************************/

/*! \struct sensorData
* \brief structure used to store calibration information
*
* This structure is designed to store the needed informations coming from
* the elphel camera calibration
*
* \var sensorData::lfWidth
*  Width of sensor image
* \var sensorData::lfHeight
*  Height of sensor image
* \var sensorData::lfChannels
*  Number of channels of elphel camera
* \var sensorData::lfXPosition
*  X coordinate of left corner of EQR tile in panorama
* \var sensorData::lfYPosition
*  Y coordinate of left corner of EQR tile in panorama
* \var sensorData::lfImageFullWidth
*  Sitched EQR panorama width
* \var sensorData::lfImageFullHeight
*  Sitched EQR panorama height
* \var sensorData::lfFocalLength
*  Focal length in mm
* \var sensorData::lfPixelSize
*  pixel size in mm
* \var sensorData::lfAzimuth
*  azimuth angle in elphel coordinate frame (in radian)
* \var sensorData::lfHeading
*  heading angle in elphel coordinate frame (in radian)
* \var sensorData::lfElevation
*  Elevation angle in elphel coordinate frame (in radian)
* \var sensorData::lfRoll
*  roll around z axis (in radian)
* \var sensorData::lfpx0
*  x coordinate of principal point of sensor image, in pixels
* \var sensorData::lfpy0
*  y coordinate of principal point of sensor image, in pixels
* \var sensorData::lfRadius
*  radius of optical center of channel in elphel coordinate frame
* \var sensorData::lfCheight
*  height of optical center of channel in elphel coordinate frame
* \var sensorData::lfEntrance
*  Entrance pupil forward of channel
*/

struct sensorData
{
  lf_Size_t   lfWidth     = 0;
  lf_Size_t   lfHeight    = 0;
  lf_Size_t   lfChannels  = 0;
  lf_Size_t   lfXPosition = 0;
  lf_Size_t   lfYPosition = 0;
  lf_Size_t   lfImageFullWidth  = 0;
  lf_Size_t   lfImageFullHeight = 0;

  lf_Real_t   lfFocalLength = 0.0;
  lf_Real_t   lfPixelSize   = 0.0;
  lf_Real_t   lfAzimuth     = 0.0;
  lf_Real_t   lfHeading     = 0.0;
  lf_Real_t   lfElevation   = 0.0;
  lf_Real_t   lfRoll        = 0.0;
  lf_Real_t   lfpx0         = 0.0;
  lf_Real_t   lfpy0         = 0.0;
  lf_Real_t   lfRadius      = 0.0;
  lf_Real_t   lfCheight     = 0.0;
  lf_Real_t   lfEntrance    = 0.0;

};

/*********************************************************************
*  load calibration data related to elphel cameras
*
**********************************************************************/

/*! \brief Calibration data loading
*
* This function takes a sensor as input and load all calibration
* needed for gnomonic projection.
*
* \param sD             An object sensorData that will be affected with calibration data
* \param sensor_index   the sensor index of elphel camera (between 0 and Channels-1)
* \param sMountPoint    The mount point of the camera folder
* \param smacAddress    The mac address of the considered elphel camera
*
* \return bool value that says if the loading was sucessfull or not
*/

bool  loadCalibrationData( sensorData & sD,
            const size_t      & sensor_index,
            const std::string & sMountPoint,
            const std::string & smacAddress) ;

/*********************************************************************
*  call to libgnomonic for projection
*
**********************************************************************/

/*! \brief EQR to gnomonic projection
*
* This function takes an EQR image and apply a gnomonic projection in order
* to retreive the original sensor image.
*
* \param  input_image      Name of EQR input image
* \param  mount_point      The mount point of the camera folder
* \param  mac_address      The mac address of the considered elphel camera
* \param  normalizedFocal  0 or 1. If 1, use normalized focal, else use calibration focal length
* \param  focal            Focal Length in mm
*
* \return bool value that says if the projection was sucessfull or not
*/

bool  eqrToGnomonic (
            const std::string & input_image,
            const std::string & mount_point,
            const std::string & mac_address,
            const int & normalizedFocal,
            const double & focal ) ;

#endif
