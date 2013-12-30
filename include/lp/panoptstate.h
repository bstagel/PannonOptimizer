#ifndef PANOPTSTATE_H
#define PANOPTSTATE_H

#include <globals.h>
#include <lp/model.h>

class PanOptState {
public:
    virtual void startWrting(const char * fileName,
                             const Model & model) = 0;

    virtual void finishWriting() = 0;

    virtual void addInfo(const char * info) = 0;

    virtual void startReading(const char * fileName,
                              const Model & model) = 0;

    virtual void finishReading() = 0;
};

#endif // PANOPTSTATE_H
