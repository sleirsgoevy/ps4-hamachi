# LogMeIn Hamachi for PS4

**This is not an official product by LogMeIn Inc.**

This is a port of LogMeIn Hamachi virtual LAN client to the PS4. It is done by wrapping the linux binary. As of now this only works on FW 6.72.

Non-free assets used:
* [Linux client](https://vpn.net/linux) binary
* [Hamachi icon](https://vpn.net/Content/Images/hamachi-small.png)

## Building

To build you will need the [OpenOrbis PS4 toolchain](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain) and FreeBSD headers. Place them into the `freebsd-headers` directory, then type `make`. The package will be built in the `pkg/` directory.
