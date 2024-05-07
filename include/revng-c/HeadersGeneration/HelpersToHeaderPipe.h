#pragma once

//
// This file is distributed under the MIT License. See LICENSE.mit for details.
//

#include <array>
#include <string>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/Support/raw_ostream.h"

#include "revng/Pipeline/Context.h"
#include "revng/Pipeline/Contract.h"
#include "revng/Pipes/FileContainer.h"
#include "revng/Pipes/Kinds.h"

#include "revng-c/Pipes/Kinds.h"

namespace revng::pipes {

inline constexpr char HelpersHeaderFactoryMIMEType[] = "text/x.c+ptml";
inline constexpr char HelpersHeaderFactorySuffix[] = ".h";
inline constexpr char HelpersHeaderFactoryName[] = "helpers-header";
using HelpersHeaderFileContainer = FileContainer<&kinds::HelpersHeader,
                                                 HelpersHeaderFactoryName,
                                                 HelpersHeaderFactoryMIMEType,
                                                 HelpersHeaderFactorySuffix>;

class HelpersToHeader {
public:
  static constexpr auto Name = "helpers-to-header";

  std::array<pipeline::ContractGroup, 1> getContract() const {
    using namespace pipeline;
    using namespace revng::kinds;

    return { ContractGroup{ Contract(StackAccessesSegregated,
                                     0,
                                     HelpersHeader,
                                     1,
                                     InputPreservation::Preserve) } };
  }

  void run(const pipeline::ExecutionContext &Ctx,
           pipeline::LLVMContainer &IRContainer,
           HelpersHeaderFileContainer &HeaderFile);

  void print(const pipeline::Context &Ctx,
             llvm::raw_ostream &OS,
             llvm::ArrayRef<std::string> ContainerNames) const;
};

} // end namespace revng::pipes
