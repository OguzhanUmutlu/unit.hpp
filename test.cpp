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
    // It infers a compound unit: mi * hour^-1
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
    auto acceleration  = dv / delta_t;

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
    auto angle    = 45.0_deg; // Input in degrees
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
