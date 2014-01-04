#include <globals.h>
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
    TCHAR username[UNLEN+1];       // TCHAR to allow for MBCS and Unicode
    DWORD len = UNLEN + 1;
    GetUserName(username, &len);
    return std::string(username);
#endif
}
