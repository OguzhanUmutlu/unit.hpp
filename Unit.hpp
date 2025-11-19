#pragma once

#include <iostream>
#include <algorithm>

static constexpr int UNIT_HPP_VERSION_MAJOR = 0;
static constexpr int UNIT_HPP_VERSION_MINOR = 2;

namespace Unit {
    namespace extra {
        constexpr long double power_of_10(int n) {
            long double result = 1.0;
            if (n >= 0) for (int i = 0; i < n; ++i) result *= 10.0L;
            else for (int i = 0; i < -n; ++i) result /= 10.0L;
            return result;
        }

        static constexpr double pi = 3.14159265358979323846;

        template <size_t N>
        struct FixedString {
            char buf[N]{};

            // ReSharper disable once CppNonExplicitConvertingConstructor
            constexpr FixedString(const char (&str)[N]) {
                std::copy_n(str, N, buf);
            }

            friend auto operator<=>(const FixedString&, const FixedString&) = default;

            friend bool operator==(const FixedString&, const FixedString&) = default;
        };

        template <FixedString Name, int Exponent>
        struct Dim {
            static constexpr auto name = Name;

            static constexpr int exp = Exponent;
        };

        template <typename... _>
        struct Unit {
        };

        template <typename D>
        void print_single_dim(std::ostream& os) {
            os << D::name.buf;
            if (D::exp != 1) {
                os << "^" << D::exp;
            }
        }

        template <typename... Ds>
        void print_unit_impl(std::ostream& os, Unit<Ds...>) {
            size_t n = 0;
            ((print_single_dim<Ds>(os), (++n < sizeof...(Ds) ? os << "*" : os)), ...);
        }

        template <typename T, typename U>
        struct Prepend;

        template <typename T, typename... Ds>
        struct Prepend<T, Unit<Ds...>> {
            using type = Unit<T, Ds...>;
        };

        template <typename NewDim, typename CurrentUnit>
        struct AddDimToUnit;

        template <typename NewDim>
        struct AddDimToUnit<NewDim, Unit<>> {
            using type = Unit<NewDim>;
        };

        template <FixedString N, int E1, int E2, typename... Rest>
        struct AddDimToUnit<Dim<N, E1>, Unit<Dim<N, E2>, Rest...>> {
            static constexpr int sum = E1 + E2;
            using type = std::conditional_t<
                sum == 0,
                Unit<Rest...>,
                Unit<Dim<N, sum>, Rest...>
            >;
        };

        template <typename NewDim, typename Head, typename... Tail>
        struct AddDimToUnit<NewDim, Unit<Head, Tail...>> {
            using type = Prepend<Head, typename AddDimToUnit<NewDim, Unit<Tail...>>::type>::type;
        };

        template <typename UnitA, typename UnitB>
        struct MultiplyUnits;

        template <typename UnitA>
        struct MultiplyUnits<UnitA, Unit<>> {
            using type = UnitA;
        };

        template <typename UnitA, typename HeadB, typename... TailB>
        struct MultiplyUnits<UnitA, Unit<HeadB, TailB...>> {
            using type = MultiplyUnits<typename AddDimToUnit<HeadB, UnitA>::type, Unit<TailB...>>::type;
        };

        template <typename D>
        struct InvertDim;

        template <FixedString N, int E>
        struct InvertDim<Dim<N, E>> {
            using type = Dim<N, -E>;
        };

        template <typename U>
        struct InvertUnit;

        template <>
        struct InvertUnit<Unit<>> {
            using type = Unit<>;
        };

        template <typename Head, typename... Tail>
        struct InvertUnit<Unit<Head, Tail...>> {
            using type = Prepend<typename InvertDim<Head>::type, typename InvertUnit<Unit<Tail...>>::type>::type;
        };

        template <typename UnitA, typename UnitB>
        struct DivideUnits {
            using type = MultiplyUnits<UnitA, typename InvertUnit<UnitB>::type>::type;
        };
    }

    template <typename U, typename V = double>
    struct Quantity {
        V value;

        constexpr Quantity() : value(0) {
        }

        explicit constexpr Quantity(V v) : value(v) {
        }

        constexpr V operator*() const {
            return value;
        }

        constexpr auto operator*(long double rhs) const {
            return Quantity(value * rhs);
        }

        constexpr auto operator/(long double rhs) const {
            return Quantity(value / rhs);
        }

        constexpr Quantity operator+(const Quantity& rhs) const {
            return Quantity(value + rhs.value);
        }

        constexpr Quantity operator-(const Quantity& rhs) const {
            return Quantity(value - rhs.value);
        }

