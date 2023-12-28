#pragma once
#include <utility>

#include "parser.h"

class Generator {
public:
    explicit Generator(NodeExit root): m_root(std::move(root)) {};

    [[nodiscard]] std::string generate() const;
private:
    const NodeExit m_root;
};
