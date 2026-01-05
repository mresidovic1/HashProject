# Advanced Hashing Models

![Hashing Banner](https://img.shields.io/badge/Hashing-Project-blue?style=for-the-badge)

A research and benchmarking suite for advanced hash function models, including perfect hashing, cryptographic hashing, and hybrid approaches. This project provides high-performance C++ implementations, benchmarking tools, and result visualization.

---

## Project Structure

```
├── include/         # Header files for all hash models and utilities
├── src/             # C++ source files for hash models, main, and benchmarks
├── build/           # Build output (executables, intermediate files, results)
├── CMakeLists.txt   # CMake build configuration
├── benchmark_results.csv # Output: benchmark results (CSV)
```

### Key Directories & Files

- **include/**: All hash model headers (see below for details)
- **src/**: Implementations, main entry, and benchmarking logic
- **build/**: Output directory after building (executables, results)
- **CMakeLists.txt**: CMake build script

---

## Building the Project

### Prerequisites

- C++17 compatible compiler (MSVC, GCC, or Clang)
- CMake 3.15+

### Build Instructions (Windows, Linux, macOS)

1. **Clone the repository**
   ```sh
   git clone <repo-url>
   cd hash_project
   ```
2. **Configure and build with CMake**
   ```sh
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Release
   ```
   - Executables will be in `build/Release/` (Windows) or `build/` (Linux/macOS)

---

## Running Benchmarks

- **Run all benchmarks:**
  ```sh
  # From build directory
  ./hash_project         # (Linux/macOS)
  .\Release\hash_project.exe  # (Windows)
  ```
- **Run throughput benchmark:**
  ```sh
  ./throughput_benchmark         # (Linux/macOS)
  .\Release\throughput_benchmark.exe  # (Windows)
  ```
- **Results:**
  - Benchmark results are saved as CSV in `build/` (e.g., `benchmark_results.csv`)

---

## Hash Model Overview

### Perfect Hashing

- **BasicPerfectHash (FKS):** Two-level perfect hashing (Fredman-Komlós-Szemerédi)
- **BDZ_MPHF:** Minimal perfect hash (Belazzougui-Botelho-Dietzfelbinger)
- **CuckooPerfectHash:** Cuckoo-based perfect hashing

### Cryptographic Hashing

- **BasicCryptoHash:** SHA-256 baseline implementation
- **SipHash:** Fast keyed hash (DoS-resistant)
- **BLAKE3Hash:** High-performance cryptographic hash
- **MurmurHash3:** Fast, non-cryptographic hash

### Hybrid & Advanced Models

- **UltimateHybridHash:** Multi-stage (SipHash → BDZ → BLAKE3 → cache-aware)
- **AdaptiveSecurityHybrid:** Runtime-tunable cryptographic verification
- **ParallelVerificationHybrid:** Latency hiding via parallel verification
- **CachePartitionedHybrid:** Hardware-aware, Bloom filter-optimized
- **UltraLowMemoryHybrid:** Minimal storage, on-demand verification
- **TwoPathHybrid:** Dual-lane (fast/secure) hybrid
- **HybridCryptoPerfect:** SipHash preprocessing + BDZ MPHF
- **HybridPerfectVerify:** Cuckoo perfect index + BLAKE3 verification

---

## File Descriptions

### include/

- `base_hash.hpp` — Abstract base class for all hash models
- `benchmark_suite.hpp` — Benchmarking utilities and result structures
- `basic_perfect_hash.hpp` — FKS two-level perfect hashing
- `bdz_mphf.hpp` — BDZ minimal perfect hash
- `cuckoo_perfect_hash.hpp` — Cuckoo-based perfect hash
- `basic_crypto_hash.hpp` — SHA-256 implementation
- `siphash.hpp` — SipHash-2-4 (keyed hash)
- `blake3_hash.hpp` — BLAKE3-inspired hash
- `murmur_hash.hpp` — MurmurHash3 (fast, non-crypto)
- `ultimate_hybrid.hpp` — Multi-stage hybrid (SipHash, BDZ, BLAKE3)
- `adaptive_security_hybrid.hpp` — Security-level-tunable hybrid
- `parallel_verification_hybrid.hpp` — Parallelized hybrid
- `cache_partitioned_hybrid.hpp` — Cache/Bloom filter hybrid
- `ultra_low_memory_hybrid.hpp` — Minimal memory hybrid
- `two_path_hybrid.hpp` — Dual-lane hybrid
- `hybrid_crypto_perfect.hpp` — SipHash + BDZ hybrid
- `hybrid_perfect_verify.hpp` — Cuckoo + BLAKE3 hybrid

### src/

- `main.cpp` — Main entry, runs all benchmarks
- `benchmark_runner.cpp` — Factory and runner for all models
- `throughput_benchmark.cpp` — Throughput-focused benchmark
- `*.cpp` — Implementations for each hash model

### Other

- `benchmark_results.csv` — Output: benchmark results

---

## Credits

- Authors: Mahir Rešidović and Nedim Bečić
- Research: Faculty of Electrical Engineering Sarajevo

---

## Contact

For questions, suggestions, or contributions, open an issue or contact the maintainers:

- mresidovic1@etf.unsa.ba
- nbecic1@etf.unsa
