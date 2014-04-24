/*
 * =====================================================================================
 *
 *       Filename:  testImProc.cpp
 *
 *    Description:  Test improc suite
 *
 *        Version:  1.0
 *        Created:  04/22/2014 02:46:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bart Verhagen (bv), bart.verhagen@tass.be
 *   Organization:  TASS
 *
 * =====================================================================================
 */


#include	<stdlib.h>
#include "check.h"
#include "testOpenSurfMatcher.hpp"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */

int main (void)
{
	int number_failed;
	Suite *s = testOpenSurfMatcher_suite ();
	SRunner *sr = srunner_create (s);
	srunner_run_all (sr, CK_NORMAL);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}				/* ----------  end of function main  ---------- */
