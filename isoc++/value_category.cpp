#include <iostream>
#include <type_traits>

/// https://stackoverflow.com/a/16638081/4715389
template<typename T>
struct value_category {
    // Or can be an integral or enum value
    static constexpr auto value = "prvalue";
};

template<typename T>
struct value_category<T&> {
    static constexpr auto value = "lvalue";
};

template<typename T>
struct value_category<T&&> {
    static constexpr auto value = "xvalue";
};

// Double parens for ensuring we inspect an expression,
// not an entity
#define VC(expr) std::cout << "\"" #expr "\": " << value_category<decltype((expr))>::value << std::endl

struct Object {
  Object() :member(0) {
    std::cout << "Object::Object()" << std::endl;
  }
  Object(const Object &rhs) {
    std::cout << "Object::Object(const Object &rhs) this=" << this << std::endl;
  }
  Object &operator=(const Object &rhs) {
    std::cout << "Object::operator=(const Object &rhs) this=" << this << std::endl;
    this->member = rhs.member;
    return *this;
  }

  Object operator+(const Object &rhs) {
    Object obj;
    obj.member = this->member + rhs.member;
    return obj;
  }
  int member;
};

Object create_object() {
	Object obj;
	return obj;
}

Object &&create_rvalue_reference(Object &obj) {
	return static_cast<Object&&>(obj);
}

void use_object(Object obj) {
}

void xvalues() {
	Object obj;
  Object arr[10];
  int Object::*pmember = &Object::member;

  // xvalue:
  // STD 7.2.1-4.1
  // the result of calling a function, whether implicitly or explicitly, whose return type is an rvalue reference
  //  to object type
  VC(create_rvalue_reference(obj));

  // 4.2 a cast to an rvalue reference to object type
  VC(static_cast<Object&&>(obj));

  // 4.3 a subscripting operation with an xvalue array operand
  VC(std::move(arr)[0]);
  // 4.4  
  // a class member access expression designating a non-static data member of non-reference type in which
  // the object expression is an xvalue
  VC(std::move(obj).member);

  // 4.5
  //  a .* pointer-to-member expression in which the first operand is an xvalue and the second operand is a
  //  pointer to data member
  VC(std::move(obj).*pmember);

  // In general, the effect of this rule is that named rvalue references are treated as lvalues and unnamed rvalue
  // references to objects are treated as xvalues; rvalue references to functions are treated as lvalues whether
  // named or not. — end note]
  Object &&rvalue_ref = std::move(obj);
  VC(std::move(obj));
  VC(rvalue_ref);

  // 7.2.1-7
  // Whenever a glvalue appears as an operand of an operator that expects a prvalue for that operand, the
  // lvalue-to-rvalue (7.3.1), array-to-pointer (7.3.2), or function-to-pointer (7.3.3) standard conversions are applied
  // to convert the expression to a prvalue
  Object a;
  // "Object()" is a prvalue, but Object(const Object &rhs) requires an glvalue, so "Object()" is temporarily converted to xvalue
  a = Object();

  {
    // 7.3.1 
    // A glvalue (7.2.1) of a non-function, non-array type T can be converted to a prvalue
    //
    // 3.1 If T is cv std::nullptr_t, the result is a null pointer constant (7.3.11). [Note: Since the conversion does
    //  not access the object to which the glvalue refers, there is no side effect even if T is volatile-qualified (6.9.1),
    //  and the glvalue can refer to an inactive member of a union]
    
    // 3.2 if T has a class type, the conversion copy-initializes the result object from the glvalue
    Object a, b;
    Object &&rref_a = a + b;

    // 3.3  if the object to which the glvalue refers contains an invalid pointer value (6.7.5.4.2, 6.7.5.4.3),
    // the behavior is implementation-defined.

    // 3.4  Otherwise, the object indicated by the glvalue is read (3.1), and the value contained in the object is the
    // prvalue result.
  }

  // 7.3.2  
  // An lvalue or rvalue of type “array of N T” or “array of unknown bound of T” can be converted to a prvalue of
  // type “pointer to T”. The temporary materialization conversion (7.3.4) is applied. The result is a pointer to
  // the first element of the array
  {
    int arr[10];
    VC(arr);
    VC(static_cast<int*>(arr));
  }

  // 7.3.4
  // A prvalue of type T can be converted to an xvalue of type T
  {
    VC(Object());
    VC(Object().member);
    int a = Object().member;
  }


}

void put_int(int x) {
}

// 7.3.6
//  Integral promotions
void integer_promotions() {
  // A prvalue of an integer type other than bool, char16_t, char32_t, or wchar_t whose integer conversion
  // rank (6.8.4) is less than the rank of int can be converted to a prvalue of type int if int can represent all the
  // values of the source type; otherwise, the source prvalue can be converted to a prvalue of type unsigned int
  short a = 3;
  put_int(a);

  
}

int main() {
  xvalues();
  integer_promotions();
}
