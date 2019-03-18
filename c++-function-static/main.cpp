
void x();

class LongClassName {
public:
  LongClassName() {
    x();
  }
  void doit() { x(); }
};

void do_something() {
  static LongClassName a;
  a.doit();
}
