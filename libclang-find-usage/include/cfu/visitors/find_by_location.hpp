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

using namespace std;
using namespace clang;

namespace cfu {
class FindTypeFromLocation : public RecursiveASTVisitor<FindTypeFromLocation> {

public:
  FindTypeFromLocation(SourceManager &sourceManager, std::vector<SourceLocation> targetLocs)
      :sourceManager(sourceManager), targetLocs(targetLocs), found(false) {}

  bool VisitMemberExpr(MemberExpr *S) {
    auto baseType = S->getBase()->getType();
    auto memberName = S->getMemberNameInfo().getAsString();
    auto isArray = S->isArrow();

    // optimize by binary search
    for (const auto &loc : targetLocs) {
      if (S->getLocStart().getRawEncoding() <= loc.getRawEncoding() &&
          loc.getRawEncoding() < S->getLocEnd().getRawEncoding()) {

        // found
        cout << "At: " << LocationString(S->getLocStart(), sourceManager) << endl;
        BaseType = baseType;
        MemberName = memberName;
        printf("found: class name: '%s', member name: '%s', isArrow '%d'\n", baseType.getAsString().c_str(), memberName.c_str(), isArray);
        found = true;
      }
    }
    return true;
  }

  std::set<FunctionDecl*> LateParsedDecls;
  SourceManager &sourceManager;
  std::string MemberName;
  QualType BaseType;
  std::vector<SourceLocation> targetLocs;
  bool found;
};
}