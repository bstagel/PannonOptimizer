#include <utils/datetime.h>
#include <sstream>
#include <ctime>

std::string DateTime::getCurrentTime() {
    time_t now = time(0);
    tm currentTime;
    char buffer[80];
    currentTime = *localtime(&now);

    strftime(buffer, sizeof(buffer), "%H.%M.%S.", &currentTime);

    return std::string(buffer);
}

std::string DateTime::getCurrentDate(DateTime::YEAR_FORMAT yearFormat,
                                     DateTime::MONTH_FORMAT monthFormat,
                                     DateTime::WEEKDAY_FORMAT weekdayFormat) {
    time_t now = time(0);
    tm currentTime;
    char buffer[80];
    currentTime = *localtime(&now);
    std::ostringstream formatStream;

    switch (yearFormat) {
    case YEAR_TWO_DIGITS:
        formatStream << "%y";
        break;
    case YEAR_FOUR_DIGITS:
        formatStream << "%Y";
        break;
    }
    formatStream << ". ";

    switch (monthFormat) {
    case MONTH_NUMBER:
        formatStream << "%m. ";
        break;
    case MONTH_ABBREVIATED:
        formatStream << "%b. ";
        break;
    case MONTH_FULL:
        formatStream << "%B ";
        break;
    }

    formatStream << "%e.";

    switch (weekdayFormat) {
    case WEEKDAY_NONE:
        break;
    case WEEKDAY_ABBREVIATED:
        formatStream << " %a";
        break;
    case WEEKDAY_FULL:
        formatStream << " %A";
        break;
    case WEEKDAY_NUMBER:
        formatStream << " %u";
        break;
    }

    strftime(buffer, sizeof(buffer), formatStream.str().c_str(), &currentTime);

    return std::string(buffer);
}

std::string DateTime::getStandardDateTime() {
    time_t now = time(0);
    tm currentTime;
    char buffer[80];
    currentTime = *localtime(&now);
    strftime(buffer, sizeof(buffer), "%c", &currentTime);

    return std::string(buffer);
}

