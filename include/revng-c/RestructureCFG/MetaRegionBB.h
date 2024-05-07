#pragma once

//
// This file is distributed under the MIT License. See LICENSE.mit for details.
//

// Forward declarations
namespace llvm {
class BasicBlock;
} // namespace llvm

#include "revng-c/RestructureCFG/MetaRegion.h"
extern template class MetaRegion<llvm::BasicBlock *>;

using MetaRegionBB = MetaRegion<llvm::BasicBlock *>;
