#pragma once

#include <iostream>
#include <algorithm>
#include <ratio>

static constexpr int UNIT_HPP_VERSION_MAJOR = 0;
static constexpr int UNIT_HPP_VERSION_MINOR = 3;

namespace Unit {
    using float_t = double;

    template <typename ThisUnit, typename ThisValue = float_t>
    struct Quantity;

    namespace extra {
        static constexpr float_t pi = 3.14159265358979323846;

        constexpr float_t ipow(float_t base, int exp) {
            float_t res = 1.0;
            if (exp < 0) {
                base = 1.0 / base;
                exp = -exp;
            }
            while (exp > 0) {
                if (exp % 2 == 1) res *= base;
                base *= base;
                exp /= 2;
            }
            return res;
        }

        template <size_t N>
        struct FixedString {
            char buf[N]{};

            // ReSharper disable once CppNonExplicitConvertingConstructor
            constexpr FixedString(const char (&str)[N]) {
                std::copy_n(str, N, buf);
            }

            constexpr FixedString(const char* str, size_t n) {
                std::copy_n(str, n, buf);
            }

            friend auto operator<=>(const FixedString&, const FixedString&) = default;

            friend bool operator==(const FixedString&, const FixedString&) = default;

            constexpr bool empty() const {
                return buf[0] == '\0';
            }
        };

        template <size_t N>
        FixedString(const char (&)[N]) -> FixedString<N>;

        template <typename T>
        static constexpr T return_self(T v) {
            return v;
        }

        // BaseSymbol = The base unit symbol, e.g., "m" for meter and even for mile
        // Exponent = The exponent of the base unit in the compound unit, e.g., 2 for m^2
        // ScaleRatio = The scaling ratio, like milli = 1/1000 etc. Default is 1 (no scaling)
        // Prefix = The prefix symbol for the base unit, e.g., "k" for kilo
        // SubSymbol = The sub-symbol for the base unit, e.g., "mi" for mile
        // SubToBase = The formula for converting the subunit to the base unit
        // BaseToSub = The formula for converting the base unit to the subunit

        // For kilomile: BaseUnit<"m", 1, std::ratio<1000>, "k", "mi", mile_to_meter, meter_to_mile>
        template <FixedString BaseSymbol, int Exponent = 1,
                  typename ScaleRatio = std::ratio<1>, FixedString Prefix = "",
                  FixedString SubSymbol = BaseSymbol, auto SubToBase = return_self<float_t>,
                  auto BaseToSub = return_self<float_t>>
        struct BaseUnit {
            static constexpr auto base_symbol = BaseSymbol;
            static constexpr auto exp = Exponent;
            static constexpr auto sub_symbol = SubSymbol;
            static constexpr auto prefix = Prefix;
            using scale = ScaleRatio;
        };

        template <typename... _>
        struct Unit {
        };

        template <typename T>
        struct is_single_unit : std::false_type {
        };

        template <typename Base>
        struct is_single_unit<Unit<Base>> : std::true_type {
            using base_type = Base;
        };

        template <typename BaseUnit>
        void print_base_unit(std::ostream& os) {
            if constexpr (BaseUnit::exp == 0) return;
            os << BaseUnit::prefix.buf << BaseUnit::sub_symbol.buf;
            if constexpr (BaseUnit::exp != 1) {
                os << "^" << BaseUnit::exp;
            }
        }

        template <typename... BaseUnits>
        void print_unit_impl(std::ostream& os, Unit<BaseUnits...>) {
            size_t n = 0;
            ((print_base_unit<BaseUnits>(os), (++n < sizeof...(BaseUnits) ? os << "*" : os)), ...);
        }

        template <typename T, typename U>
        struct PrependBaseUnit;

        template <typename HeadBaseUnit, typename... TailBaseUnits>
        struct PrependBaseUnit<HeadBaseUnit, Unit<TailBaseUnits...>> {
            using type = Unit<HeadBaseUnit, TailBaseUnits...>;
        };

