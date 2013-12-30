#ifndef DATETIME_H
#define DATETIME_H

#include <globals.h>
#include <string>

class DateTime {
public:
    enum YEAR_FORMAT {
        YEAR_TWO_DIGITS,
        YEAR_FOUR_DIGITS
    };

    enum MONTH_FORMAT {
        MONTH_NUMBER,
        MONTH_ABBREVIATED,
        MONTH_FULL
    };

    enum WEEKDAY_FORMAT {
        WEEKDAY_NONE,
        WEEKDAY_NUMBER,
        WEEKDAY_FULL,
        WEEKDAY_ABBREVIATED
    };

    static std::string getCurrentTime();

    static std::string getCurrentDate(YEAR_FORMAT yearFormat,
                                      MONTH_FORMAT monthFormat,
                                      WEEKDAY_FORMAT weekdayFormat);

    static std::string getStandardDateTime();

};

#endif // DATETIME_H
