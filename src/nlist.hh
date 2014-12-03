#if !defined(OLORIN_NLIST_H)
#define OLORIN_NLIST_H

template <class T>
class TNListElement {
public:
  // private data.
  T *value;
  bool remove;
  // constructor
  TNListElement(	T *nValue=0,
			TNListElement<T> *nNext=0,
			bool rmv=true) : value(nValue), remove(rmv), next(nNext)
  {
    if(!value)
      value=new T; 
  }
  // destuctor
  ~TNListElement()
  {
    if(remove) {
      delete value;
      if(next) delete next;
    }
    value=0; next=0;
  }
  // pointer to next element in line.
  TNListElement<T> *next;
};

template <class T>
class TNListIterator {
protected:
  TNListElement<T> *first, *cur;
  int elements;
public:
  // constructor
  TNListIterator(T *nF=0) : first(0), cur(0), elements(0)
  {
    first=new TNListElement<T>(nF);
    cur=first;
    elements++;
  }
  // gets us to the last element.
  TNListIterator<T>& WindToLast()
  {
    cur=first;
    while(cur->next)
      cur=cur->next;
    return *this;
  }
  // get us to the first element.
  TNListIterator<T>& First() { cur=first; return *this; }
  // get us to next element in line.
  TNListIterator<T>& Next() { if(!Last()) cur=cur->next; return *this; }
  // get us to previous element, if possible.
  TNListIterator<T>& Prev() {
    TNListElement<T> *tEl=cur;
    if(cur==first) return *this;
    cur=first;
    while(cur->next!=tEl&&!Last())
      cur=cur->next;
    return *this;
  }
  // is this last element?
  bool Last() { return (cur->next==0); }
  // return Current element.
  TNListElement<T>& CurrentElement() { return *cur; }
  T& Current() { return *(cur->value); }
  T& operator[](int i)
  {
    if(i < 0) i=0;
    TNListElement<T> *tEl=first;
    while(tEl && i) {
      tEl=tEl->next; i--;
    }
    if(tEl) return *(tEl->value);
    return *(first->value);
  }
  TNListIterator<T>& Add(T *nEl=0)
  {
    WindToLast().CurrentElement().next=new TNListElement<T>(nEl,0);
    cur=cur->next;
    elements++;
    return *this;
  }
  int Elements() { return elements; }
  TNListIterator<T>& MakeNElements(int nElem)
  {
    for(;elements<nElem;)
      Add();
    return *this;
  }
  bool Find(T *what)
  {
    //		First();
    TNListElement<T> *tEl=first;
    while(tEl) {
      if(*(tEl->value)==*what) { cur=tEl; return true; }
      tEl=tEl->next;
    }
    return false;
  }
};
#endif // OLORIN_NLIST_H
