//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

/**
 * @file datetime.h This file contains the API of the DateTime class.
 * @author Peter Tar
 */

#ifndef DATETIME_H
#define DATETIME_H

#include <globals.h>
#include <string>

/**
 * This class allows to get the current date and time from the system in the specified format.
 *
 * @class DateTime
 */
class DateTime {
public:

    /**
     * Type descriping the format of the year in a date string.
     */
    enum YEAR_FORMAT {
        YEAR_TWO_DIGITS,
        YEAR_FOUR_DIGITS
    };

    /**
     * Type describing the format of the month in a date string.
     */
    enum MONTH_FORMAT {
        MONTH_NUMBER,
        MONTH_ABBREVIATED,
        MONTH_FULL
    };

    /**
     * Type describing the format of the day in a date string.
     */
    enum WEEKDAY_FORMAT {
        WEEKDAY_NONE,
        WEEKDAY_NUMBER,
        WEEKDAY_FULL,
        WEEKDAY_ABBREVIATED
    };

    /**
     * @return The current local time of the system formatted.
     */
    static std::string getCurrentTime();

    /**
     * Queries the current date from the system and returns it in the specified format.
     *
     * @param yearFormat The format of the year.
     * @param monthFormat The format of the month.
     * @param weekdayFormat The format of the day.
     * @return The formatted string containing the current date.
     */
    static std::string getCurrentDate(YEAR_FORMAT yearFormat,
                                      MONTH_FORMAT monthFormat,
                                      WEEKDAY_FORMAT weekdayFormat);

    /**
     * @return The date and time of the system in standard format.
     */
    static std::string getStandardDateTime();

};

#endif // DATETIME_H
