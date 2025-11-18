# **Unit.hpp — Compile-Time Units Done Right. Zero Runtime Cost. Zero Mistakes.**

**Unit.hpp** is a single-header, zero-overhead C++20 dimensional-analysis library that gives you **physically correct
math at compile time** — with **no runtime penalty**, **no dependencies**, and **a complete suite of SI and derived
units**.

If you've ever added meters to seconds, multiplied the wrong units, or wished for a clean quantity type without pulling
in a massive framework — this is the solution.

Drop in one header. Get full-power dimensional safety.

---

# **Features**

### ✔ **Strong, compile-time unit types**

Every quantity knows its unit. Invalid operations simply don’t compile.

```cpp
meter d = 3.0_m;
second t = 1.0_s;
auto v = d / t;     // meter_per_second
```

### ✔ **Automatic unit algebra**

Multiply, divide, and combine units naturally. Everything stays dimensionally correct.

```cpp
auto A = 5.0_m * 4.0_m;       // square_meter
auto a = 3.0_m / (1.0_s * 1.0_s); // m/s²
auto E = 20.0_N * 2.0_m;      // joule
```

### ✔ **Safe arithmetic**

* Add/subtract only matching units
* Scalar math “just works”
* Impossible to accidentally mix incompatible quantities

### ✔ **Full user-defined literal suite**

All SI prefixes, all base units, angles, time conversions, and more.

```
1.0_m   5_km   3_cm   2_mm
4.0_s   3_ms   60_min 24_hr
45_deg  2_rad
```

### ✔ **Huge built-in unit catalog**

Length, mass, time, electromagnetism, photometry, thermodynamics, mechanics, angles — it’s all here.

### ✔ **Header-only, constexpr-based, dependency-free**

Just include `Unit.hpp`.

### ✔ **One import to rule them all**

```cpp
using namespace Unit::all;
```

---

# **Quick Start**

```cpp
#include "Unit.hpp"
using namespace Unit::all;

int main() {
    auto d = 10.0_m;
    auto t = 2.0_s;
    auto v = d / t;    // 5 m/s

    std::cout << v << "\n"; // prints the numeric value
}
```

---

# **Provided Units**

The library ships with a broad set of ready-to-use units — no configuration required.

---

## **Base Units**

| Unit    | Literal | Description              |
|---------|---------|--------------------------|
| meter   | `_m`    | length                   |
| second  | `_s`    | time                     |
| gram    | `_g`    | mass                     |
| ampere  | `_A`    | electric current         |
| kelvin  | `_K`    | temperature              |
| mole    | `_mol`  | amount of substance      |
| candela | `_cd`   | luminous intensity       |
| radian  | `_rad`  | angle                    |
| degree  | `_deg`  | angle (converted to rad) |
| gradian | `_grad` | angle (converted to rad) |
| pixel   | `_px`   | pixel count (unsigned)   |

---

## **SI Prefixes**

Every SI base unit supports:

| Prefix | Symbol | Scale |
|--------|--------|-------|
| E      | exa    | 10¹⁸  |
| P      | peta   | 10¹⁵  |
| T      | tera   | 10¹²  |
| G      | giga   | 10⁹   |
| M      | mega   | 10⁶   |
| k      | kilo   | 10³   |
| h      | hecto  | 10²   |
| da     | deca   | 10¹   |
| d      | deci   | 10⁻¹  |
| c      | centi  | 10⁻²  |
| m      | milli  | 10⁻³  |
| μ      | micro  | 10⁻⁶  |
| n      | nano   | 10⁻⁹  |
| p      | pico   | 10⁻¹² |
| f      | femto  | 10⁻¹⁵ |

Example:

```cpp
1_km, 5_mm, 3_Mg, 20_ms, 10_μA
```

---

# **Derived & Composite Units**

Everything you’d expect from a modern units library — but lightweight and constexpr.

---

## **Geometry & Kinematics**

| Unit                     | Definition        |
|--------------------------|-------------------|
| square_meter             | `meter * meter`   |
| cubic_meter              | `meter³`          |
| liter                    | `decimeter³`      |
| meter_per_second         | `meter / second`  |
| meter_per_second_squared | `meter / second²` |
| rad_per_second           | `rad / second`    |
| rad_per_second_squared   | `rad / second²`   |

---

## **Time Extensions**

| Literal | Meaning |
|---------|---------|
| `_min`  | minutes |
| `_hour` | hours   |
| `_day`  | days    |

---

## **Imperial Inputs → SI Outputs**

| Literal  | Converts to |
|----------|-------------|
| `_mile`  | meter       |
| `_foot`  | meter       |
| `_pound` | gram        |

---

## **Mechanics**

| Unit                     | Definition                    |
|--------------------------|-------------------------------|
| hertz                    | `1 / second`                  |
| newton                   | `kilogram * meter / second²`  |
| pascal                   | `newton / meter²`             |
| joule                    | `newton * meter`              |
| watt                     | `joule / second`              |
| kilogram_per_cubic_meter | `kilogram / meter³`           |
| newton_meter             | `newton * meter`              |
| dyne                     | `gram * centimeter / second²` |

---

## **Thermodynamics**

| Unit                  | Definition                |
|-----------------------|---------------------------|
| joule_per_kelvin      | `joule / kelvin`          |
| watt_per_meter_kelvin | `watt / (meter * kelvin)` |
| pascal_second         | `pascal * second`         |
| becquerel             | `1 / second`              |
| celsius literal       | `_celsius` → kelvin       |

---

## **Electromagnetism**

| Unit         | Definition         |
|--------------|--------------------|
| coulomb      | `ampere * second`  |
| volt         | `watt / ampere`    |
| ohm          | `volt / ampere`    |
| farad        | `coulomb / volt`   |
| weber        | `volt * second`    |
| tesla        | `weber / meter²`   |
| henry        | `weber / ampere`   |
| siemens      | `ampere / volt`    |
| gray         | `joule / kilogram` |
| permittivity | `farad / meter`    |
| permeability | `henry / meter`    |

---

## **Photometry**

| Unit      | Definition            |
|-----------|-----------------------|
| steradian | solid angle           |
| lumen     | `candela * steradian` |
| lux       | `lumen / meter²`      |
| sievert   | `joule / kilogram`    |
| katal     | `mole / second`       |

---

# **Angle Support**

Angles are **real unit types**:

```cpp
rad a = 90.0_deg;  // auto-converted
rad b = 1.0_rad;

auto s = sin(a);
auto c = cos(b);
```

---

# **Printing**

By default, printing a quantity prints its **value only**:

```cpp
std::cout << 5.0_m << "\n";   // prints: 5
```

Add your own formatting helpers if you want unit symbols.

---

# **Namespaces Overview**

### `Unit`

Core types: `Quantity`, dimensions, arithmetic.

### `Unit::defaults`

All predefined units + literals.

### `Unit::all`

**Recommended.** Everything in one import.

```cpp
using namespace Unit::all;
```

---

# **Full Example**

```cpp
#include "Unit.hpp"
#include <iostream>
using namespace Unit::all;

int main() {
    auto d  = 100.0_m;
    auto t  = 9.58_s;        // Usain Bolt’s 100m record
    auto v  = d / t;         // m/s

    auto a = 90.0_deg;       // radians
    auto y = sin(a);

    std::cout << "Speed: " << v << " m/s\n";
    std::cout << "sin(90°): " << y << "\n";
}
```
