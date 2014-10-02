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

using namespace std;
using namespace cv;
using namespace elphelphg;

int main(int argc, char** argv) {

    /* Usage branch */
    if ( argc<3 || argc>4 || !strcmp( argv[1], "help" ) || !strcmp(argv[1],"-h") || !strcmp(argv[1],"--help")  ) {
        /* Display help */
        printf( "Usage : %s <imagej_prefs_xml> <input_image> [ <focal> ]\n\n",argv[0]);
        return 1;
    }

    // load inputs
    char *imagej_prefs_xml=argv[1];  //imagej xml configuration file
    char *input_image_filename=argv[2]; // eqr image (input) filename
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

    // now load image, and do gnomonic projection
    try {
      CameraArray e4pi(CameraArray::EYESIS4PI_CAMERA,imagej_prefs_xml);

      struct utils::imagefile_info *info=utils::imagefile_parsename(input_image_filename);

      int channel_index=atoi(info->channel);
      Channel *channel=e4pi.channel(channel_index);
      EqrData *eqr=channel->eqr;
      SensorData *sensor=channel->sensor;

      // load image
      IplImage* eqr_img = cvLoadImage(input_image_filename, CV_LOAD_IMAGE_COLOR );

       /* Initialize output image structure */
       IplImage* out_img = cvCreateImage( cvSize( channel->sensor->pixelCorrectionWidth, channel->sensor->pixelCorrectionHeight ), IPL_DEPTH_8U , eqr_img->nChannels );

      if(!normalizedFocal){
          /* Gnomonic projection of the equirectangular tile */
          lg_ttg_uc(
              ( inter_C8_t *) eqr_img->imageData,
              eqr_img->width,
              eqr_img->height,
              eqr_img->nChannels,
              ( inter_C8_t *) out_img->imageData,
              out_img->width,
              out_img->height,
              out_img->nChannels,
              sensor->px0,
              sensor->py0,
              eqr->imageFullWidth,
              eqr->imageFullLength-1, // there's an extra pixel for wrapping
              eqr->xPosition,
              eqr->yPosition,
              eqr->xPosition+eqr->x0,
              eqr->yPosition+eqr->y0,
              rad(sensor->roll),
              rad(sensor->azimuth),
              rad(sensor->elevation),
              rad(sensor->heading),
              0.001*sensor->pixelSize,
              sensor->focalLength,
              li_bilinearf
          );

          // create output image name
          output_image_filename+=std::string(info->dir)+"/"+info->timestamp+"-"+info->channel+"-RECT-SENSOR."+info->extension;
      }
      else
      {
        /* Gnomonic projection of the equirectangular tile */
         lg_ttg_focal(
            ( inter_C8_t *) eqr_img->imageData,
            eqr_img->width,
            eqr_img->height,
            eqr_img->nChannels,
            ( inter_C8_t *) out_img->imageData,
            out_img->width,
            out_img->height,
            out_img->nChannels,
            eqr->imageFullWidth,
            eqr->imageFullLength-1,
            eqr->xPosition,
            eqr->yPosition,
            rad(sensor->azimuth)+rad(sensor->heading)+LG_PI,
            rad(sensor->elevation),
            rad(sensor->roll),
            focal,
            0.001*sensor->pixelSize,
            li_bilinearf
          );

          // create output image name
          output_image_filename+=std::string(info->dir)+"/"+info->timestamp+"-"+info->channel+"-RECT-CONFOC."+info->extension;
      }

      /* Gnomonic image exportation */
      cvSaveImage(output_image_filename.c_str() , out_img, NULL );

    } catch(std::exception &e) {
      std::cerr << e.what() << std::endl;
      return 1;
    } catch(std::string &msg) {
      std::cerr << msg << std::endl;
      return 1;
    } catch(...) {
      std::cerr << "unhandled exception\n";
      return 1;
    }
}
