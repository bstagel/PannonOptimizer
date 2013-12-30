#include <utils/system.h>
#ifdef UNIX
#include <unistd.h>
#endif
#ifdef WIN32
#include <windows.h>
#include <Lmcons.h>
#endif

std::string System::getCurrentUserName() {
#ifdef UNIX
    return std::string( getlogin() );
#endif

#ifdef WIN32
    char username[UNLEN+1];
    GetUserName(username, UNLEN+1);
    return std::string(username);
#endif
}
