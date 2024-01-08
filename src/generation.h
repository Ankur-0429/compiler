#pragma once
#include <utility>

#include "parser.h"

class Generator {
public:
    explicit Generator(NodeProgram program): m_program(std::move(program)) {};

    std::string generate_program();
    void generate_statement(const NodeStatement* statement);
    void generate_expression(NodeExpression* expression);
private:
    const NodeProgram m_program;
    struct Var {
        size_t stack_loc;
    };

    void memory_stack_push(const std::string& value) {
        m_output_stream << "    mov x1, #" << value << "\n";
        m_output_stream << "    sub sp, sp, #16\n";
        m_output_stream << "    str x1, [sp, #0]\n";
        m_stack_size++;
    }

    void memory_stack_push(int registerNumber) {
        m_output_stream << "    mov x1, x" << registerNumber << "\n";
        m_output_stream << "    sub sp, sp, #16\n";
        m_output_stream << "    str x1, [sp, #0]\n";
        m_stack_size++;
    }

    void memory_stack_pop() {
        m_output_stream << "    ldr x2, [sp, #0]\n";
        m_output_stream << "    add sp, sp, #16\n";
        m_stack_size--;
    }

    std::unordered_map<std::string, Var> m_vars {};
    size_t m_stack_size = 0;
    std::stringstream m_output_stream;
};
