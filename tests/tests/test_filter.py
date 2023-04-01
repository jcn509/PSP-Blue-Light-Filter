
import subprocess
import os
import shutil
import pathlib

import pytest

from .file_locations import SCREENSHOTS_DIRECTORY, SCREENSHOT_THAT_WONT_MATCH

PSP_DIR = pathlib.Path.home() / ".config" / "ppsspp"
PSP_SCREEN_READY_FILE = PSP_DIR / "screen_ready.txt"
PSP_SCREENSHOTS_DIR = PSP_DIR / "PSP" / "SCREENSHOT"

FAILURE_SCREENSHOT = pathlib.Path("__testfailure.bmp")
SCREENSHOT_WE_DONT_CARE_ABOUT = pathlib.Path("__testcompare.png")


def _run_prx_and_compare_to_screenshot(prx: pathlib.Path, compare_to_screenshot: pathlib.Path, move_screenshots_to: pathlib.Path):
    assert not FAILURE_SCREENSHOT.exists()
    assert not SCREENSHOT_WE_DONT_CARE_ABOUT.exists()

    assert compare_to_screenshot.exists()

    subprocess.run(("PPSSPPHeadless", prx, f"--screenshot={compare_to_screenshot}"), check=True)    

    if SCREENSHOT_WE_DONT_CARE_ABOUT.exists():
        shutil.move(
            SCREENSHOT_WE_DONT_CARE_ABOUT,
            move_screenshots_to
        )

    if FAILURE_SCREENSHOT.exists():
        failure_screenshot_path = shutil.move(
            FAILURE_SCREENSHOT,
            move_screenshots_to
        )
        raise RuntimeError(f"Screenshot mismatch! {failure_screenshot_path} does not match {compare_to_screenshot}")


def test_against_screenshot(prx_file: str, tmp_path: pathlib.Path):
    correct_screenshot_filename = f"{os.path.basename(prx_file)}.bmp"
    _run_prx_and_compare_to_screenshot(prx_file, SCREENSHOTS_DIRECTORY / correct_screenshot_filename, tmp_path)


def test_against_screenshot_sanity_check(prx_file: str, tmp_path: pathlib.Path):
    with pytest.raises(RuntimeError):
        _run_prx_and_compare_to_screenshot(prx_file, SCREENSHOT_THAT_WONT_MATCH, tmp_path)