#pragma once
#include <utility>

#include "parser.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

class Generator {
public:
    explicit Generator(NodeProgram program): m_program(std::move(program)) {};

    std::string generate_program();
    llvm::Value* generate_term(const NodeTerm* term);
    void generate_statement(const NodeStatement* statement);
    llvm::Value* generate_expression(NodeExpression* expression);
    llvm::Value* generate_binary_expression(const NodeBinaryExpression* binary_expression);
private:
    const NodeProgram m_program;
    llvm::LLVMContext m_context;
    llvm::Module m_module{"MyModule", m_context};
    llvm::IRBuilder<> m_builder{m_context};
    std::unordered_map<std::string, llvm::Value*> m_vars{};
};
