# BENCHMARK INSTRUKCIJE

## 📊 Šta je napravljeno?

Kreirana je kompletna benchmarking infrastruktura koja testira svih 9 hash algoritama sa:
- **6 različitih veličina ključeva**: 1k, 10k, 100k, 1M, 5M, 10M
- **2 distribucije**: Uniformna i Gausova (Gaussian)
- **Metrike**: Insert throughput, Lookup throughput, Memory usage, Build time

### Novi fajlovi:
1. `include/benchmark_suite.hpp` - Benchmark framework
2. `src/benchmark_runner.cpp` - Glavni benchmark program
3. `visualize_results.py` - Python skripta za vizuelizaciju
4. `CMakeLists.txt` - Ažuriran da kompajlira benchmark_runner

---

## 🔧 KAKO POKRENUTI BENCHMARK

### Korak 1: Kompajliranje

#### Windows (Visual Studio)
```powershell
# Pozicionirajte se u build folder
cd build

# Konfigurisanje CMake projekta (ako nije već urađeno)
cmake ..

# Build projekta u Release modu (VAŽNO za tačne performance merenja!)
cmake --build . --config Release

# Ili otvorite AdvancedHashingModels.sln u Visual Studio i kompajlirajte Release build
```

### Korak 2: Pokretanje Benchmarka

```powershell
# Iz build foldera pokrenite:
.\Release\benchmark_runner.exe

# Ili iz root foldera:
.\build\Release\benchmark_runner.exe
```

### Šta će se desiti?
- Program će testirati sve kombinacije (9 algoritama × 6 veličina × 2 distribucije = 108 testova)
- Napredak će biti prikazan u terminalu
- Na kraju će biti kreiran `benchmark_results.csv` fajl

**NAPOMENA**: Testovi sa 5M i 10M ključeva mogu trajati nekoliko minuta!

---

## 📈 VIZUELIZACIJA REZULTATA

### Korak 1: Instalacija Python zavisnosti

```powershell
pip install pandas seaborn matplotlib numpy
```

### Korak 2: Pokretanje vizuelizacije

```powershell
python visualize_results.py
```

### Šta će biti generisano?

Biće kreirane 4 slike:

1. **throughput_comparison.png**
   - 4 grafikona: Insert/Lookup × Uniform/Gaussian
   - X-osa: Broj ključeva (logaritamska skala)
   - Y-osa: Throughput (Mops/s)

2. **memory_usage_comparison.png**
   - 2 grafikona: Memory usage za obe distribucije
   - Logaritamske skale

3. **performance_heatmap.png**
   - Heatmaps sa brojevima performansi
   - Lako uočavanje najboljih algoritama za svaki scenario

4. **distribution_comparison.png**
   - Direktna komparacija Uniform vs Gaussian distribucija
   - Pokazuje kako grupisanje podataka utiče na performanse

---

## 📁 CSV FORMAT

`benchmark_results.csv` sadrži sledeće kolone:

| Kolona | Opis |
|--------|------|
| Algorithm | Ime algoritma |
| Num_Keys | Broj testiranih ključeva |
| Distribution | uniform ili gaussian |
| Insert_Ops_Per_Sec | Insert throughput (Mops/s) |
| Lookup_Ops_Per_Sec | Lookup throughput (Mops/s) |
| Memory_Usage_MB | Memorija korištenja (MB) |
| Build_Time_MS | Vreme izgradnje strukture (ms) |

---

## 🎯 CUSTOM TESTIRANJE

### Promena veličina testova

Editujte `src/benchmark_runner.cpp`, linija ~44:

```cpp
std::vector<size_t> test_sizes = {
    1000,           // 1k
    10000,          // 10k
    100000,         // 100k
    // Dodajte ili uklonite veličine po želji
};
```

### Testiranje samo određenih algoritama

U `benchmark_runner.cpp`, funkciji `createAllAlgorithms()`, zakomentarišite algoritme koje ne želite:

```cpp
std::vector<std::unique_ptr<HashModel>> createAllAlgorithms() {
    std::vector<std::unique_ptr<HashModel>> algorithms;
    
    algorithms.push_back(std::make_unique<BasicPerfectHash>());
    // algorithms.push_back(std::make_unique<BasicCryptoHash>()); // Isključeno
    algorithms.push_back(std::make_unique<BDZ_MPHF>());
    // ...
    
    return algorithms;
}
```

### Dodavanje novih distribucija

U `include/benchmark_suite.hpp`, dodajte novi tip u `KeyDistribution` enum i implementirajte generator funkciju.

---

## 💡 TIPS & TRICKS

### Za brže testiranje tokom developmenta:
```cpp
// Koristite samo male veličine
std::vector<size_t> test_sizes = {1000, 10000, 100000};
```

### Za production benchmark:
- Zatvorite sve druge programe
- Pokrenite u Release modu
- Pustite sve test veličine uključujući 10M

### Ako program pada na velikim veličinama:
- Neki algoritmi možda nemaju dovoljno RAM-a za 10M ključeva
- To je OK - benchmark će zabeležiti 0.0 za te testove
- Analizirajte koje algoritme treba optimizovati

---

## ❓ TROUBLESHOOTING

### Problem: CMake ne nalazi benchmark_runner
**Rešenje**: Obrišite build folder i ponovo pokrenite `cmake ..`

### Problem: Python ne može naći CSV
**Rešenje**: Pokrenite `visualize_results.py` iz istog foldera gde je `benchmark_results.csv`

### Problem: Benchmark traje predugo
**Rešenje**: Smanjite broj test veličina ili isključite neke algoritme

### Problem: Out of memory na 10M ključeva
**Rešenje**: Normalno je za neke algoritme - oni nisu dizajnirani za tako velike skupove

---

## 📊 INTERPRETACIJA REZULTATA

### Throughput (Mops/s)
- Više = bolje
- Mereno kao broj operacija po sekundi / 1,000,000
- Gledajte kako se performanse menjaju sa veličinom skupa

### Memory Usage (MB)
- Niže = bolje
- Pokazuje koliko RAM-a algoritam koristi
- Trade-off: brži algoritmi često koriste više memorije

### Distribution Impact
- Uporedite uniform vs gaussian grafike
- Ako algoritam ima veće razlike = osetljiv na grupisanje podataka
- Hibridni algoritmi bi trebalo da pokazuju različite performanse!

---

## 🚀 QUICK START (TL;DR)

```powershell
# 1. Kompajliranje
cd build
cmake --build . --config Release

# 2. Pokretanje benchmarka
.\Release\benchmark_runner.exe

# 3. Vizuelizacija
pip install pandas seaborn matplotlib
python visualize_results.py
```

Gotovo! Sada imate sve grafike spremne za analizu! 📊
