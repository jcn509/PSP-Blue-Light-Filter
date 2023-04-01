import argparse
import subprocess
import pathlib
import glob
import os
import shutil

from file_locations import PRX_FILES_BUILD_SUB_DIR, SCREENSHOTS_DIRECTORY, SCREENSHOT_THAT_WONT_MATCH


def _get_build_directory():
    parser = argparse.ArgumentParser()
    parser.add_argument("build_directory")

    args = parser.parse_args()
    return args.build_directory


def _get_prx_files():
    build_directory = _get_build_directory()
    return glob.glob(f"{build_directory}/{PRX_FILES_BUILD_SUB_DIR}/*.prx")


if __name__ == "__main__":
    prx_files = _get_prx_files()

    screenshot_to_copy = pathlib.Path("__testfailure.bmp")
    other_screenshot = pathlib.Path("__testcompare.png")
    for prx in prx_files:
        assert not screenshot_to_copy.exists()
        assert not other_screenshot.exists()
        print(prx)
        subprocess.run(("PPSSPPHeadless", prx, f"--screenshot={SCREENSHOT_THAT_WONT_MATCH}"), check=True)

        assert screenshot_to_copy.exists()
        assert other_screenshot.exists()
        os.unlink(other_screenshot)

        shutil.move(
            screenshot_to_copy,
            SCREENSHOTS_DIRECTORY / f"{os.path.basename(prx)}.bmp"
        )