        template <typename NewBaseUnit, typename CurrentUnit>
        struct MultiplyBaseUnitByUnit;

        template <typename BaseUnitA>
        struct MultiplyBaseUnitByUnit<BaseUnitA, Unit<>> {
            using type = Unit<BaseUnitA>;
        };

        template <FixedString BaseSymbol, int Exponent1, int Exponent2, typename Scale, FixedString Prefix,
                  FixedString SubSymbol, auto SubToBase, auto BaseToSub, typename... Rest>
        struct MultiplyBaseUnitByUnit<
                BaseUnit<BaseSymbol, Exponent1, Scale, Prefix, SubSymbol, SubToBase, BaseToSub>,
                Unit<BaseUnit<BaseSymbol, Exponent2, Scale, Prefix, SubSymbol, SubToBase, BaseToSub>, Rest...>
            > {
            static constexpr int sum = Exponent1 + Exponent2;
            using type = std::conditional_t<
                sum == 0,
                Unit<Rest...>,
                Unit<BaseUnit<BaseSymbol, sum, Scale, Prefix, SubSymbol, SubToBase, BaseToSub>, Rest...>
            >;
        };

        template <typename BaseUnitA, typename Head, typename... Tail>
        struct MultiplyBaseUnitByUnit<BaseUnitA, Unit<Head, Tail...>> {
            using type = PrependBaseUnit<Head, typename MultiplyBaseUnitByUnit<BaseUnitA, Unit<Tail...>>::type>::type;
        };

        template <typename UnitA, typename UnitB>
        struct MultiplyUnits;

        template <typename UnitA>
        struct MultiplyUnits<UnitA, Unit<>> {
            using type = UnitA;
        };

        template <typename _Unit, typename HeadBaseUnit, typename... TailBaseUnits>
        struct MultiplyUnits<_Unit, Unit<HeadBaseUnit, TailBaseUnits...>> {
            using type = MultiplyUnits<typename MultiplyBaseUnitByUnit<HeadBaseUnit, _Unit>::type, Unit<TailBaseUnits
                                           ...>>::type;
        };

        template <typename D>
        struct InvertBaseUnit;

        template <FixedString BaseSymbol, int Exponent, typename Scale, FixedString Prefix,
                  FixedString SubSymbol, auto SubToBase, auto BaseToSub>
        struct InvertBaseUnit<BaseUnit<BaseSymbol, Exponent, Scale, Prefix, SubSymbol, SubToBase, BaseToSub>> {
            using type = BaseUnit<BaseSymbol, -Exponent, Scale, Prefix, SubSymbol, SubToBase, BaseToSub>;
        };

        template <typename U>
        struct InvertUnit;

        template <>
        struct InvertUnit<Unit<>> {
            using type = Unit<>;
        };

        template <typename Head, typename... Tail>
        struct InvertUnit<Unit<Head, Tail...>> {
            using type = PrependBaseUnit<typename InvertBaseUnit<Head>::type, typename InvertUnit<Unit<Tail
                                             ...>>::type>::type;
        };

        template <typename UnitA, typename UnitB>
        struct DivideUnits {
            using type = MultiplyUnits<UnitA, typename InvertUnit<UnitB>::type>::type;
        };

        template <typename T, typename Scalar, FixedString NewPrefix>
        struct ApplyScale {
            static_assert(sizeof(T) == 0,
                          "Error: Prefix scaling (e.g., atto<T>) can only be applied to a simple Base Unit with Exponent 1 (e.g., meter, gram). "
                          "Compound units (kg*m) or powers (m^2) are not allowed.");
        };