        constexpr Quantity operator-() const {
            return Quantity(-value);
        }

        template <typename OtherU>
        constexpr auto operator*(const Quantity<OtherU>& rhs) const {
            if constexpr (std::is_same_v<typename extra::MultiplyUnits<U, OtherU>::type, extra::Unit<>>) {
                return value * rhs.value;
            } else return Quantity<typename extra::MultiplyUnits<U, OtherU>::type>(value * rhs.value);
        }

        template <typename OtherU>
        constexpr auto operator/(const Quantity<OtherU>& rhs) const {
            if constexpr (std::is_same_v<typename extra::DivideUnits<U, OtherU>::type, extra::Unit<>>) {
                return value / rhs.value;
            } else return Quantity<typename extra::DivideUnits<U, OtherU>::type>(value / rhs.value);
        }

        friend std::ostream& operator<<(std::ostream& os, const Quantity& q) {
            os << q.value;
            if constexpr (!std::is_same_v<U, extra::Unit<>>) {
                os << " ";
                extra::print_unit_impl(os, U{});
            }
            return os;
        }
    };

    template <typename U, typename V>
    static constexpr Quantity<U, V> operator*(long double lhs, const Quantity<U, V>& rhs) {
        return Quantity<U, V>(lhs * rhs.value);
    }

    template <typename U, typename V>
    static constexpr auto operator/(long double lhs, const Quantity<U, V>& rhs) {
        return Quantity<typename extra::InvertUnit<U>::type, V>(lhs / rhs.value);
    }

    template <typename U, typename V>
    static Quantity<U, V> abs(Quantity<U, V> q) {
        return Quantity<U, V>(std::abs(q.value));
    }

    template <typename U, typename V>
    static Quantity<U, V> fmod(Quantity<U, V> q) {
        return Quantity<U, V>(std::fmod(q.value));
    }

    template <typename U, typename V>
    static Quantity<U, V> sin(Quantity<U, V> q) {
        return Quantity<U, V>(std::sin(q.value));
    }

    template <typename U, typename V>
    static Quantity<U, V> cos(Quantity<U, V> q) {
        return Quantity<U, V>(std::cos(q.value));
    }

    template <typename U, typename V>
    static Quantity<U, V> tan(Quantity<U, V> q) {
        return Quantity<U, V>(std::tan(q.value));
    }

    template <typename U, typename V>
    static Quantity<U, V> fmod(Quantity<U, V> v1, Quantity<U, V> v2) {
        return Quantity<U, V>(std::fmod(v1.value, v2.value));
    }

