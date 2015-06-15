#ifndef OSMSCOUT_GEOCOORD_H
#define OSMSCOUT_GEOCOORD_H

/*
  This source is part of the libosmscout library
  Copyright (C) 2013  Tim Teulings

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/

#include <osmscout/private/CoreImportExport.h>

#include <string>
#include <ostream>

#include <osmscout/system/Types.h>
#include <osmscout/system/Math.h>

namespace osmscout {
  /**
   * \ingroup Util
   * Coordinates will be stored as unsigned long values in file.
   * For the conversion the float value is shifted to positive
   * value and afterwards multiplied by conversion factor
   * to get long values without significant values after colon.
   */
  extern OSMSCOUT_API const double lonConversionFactor;

  /**
   * \ingroup Util
   * Coordinates will be stored as unsigned long values in file.
   * For the conversion the float value is shifted to positive
   * value and afterwards multiplied by conversion factor
   * to get long values without significant values after colon.
   */
  extern OSMSCOUT_API const double latConversionFactor;

  /**
   * \ingroup Util
   * Number of bytes needed to store a lat,lon coordinate pair.
   */
  const size_t coordByteSize=7;

  /**
   * \ingroup Geometry
   *
   * Anonymous geographic coordinate.
   */
  struct OSMSCOUT_API GeoCoord
  {
    double lat;
    double lon;

    /**
     * The default constructor creates an uninitialized instance (for performance reasons).
     */
    inline GeoCoord()
    {
      // no code
    }

    /**
     * Initialize the coordinate with the given latitude and longitude values.
     */
    inline GeoCoord(double lat,
                    double lon)
     :lat(lat),lon(lon)
    {
      // no code
    }

    /**
     * Assign a new latitude and longitude value to the coordinate
     */
    inline void Set(double lat,
                    double lon)
    {
      this->lat=lat;
      this->lon=lon;
    }

    /**
     * Return the latitude value of the coordinate
     */
    inline double GetLat() const
    {
      return lat;
    }

    /**
     * Return the latitude value of the coordinate
     */
    inline double GetLon() const
    {
      return lon;
    }

    /**
     * Return a string representation of the coordinate value in a human readable format.
     */
    std::string GetDisplayText() const;

    /**
     * Encode the coordinate value into a buffer (with at least a size of coordByteSize).
     */
    inline void EncodeToBuffer(unsigned char buffer[]) const
    {
      uint32_t latValue=(uint32_t)round((lat+90.0)*latConversionFactor);
      uint32_t lonValue=(uint32_t)round((lon+180.0)*lonConversionFactor);

      buffer[0]=((latValue >>  0) & 0xff);
      buffer[1]=((latValue >>  8) & 0xff);
      buffer[2]=((latValue >> 16) & 0xff);

      buffer[3]=((lonValue >>  0) & 0xff);
      buffer[4]=((lonValue >>  8) & 0xff);
      buffer[5]=((lonValue >> 16) & 0xff);

      buffer[6]=((latValue >> 24) & 0x07) | ((lonValue >> 20) & 0x70);
    }

    /**
     * Decode the coordinate value from a buffer (with at least a size of coordByteSize).
     */
    inline void DecodeFromBuffer(const unsigned char buffer[])
    {
      uint32_t latDat=  (buffer[0] <<  0)
                      | (buffer[1] <<  8)
                      | (buffer[2] << 16)
                      | ((buffer[6] & 0x0f) << 24);

      uint32_t lonDat=  (buffer[3] <<  0)
                      | (buffer[4] <<  8)
                      | (buffer[5] << 16)
                      | ((buffer[6] & 0xf0) << 20);

      lat=latDat/latConversionFactor-90.0;
      lon=lonDat/lonConversionFactor-180.0;
    }

    /**
     * Return true if both coordinates are equals (using == operator)
     */
    inline bool IsEqual(const GeoCoord& other) const
    {
      return lat==other.lat && lon==other.lon;
    }

    /**
     * Return true if both coordinates are equals (using == operator)
     */
    inline bool operator==(const GeoCoord& other) const
    {
      return lat==other.lat && lon==other.lon;
    }

    inline bool operator<(const GeoCoord& other) const
    {
      return lat<other.lat ||
      (lat==other.lat && lon<other.lon);
    }

    /**
     * Assign the value of other
     */
    inline void operator=(const GeoCoord& other)
    {
      this->lat=other.lat;
      this->lon=other.lon;
    }

    /**
     * Parse a textual representation of a geo coordinate from a string
     * to an GeoCoord instance.
     *
     * The text should follow the following expression:
     *
     * [+|-|N|S] DD[.DDDDD] [N|S] [+|-|W|E] DDD[.DDDDD] [W|E]
     *
     * The means:
     * * You first define the latitude, then the longitude value
     * * You can define with half you mean by either prefixing or postfixing
     * the actual number with a hint
     * * The hint can either be a sign ('-' or '+') or a direction ('N' and 'S'
     * for latitude, 'E' or 'W' for longitude).
     *
     * Possibly in future more variants will be supported. It is guaranteed
     * that the result of GetDisplayText() is successfully parsed.
     *
     * @param text
     *    Text containing the textual representation
     * @param coord
     *    The resulting coordinate, if the text was correctly parsed
     * @return
     *    true, if the text was correctly parsed, else false
     */
    static bool Parse(const std::string& text,
                      GeoCoord& coord);
  };
}

#endif
