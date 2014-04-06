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