    namespace defaults {
#define __unithpp_def_op_num_r(ret, name, body) namespace literals { \
static constexpr ret operator ""_##name (long double _x) { double x = _x; body } \
static constexpr ret operator ""_##name (unsigned long long x) { body } \
}
#define __unithpp_def_op_num(ret, name) __unithpp_def_op_num_r(ret, name, { return ret{static_cast<double>(x)}; })
#define __unithpp_def_op_num_new(ret, name, sym) using ret = Quantity<extra::Unit<extra::Dim<sym, 1>>>; \
__unithpp_def_op_num(ret, name)
#define __unithpp_def_scale(scale, ret, name_suf, name, sym) \
using scale##ret = Quantity<extra::Unit<extra::Dim<sym, 1>>>; \
__unithpp_def_op_num(ret, name##name_suf)
#define __unithpp_def_scales(ret, name, sym) \
__unithpp_def_scale(exa, ret, name, E, sym) \
__unithpp_def_scale(peta, ret, name, P, sym) \
__unithpp_def_scale(tera, ret, name, T, sym) \
__unithpp_def_scale(giga, ret, name, G, sym) \
__unithpp_def_scale(mega, ret, name, M, sym) \
__unithpp_def_scale(kilo, ret, name, k, sym) \
__unithpp_def_scale(hecta, ret, name, h, sym) \
__unithpp_def_scale(deca, ret, name, da, sym) \
__unithpp_def_scale(deci, ret, name, d, sym) \
__unithpp_def_scale(centi, ret, name, c, sym) \
__unithpp_def_scale(milli, ret, name, m, sym) \
__unithpp_def_scale(micro, ret, name, μ, sym) \
__unithpp_def_scale(nano, ret, name, n, sym) \
__unithpp_def_scale(pico, ret, name, p, sym) \
__unithpp_def_scale(femto, ret, name, f, sym)
#define __unithpp_def_op_num_new_mag(ret, name, sym) __unithpp_def_op_num_new(ret, name, sym) \
    __unithpp_def_scales(ret, name, sym)

        using rad = Quantity<extra::Unit<extra::Dim<"rad", 1>>>;
        __unithpp_def_op_num_r(rad, deg, { return rad{x * extra::pi / 180.0}; })

        __unithpp_def_op_num_r(rad, grad, { return rad{x * extra::pi / 200.0}; })

        __unithpp_def_op_num(rad, rad)

        static rad sin(rad q) {
            return rad(std::sin(q.value));
        }

        static rad cos(rad q) {
            return rad(std::cos(q.value));
        }

        static rad tan(rad q) {
            return rad(std::tan(q.value));
        }

        using px = Quantity<extra::Unit<extra::Dim<"px", 1>>, unsigned>;

        namespace literals {
            static constexpr auto operator ""_px(unsigned long long x) {
                return px{static_cast<unsigned>(x)};
            }
        }

        __unithpp_def_op_num_new_mag(meter, m, "m")

        __unithpp_def_op_num_new_mag(second, s, "s")

        __unithpp_def_op_num_new_mag(gram, g, "g")

        __unithpp_def_op_num_new_mag(ampere, A, "A")

        __unithpp_def_op_num_new_mag(kelvin, K, "K")

        __unithpp_def_op_num_new_mag(mole, mol, "mol")

        __unithpp_def_op_num_new_mag(candela, cd, "cd")

        //// Compositions ////

        // Common
        using liter = decltype(decimeter{} * decimeter{} * decimeter{});
        using meter_per_second = decltype(meter{} / second{});
        using square_meter = decltype(meter{} * meter{});
        using cubic_meter = decltype(meter{} * meter{} * meter{});
        using meter_per_second_squared = decltype(meter{} / (second{} * second{}));
        using rad_per_second = decltype(rad{} / second{});
        using rad_per_second_squared = decltype(rad{} / (second{} * second{}));
        __unithpp_def_op_num_r(meter, mile, { return meter{x * 1609.344}; })

        __unithpp_def_op_num_r(meter, foot, { return meter{x * 0.3048}; })

        __unithpp_def_op_num_r(gram, pound, { return gram{x * 453.59237}; })

        __unithpp_def_op_num_r(second, minute, { return second{x * 60.0}; })

        __unithpp_def_op_num_r(second, hour, { return second{x * 3600.0}; })

        __unithpp_def_op_num_r(second, day, { return second{x * 86400.0}; })

        __unithpp_def_op_num_r(meter_per_second, mile_per_second, { return meter_per_second{x * 1609.344}; })

        // Mechanics
        using hertz = decltype(1 / second{});
        using newton = decltype(kilogram{} * meter{} / (second{} * second{}));
        using pascal = decltype(newton{} / (meter{} * meter{}));
        using joule = decltype(newton{} * meter{});
        using watt = decltype(joule{} / second{});
        using kilogram_per_cubic_meter = decltype(kilogram{} / (meter{} * meter{} * meter{}));
        using newton_meter = decltype(newton{} * meter{});
        using dyne = decltype(gram{} * centimeter{} / (second{} * second{}));

        // Thermodynamics
        using joule_per_kelvin = decltype(joule{} / kelvin{});
        using watt_per_meter_kelvin = decltype(watt{} / (meter{} * kelvin{}));
        using pascal_second = decltype(pascal{} * second{});
        using becquerel = decltype(1 / second{});
        __unithpp_def_op_num_r(kelvin, celsius, { return kelvin{x + 273.15}; })

        // Electromagnetism
        using coulomb = decltype(ampere{} * second{});
        using volt = decltype(watt{} / ampere{});
        using ohm = decltype(volt{} / ampere{});
        using farad = decltype(coulomb{} / volt{});
        using weber = decltype(volt{} * second{});
        using tesla = decltype(weber{} / (meter{} * meter{}));
        using henry = decltype(weber{} / ampere{});
        using siemens = decltype(ampere{} / volt{});
        using gray = decltype(joule{} / kilogram{});
        using magnetic_flux_density = tesla;
        using permittivity = decltype(farad{} / meter{});
        using permeability = decltype(henry{} / meter{});

        // Photometry
        using steradian = Quantity<extra::Unit<extra::Dim<"sr", 1>>>;
        using lumen = decltype(candela{} * steradian{});
        using lux = decltype(lumen{} / (meter{} * meter{}));
        using becquerel = decltype(1 / second{});
        using sievert = decltype(joule{} / kilogram{});
        using katal = decltype(mole{} / second{});
    }

    namespace all {
        using namespace Unit;
        using namespace Unit::defaults;
        using namespace Unit::defaults::literals;
    }
}
