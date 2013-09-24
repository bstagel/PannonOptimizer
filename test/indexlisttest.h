/* 
 * File:   indexlisttest.h
 * Author: smidla
 *
 * Created on 2013. augusztus 12., 19:56
 */

#ifndef _INDEXLISTTEST_H_
#define	_INDEXLISTTEST_H_

#include <framework/tester.h>
#include <framework/unittest.h>

#include <utils/indexlist.h>

class IndexListTestSuite : public UnitTest
{
public:
    IndexListTestSuite(const char * name);

private:

    void init();
};

#endif	/* _INDEXLISTTEST_H_ */

