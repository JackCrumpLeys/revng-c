#pragma once

//
// This file is distributed under the MIT License. See LICENSE.mit for details.
//

// Forward declarations
class ASTNode;
class ASTTree;

extern RecursiveCoroutine<ASTNode *> simplifyCompareNode(ASTTree &AST,
                                                         ASTNode *RootNode);
