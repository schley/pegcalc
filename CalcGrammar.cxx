module;

// This code was modified to permit use of double_::grammar as defined
// in the taocpp/pegtl example code double.hpp  and also be a c++ module.  
// double.hpp from taocpp/pegtl/include
// is basically included directly into the calculator grammar.  

#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <tao/pegtl.hpp>
#include <vector>

// Include the analyze function that checks
// a grammar for possible infinite cycles.

#include <tao/pegtl/contrib/analyze.hpp>

namespace pegtl = tao::pegtl;

export module CalcGrammar;

import Repl;

namespace calculator
{

// This double lexer is obtained from src/example/double.hpp from
// taocpp/pegtl git distribution.

// A grammar for doubles suitable for std::stod without locale support.
// See also: http://en.cppreference.com/w/cpp/string/basic_string/stof

// clng-format off

struct plus_minus : pegtl::opt<pegtl::one<'+', '-'> >
{
};

struct dot : pegtl::one<'.'>
{
};

struct inf : pegtl::seq<pegtl::istring<'i', 'n', 'f'>,
                        pegtl::opt<pegtl::istring<'i', 'n', 'i', 't', 'y'> > >
{
};

struct nan : pegtl::seq<pegtl::istring<'n', 'a', 'n'>,
                        pegtl::opt<pegtl::one<'('>, pegtl::plus<pegtl::alnum>,
                                   pegtl::one<')'> > >
{
};

template <typename D>
struct number
    : pegtl::if_then_else<
          dot, pegtl::plus<D>,
          pegtl::seq<pegtl::plus<D>, pegtl::opt<dot, pegtl::star<D> > > >
{
};

struct e : pegtl::one<'e', 'E'>
{
};

struct p : pegtl::one<'p', 'P'>
{
};

struct exponent : pegtl::seq<plus_minus, pegtl::plus<pegtl::digit> >
{
};

struct decimal : pegtl::seq<number<pegtl::digit>, pegtl::opt<e, exponent> >
{
};

struct hexadecimal : pegtl::seq<pegtl::one<'0'>, pegtl::one<'x', 'X'>,
                                number<pegtl::xdigit>, pegtl::opt<p, exponent> >
{
};

namespace double_
{
struct grammar
    : pegtl::seq<plus_minus, pegtl::sor<hexadecimal, decimal, inf, nan> >
{
};
// clng-format on
}  // namespace double_

// This enum is used for the order in which the operators are
// evaluated, i.e. the priority of the operators; a higher
// number indicates a lower priority.

enum class order : int
{
};

// For each binary operator known to the calculator we need an
// instance of the following data structure with the priority,
// and a function that performs the calculation. All operators
// are left-associative.

struct op
{
        order p;
        std::function<double(double, double)> f;
};

// Class that takes care of an operand and an operator stack for
// shift-reduce style handling of operator priority; in a
// reduce-step it calls on the functions contained in the op
// instances to perform the calculation.

export struct stack
{
        void push(const op& b)
        {
            while ((!m_o.empty()) && (m_o.back().p <= b.p))
            {
                reduce();
            }
            m_o.push_back(b);
        }

        void push(const double l)
        {
            // DEBUG std::cout << "stack::push m_l.push_back l = " << l << "\n";
            m_l.push_back(l);
        }

        double finish()
        {
            while (!m_o.empty())
            {
                reduce();
            }
            assert(m_l.size() == 1);
            const auto r = m_l.back();
            m_l.clear();
            return r;
        }

    private:
        std::vector<op> m_o;
        std::vector<double> m_l;

        void reduce()
        {
            assert(!m_o.empty());
            assert(m_l.size() > 1);

            const auto r = m_l.back();
            m_l.pop_back();
            const auto l = m_l.back();
            m_l.pop_back();
            const auto o = m_o.back();
            m_o.pop_back();
            m_l.push_back(o.f(l, r));
        }
};

// Additional layer, a "stack of stacks", to clearly show how bracketed
// sub-expressions can be easily processed by giving them a stack of
// their own. Once a bracketed sub-expression has finished evaluation on
// its stack, the result is pushed onto the next higher stack, and the
// sub-expression's temporary stack is discarded. The top-level calculation
// is handled just like a bracketed sub-expression, on the first stack pushed
// by the constructor.

export struct stacks
{
        stacks() { open(); }

        void open() { m_v.emplace_back(); }

        template <typename T>
        void push(const T& t)
        {
            assert(!m_v.empty());
            m_v.back().push(t);
        }

        void close()
        {
            assert(m_v.size() > 1);
            const auto r = m_v.back().finish();
            m_v.pop_back();
            m_v.back().push(r);
        }

        double finish()
        {
            assert(m_v.size() == 1);
            return m_v.back().finish();
        }

