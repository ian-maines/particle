# CoreScratchProject
This project exists to explore the abilities of particle devices and APIs.

Currently it is exploring using a Core device as a temperature sensor connected to a TMP102 temperature sensor.

The goal is to explore how to make these devices relaible, use them in monitoring and measurement applications, and figure out how they work.

For instance, it seems that putting the device into deep sleep after a call to Particle.Publish () without delay() results in the publish not making it to the platform
despite the documentation that Particle.Publish() is documented as blocking until the publish is complete.

Other areas of exploration:
1. Configuration of the device from the cloud (ie device name, what jobs it has, etc.)
2. Experimenting with sleep/low-power modes and how to transition in and out of those without causing issues.
3. Experimenting with socket communications and other methods of connecting to other servers/applications, ie logging readings to a database that's used by a webapp for remote monitoring.
