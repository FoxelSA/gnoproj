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
   *
   * Software main header
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

#include "gnoproj.hpp"
#include <cstring>

using namespace std;
using namespace cv;

/******************************************************************************
* sensorData
*****************************************************************************/

/*! \struct sensor Data
* \brief structure used to store calibration information
*
* This structure is designed to store the needed informations coming from
* the elphel camera calibration
*
* \var sensorData::lfWidth
*  Width of sensor image
* \var sensorData::lfHeight
*  Height of sensor image
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

    /* Creation and verification of the descriptor */
    char *c_data = new char[sMountPoint.length() + 1];
    std::strcpy(c_data, sMountPoint.c_str());

    char *c_mac = new char[smacAddress.length() + 1];
    std::strcpy(c_mac, smacAddress.c_str());

    // check input data validity
    if ( lf_parse( (unsigned char*)c_mac, (unsigned char*)c_data, & lfDesc ) == LF_TRUE )
    {
        /* Query number of camera channels */
        sD.lfChannels = lf_query_channels( & lfDesc );

        if( sensor_index > (size_t) sD.lfChannels-1 )
        {
            std::cerr << " Sensor index out of range ! " << std::endl;
            return false;
        }
        else
        {
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
        }

    }
    else
    {
        std::cerr << " Could not read calibration data. " << std::endl;
        return false;
    }

};

/*********************************************************************
*  call to libgnomonic for projection
*
**********************************************************************/

bool  eqrToGnomonic (
            const std::string & input_image,
            const std::string & mount_point,
            const std::string & mac_address,
            const int & normalizedFocal,
            const double & focal )
{
    std::string output_image_filename; // output image filename

    // extract channel information from image name
    std::vector<string>  splitted_name;
    std::vector<string>  out_split;

    split( input_image, "-", splitted_name );
    split( input_image, "_", out_split );

    const size_t sensor_index=atoi(splitted_name[1].c_str());
    sensorData   sensorSD;

    // load calibration informations
    bool  bLoadCalibration = loadCalibrationData
                                    ( sensorSD,
                                      sensor_index,
                                      mount_point,
                                      mac_address );

    if( !bLoadCalibration )
    {
      std::cerr << " Failed to load calibration informations. Exit " << std::endl;
      return false;
    }
    else
    {
        // load image
        IplImage* eqr_img = cvLoadImage(input_image.c_str(), CV_LOAD_IMAGE_COLOR );

        /* Initialize output image structure */
        IplImage* out_img = cvCreateImage( cvSize( sensorSD.lfWidth, sensorSD.lfHeight ), IPL_DEPTH_8U , eqr_img->nChannels );

        if(!normalizedFocal){
              /* Gnomonic projection of the equirectangular tile */
              lg_ttg_elphel(
                  ( inter_C8_t *) eqr_img->imageData,
                  eqr_img->width,
                  eqr_img->height,
                  eqr_img->nChannels,
                  ( inter_C8_t *) out_img->imageData,
                  out_img->width,
                  out_img->height,
                  out_img->nChannels,
                  sensorSD.lfpx0,
                  sensorSD.lfpy0,
                  sensorSD.lfImageFullWidth,
                  sensorSD.lfImageFullHeight-1, // there's an extra pixel for wrapping
                  sensorSD.lfXPosition,
                  sensorSD.lfYPosition,
                  sensorSD.lfRoll,
                  sensorSD.lfAzimuth,
                  sensorSD.lfElevation,
                  sensorSD.lfHeading,
                  sensorSD.lfPixelSize,
                  sensorSD.lfFocalLength,
                  li_bicubicf
              );

              // create output image name
              output_image_filename+=out_split[0]+"_"+out_split[1]+"-RECT-SENSOR.tiff";
        }
        else
        {
              /* Gnomonic projection of the equirectangular tile */
              lg_ttg_center(
              ( inter_C8_t *) eqr_img->imageData,
              eqr_img->width,
              eqr_img->height,
              eqr_img->nChannels,
              ( inter_C8_t *) out_img->imageData,
              out_img->width,
              out_img->height,
              out_img->nChannels,
              sensorSD.lfImageFullWidth,
              sensorSD.lfImageFullHeight-1,
              sensorSD.lfXPosition,
              sensorSD.lfYPosition,
              sensorSD.lfAzimuth + sensorSD.lfHeading + LG_PI,
              sensorSD.lfElevation,
              sensorSD.lfRoll,
              focal,
              sensorSD.lfPixelSize,
              li_bicubicf
              );

              // create output image name
              output_image_filename+=out_split[0]+out_split[1]+"-RECT-CONFOC.tiff";
          }

        /* Gnomonic image exportation */
        cvSaveImage(output_image_filename.c_str() , out_img, NULL );

        /* Free memory */
        cvReleaseImage(&eqr_img);
        cvReleaseImage(&out_img);

        return true;
    }

};

#endif
