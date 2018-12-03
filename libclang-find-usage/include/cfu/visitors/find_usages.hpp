#pragma once

#include <cfu/utils/source_file.hpp>

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

#include <sstream>
#include <iostream>


namespace cfu {


struct FindMemberExpr : public clang::RecursiveASTVisitor<FindMemberExpr> {
  clang::SourceManager &sourceManager;
  clang::QualType baseType;
  std::string memberName;

  FindMemberExpr(clang::SourceManager &sourceManager, clang::QualType BaseType,
      const std::string &MemberName)
      : sourceManager(sourceManager), baseType(BaseType), memberName(MemberName) {}

  bool VisitMemberExpr(clang::MemberExpr *S);
};

}
