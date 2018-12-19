int f() {
  bool b = true;
  unsigned char* p = reinterpret_cast<unsigned char*>(&b);
  *p = 10;
  // reading from b is now UB
  return b == 0;
}
