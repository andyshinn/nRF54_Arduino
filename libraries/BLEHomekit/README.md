# BLEHomekit

Apple HomeKit Accessory Protocol (HAP) over BLE for the nRF54L
SoftDevice. Forked from the Adafruit Bluefruit nRF52 BLEHomekit
library; the protocol code is unchanged from upstream, and the BLE
transport uses [Bluefruit54Lib](../Bluefruit54Lib/) on top of s145.

Crypto is self-contained — the library ships its own
tweetnacl-modified for Curve25519/SHA512 and a SRP implementation,
so there's no dependency on CryptoCell / `Adafruit_nRFCrypto`.

## Acknowledgments

- https://github.com/aanon4/HomeKit — reference implementation of
  HomeKit security on nrf5x
- http://tweetnacl.cr.yp.to — compact elliptic-curve and SHA-512
- https://github.com/ARMmbed/mbedtls — multi-precision math routines
  used in the SRP implementation
- http://munacl.cryptojedi.org/ — ARM-optimized Curve25519