        template <FixedString BaseSymbol, typename OldScale,
                  FixedString SubSymbol, auto SubToBase, auto BaseToSub,
                  typename ValueType, typename Scalar, FixedString NewPrefix>
        struct ApplyScale<
                Quantity<Unit<BaseUnit<BaseSymbol, 1, OldScale, "", SubSymbol, SubToBase, BaseToSub>>, ValueType>,
                Scalar,
                NewPrefix
            > {
            static_assert(std::is_same_v<OldScale, std::ratio<1>>,
                          "Error: Compound prefixes (e.g., deci<deci<meter>>) are not allowed by SI rules. "
                          "Use the correct combined prefix (e.g., centi<meter>) instead.");
            using NewScale = std::ratio_multiply<OldScale, Scalar>;
            using type = Quantity<Unit<BaseUnit<BaseSymbol, 1, NewScale, NewPrefix, SubSymbol, SubToBase, BaseToSub>>,
                                  ValueType>;
        };
    }

    template <typename ThisUnit, typename ThisValue>
    struct Quantity {
        ThisValue raw_value;

        using UnitType = ThisUnit;

        constexpr Quantity() : raw_value(0) {
        }

        explicit constexpr Quantity(ThisValue v) : raw_value(v) {
        }

        template <typename OtherUnit, typename OtherValue>
            requires (
                extra::is_single_unit<ThisUnit>::value
                && extra::is_single_unit<OtherUnit>::value
                && (extra::is_single_unit<ThisUnit>::base_type::base_symbol
                    == extra::is_single_unit<OtherUnit>::base_type::base_symbol)
                && (extra::is_single_unit<ThisUnit>::base_type::exp
                    == extra::is_single_unit<OtherUnit>::base_type::exp)
            )
        explicit constexpr Quantity(const Quantity<OtherUnit, OtherValue>& rhs) {
            using TargetBase = extra::is_single_unit<ThisUnit>::base_type;
            using SourceBase = extra::is_single_unit<OtherUnit>::base_type;
            using SourceFactor = std::ratio_multiply<typename SourceBase::scale, typename SourceBase::sub_ratio>;
            using TargetFactor = std::ratio_multiply<typename TargetBase::scale, typename TargetBase::sub_ratio>;
            using Conversion = std::ratio_divide<SourceFactor, TargetFactor>;
            constexpr float_t factor = static_cast<float_t>(Conversion::num) / static_cast<float_t>(Conversion::den);
            raw_value = static_cast<ThisValue>(rhs.raw_value * extra::ipow(factor, TargetBase::exp));
        }

        constexpr ThisValue operator*() const {
            return raw_value;
        }

        constexpr auto operator*(long double rhs) const {
            return Quantity(raw_value * rhs);
        }

        constexpr auto operator/(long double rhs) const {
            return Quantity(raw_value / rhs);
        }

        constexpr Quantity operator+(const Quantity& rhs) const {
            return Quantity(raw_value + rhs.raw_value);
        }

        constexpr Quantity operator-(const Quantity& rhs) const {
            return Quantity(raw_value - rhs.raw_value);
        }

        constexpr Quantity operator-() const {
            return Quantity(-raw_value);
        }

        template <typename OtherUnit>
        constexpr auto operator*(const Quantity<OtherUnit>& rhs) const {
            using ResultUnit = extra::MultiplyUnits<ThisUnit, OtherUnit>::type;
            if constexpr (std::is_same_v<ResultUnit, extra::Unit<>>) {
                return raw_value * rhs.raw_value;
            } else return Quantity<ResultUnit>(raw_value * rhs.raw_value);
        }

        template <typename OtherUnit>
        constexpr auto operator/(const Quantity<OtherUnit>& rhs) const {
            if constexpr (std::is_same_v<typename extra::DivideUnits<ThisUnit, OtherUnit>::type, extra::Unit<>>) {
                return raw_value / rhs.raw_value;
            } else return Quantity<typename extra::DivideUnits<ThisUnit, OtherUnit>::type>(raw_value / rhs.raw_value);
        }

        friend std::ostream& operator<<(std::ostream& os, const Quantity& q) {
            os << q.raw_value;
            if constexpr (!std::is_same_v<ThisUnit, extra::Unit<>>) {
                os << " ";
                extra::print_unit_impl(os, ThisUnit{});
            }
            return os;
        }
    };

