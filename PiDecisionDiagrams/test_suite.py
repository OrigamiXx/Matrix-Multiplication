PASSES = "passes"
FAILS = "fails"


def create():
    '''
    Creates a new test suite, in which one can run test cases (using assert_equals).

    :return: the new empty test suite
    '''
    suite = {PASSES: 0, FAILS: 0}
    return suite


def __pass(test_suite):
    print("PASS")
    test_suite[PASSES] += 1


def __fail(test_suite):
    print("FAIL")
    test_suite[FAILS] += 1


def assert_equals(test_suite, msg, expected, actual):
    '''
    Runs a test case, checking whether code being tested produces the correct result
    for a specific test case. Prints a message indicating whether it does.

    :param test_suite: the test suite this test case belongs to.
    :param: msg is a message to print at the beginning.
    :param: expected is the correct result
    :param: actual is the result of the code under test.
    '''
    print(msg)
    print("expected: " + str(expected))
    print("actual: " + str(actual))

    if expected == actual:
        __pass(test_suite)
    else:
        __fail(test_suite)

    print("")


def num_tests(test_suite):
    '''
    Returns the number of test cases run so far in the given test suite.

    :param test_suite: The suite of tests.
    :return: The number of test cases run so far
    '''
    return num_passes(test_suite) + num_fails(test_suite)


def num_fails(test_suite):
    '''
    Returns the number of failed test cases run so far in the given test suite.

    :param test_suite: The suite of tests.
    :return: The number of failed test cases run so far
    '''
    return test_suite[FAILS]


def num_passes(test_suite):
    '''
    Returns the number of passed test cases run so far in the given test suite.

    :param test_suite: The suite of tests.
    :return: The number of passed test cases run so far
    '''
    return test_suite[PASSES]


def print_summary(test_suite):
    '''
    Prints a summary of test suite results. Includes a tally of tests run, tests passed,
    and tests failed.

    :param test_suite: The suite of tests.
    '''
    print("%d Tests executed, %d Passed, %d Failed" %
          (num_tests(test_suite), num_passes(test_suite), num_fails(test_suite)))
