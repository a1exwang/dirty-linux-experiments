//===- PrintFunctionNames.cpp ---------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"

#include <sstream>
#include <iostream>
#include <cfu/visitors/find_by_location.hpp>
#include <cfu/visitors/find_usages.hpp>

using namespace clang;
using namespace std;

namespace {

class FindUsagesConsumer : public ASTConsumer {
  CompilerInstance &Instance;
  string File;
  int Line, Col;
public:
  FindUsagesConsumer(CompilerInstance &Instance,
                     string File, int Line, int Col)
      : Instance(Instance), File(File), Line(Line), Col(Col) {}


  void HandleTranslationUnit(ASTContext& context) override {
    printf("File:Line:Col = %s:%d:%d\n", File.c_str(), Line, Col);
    auto file = Instance.getFileManager().getFile(File);
    auto targetLoc = Instance.getSourceManager().translateFileLineCol(file, Line, Col);

    cfu::FindTypeFromLocation v(Instance.getSourceManager(), {targetLoc});
    v.TraverseDecl(context.getTranslationUnitDecl());

    if (!v.found) {
      return;
    }
    cfu::FindMemberExpr findUsages(Instance.getSourceManager(), v.BaseType, v.MemberName);
    findUsages.TraverseDecl(context.getTranslationUnitDecl());
//    context.getTranslationUnitDecl()->dump();
  }
};

class FindUsagesAction : public PluginASTAction {
  string File;
  int Col, Line;
public:

//  ActionType getActionType() override { return AddAfterMainAction; }
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return llvm::make_unique<FindUsagesConsumer>(CI, File, Line, Col);
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      llvm::errs() << "PrintFunctionNames arg = " << args[i] << "\n";

      // Example error handling.
      DiagnosticsEngine &D = CI.getDiagnostics();
      if (args[i] == "-file") {
        if (i + 1 >= e) {
          D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                     "missing -file argument"));
          return false;
        }
        ++i;
        File = args[i];
      } else if (args[i] == "-line") {
        if (i + 1 >= e) {
          D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                     "missing -line argument"));
          return false;
        }
        ++i;
        Line = atoi(args[i].c_str());
      } else if (args[i] == "-col") {
        if (i + 1 >= e) {
          D.Report(D.getCustomDiagID(DiagnosticsEngine::Error,
                                     "missing -col argument"));
          return false;
        }
        ++i;
        Col = atoi(args[i].c_str());
      }
    }
    if (!args.empty() && args[0] == "help")
      PrintHelp(llvm::errs());

    return true;
  }
  void PrintHelp(llvm::raw_ostream& ros) {
    ros << "Help for PrintFunctionNames plugin goes here\n";
  }
};
}

static FrontendPluginRegistry::Add<FindUsagesAction> X("find-usages", "find all usages of a struct field");
