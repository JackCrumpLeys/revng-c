#pragma once

//
// This file is distributed under the MIT License. See LICENSE.mit for details.
//

#include "mlir/IR/AttributeSupport.h"
#include "mlir/IR/Attributes.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinAttributes.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/DialectImplementation.h"

#include "revng/Support/Assert.h"

#include "revng-c/mlir/Dialect/Clift/IR/CliftEnums.h"
#include "revng-c/mlir/Dialect/Clift/IR/CliftInterfaces.h"

// This include should stay here for correct build procedure
#define GET_ATTRDEF_CLASSES
#include "revng-c/mlir/Dialect/Clift/IR/CliftAttributes.h.inc"
#include "revng-c/mlir/Dialect/Clift/IR/CliftStorage.h"

namespace mlir::clift {

// VERY IMPORTANT!!!
// If you upgraded to LLVM 17 and walks on types stopped working, you need to
// read:
// discourse.llvm.org/t/custom-walk-and-replace-for-non-tablegen-types/74229
// This is very brittle and it is very likely that it will change again in
// future llvm releases
class StructType
  : public ::mlir::Attribute::AttrBase<StructType,
                                       Attribute,
                                       StructTypeStorage,
                                       SubElementAttrInterface::Trait,
                                       SizedType::Trait,
                                       TypeDefinition::Trait,
                                       AliasableAttr::Trait,
                                       AttributeTrait::IsMutable> {
public:
  using Base::Base;

  static StructType get(MLIRContext *ctx, uint64_t ID) {
    return Base::get(ctx, ID);
  }

  static StructType get(MLIRContext *ctx,
                        uint64_t ID,
                        llvm::StringRef Name,
                        uint64_t Size,
                        llvm::ArrayRef<FieldAttr> Fields) {
    auto Result = Base::get(ctx, ID);
    Result.setBody(Name, Size, Fields);
    return Result;
  }

  static llvm::StringRef getMnemonic() { return "struct"; }

  std::string getAlias() const { return getName().str(); }

  void setBody(llvm::StringRef Name,
               uint64_t Size,
               llvm::ArrayRef<FieldAttr> fields) {
    // Call into the base to mutate the type.
    LogicalResult result = Base::mutate(Name, Size, fields);

    // Most types expect the mutation to always succeed, but types can implement
    // custom logic for handling mutation failures.
    revng_assert(succeeded(result)
                 && "attempting to change the body of an already-initialized "
                    "type");
  }

  /// Returns the contained type, which may be null if it has not been
  /// initialized yet.
  llvm::ArrayRef<FieldAttr> getFields() { return getImpl()->getFields(); }

  /// Returns the name.
  StringRef getName() const { return getImpl()->getName(); }

  bool isDefinition() const { return getImpl()->isInitialized(); }

  uint64_t getId() const { return getImpl()->getID(); }

  uint64_t getByteSize() { return getImpl()->getSize(); }

  static Attribute parse(AsmParser &parser);

  Attribute print(AsmPrinter &p) const;

  static LogicalResult verify(function_ref<InFlightDiagnostic()> emitError,
                              uint64_t id);
  static LogicalResult verify(function_ref<InFlightDiagnostic()> emitError,
                              uint64_t ID,
                              llvm::StringRef Name,
                              uint64_t Size,
                              llvm::ArrayRef<FieldAttr> fields);

  void walkImmediateSubElements(function_ref<void(Attribute)> walkAttrsFn,
                                function_ref<void(Type)> walkTypesFn) const;
  Attribute replaceImmediateSubElements(ArrayRef<Attribute> replAttrs,
                                        ArrayRef<Type> replTypes) const;
};

class UnionType : public Attribute::AttrBase<UnionType,
                                             Attribute,
                                             UnionTypeStorage,
                                             SubElementAttrInterface::Trait,
                                             SizedType::Trait,
                                             TypeDefinition::Trait,
                                             AliasableAttr::Trait,
                                             AttributeTrait::IsMutable> {
public:
  using Base::Base;

  static UnionType get(MLIRContext *ctx, uint64_t ID) {
    // Call into the base to get a uniqued instance of this type. The parameter
    // (name) is passed after the context.
    return Base::get(ctx, ID);
  }

  static UnionType get(MLIRContext *ctx,
                       uint64_t ID,
                       llvm::StringRef Name,
                       llvm::ArrayRef<FieldAttr> Fields) {
    // Call into the base to get a uniqued instance of this type. The parameter
    // (name) is passed after the context.
    auto Result = Base::get(ctx, ID);
    Result.setBody(Name, Fields);
    return Result;
  }

  static llvm::StringRef getMnemonic() { return "union"; }

  void setBody(llvm::StringRef Name, llvm::ArrayRef<FieldAttr> fields) {
    // Call into the base to mutate the type.
    LogicalResult result = Base::mutate(Name, fields);

    // Most types expect the mutation to always succeed, but types can implement
    // custom logic for handling mutation failures.
    revng_assert(succeeded(result)
                 && "attempting to change the body of an already-initialized "
                    "type");
  }

  /// Returns the contained type, which may be null if it has not been
  /// initialized yet.
  llvm::ArrayRef<FieldAttr> getFields() { return getImpl()->getFields(); }

  /// Returns the name.
  StringRef getName() const { return getImpl()->getName(); }

  bool isDefinition() const { return getImpl()->isInitialized(); }

  uint64_t getId() const { return getImpl()->getID(); }

  uint64_t getByteSize() {
    if (not isDefinition())
      return 0;
    uint64_t Max = 0;
    for (auto Field : getFields()) {
      mlir::Type FieldType = Field.getType();
      uint64_t Size = FieldType.cast<mlir::clift::ValueType>().getByteSize();
      Max = Size > Max ? Size : Max;
    }
    return Max;
  }

  static Attribute parse(AsmParser &parser);
  Attribute print(AsmPrinter &p) const;

  static LogicalResult verify(function_ref<InFlightDiagnostic()> emitError,
                              uint64_t id) {
    return mlir::success();
  }
  static LogicalResult verify(function_ref<InFlightDiagnostic()> emitError,
                              uint64_t ID,
                              llvm::StringRef Name,
                              uint64_t Size,
                              llvm::ArrayRef<FieldAttr> fields);
  std::string getAlias() const { return getName().str(); }

  // since mlir types and attributes are immutable, the infrastructure must
  // provide to replace a subelement of the hierarchy. These methods allow
  // to do. Notice that since LLVM17 these are no longer methods requested
  // by the SubElementAttrInterface but are instead a builtin property of
  // all types and attributes, so it will break.
  void walkImmediateSubElements(function_ref<void(Attribute)> walkAttrsFn,
                                function_ref<void(Type)> walkTypesFn) const;
  Attribute replaceImmediateSubElements(ArrayRef<Attribute> replAttrs,
                                        ArrayRef<Type> replTypes) const;
};
} // namespace mlir::clift
