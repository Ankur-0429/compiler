#include <sstream>
#include <iostream>
#include "generation.h"

std::string Generator::generate_exit() const {
    if (!m_root.expr.integer_literal.value.has_value()) {
        std::cerr << "generating exit produced an error: integer literal value expected but not found" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::stringstream output_stream;
    output_stream << ".global _main\n.align 2\n_main:\n";
    output_stream << "    mov x0, #" << m_root.expr.integer_literal.value.value() << "\n";
    output_stream << "    mov x16, #1\n";
    output_stream << "    svc 0";

    return output_stream.str();
}
