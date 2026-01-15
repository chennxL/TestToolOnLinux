# libpsi

## requirements

### libseal

[microsoft/SEAL](https://github.com/microsoft/SEAL)

compile with `-DSEAL_THROW_ON_TRANSPARENT_CIPHERTEXT=OFF` flag

## Compile

```bash
g++ ./example.cc -o example -L./lib -lpsi -lseal -fopenmp -O3 -std=c++23
```

## Run

```bash
LD_LIBRARY_PATH=./lib ./example
```
