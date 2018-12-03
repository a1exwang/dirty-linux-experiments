#include <cfu/visitors/find_usages.hpp>


using namespace std;
using namespace clang;

namespace cfu {

bool FindMemberExpr::VisitMemberExpr(clang::MemberExpr *S) {
  auto baseType = S->getBase()->getType();
  auto memberName = S->getMemberNameInfo().getAsString();
  auto isArray = S->isArrow();

  if (baseType.getAsString() == baseType.getAsString() && memberName == memberName) {
    printf("usage found: class name: '%s', member name: '%s', isArrow '%d'\n", baseType.getAsString().c_str(), memberName.c_str(), isArray);
    cout << "At: " << LocationString(S->getLocStart(), sourceManager) << endl;
  }

  return true;
}

}
