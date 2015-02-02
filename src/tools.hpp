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

#ifndef TOOLS_HPP_
#define TOOLS_HPP_

#include "gnoproj.hpp"
#include <cstring>

using namespace std;
using namespace cv;

/**
* Split an input string with a delimiter and fill a string vector
*/
static bool split ( const std::string src, const std::string& delim, std::vector<std::string>& vec_value )
{
  bool bDelimiterExist = false;
  if ( !delim.empty() )
  {
    vec_value.clear();
    std::string::size_type start = 0;
    std::string::size_type end = std::string::npos -1;
    while ( end != std::string::npos )
    {
      end = src.find ( delim, start );
      vec_value.push_back ( src.substr ( start, end - start ) );
      start = end + delim.size();
    }
    if ( vec_value.size() >= 2 )
      bDelimiterExist = true;
    }
    return bDelimiterExist;
}

/******************************************************************************
* sensorData
*****************************************************************************/
struct sensorData
{
  lf_Size_t   lfWidth     = 0;
  lf_Size_t   lfHeight    = 0;
  lf_Size_t   lfChannels  = 0;
  lf_Size_t   lfXPosition = 0;
  lf_Size_t   lfYPosition = 0;
  lf_Size_t   lfImageFullWidth  = 0;
  lf_Size_t   lfImageFullHeight = 0;
  lf_Size_t   lfpixelCorrectionWidth  = 0;
  lf_Size_t   lfpixelCorrectionHeight = 0;

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

bool  loadCalibrationData( sensorData & sD,
            const size_t      & sensor_index,
            const std::string & sMountPoint,
            const std::string & smacAddress)
{

    /* Key/value-file descriptor */
    lf_Descriptor_t lfDesc;
    lf_Size_t       lfChannels=0;

    /* Creation and verification of the descriptor */
    char *c_data = new char[sMountPoint.length() + 1];
    std::strcpy(c_data, sMountPoint.c_str());

    char *c_mac = new char[smacAddress.length() + 1];
    std::strcpy(c_mac, smacAddress.c_str());

    // check input data validity
    if ( lf_parse( (unsigned char*)c_mac, (unsigned char*)c_data, & lfDesc ) == LF_TRUE ) {

      /* Query number of camera channels */
      lfChannels = lf_query_channels( & lfDesc );
    }
    else
    {
      std::cerr << " Could not read calibration data. " << std::endl;
      return false;
    }

    lfChannels = lf_query_channels( & lfDesc );


    // query panorama width and height
    sD.lfImageFullWidth  = lf_query_ImageFullWidth ( sensor_index, & lfDesc );
    sD.lfImageFullHeight = lf_query_ImageFullLength( sensor_index, & lfDesc );

    sD.lfpixelCorrectionWidth  = lf_query_pixelCorrectionWidth (sensor_index, &lfDesc);
    sD.lfpixelCorrectionHeight = lf_query_pixelCorrectionHeight(sensor_index, &lfDesc);

    /* Query position of eqr tile in panorama */
    sD.lfXPosition = lf_query_XPosition ( sensor_index, & lfDesc );
    sD.lfYPosition = lf_query_YPosition ( sensor_index, & lfDesc );

    /* Query number width and height of sensor image */
    sD.lfWidth  = lf_query_pixelCorrectionWidth ( sensor_index, & lfDesc );
    sD.lfHeight = lf_query_pixelCorrectionHeight( sensor_index, & lfDesc );

    /* Query focal length of camera sensor index */
    sD.lfFocalLength = lf_query_focalLength( sensor_index , & lfDesc );
    sD.lfPixelSize   = lf_query_pixelSize  ( sensor_index , & lfDesc );

    /* Query angles used for gnomonic rotation */
    sD.lfAzimuth    = lf_query_azimuth    ( sensor_index , & lfDesc );
    sD.lfHeading    = lf_query_heading    ( sensor_index , & lfDesc );
    sD.lfElevation  = lf_query_elevation  ( sensor_index , & lfDesc );
    sD.lfRoll       = lf_query_roll       ( sensor_index , & lfDesc );

    /* Query principal point */
    sD.lfpx0 = lf_query_px0 ( sensor_index , & lfDesc );
    sD.lfpy0 = lf_query_py0 ( sensor_index , & lfDesc );

    /* Query information related to entrance pupil center */
    sD.lfRadius   = lf_query_radius               ( sensor_index , & lfDesc );
    sD.lfCheight  = lf_query_height               ( sensor_index , & lfDesc );
    sD.lfEntrance = lf_query_entrancePupilForward ( sensor_index , & lfDesc );

    /* Release descriptor */
    lf_release( & lfDesc );

    return true;
};

#endif
