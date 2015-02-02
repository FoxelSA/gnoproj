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

#endif
