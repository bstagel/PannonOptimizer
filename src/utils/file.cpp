/**
 * @file file.cpp
 */

#include <cstring>
#include <iostream>

#include <utils/file.h>
#include <debug.h>

File::File()
{
    m_fd = 0;
    m_buffer = 0;
    m_bufferSize = 0;
    m_recordSize = 0;
    m_size = 0;
    m_last_block = false;
    m_ptr = 0;
    m_end = 0;
}

File::~File()
{
    if (m_buffer) {
        close();
    }
}

void File::openForRead(const char * fileName, int bufferSize, int recordSize)
{
    m_fd = fopen(fileName, "rb");
    if (m_fd == 0)
        throw FileNotFoundException();

    m_recordSize = recordSize;
    m_bufferSize = bufferSize;
    m_buffer = new char[m_bufferSize + 1];
    m_size = fread(m_buffer, 1, m_bufferSize, m_fd);
    m_last_block = (int) m_size < m_bufferSize;

    m_ptr = m_buffer;
    m_end = m_buffer + m_size;
    *m_end = 0;
}

const char * File::getBuffer() const
{
    return m_ptr;
}

bool File::endOfFile(const char * ptr)
{
    m_ptr = const_cast<char*> (ptr);
    return !((m_last_block && m_ptr < m_end) ||
            (!m_last_block && ((m_ptr < m_end) || !feof(m_fd))));
}

// TODO: megcsinalni, hogy a ptr elott nehany karaktert meghagyjon

const char * File::nextBlock(const char * ptr)
{

    //    std::cout << "tartalom: \"" << ptr << "\"" << std::endl;
    //    std::cin.get();

    m_ptr = const_cast<char*> (ptr);
    if (endOfFile(ptr)) {
        //    LPERROR("throw");
        throw EndOfFileException();
    }
    if (m_end - m_ptr <= m_recordSize && !m_last_block) {
        //char ch = m_buffer[m_bufferSize - 1];
        int diff = m_end - m_ptr;

        //memcpy(m_buffer, m_ptr, diff);
        unsigned int * target = (unsigned int*) m_buffer;
        const unsigned int * const end = (unsigned int*) target + diff / sizeof (unsigned int);
        unsigned int * source = (unsigned int*) m_ptr;
        while (target < end) {
            *target = *source;
            target++;
            source++;
        }
        char * target2 = (char*) target;
        const char * const end2 = (char*) target + diff % sizeof (unsigned int);
        char * source2 = (char*) source;
        while (target2 < end2) {
            *target2 = *source2;
            target2++;
            source2++;
        }


        m_size = fread(m_buffer + diff, 1, m_bufferSize - diff, m_fd);
        m_last_block = (int) m_size < m_bufferSize - diff;
        m_ptr = m_buffer;
        m_end = m_buffer + m_size + diff;
        *m_end = 0;
    }

    return m_ptr;
}

void File::close()
{
    fclose(m_fd);
    delete [] m_buffer;
    m_buffer = 0;
}
