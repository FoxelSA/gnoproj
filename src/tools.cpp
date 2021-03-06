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
#include <cstring>

using namespace std;
using namespace cv;


/*********************************************************************
* Split an input string with a delimiter and fill a string vector
*
*********************************************************************
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
*  Project EQR image using libgnomonic
*
**********************************************************************/

bool  eqrToGnomonic (
            const std::string & input_image,
            const std::string & output_directory,
            const std::string & mount_point,
            const std::string & mac_address,
            const int & normalizedFocal,
            const double & focal )
{
    std::string output_image_filename=output_directory+"/"; // output image filename

    //extract image basename
    std::vector<string>  split_slash;
    split( input_image, "/", split_slash );

    const std::string image_basename =  split_slash[split_slash.size()-1];

    // extract channel information from image name
    std::vector<string>  splitted_name;
    std::vector<string>  out_split;

    split( image_basename, "-", splitted_name );
    split( image_basename, "_", out_split );

    // check if output image already exists
    if(!normalizedFocal)
    {
        output_image_filename+=out_split[0]+"_"+out_split[1]+"-RECT-SENSOR.tiff";
    }
    else
    {
      // create output image name
      output_image_filename+=out_split[0]+out_split[1]+"-RECT-CONFOC.tiff";
    }

    if ( stlplus::file_exists( output_image_filename ) )
    {
      std::cerr << "\nThe output image exists, do nothing" << std::endl;
      return false;
    }

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
          }

        /* Gnomonic image exportation */
        cvSaveImage(output_image_filename.c_str() , out_img, NULL );

        /* Free memory */
        cvReleaseImage(&eqr_img);
        cvReleaseImage(&out_img);

        return true;
    }

};
