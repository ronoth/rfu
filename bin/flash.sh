python toggle.py write
stm32flash/stm32flash -j $2
stm32flash/stm32flash -k $2
stm32flash/stm32flash -e 0 -w $1 -v -g 0x0 $2