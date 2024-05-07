#pragma once

//
// This file is distributed under the MIT License. See LICENSE.mit for details.
//

// Forward declarations
namespace llvm {
class BasicBlock;
} // namespace llvm

#include "revng-c/RestructureCFG/BasicBlockNode.h"
extern template class BasicBlockNode<llvm::BasicBlock *>;

using BasicBlockNodeBB = BasicBlockNode<llvm::BasicBlock *>;
