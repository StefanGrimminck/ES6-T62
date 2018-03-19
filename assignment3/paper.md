# Controlling the I²C bus on the LPC3250.
Stefan Grimminck & Skip Geldens
T62
## Part 1 
We set out to make an user space program for a hardware engineer that can write registers to the  I²C bus.
The Hardware Engineer is mainly focused what he can see on this oscilloscope. So he wants to have a
program where he can enter the same bytes as he expects to see on the scope.

The LPC3250 has various I²C busses and on one I²C bus an io-expanderPCA9532 is attached. This expander is used to control LED 1 - LED 8 on the LPC3250

## Part 2
