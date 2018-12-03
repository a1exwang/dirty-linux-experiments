#pragma once

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <sstream>

namespace cfu {

static std::string LocationString(const clang::SourceLocation &location, const clang::SourceManager &sm) {
  clang::SourceLocation SpellingLoc = sm.getSpellingLoc(location);
  clang::PresumedLoc PLoc = sm.getPresumedLoc(SpellingLoc);
  std::stringstream ss;
  ss << PLoc.getFilename() << ":" << PLoc.getLine() << ":" << PLoc.getColumn();
  return ss.str();
}

}