    private:
        std::vector<stack> m_v;
};

// A wrapper around the data structures that contain the binary
// operators for the calculator.

export struct operators
{
        operators()
        {
            insert("*", order(5),
                   [](const double l, const double r) { return l * r; });
            insert("/", order(5),
                   [](const double l, const double r) { return l / r; });
            insert("+", order(6),
                   [](const double l, const double r) { return l + r; });
            insert("-", order(6),
                   [](const double l, const double r) { return l - r; });
            insert("<", order(8),
                   [](const double l, const double r) { return l < r; });
            insert(">", order(8),
                   [](const double l, const double r) { return l > r; });
            insert("<=", order(8),
                   [](const double l, const double r) { return l <= r; });
            insert(">=", order(8),
                   [](const double l, const double r) { return l >= r; });
            insert("==", order(9),
                   [](const double l, const double r) { return l == r; });
            insert("!=", order(9),
                   [](const double l, const double r) { return l != r; });
        }

        // Arbitrary user-defined operators can be added at runtime.

        void insert(const std::string& name, const order p,
                    const std::function<double(double, double)>& f)
        {
            assert(!name.empty());
            m_ops.try_emplace(name, op{p, f});
        }

        [[nodiscard]] const std::map<std::string, op>& ops() const noexcept
        {
            return m_ops;
        }

    private:
        std::map<std::string, op> m_ops;
};

// Here the actual grammar starts.

// Comments are introduced by a '#' and proceed to the end-of-line/file.

struct comment : pegtl::seq<pegtl::one<'#'>, pegtl::until<pegtl::eolf> >
{
};

// The calculator ignores all spaces and comments; space is a pegtl rule
// that matches the usual ascii characters ' ', '\t', '\n' etc. In other
// words, everything that is space or a comment is ignored.

struct ignored : pegtl::sor<pegtl::space, comment>
{
};

// Since the binary operators are taken from a runtime data structure
// (rather than hard-coding them into the grammar), we need a custom
// rule that attempts to match the input against the current map of
// operators.

struct infix
{
        using rule_t = pegtl::ascii::any::rule_t;

        template <pegtl::apply_mode, pegtl::rewind_mode,
                  template <typename...> class Action,
                  template <typename...> class Control, typename ParseInput,
                  typename... States>

        static bool match(ParseInput& in, const operators& b, stacks& s,
                          States&&...)
        {
            // Look for the longest match of the input against the operators in
            // the operator map.

            return match(in, b, s, std::string());
        }

    private:
        template <typename ParseInput>
        static bool match(ParseInput& in, const operators& b, stacks& s,
                          std::string t)
        {
            if (in.size(t.size() + 1) > t.size())
            {
                t += in.peek_char(t.size());
                const auto i = b.ops().lower_bound(t);
                if (i != b.ops().end())
                {
                    if (match(in, b, s, t))
                    {
                        return true;
                    }
                    if (i->first == t)
                    {
                        // While we are at it, this rule also performs the task
                        // of what would usually be an associated action: To
                        // push the matched operator onto the operator stack.
                        s.push(i->second);
                        in.bump(t.size());
                        return true;
                    }
                }
            }
            return false;
        }
};

// A int_number is a non-empty sequence of digits preceded by an optional sign.

struct int_number
    : pegtl::seq<pegtl::opt<pegtl::one<'+', '-'> >, pegtl::plus<pegtl::digit> >
{
};

struct expression;

// A bracketed expression is introduced by a '(' and, in this grammar, must
// proceed with an expression and a ')'.

struct bracket : pegtl::seq<pegtl::one<'('>, expression, pegtl::one<')'> >
{
};

// An atomic expression, i.e. one without operators, is either a number or
// a bracketed expression.

struct atomic : pegtl::sor<double_::grammar, bracket>
{
};

// An expression is a non-empty list of atomic expressions where each pair
// of atomic expressions is separated by an infix operator and we allow
// the rule ignored as padding (before and after every single expression).

struct expression : pegtl::list<atomic, infix, ignored>
{
};

// The top-level grammar allows one expression and then expects eof.

export struct grammar : pegtl::seq<expression, pegtl::eof>
{
};

// After the grammar we proceed with the additional actions that are
// required to let our calculator actually do something.

// The base-case of the class template for the actions, does nothing.

export template <typename Rule>
struct action
{
};

// This action will be called when the number rule matches; it converts the
// matched portion of the input to a long and pushes it onto the operand
// stack.

export template <>
struct action<double_::grammar>
{
        template <typename ActionInput>
        static void apply(const ActionInput& in, const operators&, stacks& s)
        {
            std::stringstream ss(in.string());
            double v;
            ss >> v;
            // DEBUG std::cout << "CalcGrammar: action v = " << v << std::endl;
            s.push(v);
        }
};

// The actions for the brackets call functions that create, and collect
// a temporary additional stack for evaluating the bracketed expression.

export template <>
struct action<pegtl::one<'('> >
{
        static void apply0(const operators&, stacks& s) { s.open(); }
};

export template <>
struct action<pegtl::one<')'> >
{
        static void apply0(const operators&, stacks& s) { s.close(); }
};

}  // namespace calculator
