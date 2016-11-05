#ifndef std_tmpl_pp_h
#define std_tmpl_pp_h

#include <vector>
#include <list>
#include <map>
#include <sys/types.h>

template<class In, class Out, class Pred>
Out
copy_if(In first, In last, Out res, Pred p) {
  while (first != last) {
    if (p(*first))
      *res++ = *first;

    ++first;
  }
  return res;
}

#if 0
template<class T1>
void
process_vector(const std::vector<T1> &v, void (*proc)(T1)) {
  typename std::vector<T1>::const_iterator vi1 = v.begin();
  typename std::vector<T1>::const_iterator vi2 = v.end  ();

  for ( ; vi1 != vi2; ++vi1) {
    (*proc)(*vi1);
  }
}

template<class T1, class T2>
void
process_vector(const std::vector<T1> &v, void (*proc)(T1, T2), T2 data) {
  typename std::vector<T1>::const_iterator vi1 = v.begin();
  typename std::vector<T1>::const_iterator vi2 = v.end  ();

  for ( ; vi1 != vi2; ++vi1) {
    (*proc)(*vi1, data);
  }
}

template<class T1, class T2>
void
process_vector(const std::vector<T1> &v, const T2 &proc) {
  typename std::vector<T1>::const_iterator vi1 = v.begin();
  typename std::vector<T1>::const_iterator vi2 = v.end  ();

  for ( ; vi1 != vi2; ++vi1) {
    proc(*vi1);
  }
}
#endif

template<class T1>
void
rprocess_vector(const std::vector<T1> &v, void (*proc)(T1)) {
  typename std::vector<T1>::const_reverse_iterator vi1 = v.rbegin();
  typename std::vector<T1>::const_reverse_iterator vi2 = v.rend  ();

  for ( ; vi1 != vi2; ++vi1) {
    (*proc)(*vi1);
  }
}

template<class T1, class T2>
void
rprocess_vector(const std::vector<T1> &v, void (*proc)(T1, T2), T2 data) {
  typename std::vector<T1>::const_iterator vi1 = v.rbegin();
  typename std::vector<T1>::const_iterator vi2 = v.rend  ();

  for ( ; vi1 != vi2; ++vi1) {
    (*proc)(*vi1, data);
  }
}

template<class T1>
void
process_vector_if(const std::vector<T1> &v, bool (*check_proc)(T1), void (*proc)(T1)) {
  typename std::vector<T1>::const_iterator vi1 = v.begin();
  typename std::vector<T1>::const_iterator vi2 = v.end  ();

  for ( ; vi1 != vi2; ++vi1) {
    if ((*check_proc)(*vi1))
      (*proc)(*vi1);
  }
}

template<class T1, class T2>
void
process_vector_if(const std::vector<T1> &v, bool (*check_proc)(T1),
                  void (*proc)(T1, T2), T2 data) {
  typename std::vector<T1>::const_iterator vi1 = v.begin();
  typename std::vector<T1>::const_iterator vi2 = v.end  ();

  for ( ; vi1 != vi2; ++vi1) {
    if ((*check_proc)(*vi1))
      (*proc)(*vi1, data);
  }
}

template<class T1, class T2>
void
process_vector_if(const std::vector<T1> &v, bool (*check_proc)(T1, T2),
                  T2 check_data, void (*proc)(T1)) {
  typename std::vector<T1>::const_iterator vi1 = v.begin();
  typename std::vector<T1>::const_iterator vi2 = v.end  ();

  for ( ; vi1 != vi2; ++vi1) {
    if ((*check_proc)(*vi1, check_data))
      (*proc)(*vi1);
  }
}

template<class T1, class T2, class T3>
void
process_vector_if(const std::vector<T1> &v, bool (*check_proc)(T1, T2),
                  T2 check_data, void (*proc)(T1, T3), T3 data) {
  typename std::vector<T1>::const_iterator vi1 = v.begin();
  typename std::vector<T1>::const_iterator vi2 = v.end  ();

  for ( ; vi1 != vi2; ++vi1) {
    if ((*check_proc)(*vi1, check_data))
      (*proc)(*vi1, data);
  }
}

template<class T>
void
vector_to_array(const std::vector<T> &v, T **data, int *num) {
  typename std::vector<T>::const_iterator vi1 = v.begin();
  typename std::vector<T>::const_iterator vi2 = v.end  ();

  *num = v.size();

  *data = new T [*num];

  for (int i = 0; vi1 != vi2; ++vi1, ++i)
    (*data)[i] = *vi1;
}

