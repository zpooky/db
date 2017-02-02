
class Base {
  public:
  virtual void dox() = 0;
};
class Sub : public Base {
  public:
  void dox();
};
  
