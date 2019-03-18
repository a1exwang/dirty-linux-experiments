typedef int TYPE;
//typedef unsigned int TYPE;

TYPE test(TYPE x) {
  TYPE a = x + 1;
  
  if (a > x) {
    // This is will always be true
    return test(x);
  }
  return 0;
}
