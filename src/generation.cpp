//
// Created by Ankur Ahir on 12/28/23.
//

#include <sstream>
#include "generation.h"

std::string Generator::generate() const {
    std::stringstream output_stream;
    output_stream << ".global _main\n.align 2\n_main:\n";
    output_stream << "    mov x0, #" << m_root.expr.integer_literal.value.value() << "\n";
    output_stream << "    mov x16, #1\n";
    output_stream << "    svc 0";

    return output_stream.str();
}
