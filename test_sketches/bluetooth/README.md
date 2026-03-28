# Information Regarding Bluetooth

## Setup Information and Flashed Parameters
The Arduino Nano dev board was used to flash the parameters onto the slave and master HC-05s. Starting with the slave, the parameters were set to each of the boards so that they will now look to connect to each ther whenever they receive powers. These boards take 5v as their VCC but require a lesser logic level of ~3.3 volts or 0.6 * 5v for more accuracy.

## Address Information:
Master: +ADDR:98d3:71:ff06c1
Slave: +ADDR:98d3:c1:fea800
Binding address (same as slave): +BIND:98d3:c1:fea800