# PSP blue light filter
A PSP plugin that filters out all blue light as blue light can disrupt your circadian rhythm.

Note: currently only runs in game and not on the XMB and is not compatible with all games.

## Container
Use the provided container to build and test the code. When running the container you must use `--shm-size 8G` or the tests will fail.

## Building the code and tests
From inside the container run `.github/scripts/build.sh`. Most compiler warnings are considered errors and so the build will fail if any warnings are generated. Clang-tidy and Cppcheck are also run whenever the code is built and if any errors are found by these tools then they will also cause the build to fail.

## Automated tests
A number of automated test PRX modules that can be used with PPSSPP get built that test that the blue light filter works as expected.
The automated tests don't test the full plugin end to end as PPSSPP doesn't seem to support running plugins in the background
or hooking system calls but they do test the most important code. All frame buffer formats are tested with a wide variety of
different colours and patterns.

After building the code you can use pytest to automatically run each PRX module in the headless version of PPSSPP and compare
the output to a pre-approved screenshot by running `pytest --pylint tests/tests --builddirectory=<path to build directory>`. (`--pylint` is optional.)

When adding new tests or fixing bugs you can run `python3 tests/tests/get_all_screenshots.py <path to build directory>` to automatically update the set of pre-approved screenshots by running each PRX module and capturing a screenshot. Note: after this is done all the new screenshots will have to be manually reviewed to make sure that they are correct.

There is a "filtered" and "unfiltered" version of each PRX. The filtered versions apply the blue light filter and the unfiltered versions do not. The unfiltered versions exist so that we can be sure that we are applying the filter to the correct image. For example, we may want to make sure that blue becomes black and so we need to be sure that the code used to generate a blue screen actually does generate one.
