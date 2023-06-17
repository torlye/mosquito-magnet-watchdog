# mosquito-magnet-watchdog
Arduino-based watchdog for Mosquito Magnet Pioneer that keeps the trap from stopping

## The problem
The Mosquito Magnet Pioneer is an effective mosquito trap, but it has a critical flaw:
It keeps stopping, annoyingly frequently, for numerous different reasons.

Some reasons why the trap can stop working:
* The propane tank is empty
* The propane line is polluted with other gases (e.g. air)
* The propane valve was opened too quickly
* The gas nozzle is clogged
* The trap is dusty and needs cleaning
* It's a little bit too cold overnight
* It just stops sometimes for no apparent reason and works after restarting

When the trap decides to stop, obviously it no longer works. You have no idea why it stopped, there is no indicator for the type of failure.
When you restart the trap, it does not stop immediately, it runs for about 20 minutes before stopping again.
Typically you'd have to test different solutions, restarting the trap, waiting for it to stop again.
You have to run in and out, checking on the trap to see if it's running, restarting it and trying different fixes.
This can take a couple of days in each instance until the correct fix is found.

These stoppages can happen several times during the mosquito season, and while it's going on, you're not catching
mosquitoes, so the population is not kept in check.

## The solution
The watchdog device descibed here will monitor the mosquito trap by reading the current flowing from the transformer. If the current drops to near zero, the trap is assumed to have stopped, and
the watchdog device will cycle the power off and on again to restart it.

This obviously does not fix the underlying problem such as an empty propane tank. But it does fix random stoppages
in many cases. Even for the persistent issues, it will keep the trap running.
After startup, the trap typically runs for
about 20 minutes before it stops, and at that point the watchdog will restart it again.

Keeping the trap running despite being out of propane has the advantage that it will keep catching mosquitoes,
and the mosquitoes already caught cannot escape the net. Although the effecitveness of the trap is reduced without
propane, it does still attract and catch mosquitoes due to the R-Octenol chemical attractant.

Using this watchdog also makes it easier and more convenient to diagnose and fix problems. It is no longer necessary
to run in and out of the house all day to check and power cycle the trap manually.
The power cycling will happen automatically, and
by using the watchdog LCD screen, the status can be checked from indoors.

## Description

The Mosquito Magnet Pioneer runs off AC power via an external transformer that outputs 12 V AC.

The watchdog device consists of an Arduino microcontroller, a relay module and an AC current sensor module.
The relay and current sensor are connected in series to the 12 V side of the Mosquito Magnet transformer.

The current is read by the Arduino at specified intervals. If the current drops below a threshold that indicates
the trap is off, the power is cycled off and on again.

A character LCD panel can be connected to the Arduino to monitor the status at a glance.

## Arduino libraries needed
* [StateMachine by Jose Rullan](http://github.com/jrullan/StateMachine) (install via Arduino Studio Library Manager)
* LiquidCrystal_PCF8574 by Matthias Hertel (install via Arduino Studio Library Manager)
* [Filters by Jonathan Driscoll](https://github.com/JonHub/Filters) (install manually by downloading or cloning, and copying into the Arduino Studio library folder)

## Components used
* Arduino Uno
* [AC/DC current sensor ACS712](https://www.electrokit.com/produkt/stromsensor-acs712-5a/)
* [Relay module](https://www.electrokit.com/produkt/relakort-5v-opto-isolerat/)
* [HD44780 compatible 16x1 character LCD](https://www.electrokit.com/produkt/lcd-1x16-tecken-jhd161a-tn/)
* [I2C interface for LCD](https://www.electrokit.com/produkt/i2c-interface-for-lcd/)