template<class T>
void
vector_to_array(const std::vector<T> &v, T **data, uint *num) {
  typename std::vector<T>::const_iterator vi1 = v.begin();
  typename std::vector<T>::const_iterator vi2 = v.end  ();

  *num = v.size();

  *data = new T [*num];

  for (int i = 0; vi1 != vi2; ++vi1, ++i)
    (*data)[i] = *vi1;
}

template<class T1>
void
process_list(const std::list<T1> &v, void (*proc)(T1)) {
  typename std::list<T1>::const_iterator vi1 = v.begin();
  typename std::list<T1>::const_iterator vi2 = v.end  ();

  for ( ; vi1 != vi2; ++vi1) {
    (*proc)(*vi1);
  }
}

template<class T1, class T2>
void
process_list(const std::list<T1> &v, void (*proc)(T1, T2), T2 data) {
  typename std::list<T1>::const_iterator vi1 = v.begin();
  typename std::list<T1>::const_iterator vi2 = v.end  ();

  for ( ; vi1 != vi2; ++vi1) {
    (*proc)(*vi1, data);
  }
}

template<class KEY, class VALUE>
void
process_map_second(const std::map<KEY,VALUE> &v, void (*proc)(VALUE)) {
  typename std::map<KEY,VALUE>::const_iterator vi1 = v.begin();
  typename std::map<KEY,VALUE>::const_iterator vi2 = v.end  ();

  for ( ; vi1 != vi2; ++vi1) {
    (*proc)((*vi1).second);
  }
}

template<class KEY, class VALUE, class T3>
void
process_map_second(const std::map<KEY,VALUE> &v, void (*proc)(VALUE,T3), T3 data) {
  typename std::map<KEY,VALUE>::const_iterator vi1 = v.begin();
  typename std::map<KEY,VALUE>::const_iterator vi2 = v.end  ();

  for ( ; vi1 != vi2; ++vi1) {
    (*proc)((*vi1).second, data);
  }
}

template<class KEY, class VALUE, class F>
void
process_map_second(const std::map<KEY,VALUE> &v, const F &f) {
  typename std::map<KEY,VALUE>::const_iterator vi1 = v.begin();
  typename std::map<KEY,VALUE>::const_iterator vi2 = v.end  ();

  for ( ; vi1 != vi2; ++vi1)
    f((*vi1).second);
}

template<class KEY, class VALUE>
bool
find_map_value(const std::map<KEY,VALUE> &v, const KEY &key, VALUE &value) {
  typename std::map<KEY,VALUE>::const_iterator p = v.find(key);

  if (p == v.end()) return false;

  value = (*p).second;

  return true;
}

template<class KEY, class VALUE>
bool
find_map_value(const std::map<KEY,VALUE> &v, const KEY &key, VALUE *value) {
  typename std::map<KEY,VALUE>::const_iterator p = v.find(key);

  if (p == v.end()) return false;

  *value = (*p).second;

  return true;
}

template<class T>
void
slice_vector(const std::vector<T> &v, unsigned int start, std::vector<T> &v1) {
  typename std::vector<T>::const_iterator vi1 = v.begin();
  typename std::vector<T>::const_iterator vi2 = v.end  ();

  unsigned int pos = 0;

  while (vi1 != vi2 && pos < start) {
    ++vi1;

    ++pos;
  }

  while (vi1 != vi2)
    v1.push_back(*vi1);
}

template<class T>
void
slice_vector(const std::vector<T> &v, unsigned int start, unsigned int end, std::vector<T> &v1) {
  typename std::vector<T>::const_iterator vi1 = v.begin();
  typename std::vector<T>::const_iterator vi2 = v.end  ();

  unsigned int pos = 0;

  while (vi1 != vi2 && pos < start) {
    ++vi1;

    ++pos;
  }

  while (vi1 != vi2 && pos < end) {
    v1.push_back(*vi1);

    ++pos;
  }
}

template<class T>
void
append_vector(std::vector<T> &v1, const std::vector<T> &v2)
{
  std::copy(v2.begin(), v2.end(), back_inserter(v1));
}

template<class T1>
void
set_bool_second(T1 t, bool *b) {
  *b = true;
}

#endif
