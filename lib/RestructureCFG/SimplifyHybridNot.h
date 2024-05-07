#pragma once

//
// This file is distributed under the MIT License. See LICENSE.mit for details.
//

// Forward declarations
class ASTNode;
class ASTTree;

extern ASTNode *simplifyHybridNot(ASTTree &AST, ASTNode *RootNode);
