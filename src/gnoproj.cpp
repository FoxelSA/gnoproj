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

#include <iostream>
#include <cstdlib>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "gnoproj.hpp"


typedef std::string string;


/******************************************************************************
 *  ProgramOptions
 *****************************************************************************/
struct ProgramOptions {
    string          mac;
    string          mountPoint;
    string          input;
    string          output;
    unsigned        sensorIndex = 0;
    double          focal = 0.0;
    bool            hasSensorIndex = false;
    bool            hasFocal = false;


    void parse (int argc, char** argv)
    {
        // a bit horrible, but for later use, make this class reusable in header
        // without need to include program options
        auto printUsage = [&](string e = "") {
            std::cout << argv[0]
                      << " input_file mac_address mount_point "
                      << "[-f focal] [-s sensor] [-o output_file]"
                      << std::endl;

            if(e.size())
                std::cout << std::endl << e << std::endl;
        };


        try
        {
            std::vector<string> args(argv, argv + argc);
            int k = 0;
            for(auto i = 0u; i < args.size(); i++) {
                auto& s = args[i];

                // sensor index
                if (s == "-s") {
                    sensorIndex     = std::stoul(args.at(i+1));
                    hasSensorIndex  = true;
                    ++i;
                }

                // focal
                if (s == "-f") {
                    focal           = std::stod(args.at(i+1));
                    hasFocal        = true;

                    if (focal < MinFocal || focal > MaxFocal)
                        throw string("Focal must be between "
                                        + std::to_string(MinFocal) + " and "
                                        + std::to_string(MaxFocal));

                    ++i;
                }

                // output file
                if (s == "-o") {
                    output          = args.at(i+1);
                    ++i;
                }

                // help
                if (s == "-h" || s == "--help") {
                    printUsage();
                    ++i;
                }

                // avoid storing a vector of references and so on
                switch(k) {
                    case 0: input       = s; break;
                    case 1: mac         = s; break;
                    case 2: mountPoint  = s; break;
                }
                k++;
            }

            if(k < 3)
                throw string("missing arguments");
        }
        catch(std::exception& e) {
           printUsage(std::string("Error: ") + e.what());
           std::exit(1);
        }
        catch(string& e) {
           printUsage(std::string("Error: ") + e);
           std::exit(1);
        }
    }
};


/******************************************************************************
 *  SensorData
 *****************************************************************************/
bool
SensorData::parse (unsigned sensorIndex, const string& mac, const string& mountPoint)
{
    auto r = lf_parse(  mac.c_str(),
                        mountPoint.c_str(),
                        &desc);

    if(r != LF_TRUE)
        return false;

    /* Query number width and height of sensor image */
    width  = lf_query_pixelCorrectionWidth ( sensorIndex, &desc );
    height = lf_query_pixelCorrectionHeight( sensorIndex, &desc );

    /* Query focal length of camera sensor index */
    focalLength = lf_query_focalLength( sensorIndex , &desc );
    pixelSize   = lf_query_pixelSize  ( sensorIndex , &desc );

    /* Query angles used for gnomonic rotation */
    azimuth     = lf_query_azimuth    ( sensorIndex , &desc );
    heading     = lf_query_heading    ( sensorIndex , &desc );
    elevation   = lf_query_elevation  ( sensorIndex , &desc );
    roll        = lf_query_roll       ( sensorIndex , &desc );

    /* Query principal point */
    px0  = lf_query_px0 ( sensorIndex , &desc );
    py0  = lf_query_py0 ( sensorIndex , &desc );

    /* Query information related to panoramas */
    imageFullWidth  = lf_query_ImageFullWidth ( sensorIndex , &desc );
    imageFullHeight = lf_query_ImageFullLength( sensorIndex , &desc );
    xPosition       = lf_query_XPosition( sensorIndex , &desc );
    yPosition       = lf_query_YPosition( sensorIndex , &desc );

    lf_release( &desc );
    return true;
}



/******************************************************************************
 *  Main
 *****************************************************************************/
void
ensureOptions (ProgramOptions& options)
{
    if(!options.output.size()) {
        // FIXME: on base code, output name for normalized focal
        //          has no "_" separator between options.output[0] and [1]
        auto index = options.input.find('_');
        index = options.input.find('_', index+1);

        options.output = options.input.substr(0, index) + '-';
        if(options.hasFocal)
            options.output += "RECT-CONFOC.tiff";
        else
            options.output += "RECT-SENSOR.tiff";
    }

    if(!options.hasSensorIndex) {
        // Get the sensor index that is given after the first "-" in input
        // filename
        auto index = options.input.find('-');
        options.sensorIndex = std::stoul(
                                options.input.substr(index+1),
                                nullptr, 0);
    }
}


int
main (int argc, char** argv)
{
    ProgramOptions options;
    options.parse(argc, argv);
    ensureOptions(options);

    //-- get data
    SensorData data;
    auto r = data.parse(options.sensorIndex, options.mac, options.mountPoint);
    if (!r) {
        std::cerr << "Could not read calibration data" << std::endl;
        return 1;
    }

    //-- make image using cv
    auto eqr = cv::imread(options.input, CV_LOAD_IMAGE_COLOR);
    auto out = cv::Mat(data.width, data.height, CV_8U, eqr.channels());

    if(options.hasFocal) {
        /* Gnomonic projection of the equirectangular tile */
        lg_ttg_center(
            eqr.ptr(),
            eqr.cols,
            eqr.rows,
            eqr.channels(),
            out.ptr(),
            out.cols,
            out.rows,
            out.channels(),
            data.imageFullWidth,
            data.imageFullHeight-1,
            data.xPosition,
            data.yPosition,
            data.azimuth + data.heading + LG_PI,
            data.elevation,
            data.roll,
            options.focal,
            data.pixelSize,
            li_bicubicf
        );
    }
    else {
        /* Gnomonic projection of the equirectangular tile */
        lg_ttg_elphel(
            eqr.ptr(),
            eqr.cols,
            eqr.rows,
            eqr.channels(),
            out.ptr(),
            out.cols,
            out.rows,
            out.channels(),
            data.px0,
            data.py0,
            data.imageFullWidth,
            data.imageFullHeight-1, // there's an extra pixel for wrapping
            data.xPosition,
            data.yPosition,
            data.roll,
            data.azimuth,
            data.elevation,
            data.heading,
            data.pixelSize,
            data.focalLength,
            li_bicubicf
        );
    }

    /* Gnomonic image exportation */
    cv::imwrite(options.output, out);
    return 0;
}
