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
