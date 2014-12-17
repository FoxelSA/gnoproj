/*
 * gnoproj
 *
 * Copyright (c) 2013-2014 FOXEL SA - http://foxel.ch
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

// main executable

int main(int argc, char** argv) {

    /* Usage branch */
    if ( argc<3 || argc>4 || !strcmp( argv[1], "help" ) || !strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")  ) {
        /* Display help */
        printf( "Usage : %s <input_image> <camera mac adress> [ <focal> ]\n\n",argv[0]);
        return 1;
    }

    // load inputs
    char* input_image_filename=argv[1]; // eqr image (input) filename
    std::string mac_address(argv[2]);  //mac adress
    std::string input_image(input_image_filename);
    std::string output_image_filename; // output image filename

    // check is a focal length is given, and update method if necessary
    int  normalizedFocal(0);  // gnomonic projection method. 0 elphel method (default), 1 with constant focal
    double focal = 0.0;       // focal length (in mm)
    double minFocal = 0.05 ;  // lower bound for focal length
    double maxFocal = 500.0;  // upper bound for focal length
    std::string inputFocal((argc==4)?argv[3]:"");

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

    // extract channel information from image name
    std::vector<string>  splitted_name;
    std::vector<string>  out_split;

    split( input_image, "-", splitted_name );
    split( input_image, "_", out_split );

    int sensor_index=atoi(splitted_name[1].c_str());

    // extract information related to sensor
    /* Key/value-file descriptor */
    lf_Descriptor_t lfDesc;

    // calibration data used for gnomonic projection
    lf_Size_t lfWidth   = 0;
    lf_Size_t lfHeight  = 0;

    lf_Size_t lfImageFullWidth  = 0;
    lf_Size_t lfImageFullHeight = 0;
    lf_Size_t lfXPosition       = 0;
    lf_Size_t lfYPosition       = 0;

    lf_Real_t lfFocalLength = 0.0;
    lf_Real_t lfPixelSize   = 0.0;
    lf_Real_t lfAzimuth     = 0.0;
    lf_Real_t lfHeading     = 0.0;
    lf_Real_t lfElevation   = 0.0;
    lf_Real_t lfRoll        = 0.0;

    lf_Real_t lfpx0  = 0.0;
    lf_Real_t lfpy0  = 0.0;

    /* Creation and verification of the descriptor */
    std::string  data("/data/");
    char *c_data = new char[data.length() + 1];
    std::strcpy(c_data, data.c_str());

    char *c_mac = new char[mac_address.length() + 1];
    std::strcpy(c_mac, mac_address.c_str());

    if ( lf_parse( (unsigned char*)c_mac, (unsigned char*)c_data, & lfDesc ) == LF_TRUE ) {

      /* Query number width and height of sensor image */
      lfWidth  = lf_query_pixelCorrectionWidth ( sensor_index, & lfDesc );
      lfHeight = lf_query_pixelCorrectionHeight( sensor_index, & lfDesc );

      /* Query focal length of camera sensor index */
      lfFocalLength = lf_query_focalLength( sensor_index , & lfDesc );
      lfPixelSize   = lf_query_pixelSize  ( sensor_index , & lfDesc );

      /* Query angles used for gnomonic rotation */
      lfAzimuth     = lf_query_azimuth    ( sensor_index , & lfDesc );
      lfHeading     = lf_query_heading    ( sensor_index , & lfDesc );
      lfElevation   = lf_query_elevation  ( sensor_index , & lfDesc );
      lfRoll        = lf_query_roll       ( sensor_index , & lfDesc );

      /* Query principal point */
      lfpx0  = lf_query_px0 ( sensor_index , & lfDesc );
      lfpy0  = lf_query_py0 ( sensor_index , & lfDesc );

      /* Query information related to panoramas */
      lfImageFullWidth  = lf_query_ImageFullWidth ( sensor_index , & lfDesc );
      lfImageFullHeight = lf_query_ImageFullLength( sensor_index , & lfDesc );
      lfXPosition       = lf_query_XPosition( sensor_index , & lfDesc );
      lfYPosition       = lf_query_YPosition( sensor_index , & lfDesc );

    }
    else
    {
       std::cerr << " Could not read calibration data. " << std::endl;
       return 1;
    }

    // load image
    IplImage* eqr_img = cvLoadImage(input_image_filename, CV_LOAD_IMAGE_COLOR );

    /* Initialize output image structure */
    IplImage* out_img = cvCreateImage( cvSize( lfWidth, lfHeight ), IPL_DEPTH_8U , eqr_img->nChannels );

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
            lfpx0,
            lfpy0,
            lfImageFullWidth,
            lfImageFullHeight-1, // there's an extra pixel for wrapping
            lfXPosition,
            lfYPosition,
            lfRoll,
            lfAzimuth,
            lfElevation,
            lfHeading,
            lfPixelSize,
            lfFocalLength,
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
          lfImageFullWidth,
          lfImageFullHeight-1,
          lfXPosition,
          lfYPosition,
          lfAzimuth + lfHeading + LG_PI,
          lfElevation,
          lfRoll,
          focal,
          lfPixelSize,
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

    return 0;
}
