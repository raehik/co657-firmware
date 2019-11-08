# TODOs
## Reliable reading
  * For each tag, do a timeout to ensure we don't re-scan within X secs (5s?)

## Power management
### NFC transceiver
*Assuming read-only for now.*

The transceiver only needs to be powered in the TAGCHECK state. However, that
state should generally last under 100ms (needs verification). I'm not sure if
the transceiver supports power cycling so frequently, and whether it would
actually save any power.

I need to do a few tests there. If such frequent power cycling doesn't work out,
then we'll keep the transceiver on during READY as well.

Overview of transceiver power state between state changes follows.

With constant power cycling:

  * READY->TAGCHECK = on
  * TAGCHECK->READY = off
  * READ->READY = off

Without (more likely):

  * POWER->READY = on
  * READY->IDLE = off
  * SLEEP->READY = on
