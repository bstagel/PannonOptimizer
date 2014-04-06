#include <simplex/pricing/simpri.h>
#include <debug.h>

Simpri::Simpri():
    m_clusterCount(0),
    m_clustersForScan(0),
    m_improvingIndicesToFound(0),
    m_currentCluster(0),
    m_investigatedClusters(0) {

}

void Simpri::init(unsigned int clusterCount,
                  unsigned int clustersForScan,
                  unsigned int improvingVectors,
                  unsigned int indexCount) {
    //LPWARNING("indexCount: " << indexCount);
    //LPWARNING("clusterCount: " << clusterCount);
    if (indexCount < clusterCount) {
        clusterCount = indexCount;
    }
    //LPWARNING("clusterCount: " << clusterCount);
    unsigned int clusterSize = indexCount / clusterCount;
    /*if (indexCount % clusterCount > 0) {
        clusterCount++;
    }*/
    //LPWARNING("clusterCount: " << clusterCount);
    m_clusters.init(indexCount, clusterCount + 1);
    m_indexToCluster.clear();
    m_indexToCluster.resize(indexCount);

    m_clusterCount = clusterCount;
    m_clustersForScan = clustersForScan;
    m_improvingIndicesToFound = improvingVectors;

    if (improvingVectors == 0) {
        m_improvingIndicesToFound = indexCount;
    }

    unsigned int index;
    unsigned int counter = 0;
    unsigned int clusterIndex = 0;
    unsigned int largerCount = indexCount % clusterCount;
    for (index = 0; index < indexCount; index++) {
        m_indexToCluster[index] = clusterIndex;
      //  LPINFO("Index: " << index << " Cluster: " << clusterIndex);
        counter++;
        if (counter >= (clusterSize + (clusterIndex < largerCount ? 1 : 0) )) {
            counter = 0;
            clusterIndex++;
        //    std::cin.get();
        }
        m_clusters.insert(clusterCount, index);
    }
    m_currentCluster = 0;
}

void Simpri::insertCandidate(unsigned int index) {
    unsigned int cluster = m_indexToCluster[index];
    m_clusters.move(index, cluster);
}

void Simpri::removeCandidate(unsigned int index) {
    m_clusters.move(index, m_clusterCount);
}

void Simpri::start() {
    //LPINFO("----------------------");
    m_improvingIndices = 0;
    m_investigatedClusters = 0;
    m_totalImprovingIndices = 0;
    m_clusters.getIterators(&m_clusterIterator,
                            &m_clusterIteratorEnd,
                            m_currentCluster);
}

bool Simpri::getCandidateIndex(unsigned int * index) {
    if (unlikely(m_totalImprovingIndices == 0 && m_investigatedClusters == m_clusterCount)) {
        //LPINFO("\tSIMRPI: no indices found");
        return false;
    }
    if (unlikely(m_totalImprovingIndices > 0 &&
                 (m_investigatedClusters >= m_clustersForScan
                  || m_improvingIndices >= m_improvingIndicesToFound) )) {
      //  LPINFO("\tSIMRPI: enough indices found");
        return false;
    }
    *index = m_clusterIterator.getData();
    m_clusterIterator++;
    if (unlikely(m_clusterIterator == m_clusterIteratorEnd)) {
        m_investigatedClusters++;
        m_currentCluster = (m_currentCluster + 1) % m_clusterCount;
        m_clusters.getIterators(&m_clusterIterator,
                                &m_clusterIteratorEnd,
                                m_currentCluster);
        m_improvingIndices = 0;
    }
    return true;
}

void Simpri::improvingIndexFound() {
    m_improvingIndices++;
    m_totalImprovingIndices++;
   // LPINFO("\tSIMRPI: new improving found");
    if (unlikely(m_improvingIndices >= m_improvingIndicesToFound)) {
        m_investigatedClusters++;
        m_currentCluster = (m_currentCluster + 1) % m_clusterCount;
        m_clusters.getIterators(&m_clusterIterator,
                                &m_clusterIteratorEnd,
                                m_currentCluster);
        m_improvingIndices = 0;
    }
}