    template <typename U, typename V>
    static constexpr Quantity<U, V> operator*(long double lhs, const Quantity<U, V>& rhs) {
        return Quantity<U, V>(lhs * rhs.raw_value);
    }

    template <typename U, typename V>
    static constexpr auto operator/(long double lhs, const Quantity<U, V>& rhs) {
        return Quantity<typename extra::InvertUnit<U>::type, V>(lhs / rhs.raw_value);
    }

#define __unithpp_bring_math_func(NAME) \
    template <typename U, typename V> \
    static Quantity<U, V> NAME(Quantity<U, V> q) { \
        return Quantity<U, V>(std::NAME(q.raw_value)); \
    }

    __unithpp_bring_math_func(abs)
    __unithpp_bring_math_func(fmod)
    __unithpp_bring_math_func(log)
    __unithpp_bring_math_func(log2)
    __unithpp_bring_math_func(log10)
    __unithpp_bring_math_func(pow)
    __unithpp_bring_math_func(sqrt)
    __unithpp_bring_math_func(cbrt)

#define __unithpp_SCALE(NAME, SYMBOL) \
template <typename Q> \
using NAME = typename Unit::extra::ApplyScale<Q, std::NAME, #SYMBOL>::type;
#define __unithpp_LITERAL(NAME, SHORT) \
namespace literals { \
    constexpr NAME operator ""_##SHORT(long double x) { \
        return NAME(x); \
    } \
    constexpr NAME operator ""_##SHORT(unsigned long long x) { \
        return NAME(static_cast<float_t>(x)); \
    } \
}
#define __unithpp_BASE_NOSCALE(NAME, SHORT) \
using NAME = Quantity<extra::Unit<extra::BaseUnit<#SHORT>>>; \
__unithpp_LITERAL(NAME, SHORT)
#define __unithpp_BASE(NAME, SHORT) \
__unithpp_BASE_NOSCALE(NAME, SHORT) \
__unithpp_SCALES(NAME, SHORT)

#define __unithpp_SUB(NAME, ORIGIN, SUB_SYMBOL, SUB_TO_BASE, BASE_TO_SUB) \
__unithpp_SUB_NOSCALE(NAME, ORIGIN, SUB_SYMBOL, SUB_TO_BASE, BASE_TO_SUB) \
__unithpp_SCALES(NAME, SUB_SYMBOL)
#define __unithpp_SUB_NOSCALE(NAME, ORIGIN, SUB_SYMBOL, SUB_TO_BASE, BASE_TO_SUB) \
namespace temp_functions { \
    constexpr float_t __s2b__##NAME(float_t x) SUB_TO_BASE \
    constexpr float_t __b2s__##NAME(float_t x) BASE_TO_SUB \
} \
using NAME = Quantity<extra::Unit<extra::BaseUnit<#ORIGIN, 1, std::ratio<1>, "", #SUB_SYMBOL, temp_functions::__s2b__##NAME, temp_functions::__b2s__##NAME>>>; \
__unithpp_LITERAL(NAME, SUB_SYMBOL)
#define __unithpp_SUB_RATIO(NAME, ORIGIN, SUB_SYMBOL, NUM, DEN) \
__unithpp_SUB_RATIO_NOSCALE(NAME, ORIGIN, SUB_SYMBOL, NUM, DEN) \
__unithpp_SCALES(NAME, SUB_SYMBOL)
#define __unithpp_SUB_RATIO_NOSCALE(NAME, ORIGIN, SUB_SYMBOL, NUM, DEN) \
__unithpp_SUB_NOSCALE(NAME, ORIGIN, SUB_SYMBOL, { \
    return static_cast<float_t>(NUM) / static_cast<float_t>(DEN) * x; \
}, { \
    return static_cast<float_t>(DEN) / static_cast<float_t>(NUM) * x; \
})

#define __unithpp_COMPOUND(NAME, SHORT, FORMULA) \
using NAME = decltype(FORMULA); \
__unithpp_LITERAL(NAME, SHORT)

