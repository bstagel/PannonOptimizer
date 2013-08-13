/* 
 * File:   file.h
 * Author: smidla
 *
 * Created on 2011. január 26., 18:57
 */

#ifndef FILE_H
#define	FILE_H

#include <cstdio>

/**
 *
 */
class File
{
public:

    class EndOfFileException
    {
    };

    class FileNotFoundException
    {
    };

    /**
     *
     */
    File();
    /**
     *
     */
    ~File();
    /**
     *
     * @param fileName
     * @param bufferSize
     * @param recordSize
     */
    void openForRead(const char * fileName, int bufferSize, int recordSize);
    /**
     *
     * @param ptr
     * @return
     */
    bool endOfFile(const char * ptr);
    /**
     *
     * @param ptr
     * @return
     */
    const char * nextBlock(const char * ptr);
    /**
     *
     * @return
     */
    const char * getBuffer() const;
    /**
     *
     */
    void close();

private:
    /**
     */
    FILE * m_fd;
    /**
     */
    char * m_buffer;
    /**
     */
    int m_bufferSize;
    /**
     */
    int m_recordSize;
    /**
     */
    size_t m_size;
    /**
     */
    bool m_last_block;
    /**
     */
    char * m_ptr;
    /**
     */
    char * m_end;
};

#endif	/* FILE_H */

