# STM32 NUCLEO-F411RE lab setup (MIMUW "Mikrokontrolery") — reproduce on a new Debian/Ubuntu PC

  Goal: build lab solutions with the unmodified lab `makefile`, flash the board, and talk to it
  over serial, exactly like on the lab machines (students.mimuw.edu.pl), without SSH to the lab.

  ## 0. Inputs you need on the new machine

  1. `stm32-lab-bundle.tar.gz` (copied from the laptop: `~/mikrokontrolery/stm32-lab-bundle.tar.gz`).
     It contains:
     - `stm32/` = the lab's `/opt/arm/stm32` tree: `inc/`, `CMSIS/`, `src/`, `lds/`, `ocd/`, `doc/`
       (`inc` + `CMSIS` were originally scp'd from the lab; `src`, `lds`, `ocd`, `doc` were added later).
     - `arm-eabi-*` = 9 tiny wrapper scripts (see step 3) that map the lab's toolchain names to Debian's.
  2. The project sources, e.g. `~/mikrokontrolery/lab1/solution/` (makefile + *.c/*.h).
  3. The NUCLEO-F411RE board and a USB cable. The user account must be in the `dialout` group.

  ## 1. Install packages (sudo)

      sudo apt install gcc-arm-none-eabi openocd stlink-tools minicom

  - `gcc-arm-none-eabi` (Debian 12 ships 12.2.1 with newlib) replaces the lab's self-built
    `arm-eabi-gcc` 16.2. Both target Cortex-M4 thumb; the only incompatibility is that GCC 12
    spells C23 as `-std=c2x`, handled by the shim in step 3.
  - `openocd` 0.12.0 is the same version as the lab and provides `board/st_nucleo_f4.cfg`
    plus udev rules (`60-openocd.rules`) for the on-board ST-Link.
  - `stlink-tools` is optional (`st-info --probe` is handy for checking the board), also adds udev rules.
  - `minicom` is the serial terminal used in the lab.

  ## 2. Install the lab tree into /opt/arm/stm32

      sudo mkdir -p /opt/arm
      sudo tar -xzf stm32-lab-bundle.tar.gz -C /opt/arm stm32
      sudo chmod +x /opt/arm/stm32/ocd/qfn4 /opt/arm/stm32/ocd/dbgn4
      ls /opt/arm/stm32        # expect: CMSIS doc inc lds ocd src

  The lab makefile hardcodes these paths:
  - `-I/opt/arm/stm32/inc -I/opt/arm/stm32/CMSIS/Include -I/opt/arm/stm32/CMSIS/Device/ST/STM32F4xx/Include`
  - `vpath %.c /opt/arm/stm32/src` (for `startup_stm32.c`, `delay.c`, `gpio.c`)
  - `-L/opt/arm/stm32/lds -Tstm32f411re.lds` (which `INCLUDE`s `cortex-m.lds`)

  ## 3. Toolchain name shim (no sudo)

      mkdir -p ~/.local/bin
      tar -xzf stm32-lab-bundle.tar.gz -C ~/.local/bin --wildcards 'arm-eabi-*'
      chmod +x ~/.local/bin/arm-eabi-*
      echo "$PATH" | tr : '\n' | grep -q "$HOME/.local/bin" || echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc

  If the tarball is unavailable, recreate the shims by hand:

      cat > ~/.local/bin/arm-eabi-gcc <<'SH'
      #!/usr/bin/env bash
      # Lab toolchain is arm-eabi-gcc (GCC 16). Debian's arm-none-eabi-gcc 12 calls C23 "-std=c2x".
      args=()
      for a in "$@"; do
        case "$a" in -std=c23|-std=gnu23) a="${a/23/2x}" ;; esac
        args+=("$a")
      done
      exec arm-none-eabi-gcc "${args[@]}"
      SH
      for t in objcopy objdump size nm ar as ld readelf; do
        printf '#!/usr/bin/env bash\nexec arm-none-eabi-%s "$@"\n' "$t" > ~/.local/bin/arm-eabi-$t
      done
      chmod +x ~/.local/bin/arm-eabi-*

  Check: `arm-eabi-gcc --version` prints "arm-none-eabi-gcc (15:12.2.rel1-1) 12.2.1".

  ## 4. Build

      cd ~/mikrokontrolery/lab1/solution
      make clean && make
      arm-eabi-size solution.elf

  Expected: `solution.elf` and `solution.bin` with no warnings. (On the laptop: text=2864, data=0, bss=0.)
  The makefile is the lab template and must NOT be edited — it is what gets submitted.

  ## 5. Serial permissions

      groups | grep -q dialout || sudo usermod -aG dialout "$USER"   # then log out/in

  ## 6. Flash and run (board plugged into the ST-Link mini-USB port)

      lsusb | grep -i STMicro          # expect "ST-LINK/V2-1"
      ls -l /dev/ttyACM0               # ST-Link virtual COM port -> MCU USART2 (PA2/PA3)
      /opt/arm/stm32/ocd/qfn4 solution.bin
      minicom -b 9600 -D /dev/ttyACM0  # exit: Ctrl-A X ; local echo: Ctrl-A E

  `qfn4` ("quick flash nucleo f4") is the lab's script. It runs:
  `openocd -d0 -f board/st_nucleo_f4.cfg -c init -c "reset halt" -c "flash write_image erase FILE 0x8000000" -c "verify_image FILE" -c reset -c
  shutdown`.
  `dbgn4` starts `openocd -f board/st_nucleo_f4.cfg` as a GDB server (port 3333) for debugging.

  ## 7. Troubleshooting

  - `arm-eabi-gcc: command not found`: `~/.local/bin` not on PATH in this shell; `source ~/.bashrc` or re-login.
  - `cannot open linker script file stm32f411re.lds` / `No rule to make target startup_stm32.c`:
    `/opt/arm/stm32/lds` or `/opt/arm/stm32/src` missing (step 2).
  - `unrecognized command-line option '-std=c23'`: the real `arm-none-eabi-gcc` is being called directly; use the shim.
  - OpenOCD `LIBUSB_ERROR_ACCESS`: udev rules not applied; `sudo udevadm control --reload && sudo udevadm trigger`, replug.
  - `/dev/ttyACM0: Permission denied`: not in `dialout`, or not re-logged in after `usermod`.
  - SSH to students.mimuw.edu.pl (if ever needed): use `ssh -4`, the host publishes an IPv6 address first and
    machines without IPv6 get "Network is unreachable".

