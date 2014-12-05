#include <cstdlib>

#include "cm/utils.hpp"
#include "findContoursTestSuite.hpp"
#include "growRegionsTestSuite.hpp"

#include "check.h"

using embeddedMT::test::growRegionsTestSuite;
using embeddedMT::test::findContoursTestSuite;

int main() {
    int number_failed;
    Suite *s;
    Suite *s_regions;
    SRunner *sr;
    SRunner *sr_regions;

    s_regions = growRegionsTestSuite();
    sr_regions = srunner_create(s_regions);
    
    srunner_run_all(sr_regions, CK_NORMAL);


    s = findContoursTestSuite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);

    srunner_free(sr_regions);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

