import os
import pathlib

PRX_FILES_BUILD_SUB_DIR = "tests/test_prx_files"

SCREENSHOTS_DIRECTORY = pathlib.Path(__file__).parent.parent.parent / "screenshots"
assert os.path.exists(SCREENSHOTS_DIRECTORY), f"{SCREENSHOTS_DIRECTORY} exists"
assert os.path.isdir(SCREENSHOTS_DIRECTORY), f"{SCREENSHOTS_DIRECTORY} is a directory"

SCREENSHOT_THAT_WONT_MATCH = SCREENSHOTS_DIRECTORY / "failure_sanity_check.bmp"
assert os.path.exists(SCREENSHOT_THAT_WONT_MATCH), f"{SCREENSHOT_THAT_WONT_MATCH} exists"
assert os.path.isfile(SCREENSHOT_THAT_WONT_MATCH), f"{SCREENSHOT_THAT_WONT_MATCH} is a file"