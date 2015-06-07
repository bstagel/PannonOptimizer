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

#ifndef SIMPRI_H
#define SIMPRI_H

#include <globals.h>
#include <utils/indexlist.h>
#include <vector>

class Simpri {
public:
    Simpri();

    void init(unsigned int clusterCount,
              unsigned int clustersForScan,
              unsigned int improvingVectors,
              unsigned int indexCount);

    void insertCandidate(unsigned int index);

    void removeCandidate(unsigned int index);

    bool getCandidateIndex(unsigned int *index);

    void start();

    void improvingIndexFound();

private:

    unsigned int m_clusterCount;

    unsigned int m_clustersForScan;

    unsigned int m_improvingIndicesToFound;

    unsigned int m_currentCluster;

    unsigned int m_investigatedClusters;

    unsigned int m_improvingIndices;

    unsigned int m_totalImprovingIndices;

    IndexList<>::Iterator m_clusterIterator;

    IndexList<>::Iterator m_clusterIteratorEnd;

    IndexList<> m_clusters;

    std::vector<unsigned int> m_indexToCluster;

};

#endif // SIMPRI_H
