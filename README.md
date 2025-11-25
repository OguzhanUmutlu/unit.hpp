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
auto v = d / t;
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
using namespace Unit::defaults;
```

---

# **Quick Start**

```cpp
#include "Unit.hpp"
using namespace Unit::defaults;

int main() {
    auto d = 10.0_m;
    auto t = 2.0_s;
    auto v = d / t;    // 5 m/s

    std::cout << v << "\n"; // prints: 5.0 m/s
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
| u      | micro  | 10⁻⁶  |
| n      | nano   | 10⁻⁹  |
| p      | pico   | 10⁻¹² |
| f      | femto  | 10⁻¹⁵ |

Example:

```cpp
1_km, 5_mm, 3_Mg, 20_ms, 10_uA
```

---

## **Time Extensions**

| Literal   | Meaning |
|-----------|---------|
| `_minute` | minutes |
| `_hour`   | hours   |
| `_day`    | days    |
| `_week`   | weeks   |

---

## **Imperial Inputs → SI Outputs**

These are first-class citizens. They convert to base SI units automatically when used in calculations, but retain their
identity for storage and printing.

| Literal | Base unit |
|---------|-----------|
| `_mi`   | meter     |
| `_ft`   | meter     |
| `_lb`   | gram      |
| `_oz`   | gram      |

---

## **Mechanics**

| Unit | Definition                    |
|------|-------------------------------|
| Hz   | `1 / second`                  |
| N    | `kilogram * meter / second²`  |
| Pa   | `newton / meter²`             |
| J    | `newton * meter`              |
| W    | `joule / second`              |
| dyn  | `gram * centimeter / second²` |

---

## **Thermodynamics**

| Unit               | Definition       |
|--------------------|------------------|
| becquerel          | `1 / second`     |
| celsius literal    | `_degC` → kelvin |
| fahrenheit literal | `_degF` → kelvin |

---

## **Electromagnetism**

| Unit    | Definition         |
|---------|--------------------|
| coulomb | `ampere * second`  |
| volt    | `watt / ampere`    |
| ohm     | `volt / ampere`    |
| farad   | `coulomb / volt`   |
| weber   | `volt * second`    |
| tesla   | `weber / meter²`   |
| henry   | `weber / ampere`   |
| siemens | `ampere / volt`    |
| gray    | `joule / kilogram` |

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

# **Math Support**

The library provides unit-safe wrappers for common mathematical operations.

```cpp
auto x = -10.0_m;
auto y = Unit::math::abs(x); // 10m

auto a = 4.5_s;
auto b = Unit::math::ceil(a); // 5s

---

# **Printing**

The library includes an automatic unit formatter. When you print a quantity, it outputs the value followed by the
simplified unit breakdown.

```cpp
std::cout << 10.0_m << "\n";             // prints: 10.0 m
std::cout << 10.0_N << "\n";             // prints: 10.0 N
std::cout << 5.0_m * 5.0_m << "\n";      // prints: 25.0 m^2
```

Add your own formatting helpers if you want unit symbols.

---

# **Full Example**

```cpp
#include <iostream>
#include "Unit.hpp"

// Bring all literals (_m, _s, _N, etc.) into scope
using namespace Unit::defaults;

// Helper to print section headers
void print_header(const char* title) {
    std::cout << "\n================ " << title << " ================\n";
}

int main() {
    print_header("1. BASIC MOTION (Unit Inference)");
    
    // Define variables using literals.
    // Notice we can use 'mi' (miles) and 'hour' directly.
    auto distance = 60.0_mi;
    auto time     = 1.0_hour;

    // The type of 'speed' is automatically deduced.
    // It infers a compound unit: mi / hour
    auto speed = distance / time;

    std::cout << "Distance: " << distance << "\n";
    std::cout << "Time:     " << time << "\n";
    std::cout << "Speed:    " << speed << "\n";


    print_header("2. CONVERSIONS (Imperial -> SI)");

    // We want to convert that speed into meters per second (m/s).
    // The library allows explicit conversion between compatible dimensions.
    
    // 1. Define the target unit type (m/s)
    using MetersPerSecond = decltype(m{} / s{});
    
    // 2. Explicitly cast the imperial speed to SI
    MetersPerSecond speed_si(speed);

    std::cout << "Speed (Imperial): " << speed << "\n";
    std::cout << "Speed (SI):       " << speed_si << "\n";


    print_header("3. PHYSICS & DERIVED UNITS");

    // Let's calculate the Force required to accelerate a car.
    // F = m * a
    
    auto mass = 1500.0_kg;
    
    // Acceleration: 0 to 100 km/h in 5 seconds
    auto v_initial = 0.0_km / 1.0_hour;
    auto v_final   = 100.0_km / 1.0_hour;
    auto delta_t   = 5.0_s;

    // Note: We cast to m/s for the calculation to keep units clean, 
    // though the library would handle the math regardless.
    MetersPerSecond dv = MetersPerSecond(v_final) - MetersPerSecond(v_initial);
    auto acceleration = dv / delta_t;

    // F = m * a
    // The result is implicitly in [kg * m * s^-2]
    auto force_raw = mass * acceleration;

    // We can cast this raw result to Newtons to see the pretty "N" symbol
    N force_N(force_raw);

    std::cout << "Mass:         " << mass << "\n";
    std::cout << "Acceleration: " << acceleration << "\n";
    std::cout << "Force (Raw):  " << force_raw << "\n";
    std::cout << "Force (N):    " << force_N << "\n";


    print_header("4. WORK & ENERGY");

    // Work = Force * Distance
    // Let's say we apply this force for 100 meters
    auto push_dist = 100.0_m;

    auto work = force_N * push_dist;

    // Convert raw work result to Joules
    J energy_joules(work);

    // Convert Joules to Kilojoules for readability
    // We can create a scaled unit type on the fly or use 'kilo<J>' if defined,
    // but here is how to use the 'kilo' wrapper manually:
    kilo<J> energy_kJ(energy_joules);

    std::cout << "Work (J):  " << energy_joules << "\n";
    std::cout << "Work (kJ): " << energy_kJ << "\n";


    print_header("5. TRIGONOMETRY (Projectile)");

    // Calculate the horizontal range of a projectile
    // Formula: R = (v^2 * sin(2*theta)) / g
    
    auto v_launch = 50.0_m / 1.0_s;
    auto angle    = 45.0_deg;      // Input in degrees
    auto g_val    = 9.81_m / (1.0_s * 1.0_s);

    // Unit::defaults::sin takes an angle and returns a dimensionless double
    // We compute sin(2 * theta). 
    // Note: We multiply the angle by scalar 2.
    auto theta_rad = rad(angle); // Explicit view as radians
    double sin_val = std::sin(2.0 * theta_rad.value); 

    // Calculate Range
    auto range = (v_launch * v_launch * sin_val) / g_val;

    std::cout << "Launch Vel: " << v_launch << "\n";
    std::cout << "Angle:      " << angle << "\n";
    std::cout << "Range:      " << range << "\n";
    
    return 0;
}
```