#define __unithpp_SCALES(NAME, SHORT) \
__unithpp_LITERAL(atto<NAME>, a##SHORT) \
__unithpp_LITERAL(femto<NAME>, f##SHORT) \
__unithpp_LITERAL(pico<NAME>, p##SHORT) \
__unithpp_LITERAL(nano<NAME>, n##SHORT) \
__unithpp_LITERAL(micro<NAME>, μ##SHORT) \
__unithpp_LITERAL(milli<NAME>, m##SHORT) \
__unithpp_LITERAL(centi<NAME>, c##SHORT) \
__unithpp_LITERAL(deci<NAME>, d##SHORT) \
__unithpp_LITERAL(deca<NAME>, da##SHORT) \
__unithpp_LITERAL(hecto<NAME>, h##SHORT) \
__unithpp_LITERAL(kilo<NAME>, k##SHORT) \
__unithpp_LITERAL(mega<NAME>, M##SHORT) \
__unithpp_LITERAL(giga<NAME>, G##SHORT) \
__unithpp_LITERAL(tera<NAME>, T##SHORT) \
__unithpp_LITERAL(peta<NAME>, P##SHORT) \
__unithpp_LITERAL(exa<NAME>, E##SHORT)

    __unithpp_SCALE(atto, a)
    __unithpp_SCALE(femto, f)
    __unithpp_SCALE(pico, p)
    __unithpp_SCALE(nano, n)
    __unithpp_SCALE(micro, μ)
    __unithpp_SCALE(milli, m)
    __unithpp_SCALE(centi, c)
    __unithpp_SCALE(deci, d)
    __unithpp_SCALE(deca, da)
    __unithpp_SCALE(hecto, h)
    __unithpp_SCALE(kilo, k)
    __unithpp_SCALE(mega, M)
    __unithpp_SCALE(giga, G)
    __unithpp_SCALE(tera, T)
    __unithpp_SCALE(peta, P)
    __unithpp_SCALE(exa, E)

    namespace defaults {
        __unithpp_BASE(meter, m)

        __unithpp_BASE(gram, g)

        __unithpp_BASE(second, s)

        __unithpp_BASE(ampere, A)

        __unithpp_BASE(kelvin, K)

        __unithpp_BASE(mole, mol)

        __unithpp_BASE(candela, cd)

        __unithpp_BASE_NOSCALE(radians, rad) // special useful unit

        __unithpp_BASE_NOSCALE(pixels, px) // special useful unit

        __unithpp_COMPOUND(liter, L, deci<meter>{} * deci<meter>{} * deci<meter>{})

        __unithpp_COMPOUND(meter_per_second, mps, meter{} / second{})

        __unithpp_COMPOUND(square_meter, m2, meter{} * meter{})

        __unithpp_COMPOUND(cubic_meter, m3, meter{} * meter{} * meter{})

        __unithpp_COMPOUND(meter_per_second_squared, mps2, meter{} / (second{} * second{}))

        __unithpp_COMPOUND(rad_per_second, rads, radians{} / second{})

        __unithpp_COMPOUND(rad_per_second_squared, rads2, radians{} / (second{} * second{}))

        __unithpp_SUB_RATIO(mile, m, mi, 1609344, 1000)

        __unithpp_SUB_RATIO(foot, m, ft, 3048, 10000)

        __unithpp_SUB_RATIO(pound, g, lb, 45359237, 100000)

        __unithpp_SUB_RATIO(ounce, g, oz, 28349523125, 1000000000)

        __unithpp_SUB_RATIO(minute, s, min, 60, 1)

        __unithpp_SUB_RATIO(hour, s, hr, 3600, 1)

        __unithpp_SUB_RATIO(day, s, day, 86400, 1)

        __unithpp_SUB_RATIO(degrees, rad, deg, 3141592653589793238, 180000000000000000)

        __unithpp_SUB_RATIO(gradians, rad, grad, 3141592653589793238, 200000000000000000)

        __unithpp_COMPOUND(mile_per_second, mips, mile{} / second{})

        __unithpp_COMPOUND(foot_per_second, ftps, foot{} / second{})

        // Mechanics
        __unithpp_COMPOUND(hertz, Hz, 1 / second{})

        __unithpp_COMPOUND(newton, N, kilo<gram>{} * meter{} / (second{} * second{}))

        __unithpp_COMPOUND(pascal, Pa, newton{} / (meter{} * meter{}))

        __unithpp_COMPOUND(joule, J, newton{} * meter{})

        __unithpp_COMPOUND(watt, W, joule{} / second{})

        __unithpp_COMPOUND(kilogram_per_cubic_meter, kg_per_m3, kilo<gram>{} / (meter{} * meter{} * meter{}))

        __unithpp_COMPOUND(newton_meter, N_m, newton{} * meter{})

        __unithpp_COMPOUND(dyne, dyn, gram{} * centi<meter>{} / (second{} * second{}))

        // Thermodynamics
        __unithpp_COMPOUND(joule_per_kelvin, JpK, joule{} / kelvin{})

        __unithpp_COMPOUND(watt_per_meter_kelvin, WpMK, watt{} / (meter{} * kelvin{}))

        __unithpp_COMPOUND(pascal_second, Pa_s, pascal{} * second{})

        __unithpp_COMPOUND(becquerel, Bq, 1 / second{})

        __unithpp_SUB(celsius, kelvin, degC, { return x + 273.15; }, { return x - 273.15; })

        // Electromagnetism
        __unithpp_COMPOUND(coulomb, C, ampere{} * second{})

        __unithpp_COMPOUND(volt, V, watt{} / ampere{})

        __unithpp_COMPOUND(ohm, Ohm, volt{} / ampere{})

        __unithpp_COMPOUND(farad, F, coulomb{} / volt{})

        __unithpp_COMPOUND(weber, Wb, volt{} * second{})

        __unithpp_COMPOUND(tesla, T, weber{} / (meter{} * meter{}))

        __unithpp_COMPOUND(henry, H, weber{} / ampere{})

        __unithpp_COMPOUND(siemens, S, ampere{} / volt{})

        __unithpp_COMPOUND(gray, Gy, joule{} / kilo<gram>{})

        __unithpp_COMPOUND(magnetic_flux_density, MFB, tesla{})

        __unithpp_COMPOUND(permittivity, Perm, farad{} / meter{})

        __unithpp_COMPOUND(permeability, Permb, henry{} / meter{})

        // Photometry
        __unithpp_BASE(steradian, sr)

        __unithpp_COMPOUND(lumen, lm, candela{} * steradian{})

        __unithpp_COMPOUND(lux, lx, lumen{} / (meter{} * meter{}))

        __unithpp_COMPOUND(sievert, Sv, joule{} / kilo<gram>{})

        __unithpp_COMPOUND(katal, kat, mole{} / second{})

        // Geometry functions
#define __unithpp_bring_rad_func(NAME) \
        template <typename V, extra::FixedString SubSymbol, auto SubToBase, auto BaseToSub> \
        static V NAME(Quantity<extra::Unit<extra::BaseUnit<"rad", 1, std::ratio<1>, "", SubSymbol, SubToBase, BaseToSub>>, V> q) { \
            return std::NAME(radians(q).raw_value); \
        }

        __unithpp_bring_rad_func(sin)
        __unithpp_bring_rad_func(cos)
        __unithpp_bring_rad_func(tan)
        __unithpp_bring_rad_func(sinh)
        __unithpp_bring_rad_func(cosh)
        __unithpp_bring_rad_func(tanh)
        __unithpp_bring_rad_func(asin)
        __unithpp_bring_rad_func(acos)
        __unithpp_bring_rad_func(atan)
        __unithpp_bring_rad_func(asinh)
        __unithpp_bring_rad_func(acosh)
        __unithpp_bring_rad_func(atanh)
    }

    namespace all {
        using namespace Unit;
        using namespace Unit::defaults;
        using namespace Unit::defaults::literals;
    }
}
