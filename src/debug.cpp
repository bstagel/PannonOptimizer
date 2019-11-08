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
 * @file debug.cpp
 */

#include <debug.h>




/*std::map<D::RegisteredModuleType, std::string> D::m_registeredModules =
        D::fillInModuleTypes();

int D::m_activeModules = D::fillInActiveModules();*/

std::map<D::RegisteredModuleType, std::string> * D::m_registeredModules;

int D::m_activeModules;


std::string D::getName(RegisteredModuleType mod)
{
    std::ostringstream dstr;
    dstr << std::setw(15) << std::left << (*D::m_registeredModules)[mod];
    std::string str = dstr.str().substr(0, 10);
    return str;
}

int D::getActiveModules()
{
    return D::m_activeModules;
}

void D::disableModule(RegisteredModuleType mod)
{
    D::m_activeModules &= ~mod;
}

void D::enableModule(RegisteredModuleType mod)
{
    D::m_activeModules |= mod;
}

std::map<D::RegisteredModuleType, std::string> D::fillInModuleTypes()
{
    std::map<RegisteredModuleType, std::string> map;
    map.insert(std::make_pair(D::MPSREADER, "MpsReader"));
    map.insert(std::make_pair(D::PRESOLVER, "Presolver"));
    map.insert(std::make_pair(D::CFMAKER, "ComputationalFormMaker"));
    map.insert(std::make_pair(D::STARTINGBASISFINDER, "StartingBasisFinder"));
    map.insert(std::make_pair(D::PFIMAKER, "PFIMaker"));
    map.insert(std::make_pair(D::SIMPLEX, "Simplex"));
    map.insert(std::make_pair(D::RATIOTEST, "RatioTest"));
    map.insert(std::make_pair(D::FEASIBILITYCHECK, "FeasibilityCheck"));
    map.insert(std::make_pair(D::MODEL, "Model"));
    map.insert(std::make_pair(D::PRICING, "Pricing"));
    map.insert(std::make_pair(D::SBF_SUPER, "SBF/Super"));
    map.insert(std::make_pair(D::SBF_LOGICAL, "SBF/Logical"));
    map.insert(std::make_pair(D::SBF_SYMBO, "SBF/SymboCrash"));
    map.insert(std::make_pair(D::SBF_LTSF, "SBF/LTSF"));
    map.insert(std::make_pair(D::SBF_ADG, "SBF/ADGl"));
    map.insert(std::make_pair(D::SBF_CPLEX, "SBF/CPLEX"));
    return map;
}

int D::fillInActiveModules()
{
    int am = 0;
    //        am |= D::MPSREADER;
    //        am |= D::PRESOLVER;
    //        am |= D::CFMAKER;
    //        am |= D::STARTINGBASISFINDER;
    //        am |= D::PFIMAKER;
    //        am |= D::SBF1;
    //        am |= D::SIMPLEX;
    //        am |= D::RATIOTEST;
    //        am |= D::FEASIBILITYCHECK;
    //        am |= D::MODEL;
    //        am |= D::PRICING;
    return am;
}

void D::init() {
    m_registeredModules = new std::map<RegisteredModuleType, std::string>;
    *m_registeredModules = D::fillInModuleTypes();

    m_activeModules = D::fillInActiveModules();
}

void D::release() {
    delete m_registeredModules;
    m_registeredModules = nullptr;
}



#ifdef DEVELOPMENT

unsigned int _debug_strlen_(const std::string strr)
{
    const char* str = strr.data();
    if (!str) return 0;

    unsigned int len = 0;
    bool skip = false;
    while( *str )
    {
        switch (*str)
        {
            case 0:    return len;
            case '\e': skip = true; break;
            case 'm':  if (skip) { skip = false; break; } //otherwise fall through
            /* no break */
            default:   if (!skip) len += 1; break;
        }
        str++;
    }
    return len;
}


std::string _debug_justify_ (int n) {
    int c = DEBUG_JUSTIFY_TO - n;
    std::ostringstream o; o << std::setfill (' ') << std::setw (c) << "k";
    std::string str = o.str();
    str.erase(str.size()-1, 1);
    return str;
}

const std::string _debug_file_name_(const std::string path)
 {
     if (const char *filename = strrchr(path.data(), '/')) {
         std::string s = filename+1;
         return s;
     } else {
         return path;
     }
 }

 struct Debug::Private
 {
     // static variables in functions get initialized the first time the
     // function is called
     static std::string& indentString()
     {
         static std::string s;
         return s;
     }
     static void indent() { indentString() += "    "; }
     static void deindent() { indentString().erase(indentString().size()-4,4); }

     friend class Block;
 };

 clock_t Debug::cl_start;
 clock_t Debug::cl_end;

 void Debug::init()
 {
     Debug::cl_start = clock();
     Debug::cl_end = 0;
 }

 std::string Debug::time()
 {
     Debug::cl_end += clock();
     long double m = ((long double) (Debug::cl_end - Debug::cl_start) / (long double) CLOCKS_PER_SEC);
     std::ostringstream dstr;
     dstr << std::setfill('0') << std::setw(8) << std::setprecision(4) << std::setiosflags(std::ios::fixed) << m;
     std::string str = dstr.str();
     Debug::cl_start = Debug::cl_end;
     return str;
 }

 std::string Debug::indentation()
 {
     return Private::indentString();
 }

 Debug::Block Debug::block(const std::string level, const std::string file, int line,
                             const std::string func, const std::string text)
 {
     std::string str = level + _debug_justify_(50-12);
     return Block(str, file, line, func, text);
 }

 Debug::Block::Block(const std::string level, const std::string file, int line,
                     const std::string func,  const std::string text)
     : m_level(level), m_file(file), m_func(func), m_text(text), m_line(line)
 {

#define DBLH(msg, f, white, def_color, bg_color, em_color) DEBUG_LONG_HEADER(m_level, m_file, m_line, f, white, def_color, bg_color, em_color, msg)

     std::ostringstream o;
     if (getenv("ECLIPSE")) {
         DBLH(m_text, m_func, "", "", "", "");
         o << "" << ">" << "" << m_func;
         std::string str = o.str();
         DBLH("{", str, "", "", "", "");
     } else {
         DBLH(m_text, m_func, DC_EMW, DC_D, DC_BGM, DC_EMM);
         o << DC_EMM << ">" << DC_EMW << m_func;
         std::string str = o.str();
         DBLH("{", str, DC_EMW, DC_D, DC_BGM, DC_EMM);
     }
     Private::indent();
 }

 Debug::Block::~Block()
 {
     std::ostringstream o;
     if (getenv("ECLIPSE")) {
         o << "" << "<" << "" << m_func;
         Private::deindent();
         std::string str = o.str();
         DBLH("}", str, "", "", "", "");
     } else {
         o << DC_EMM << "<" << DC_EMW << m_func;
         Private::deindent();
         std::string str = o.str();
         DBLH("}", str, DC_EMW, DC_D, DC_BGM, DC_EMM);
     }
#undef DBLH

 }


#endif
