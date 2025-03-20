// This code was modified to allow the calculator example to
// operate on floating point numbers (only).  The copyright is left although
// the code was modified to use the "double.hpp" example with the calculator
// example (as a module which finally works with clang++17 and cmake 3.28).
//
// Copyright (c) 2014-2023 Dr. Colin Hirsch and Daniel Frey
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <map>
#include <sstream>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/analyze.hpp>
#include <vector>

namespace pegtl = tao::pegtl;

import Repl;
import CalcGrammar;

int main()
{
    int status = 0;

    // Check the grammar for some possible issues.
    if (pegtl::analyze<calculator::grammar>() != 0)
    {
        std::cerr << "main: ERROR calculator grammar fails analysis!\n";
        status = 1;
        return status;
    }

    std::cout << "Enter an equation with valid floating point numbers.  exit "
                 "exits\n\n";

    Tb::ReplEcho recho;
    while (recho.keepEchoing())
    {
        pegtl::string_input in1(recho.echoInput(), "from_recho");
        if (recho.keepEchoing())
        {
            calculator::stacks s;
            calculator::operators b;
            if (pegtl::parse<calculator::grammar, calculator::action>(in1, b,
                                                                      s))
            {
                std::cout << "CALC " << recho.count() << ": " << recho.echoed()
                          << " = " << s.finish() << "\n";
            }
            else
            {
                std::cerr << "CALC ERROR " << recho.count() << ": "
                          << recho.echoed() << " parsing error?\n";
            }
        }
    }

    std::cout << "Processed " << recho.count() << " equations." << std::endl;

    return status;
}
