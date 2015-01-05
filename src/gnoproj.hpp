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

#ifndef GNOPROJ_HPP_
#define GNOPROJ_HPP_

#include <gnomonic-all.h>
#include <fastcal-all.h>
#include <inter-all.h>

#define DEBUG 0


const double MinFocal = 0.05;
const double MaxFocal = 500.0;


/**
 *  Sensor Data:
 *  Gather all data from the sensor.
 **/
struct SensorData {
    lf_Descriptor_t desc;

    lf_Size_t width             = 0;
    lf_Size_t height            = 0;

    lf_Size_t imageFullWidth    = 0;
    lf_Size_t imageFullHeight   = 0;
    lf_Size_t xPosition         = 0;
    lf_Size_t yPosition         = 0;

    lf_Real_t focalLength       = 0.0;
    lf_Real_t pixelSize         = 0.0;
    lf_Real_t azimuth           = 0.0;
    lf_Real_t heading           = 0.0;
    lf_Real_t elevation         = 0.0;
    lf_Real_t roll              = 0.0;

    lf_Real_t px0               = 0.0;
    lf_Real_t py0               = 0.0;

    //! Return true if success
    bool parse (unsigned sensorIndex, const std::string& macAdress,
                    const std::string& mountPoint);
};


#endif /* GNOPROJ_HPP_ */
