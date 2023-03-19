import glob

def pytest_addoption(parser):
    parser.addoption("--builddirectory", default="build")


def pytest_generate_tests(metafunc):
    # This is called for every test. Only get/set command line arguments
    # if the argument is specified in the list of test "fixturenames".

    option_value = metafunc.config.getoption("builddirectory")
    if 'prx_file' in metafunc.fixturenames and option_value is not None:
        test_prx_files = glob.glob(f"{option_value}/tests/*.prx")
        assert test_prx_files
        metafunc.parametrize("prx_file", test_prx_files)