#ifndef UNKNOWN_H
#define UNKNOWN_H

#include <globals.h>
#include <utils/architecture.h>
#include <cstring>

#define PLATFORM_UNKNOWN


/**
 * This class implements detection of properties of the unknown architectures.
 */
class ArchitectureUnknown: public Architecture {
public:

    /**
     * Default constructor of the ArchitectureUnknown class.
     *
     * @constructor
     */
    ArchitectureUnknown();

    void detect();

    AddVecDenseToDense_NonzCount getAddVecDenseToDense_NonzCount() const;

    AddVecDenseToDense getAddVecDenseToDense() const;

protected:

};


#endif // UNKNOWN_H
