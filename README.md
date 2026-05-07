This repository contains the implementation files for the PUF-based 
Fuzzy Extractor and ECC performance evaluation used in the paper. 
The code is implemented in C++ using the MIRACL cryptographic library 
and includes the following components:

- PUF simulation (SRAM-based, software model)
- Fuzzy Extractor Gen/Rep functions with syndrome-based error correction
- ECC scalar multiplication 
- SHA-256 hash computation via MIRACL
- Performance measurement (max/min/average execution time over 100 trials)
