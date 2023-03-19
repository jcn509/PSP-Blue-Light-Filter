import argparse
import subprocess
import pathlib
import glob
import os
import shutil

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("build_directory")

    args = parser.parse_args()
    build_directory = args.build_directory
    prx_files = glob.glob(f"{build_directory}/tests/*.prx")
    screenshots_directory = pathlib.Path(__file__).parent / "screenshots" 
    screenshot_that_wont_match = screenshots_directory / "failure_sanity_check.bmp"
    assert os.path.exists(screenshot_that_wont_match)
    assert os.path.isfile(screenshot_that_wont_match)
    screenshot_to_copy = pathlib.Path("__testfailure.bmp")
    other_screenshot = pathlib.Path("__testcompare.png")
    for prx in prx_files:
        assert not screenshot_to_copy.exists()
        assert not other_screenshot.exists()
        print(prx)
        subprocess.run(("PPSSPPHeadless", prx, f"--screenshot={screenshot_that_wont_match}"), check=True)

        assert screenshot_to_copy.exists()
        assert other_screenshot.exists()
        os.unlink(other_screenshot)

        shutil.move(
            screenshot_to_copy,
            screenshots_directory / f"{os.path.basename(prx)}.bmp"
        )