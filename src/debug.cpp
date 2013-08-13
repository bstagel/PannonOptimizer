/**
 * @file debug.cpp
 */

#include <debug.h>




std::map<D::RegisteredModuleType, std::string> D::m_registeredModules =
        D::fillInModuleTypes();

int D::m_activeModules = D::fillInActiveModules();

#ifdef DEVELOPMENT

unsigned int _debug_strlen_(const std::string strr)
{
    const char* str = strr.data();
    if (!str) return 0;

    int len = 0;
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
