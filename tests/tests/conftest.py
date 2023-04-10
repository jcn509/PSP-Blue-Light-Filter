"""Pytest configuration"""

import glob

from .file_locations import PRX_FILES_BUILD_SUB_DIR


def pytest_addoption(parser):
    """Add command line flags to pytest"""
    parser.addoption("--builddirectory", default="build")


def pytest_generate_tests(metafunc):
    """This is called for every test. Will automatically
    parametrise tests that have a prx_file argument with
    the path of every prx test file found in the build
    directory
    """

    option_value = metafunc.config.getoption("builddirectory")
    if "prx_file" in metafunc.fixturenames and option_value is not None:
        test_prx_files = glob.glob(f"{option_value}/{PRX_FILES_BUILD_SUB_DIR}/*.prx")
        assert test_prx_files
        metafunc.parametrize("prx_file", test_prx_files)
