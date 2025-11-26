/*
 * Unit.hpp
 * A header-only C++20 library for compile-time dimensional analysis and unit conversion.
 *
 * Version: 0.14
 * Author:  OguzhanUmutlu
 * GitHub:  https://github.com/OguzhanUmutlu/unit.hpp
 *
 * Licensed under the MIT License.
 */

#pragma once

#include <iostream>
#include <tuple>
#include <type_traits>
#include <algorithm>
#include <cmath>
#include <ratio>

static constexpr int UNIT_HPP_VERSION_MAJOR = 0;
static constexpr int UNIT_HPP_VERSION_MINOR = 14;

namespace Unit {
    using float_t = double;

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

        template <size_t M> constexpr bool operator==(const FixedString<M>& rhs) const {
            if constexpr (N != M) return false;
            for (size_t i = 0; i < N; ++i) if (buf[i] != rhs.buf[i]) return false;
            return true;
        }

        template <size_t M> constexpr auto operator+(const FixedString<M>& rhs) const {
            char combined_buf[N + M - 1]{};
            std::copy_n(buf, N - 1, combined_buf);
            std::copy_n(rhs.buf, M, combined_buf + N - 1);
            return FixedString<N + M - 1>(combined_buf, N + M - 1);
        }

        constexpr bool empty() const {
            return buf[0] == '\0';
        }
    };

    template <size_t N> std::ostream& operator<<(std::ostream& os, const FixedString<N>& fs) {
        return os << fs.buf;
    }

    template <size_t N> FixedString(const char (&)[N]) -> FixedString<N>;

    template <FixedString Symbol>
    struct BaseUnit;

    template <typename Tpl, int Exponent = 1, FixedString Symbol = "", typename Ratio = std::ratio<1>>
    struct Unit;

    template <typename ThisUnit, typename ValueType = float_t>
    struct Quantity;

    template <typename T> struct is_base_unit : std::false_type {
    };

    template <FixedString S> struct is_base_unit<BaseUnit<S>> : std::true_type {
    };

    template <typename T> inline constexpr auto is_base_unit_v = is_base_unit<T>::value;

    template <typename T> struct is_unit : std::false_type {
    };

    template <typename Tpl, int E, FixedString S, typename R>
    struct is_unit<Unit<Tpl, E, S, R>> : std::true_type {
    };

    template <typename T> struct get_exponent {
        static constexpr int value = 1;
    };

    template <typename Tpl, int E, FixedString S, typename R>
    struct get_exponent<Unit<Tpl, E, S, R>> {
        static constexpr int value = E;
    };

    template <typename T> inline constexpr auto get_exponent_v = get_exponent<T>::value;

    template <typename T> struct get_symbol;
    template <typename T> inline constexpr auto get_symbol_v = get_symbol<T>::value;

    template <FixedString S>
    struct get_symbol<BaseUnit<S>> {
        static constexpr auto value = S;
    };

    template <typename Tpl, int E, FixedString S, typename R>
    struct get_symbol<Unit<Tpl, E, S, R>> {
        static constexpr auto value = []() {
            if constexpr (!S.empty()) {
                return S;
            } else if constexpr (std::tuple_size_v<Tpl> == 1 && E == 1) {
                return get_symbol_v<std::tuple_element_t<0, Tpl>>;
            } else {
                return FixedString("");
            }
        }();
    };

    constexpr float_t ipow(float_t base, int exp) {
        float_t res = 1.0;
        if (exp < 0) {
            base = 1.0 / base;
            exp  = -exp;
        }
        while (exp > 0) {
            if (exp % 2 == 1) res *= base;
            base *= base;
            exp /= 2;
        }
        return res;
    }

    static constexpr float_t pi = 3.14159265358979323846;

    template <FixedString Symbol>
    struct BaseUnit {
        static constexpr auto Sym = Symbol;
    };

    template <typename Tpl, int Exponent, FixedString Symbol, typename Ratio>
    struct Unit {
        using Units               = Tpl;
        static constexpr int Exp  = Exponent;
        static constexpr auto Sym = Symbol;
        using R                   = Ratio;
    };

    template <typename T>
    constexpr float_t get_unit_scale();

    template <typename Tuple>
    constexpr float_t tuple_scale_product() {
        constexpr size_t N = std::tuple_size_v<Tuple>;
        if constexpr (N == 0) return 1.0;
        else {
            return []<size_t... I>(std::index_sequence<I...>) {
                return (... * get_unit_scale<std::tuple_element_t<I, Tuple>>());
            }(std::make_index_sequence<N>{});
        }
    }

    template <typename T>
    constexpr float_t get_unit_scale() {
        if constexpr (is_base_unit_v<T>) {
            return 1.0;
        } else {
            float_t inner = tuple_scale_product<typename T::Units>();
            return ipow(inner * (static_cast<float_t>(T::R::num) / static_cast<float_t>(T::R::den)), T::Exp);
        }
    }

    template <typename T>
    constexpr void print_unit(std::ostream& os);

    template <typename T>
    constexpr void print_unit_magnitude(std::ostream& os) {
        if constexpr (is_base_unit_v<T>) {
            os << T::Sym;
        } else {
            if constexpr (!T::Sym.empty()) {
                os << T::Sym;
            } else {
                constexpr size_t N = std::tuple_size_v<typename T::Units>;
                if constexpr (N > 0) {
                    [&]<size_t... I>(std::index_sequence<I...>) {
                        size_t idx = 0;
                        ((print_unit<std::tuple_element_t<I, typename T::Units>>(os), (++idx < N ? os << "*" : os)), ...
                        );
                    }(std::make_index_sequence<N>{});
                }
            }

            constexpr int exp     = get_exponent_v<T>;
            constexpr int abs_exp = (exp < 0) ? -exp : exp;

            if constexpr (abs_exp != 1) {
                os << "^" << abs_exp;
            }
        }
    }

    template <typename T>
    constexpr void print_unit(std::ostream& os) {
        if constexpr (is_base_unit_v<T>) {
            os << T::Sym;
        } else {
            if constexpr (!T::Sym.empty()) {
                if constexpr (T::Exp < 0) {
                    os << "1/" << T::Sym;
                    if constexpr (T::Exp != -1) {
                        os << "^" << -T::Exp;
                    }
                } else {
                    os << T::Sym;
                    if constexpr (T::Exp != 1) {
                        os << "^" << T::Exp;
                    }
                }
            } else {
                if constexpr (T::Exp < 0) {
                    os << "1/";
                    print_unit_magnitude<T>(os);
                } else {
                    using Tuple = T::Units;

                    constexpr size_t N = std::tuple_size_v<Tuple>;

                    bool has_numerator   = false;
                    bool has_denominator = false;

                    [&]<size_t... I>(std::index_sequence<I...>) {
                        size_t count = 0;
                        ((get_exponent_v<std::tuple_element_t<I, Tuple>> >= 0
                              ? (
                                  (count++ > 0 ? os << "*" : os),
                                  print_unit<std::tuple_element_t<I, Tuple>>(os),
                                  has_numerator = true
                              )
                              : 0), ...);
                    }(std::make_index_sequence<N>{});

                    [&]<size_t... I>(std::index_sequence<I...>) {
                        ((get_exponent_v<std::tuple_element_t<I, Tuple>> < 0 ? has_denominator = true : 0), ...);
                    }(std::make_index_sequence<N>{});

                    if (has_denominator) {
                        if (!has_numerator) {
                            os << "1";
                        }
                        os << "/";

                        [&]<size_t... I>(std::index_sequence<I...>) {
                            size_t count = 0;
                            ((get_exponent_v<std::tuple_element_t<I, Tuple>> < 0
                                  ? (
                                      (count++ > 0 ? os << "*" : os),
                                      print_unit_magnitude<std::tuple_element_t<I, Tuple>>(os),
                                      0
                                  )
                                  : 0), ...);
                        }(std::make_index_sequence<N>{});
                    }

                    if constexpr (T::Exp != 1) {
                        os << "^" << T::Exp;
                    }
                }
            }
        }
    }

    template <typename T1, typename T2> struct tuple_cat_type;
    template <typename T1, typename T2> using tuple_cat_type_t = tuple_cat_type<T1, T2>::type;

    template <typename... Ts1, typename... Ts2>
    struct tuple_cat_type<std::tuple<Ts1...>, std::tuple<Ts2...>> {
        using type = std::tuple<Ts1..., Ts2...>;
    };

    template <typename U, int InheritedExp> struct flatten;
    template <typename U, int InheritedExp> using flatten_t = flatten<U, InheritedExp>::type;

    template <typename Tuple, int ParentExp> struct flatten_tuple;
    template <typename Tuple, int ParentExp> using flatten_tuple_t = flatten_tuple<Tuple, ParentExp>::type;

    template <typename... Ts, int ParentExp>
    struct flatten_tuple<std::tuple<Ts...>, ParentExp> {
        using type = decltype(std::tuple_cat(flatten_t<Ts, ParentExp>{}...));
    };

    template <FixedString S, int InheritedExp>
    struct flatten<BaseUnit<S>, InheritedExp> {
        using type = std::tuple<Unit<std::tuple<BaseUnit<S>>, InheritedExp>>;
    };

    template <typename Tpl, int Exp, FixedString Sym, typename R, int InheritedExp>
    struct flatten<Unit<Tpl, Exp, Sym, R>, InheritedExp> {
        static constexpr int EffectiveExp = Exp * InheritedExp;
        using ThisUnit                    = Unit<Tpl, Exp, Sym, R>;

        using type = std::conditional_t<
            !Sym.empty(),
            std::tuple<Unit<std::tuple<ThisUnit>, InheritedExp>>,
            flatten_tuple_t<Tpl, EffectiveExp>
        >;
    };

    template <typename Term1, typename Term2>
    struct is_same_basis {
        using B1                    = std::tuple_element_t<0, typename Term1::Units>;
        using B2                    = std::tuple_element_t<0, typename Term2::Units>;
        static constexpr bool value = std::is_same_v<B1, B2>;
    };

    template <typename Term1, typename Term2>
    inline constexpr auto is_same_basis_v = is_same_basis<Term1, Term2>::value;

    template <typename Term, int ExtraExp>
    struct add_exp_to_term {
        using Basis                 = std::tuple_element_t<0, typename Term::Units>;
        static constexpr int OldExp = Term::Exp;
        using type                  = Unit<std::tuple<Basis>, OldExp + ExtraExp>;
    };

    template <typename Term, int ExtraExp> using add_exp_to_term_t = add_exp_to_term<Term, ExtraExp>::type;

    template <typename Term1, typename Term2>
    struct term_less {
        using B1 = std::tuple_element_t<0, typename Term1::Units>;
        using B2 = std::tuple_element_t<0, typename Term2::Units>;

        static constexpr bool value = []() {
            constexpr auto s1       = B1::Sym;
            constexpr auto s2       = B2::Sym;
            constexpr size_t N1     = sizeof(s1.buf);
            constexpr size_t N2     = sizeof(s2.buf);
            constexpr size_t common = (N1 < N2) ? N1 : N2;

            for (size_t i = 0; i < common; ++i) {
                if (s1.buf[i] != s2.buf[i]) {
                    return s1.buf[i] < s2.buf[i];
                }
            }
            return N1 < N2;
        }();
    };

    template <typename Term1, typename Term2> inline constexpr auto term_less_v = term_less<Term1, Term2>::value;


    template <typename AccumTuple, typename Term> struct merge_one;
    template <typename AccumTuple, typename Term> using merge_one_t = merge_one<AccumTuple, Term>::type;

    template <typename Term>
    struct merge_one<std::tuple<>, Term> {
        using type = std::tuple<Term>;
    };

    template <typename Head, typename... Tail, typename Term>
    struct merge_one<std::tuple<Head, Tail...>, Term> {
        using type = std::conditional_t<
            is_same_basis_v<Head, Term>,
            std::tuple<add_exp_to_term_t<Head, Term::Exp>, Tail...>,
            std::conditional_t<
                term_less_v<Term, Head>,
                std::tuple<Term, Head, Tail...>,
                tuple_cat_type_t<std::tuple<Head>, merge_one_t<std::tuple<Tail...>, Term>>
            >
        >;
    };

    template <typename Accum, typename Remaining> struct merge_all;
    template <typename Accum, typename Remaining> using merge_all_t = merge_all<Accum, Remaining>::type;

    template <typename Accum>
    struct merge_all<Accum, std::tuple<>> {
        using type = Accum;
    };

    template <typename Accum, typename Head, typename... Tail>
    struct merge_all<Accum, std::tuple<Head, Tail...>> {
        using NextAccum = merge_one_t<Accum, Head>;
        using type      = merge_all_t<NextAccum, std::tuple<Tail...>>;
    };

    template <typename Tuple> struct filter_zero;
    template <typename Tuple> using filter_zero_t = filter_zero<Tuple>::type;

    template <> struct filter_zero<std::tuple<>> {
        using type = std::tuple<>;
    };

    template <typename Head, typename... Tail>
    struct filter_zero<std::tuple<Head, Tail...>> {
        using TailFiltered = filter_zero_t<std::tuple<Tail...>>;
        using type         = std::conditional_t<
            Head::Exp == 0,
            TailFiltered,
            tuple_cat_type_t<std::tuple<Head>, TailFiltered>
        >;
    };

    template <typename Tuple> struct reconstruct {
        using type = Unit<Tuple>;
    };

    template <typename Tuple> using reconstruct_t = reconstruct<Tuple>::type;

    template <typename SingleTerm>
    struct reconstruct<std::tuple<SingleTerm>> {
        using type = SingleTerm;
    };

    template <typename U, int InheritedExp> struct decompose;
    template <typename U, int InheritedExp> using decompose_t = decompose<U, InheritedExp>::type;

    template <typename Tuple, int ParentExp> struct decompose_tuple;
    template <typename Tuple, int ParentExp> using decompose_tuple_t = decompose_tuple<Tuple, ParentExp>::type;

    template <typename... Ts, int ParentExp>
    struct decompose_tuple<std::tuple<Ts...>, ParentExp> {
        using type = decltype(std::tuple_cat(decompose_t<Ts, ParentExp>{}...));
    };

    template <FixedString S, int InheritedExp>
    struct decompose<BaseUnit<S>, InheritedExp> {
        using type = std::tuple<Unit<std::tuple<BaseUnit<S>>, InheritedExp>>;
    };

    template <typename Tpl, int Exp, FixedString Sym, typename R, int InheritedExp>
    struct decompose<Unit<Tpl, Exp, Sym, R>, InheritedExp> {
        static constexpr int EffectiveExp = Exp * InheritedExp;

        using type = decompose_tuple_t<Tpl, EffectiveExp>;
    };

    template <typename U>
    struct pure_unit {
        using Decomposed = decompose_t<U, 1>;
        using Merged     = merge_all_t<std::tuple<>, Decomposed>;
        using Filtered   = filter_zero_t<Merged>;
        using type       = reconstruct_t<Filtered>;
    };

    template <typename U>
    using pure_unit_t = pure_unit<U>::type;

    template <typename U1, typename U2, int Sign>
    struct binary_op_result {
        using Terms1   = flatten_t<U1, 1>;
        using Terms2   = flatten_t<U2, Sign>;
        using Combined = tuple_cat_type_t<Terms1, Terms2>;
        using Merged   = merge_all_t<std::tuple<>, Combined>;
        using Filtered = filter_zero_t<Merged>;
        using type     = reconstruct_t<Filtered>;
    };

    template <typename U1, typename U2, int Sign> using binary_op_result_t = binary_op_result<U1, U2, Sign>::type;

    template <typename ThisUnit, typename ValueType>
    struct Quantity {
        using u = ThisUnit;
        ValueType value;

        explicit constexpr Quantity(ValueType v = 0) : value(v) {
        }

        template <typename OtherUnit, typename OtherValue>
            requires std::is_same_v<pure_unit_t<ThisUnit>, pure_unit_t<OtherUnit>>
        explicit(!std::is_same_v<ThisUnit, OtherUnit>) constexpr Quantity(
            const Quantity<OtherUnit, OtherValue>& other
        ) {
            if constexpr (std::is_same_v<ThisUnit, OtherUnit>) {
                value = static_cast<ValueType>(other.value);
            } else {
                constexpr ValueType from_scale = get_unit_scale<ThisUnit>();
                constexpr ValueType to_scale   = get_unit_scale<OtherUnit>();
                value                          = static_cast<ValueType>(other.value * (to_scale / from_scale));
            }
        }

        friend std::ostream& operator<<(std::ostream& os, const Quantity& q) {
            os << q.value << " ";
            print_unit<ThisUnit>(os);
            return os;
        }

        template <typename OtherUnit, typename OtherValue>
        constexpr auto operator*(const Quantity<OtherUnit, OtherValue>& rhs) const {
            using ResultUnit = binary_op_result_t<ThisUnit, OtherUnit, 1>;
            return Quantity<ResultUnit, decltype(value * rhs.value)>(value * rhs.value);
        }

        template <typename OtherUnit, typename OtherValue>
        constexpr auto operator/(const Quantity<OtherUnit, OtherValue>& rhs) const {
            using ResultUnit = binary_op_result_t<ThisUnit, OtherUnit, -1>;
            return Quantity<ResultUnit, decltype(value / rhs.value)>(value / rhs.value);
        }

        constexpr auto operator*(ValueType rhs) const {
            return Quantity(value * rhs);
        }

        constexpr auto operator/(ValueType rhs) const {
            return Quantity(value / rhs);
        }

        friend constexpr auto operator*(ValueType lhs, const Quantity& rhs) {
            return Quantity(rhs.value * lhs);
        }

        friend constexpr auto operator/(ValueType lhs, const Quantity& rhs) {
            using Dimensionless = Unit<std::tuple<>, 0>;
            using ResultUnit    = binary_op_result_t<Dimensionless, ThisUnit, -1>;
            return Quantity<ResultUnit, ValueType>(lhs / rhs.value);
        }

        constexpr auto operator+(const Quantity& rhs) const {
            return Quantity(value + rhs.value);
        }

        constexpr auto operator-(const Quantity& rhs) const {
            return Quantity(value - rhs.value);
        }

        constexpr auto operator<=>(const Quantity& rhs) const = default;

        template <typename OtherUnit>
            requires std::same_as<Quantity, decltype(Quantity() + Quantity<OtherUnit, ValueType>())>
        constexpr auto& operator+=(const Quantity<OtherUnit, ValueType>& rhs) {
            *this = *this + rhs;
            return *this;
        }

        template <typename OtherUnit>
            requires std::same_as<Quantity, decltype(Quantity() - Quantity<OtherUnit, ValueType>())>
        constexpr auto& operator-=(const Quantity<OtherUnit, ValueType>& rhs) {
            *this = *this - rhs;
            return *this;
        }

        template <typename OtherUnit>
            requires std::same_as<Quantity, decltype(Quantity() * Quantity<OtherUnit, ValueType>())>
        constexpr auto& operator*=(const Quantity<OtherUnit, ValueType>& rhs) {
            *this = *this * rhs;
            return *this;
        }

        template <typename OtherUnit>
            requires std::same_as<Quantity, decltype(Quantity() / Quantity<OtherUnit, ValueType>())>
        constexpr auto& operator/=(const Quantity<OtherUnit, ValueType>& rhs) {
            *this = *this / rhs;
            return *this;
        }

        constexpr auto operator+() const {
            return *this;
        }

        constexpr auto operator-() const {
            return Quantity(-value);
        }

        explicit operator ValueType() const {
            return value;
        }
    };

    template <FixedString Sym, int Exp = 1>
    using base_unit = Unit<std::tuple<BaseUnit<Sym>>, Exp>;
    template <FixedString Sym, int Exp = 1>
    using base_unit_q = Quantity<base_unit<Sym, Exp>>;

    template <typename U, FixedString Sym, typename Ratio = std::ratio<1>, int Exp = U::Exp>
    using compound_unit = Unit<
        typename U::Units,
        Exp,
        Sym,
        Ratio
    >;

    template <typename Q, FixedString Sym, typename Ratio = std::ratio<1>, int Exp = Q::u::Exp>
    using compound_unit_q = Quantity<compound_unit<typename Q::u, Sym, Ratio, Exp>>;

    template <typename U, FixedString Prefix, typename Ratio = std::ratio<1>, int Exp = U::Exp>
    using scaled_unit = compound_unit<U, Prefix + get_symbol_v<U>, Ratio, Exp>;

    template <typename Q, FixedString Prefix, typename Ratio = std::ratio<1>, int Exp = Q::u::Exp>
    using scaled_unit_q = Quantity<scaled_unit<typename Q::u, Prefix, Ratio, Exp>>;

    namespace math {
        template <typename Q> constexpr auto abs(const Q& q) {
            return Q(std::abs(q.value));
        }

        template <typename Q> constexpr auto fmod(const Q& q1, const Q& q2) {
            return Q(std::fmod(q1.value, q2.value));
        }

        template <typename Q> constexpr auto ceil(const Q& q) {
            return Q(std::ceil(q.value));
        }

        template <typename Q> constexpr auto floor(const Q& q) {
            return Q(std::floor(q.value));
        }
    }

    namespace defaults {
        using namespace math;

        template <typename Q, int E = 1> using atto  = scaled_unit_q<Q, "a", std::atto, E>;
        template <typename Q, int E = 1> using femto = scaled_unit_q<Q, "f", std::femto, E>;
        template <typename Q, int E = 1> using pico  = scaled_unit_q<Q, "p", std::pico, E>;
        template <typename Q, int E = 1> using nano  = scaled_unit_q<Q, "n", std::nano, E>;
        template <typename Q, int E = 1> using micro = scaled_unit_q<Q, "μ", std::micro, E>;
        template <typename Q, int E = 1> using milli = scaled_unit_q<Q, "m", std::milli, E>;
        template <typename Q, int E = 1> using centi = scaled_unit_q<Q, "c", std::centi, E>;
        template <typename Q, int E = 1> using deci  = scaled_unit_q<Q, "d", std::deci, E>;
        template <typename Q, int E = 1> using deca  = scaled_unit_q<Q, "da", std::deca, E>;
        template <typename Q, int E = 1> using hecto = scaled_unit_q<Q, "h", std::hecto, E>;
        template <typename Q, int E = 1> using kilo  = scaled_unit_q<Q, "k", std::kilo, E>;
        template <typename Q, int E = 1> using mega  = scaled_unit_q<Q, "M", std::mega, E>;
        template <typename Q, int E = 1> using giga  = scaled_unit_q<Q, "G", std::giga, E>;
        template <typename Q, int E = 1> using tera  = scaled_unit_q<Q, "T", std::tera, E>;
        template <typename Q, int E = 1> using peta  = scaled_unit_q<Q, "P", std::peta, E>;
        template <typename Q, int E = 1> using exa   = scaled_unit_q<Q, "E", std::exa, E>;

#define __unithpp_literal_(TYPE, SYM) \
        constexpr auto operator ""_##SYM(long double val) { return TYPE(static_cast<float_t>(val)); } \
        constexpr auto operator ""_##SYM(unsigned long long val) { return TYPE(static_cast<float_t>(val)); }
#define __unithpp_literal(TYPE) __unithpp_literal_(TYPE, TYPE)
#define __unithpp_scales(UNIT) \
    __unithpp_literal_(atto<UNIT>, a##UNIT) \
    __unithpp_literal_(femto<UNIT>, f##UNIT) \
    __unithpp_literal_(pico<UNIT>, p##UNIT) \
    __unithpp_literal_(nano<UNIT>, n##UNIT) \
    __unithpp_literal_(micro<UNIT>, u##UNIT) \
    __unithpp_literal_(milli<UNIT>, m##UNIT) \
    __unithpp_literal_(centi<UNIT>, c##UNIT) \
    __unithpp_literal_(deci<UNIT>, d##UNIT) \
    __unithpp_literal_(deca<UNIT>, da##UNIT) \
    __unithpp_literal_(hecto<UNIT>, h##UNIT) \
    __unithpp_literal_(kilo<UNIT>, k##UNIT) \
    __unithpp_literal_(mega<UNIT>, M##UNIT) \
    __unithpp_literal_(giga<UNIT>, G##UNIT) \
    __unithpp_literal_(tera<UNIT>, T##UNIT) \
    __unithpp_literal_(peta<UNIT>, P##UNIT) \
    __unithpp_literal_(exa<UNIT>, E##UNIT)
#define __unithpp_literals(UNIT) \
    __unithpp_literal(UNIT) \
    __unithpp_scales(UNIT)

        using m   = base_unit_q<"m">;
        using g   = base_unit_q<"g">;
        using s   = base_unit_q<"s">;
        using mol = base_unit_q<"mol">;
        using K   = base_unit_q<"K">;
        using A   = base_unit_q<"A">;
        using cd  = base_unit_q<"cd">;
        using rad = base_unit_q<"rad">;
        using px  = Quantity<base_unit<"px">, unsigned>;

        using L      = compound_unit_q<deci<m, 3>, "L">;
        using deg    = compound_unit_q<rad, "deg", std::ratio<17453292519943296, 1000000000000000000>>;
        using grad   = compound_unit_q<rad, "grad", std::ratio<15707963267948966, 1000000000000000000>>;
        using mi     = compound_unit_q<m, "mi", std::ratio<1609344, 1000>>;
        using ft     = compound_unit_q<m, "ft", std::ratio<3048, 10000>>;
        using in     = compound_unit_q<m, "in", std::ratio<254, 10000>>;
        using yd     = compound_unit_q<m, "yd", std::ratio<9144, 10000>>;
        using oz     = compound_unit_q<g, "oz", std::ratio<28349523125, 1000000000>>;
        using lb     = compound_unit_q<g, "lb", std::ratio<45359237, 100>>;
        using ton    = compound_unit_q<g, "ton", std::ratio<90718474, 10>>;
        using gal    = compound_unit_q<L, "gal", std::ratio<3785411784, 10000000>>;
        using minute = compound_unit_q<s, "minute", std::ratio<60>>;
        using hour   = compound_unit_q<s, "hour", std::ratio<3600>>;
        using day    = compound_unit_q<s, "day", std::ratio<86400>>;
        using week   = compound_unit_q<s, "week", std::ratio<604800>>;
        using Hz     = compound_unit_q<decltype(1 / s{}), "Hz">;
        using N      = compound_unit_q<decltype(kilo<g>{} * m{} / (s{} * s{})), "N">;
        using Pa     = compound_unit_q<decltype(N{} / (m{} * m{})), "Pa">;
        using J      = compound_unit_q<decltype(N{} * m{}), "J">;
        using W      = compound_unit_q<decltype(J{} / s{}), "W">;
        using C      = compound_unit_q<decltype(A{} * s{}), "C">;
        using V      = compound_unit_q<decltype(W{} / A{}), "V">;
        using Ohm    = compound_unit_q<decltype(V{} / A{}), "Ohm">;
        using S      = compound_unit_q<decltype(A{} / V{}), "S">;
        using F      = compound_unit_q<decltype(C{} / V{}), "F">;
        using H      = compound_unit_q<decltype(Ohm{} * s{}), "H">;
        using Wb     = compound_unit_q<decltype(V{} * s{}), "Wb">;
        using Tesla  = compound_unit_q<decltype(Wb{} / (m{} * m{})), "T">;
        using sr     = compound_unit_q<decltype(rad{} * rad{}), "sr">;
        using lm     = compound_unit_q<decltype(cd{} * sr{}), "lm">;
        using lx     = compound_unit_q<decltype(lm{} / (m{} * m{})), "lx">;
        using Bq     = compound_unit_q<decltype(1 / s{}), "Bq">;
        using Gy     = compound_unit_q<decltype(J{} / kilo<g>{}), "Gy">;
        using Sv     = compound_unit_q<decltype(J{} / kilo<g>{}), "Sv">;
        using Kat    = compound_unit_q<decltype(mol{} / s{}), "Kat">;
        using dyn    = compound_unit_q<decltype(g{} * m{} / (s{} * s{})), "dyn">;

        __unithpp_literals(m)
        __unithpp_literals(g)
        __unithpp_literals(s)
        __unithpp_literals(mol)
        __unithpp_literals(K)
        __unithpp_literals(A)
        __unithpp_literals(cd)
        __unithpp_literals(rad)
        __unithpp_literals(px)

        __unithpp_literals(L)
        __unithpp_literals(deg)
        __unithpp_literals(grad)
        __unithpp_literals(mi)
        __unithpp_literals(ft)
        __unithpp_literals(in)
        __unithpp_literals(yd)
        __unithpp_literals(oz)
        __unithpp_literals(lb)
        __unithpp_literals(ton)
        __unithpp_literals(gal)
        __unithpp_literal(minute)
        __unithpp_literal(hour)
        __unithpp_literal(day)
        __unithpp_literal(week)
        __unithpp_literals(Hz)
        __unithpp_literals(N)
        __unithpp_literals(Pa)
        __unithpp_literals(J)
        __unithpp_literals(W)
        __unithpp_literals(C)
        __unithpp_literals(V)
        __unithpp_literals(Ohm)
        __unithpp_literals(S)
        __unithpp_literals(F)
        __unithpp_literals(H)
        __unithpp_literals(Wb)
        __unithpp_literals(Tesla)
        __unithpp_literals(sr)
        __unithpp_literals(lm)
        __unithpp_literals(lx)
        __unithpp_literals(Bq)
        __unithpp_literals(Gy)
        __unithpp_literals(Sv)
        __unithpp_literals(Kat)
        __unithpp_literals(dyn)

#undef __unithpp_literal
#undef __unithpp_literals
#undef __unithpp_scales

        constexpr auto operator ""_degC(long double val) {
            return K(static_cast<float_t>(val) + 273.15);
        }

        constexpr auto operator ""_degC(unsigned long long val) {
            return K(static_cast<float_t>(val) + 273.15);
        }

        constexpr auto operator ""_degF(long double val) {
            return K(static_cast<float_t>((val - 32) * 5 / 9 + 273.15));
        }

        constexpr auto operator ""_degF(unsigned long long val) {
            return K((val - 32) * 5 / 9 + 273.15);
        }

        template <typename U> requires requires { rad{std::declval<Quantity<U>>()}; }
        constexpr auto sin(const Quantity<U>& q) {
            return std::sin(rad{q}.value);
        }

        template <typename U> requires requires { rad{std::declval<Quantity<U>>()}; }
        constexpr auto cos(const Quantity<U>& q) {
            return std::cos(rad{q}.value);
        }

        template <typename U> requires requires { rad{std::declval<Quantity<U>>()}; }
        constexpr auto tan(const Quantity<U>& q) {
            return std::tan(rad{q}.value);
        }
    }
}
