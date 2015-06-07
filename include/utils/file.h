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
 * @file file.h This file contains the API of the File class.
 * @author Jozsef Smidla
 */


#ifndef FILE_H
#define	FILE_H

#include <globals.h>
#include <utils/exceptions.h>

#include <cstdio>

/**
 * This class is used to open and read files.
 * It is able to divide the read data to blocks and jump ahead in the file
 * to the next block.
 *
 * @class File
 */
class File
{
public:

    /**
     * This exception should be thrown when the end of file is reached.
     *
     * @class EndOfFileException
     */
    class EndOfFileException
    {
    };

    /**
     * Constructor of the File class.
     *
     * @constructor
     */
    File();

    /**
     * Destructor of the File class.
     *
     * @destructor
     */
    ~File();

    /**
     * Opens a specified file for read.
     * The read buffer size and the size of blocks in the file can be also specified.
     *
     * @throws FileNotFoundException when the specified file is not found.
     *
     * @param fileName The name of the file to be opened.
     * @param bufferSize The size of the buffer while reading the file.
     * @param recordSize The size of blocks the read data will be separated into.
     */
    void openForRead(const char * fileName, int bufferSize, int recordSize);

    /**
     * Returns true if the pointer has reached the end of file.
     *
     * @param ptr The pointer to the currently read character of the file.
     * @return True if the pointer has reached the end of file.
     */
    bool endOfFile(const char * ptr);

    /**
     * Sets the pointer to the current character one block ahead.
     * The size of blocks are determined when the file is opened.
     *
     * @throws EndOfFileException if the pointer has reached the end of file.
     *
     * @param ptr The pointer to the currently read character of the file.
     * @return Pointer to the character one block ahead of ptr.
     */
    const char * nextBlock(const char * ptr);

    /**
     * Returns the pointer to the current character.
     *
     * @return The pointer to the current character.
     */
    const char * getBuffer() const;

    /**
     * Closes the file.
     */
    void close();

private:

    /**
     * The file to be read.
     */
    FILE * m_fd;

    /**
     * The buffer containing the data from the file.
     */
    char * m_buffer;

    /**
     * The size of the buffer filled up with the data from the file.
     */
    int m_bufferSize;

    /**
     * The size of blocks the read data is divided to.
     */
    int m_recordSize;

    /**
     * The total size of the file.
     */
    size_t m_size;

    /**
     * True if the pointer is in the last block of the file.
     */
    bool m_last_block;

    /**
     * Pointer pointing to the currently read character of the file.
     */
    char * m_ptr;

    /**
     * Pointer to the last character of the file.
     */
    char * m_end;
};

#endif	/* FILE_H */

