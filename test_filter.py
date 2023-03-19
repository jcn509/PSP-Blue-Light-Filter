
import contextlib
import subprocess
import glob
import time
import os
import shutil
import sys
import pathlib
from typing import Iterable, Set

import pytest

#from PIL import Image
from xvfbwrapper import Xvfb

PSP_DIR = pathlib.Path.home() / ".config" / "ppsspp"
PSP_SCREEN_READY_FILE = PSP_DIR / "screen_ready.txt"
PSP_SCREENSHOTS_DIR = PSP_DIR / "PSP" / "SCREENSHOT"
CORRECT_SCREENSHOTS_DIR = pathlib.Path(__file__).parent / "screenshots"

FAILURE_SCREENSHOT = pathlib.Path("__testfailure.bmp")
SCREENSHOT_WE_DONT_CARE_ABOUT = pathlib.Path("__testcompare.png")

def _wait_for_screen_ready_file(max_wait_time_seconds: float = 30.0, test_interval_seconds: float = 0.05) -> None:
    time_begin = time.time()
    while time.time() - time_begin < max_wait_time_seconds:
        if PSP_SCREEN_READY_FILE.exists():
            return
        time.sleep(test_interval_seconds)
    
    raise RuntimeError(f"Screen ready file not found after {max_wait_time_seconds} seconds")





@contextlib.contextmanager
def _run_and_kill(args: Iterable[str]):
    proc = subprocess.Popen(args)
    yield proc
    proc.kill()

def _get_screenshots() -> Set[pathlib.Path]:
    return set(PSP_SCREENSHOTS_DIR.iterdir()) if PSP_SCREENSHOTS_DIR.exists() else set()

def _wait_for_new_screenshot_file(existing_screenshots: Set[pathlib.Path], max_wait_time_seconds: float = 30.0, test_interval_seconds: float = 0.05) -> pathlib.Path:
    time_begin = time.time()
    while time.time() - time_begin < max_wait_time_seconds:
        screenshots = _get_screenshots()
        if len(screenshots) > len(existing_screenshots):
            new_screenshots = screenshots - existing_screenshots
            assert len(new_screenshots) == 1
            return new_screenshots.pop()
        time.sleep(test_interval_seconds)
    
    raise RuntimeError(f"Screenshot not found after {max_wait_time_seconds} seconds")


def _wait_for_screenshot_file_to_be_valid(screenshot_path: pathlib.Path, max_wait_time_seconds: float = 30.0, test_interval_seconds: float = 0.05) -> None:
    time_begin = time.time()
    while time.time() - time_begin < max_wait_time_seconds:
        try:
            return#Image.open(screenshot_path)
        except OSError:
            # File is probably still being generated
            pass
        else:
            return
        
        time.sleep(test_interval_seconds)


def _wait_for_new_screenshot(existing_screenshots: Set[pathlib.Path], max_wait_time_seconds: float = 30.0, test_interval_seconds: float = 0.05) -> pathlib.Path:
    time_begin = time.time()
    screenshot_path = _wait_for_new_screenshot_file(existing_screenshots, max_wait_time_seconds=max_wait_time_seconds, test_interval_seconds=test_interval_seconds)
    elapsed_time = time.time() - time_begin

    _wait_for_screenshot_file_to_be_valid(screenshot_path, max_wait_time_seconds=max_wait_time_seconds-elapsed_time, test_interval_seconds=test_interval_seconds)
    
    return screenshot_path


def _run_prx_and_compare_to_screenshot(prx: pathlib.Path, compare_to_screenshot: pathlib.Path, move_screenshots_to: pathlib.Path):
    assert not FAILURE_SCREENSHOT.exists()
    assert not SCREENSHOT_WE_DONT_CARE_ABOUT.exists()
    if PSP_SCREEN_READY_FILE.exists():
        PSP_SCREEN_READY_FILE.unlink()

    assert compare_to_screenshot.exists()

    subprocess.run(("PPSSPPHeadless", prx, f"--screenshot={compare_to_screenshot}"), check=True)    

    if SCREENSHOT_WE_DONT_CARE_ABOUT.exists():
        shutil.move(
            SCREENSHOT_WE_DONT_CARE_ABOUT,
            move_screenshots_to
        )

    if FAILURE_SCREENSHOT.exists():
        shutil.move(
            FAILURE_SCREENSHOT,
            move_screenshots_to
        )
        raise RuntimeError(f"Screenshot mismatch! {compare_to_screenshot} does not match {compare_to_screenshot}")

def test_against_screenshot(prx_file: str, tmp_path: pathlib.Path):
    correct_screenshot_filename = f"{os.path.basename(prx_file)}.bmp"
    _run_prx_and_compare_to_screenshot(prx_file, CORRECT_SCREENSHOTS_DIR / correct_screenshot_filename, tmp_path)


def test_against_screenshot_sanity_check(prx_file: str, tmp_path: pathlib.Path):
    with pytest.raises(RuntimeError):
        _run_prx_and_compare_to_screenshot(prx_file, CORRECT_SCREENSHOTS_DIR / "failure_sanity_check.bmp", tmp_path)