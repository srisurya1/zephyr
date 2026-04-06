# TI Tiva C Series TM4C123GXL LaunchPad board configuration

# Copyright (c) 2024, Your Name
# SPDX-License-Identifier: Apache-2.0

board_runner_args(openocd "--config=${BOARD_DIR}/support/openocd.cfg")

include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)

board_set_flasher(openocd)
board_set_debugger(openocd)
