
import contextlib
import subprocess
import time
import os
import shutil
import pathlib
from typing import Iterable, Set

from PIL import Image
from xvfbwrapper import Xvfb

PSP_DIR = pathlib.Path.home() / ".config" / "ppsspp"
PSP_SCREEN_READY_FILE = PSP_DIR / "screen_ready.txt"
PSP_SCREENSHOTS_DIR = PSP_DIR / "PSP" / "SCREENSHOT"
CORRECT_SCREENSHOTS_DIR = pathlib.Path(__file__).parent / "screenshots"

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
            Image.open(screenshot_path)
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


def _run_prx_get_screenshot(prx: str) -> pathlib.Path:
    if PSP_SCREEN_READY_FILE.exists():
        PSP_SCREEN_READY_FILE.unlink()
    existing_screenshots = _get_screenshots()

    # os.environ["QT_QPA_PLATFORM"] = "xcb"
    os.environ["QT_DEBUG_PLUGINS"] = "1"
    with Xvfb(width=1920, height=1080):
        with _run_and_kill(["PPSSPPQt", prx]):

            _wait_for_screen_ready_file()
            subprocess.run(["xdotool", "search", "PPSSPP ", "windowfocus", "key", "F12"])
            new_screenshot = _wait_for_new_screenshot(existing_screenshots)
            return new_screenshot


def _run_prx_and_check_screenshot(prx_file: str, tmp_path: pathlib.Path, expected_screenshot_path: pathlib.Path) -> None:
    assert expected_screenshot_path.exists()
    psp_screenshot_path = shutil.move(
        _run_prx_get_screenshot(prx_file),
        tmp_path
    )
    CORRECT_SCREENSHOTS_DIR

    screenshot_from_psp = Image.open(psp_screenshot_path)
    correct_screenshot = Image.open(expected_screenshot_path)

    # Might make some incorrect assumptions about how the data is encoded...
    assert list(screenshot_from_psp.getdata()) == list(correct_screenshot.getdata()), f"{psp_screenshot_path} does not match {expected_screenshot_path}"

def test_white_filtered_8888(tmp_path: pathlib.Path):
    _run_prx_and_check_screenshot("build/tests/WhiteTest8888Filtered.prx", tmp_path, CORRECT_SCREENSHOTS_DIR / "white_filtered_8888.jpg")