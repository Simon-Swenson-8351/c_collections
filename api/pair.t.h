#define PAIR_CAT_(x, y) x ## y
#define PAIR_CAT(x, y) PAIR_CAT_(x, y)

#ifndef PAIR_FIRST_TYPENAME
#error "A pair needs a first type, COLN_FIRST_TYPENAME"
#endif

#ifndef PAIR_SECOND_TYPENAME
#error "A pair needs a second type, COLN_SECOND_TYPENAME"
#endif

#if !defined(PAIR_HEADER) && !defined(PAIR_IMPL)
#errir "In order to generate code, you must define either PAIR_HEADER or PAIR_IMPL"
#endif

#ifndef PAIR_TYPENAME
#define PAIR_TYPENAME PAIR_CAT(PAIR_CAT(PAIR_CAT(PAIR_FIRST_TYPENAME, _), PAIR_SECOND_TYPENAME), _pair)
#endif

#if defined(PAIR_FIRST_COPY) || defined(PAIR_SECOND_COPY)
#define PAIR_COPY_FNNAME PAIR_CAT(PAIR_TYPENAME, _copy)
#define PAIR_COPY_SIGN \
    bool PAIR_COPY_FNNAME(PAIR_TYPENAME *dest, PAIR_TYPENAME *src)
#define PAIR_COPY_DECL PAIR_COPY_SIGN;
#elseif
#define PAIR_COPY_FNNAME
#define PAIR_COPY_SIGN
#define PAIR_COPY_DECL
#endif

#if defined(PAIR_FIRST_MOVE) || defined(PAIR_SECOND_MOVE)
#define PAIR_MOVE_FNNAME PAIR_CAT(PAIR_TYPENAME, _move)
#define PAIR_MOVE_SIGN \
    void PAIR_MOVE_FNNAME(PAIR_TYPENAME *dest, PAIR_TYPENAME *src)
#define PAIR_MOVE_DECL PAIR_MOVE_SIGN;
#else
#define PAIR_MOVE_FNNAME
#define PAIR_MOVE_SIGN
#define PAIR_MOVE_DECL
#endif

#if defined(PAIR_FIRST_CLEAR) || defined(PAIR_SECOND_CLEAR)
#define PAIR_CLEAR_FNNAME PAIR_CAT(PAIR_TYPENAME, _clear)
#define PAIR_CLEAR_SIGN \
    void PAIR_CLEAR_FNNAME(PAIR_TYPENAME *to_clear)
#define PAIR_CLEAR_DECL PAIR_CLEAR_SIGN;
#else
#define PAIR_CLEAR_FNNAME
#define PAIR_CLEAR_SIGN
#define PAIR_CLEAR_DECL
#endif

#if defined(PAIR_FIRST_HASH)
#define PAIR_HASH_FIRST_FNNAME PAIR_CAT(PAIR_TYPENAME, _hash_first)
#define PAIR_HASH_FIRST_SIGN \
    size_t PAIR_HASH_FIRST_FNNAME(PAIR_TYPENAME *pair)
#define PAIR_HASH_FIRST_DECL PAIR_HASH_FIRST_SIGN;
#else
#define PAIR_HASH_FIRST_FNNAME
#define PAIR_HASH_FIRST_SIGN
#define PAIR_HASH_FIRST_DECL
#endif

#if defined(PAIR_SECOND_HASH)
#define PAIR_HASH_SECOND_FNNAME PAIR_CAT(PAIR_TYPENAME, _hash_second)
#define PAIR_HASH_SECOND_SIGN \
    size_t PAIR_HASH_SECOND_FNNAME(PAIR_TYPENAME *pair)
#define PAIR_HASH_SECOND_DECL PAIR_HASH_SECOND_SIGN;
#else
#define PAIR_HASH_SECOND_FNNAME
#define PAIR_HASH_SECOND_SIGN
#define PAIR_HASH_SECOND_DECL
#endif

#if defined(PAIR_FIRST_HASH) && defined(PAIR_SECOND_HASH)
#define PAIR_HASH_FNNAME PAIR_CAT(PAIR_TYPENAME, _hash)
#define PAIR_HASH_SIGN size_t PAIR_HASH_FNNAME(PAIR_TYPENAME *pair)
#define PAIR_HASH_DECL PAIR_HASH_SIGN;
#else
#define PAIR_HASH_FNNAME
#define PAIR_HASH_SIGN
#define PAIR_HASH_DECL
#endif

#if defined(PAIR_FIRST_COMPARE)
#define PAIR_COMPARE_FIRST_FNNAME PAIR_CAT(PAIR_TYPENAME, _compare_first)
#define PAIR_COMPARE_FIRST_SIGN \
    int PAIR_COMPARE_FIRST_FNNAME(PAIR_TYPENAME *a, PAIR_TYPENAME *b)
#define PAIR_COMPARE_FIRST_DECL PAIR_COMPARE_FIRST_SIGN;
#else
#define PAIR_COMPARE_FIRST_FNNAME
#define PAIR_COMPARE_FIRST_SIGN
#define PAIR_COMPARE_FIRST_DECL
#endif

#if defined(PAIR_SECOND_COMPARE)
#define PAIR_COMPARE_SECOND_FNNAME PAIR_CAT(PAIR_TYPENAME, _compare_second)
#define PAIR_COMPARE_SECOND_SIGN \
    int PAIR_COMPARE_SECOND_FNNAME(PAIR_TYPENAME *a, PAIR_TYPENAME *b)
#define PAIR_COMPARE_SECOND_DECL PAIR_COMPARE_SECOND_SIGN;
#else
#define PAIR_COMPARE_SECOND_FNNAME
#define PAIR_COMPARE_SECOND_SIGN
#define PAIR_COMPARE_SECOND_DECL
#endif

#if defined(PAIR_FIRST_COMPARE) && defined(PAIR_SECOND_COMPARE)
#define PAIR_COMPARE_FNNAME PAIR_CAT(PAIR_TYPENAME, _compare)
#define PAIR_COMPARE_SIGN \
    int PAIR_COMPARE_FNNAME(PAIR_TYPENAME *a, PAIR_TYPENAME *b)
#define PAIR_COMPARE_DECL PAIR_COMPARE_SIGN;
#else
#define PAIR_COMPARE_FNNAME
#define PAIR_COMPARE_SIGN
#define PAIR_COMPARE_DECL
#endif

#if defined(PAIR_FIRST_COMPARE) || defined(PAIR_FIRST_EQUALS)
#define PAIR_EQUALS_FIRST_FNNAME PAIR_CAT(PAIR_TYPENAME, _equals_first)
#define PAIR_EQUALS_FIRST_SIGN \
    bool PAIR_EQUALS_FIRST_FNNAME(PAIR_TYPENAME *a, PAIR_TYPENAME *b)
#define PAIR_EQUALS_FIRST_DECL PAIR_EQUALS_FIRST_SIGN;
#else
#define PAIR_EQUALS_FIRST_FNNAME
#define PAIR_EQUALS_FIRST_SIGN
#define PAIR_EQUALS_FIRST_DECL
#endif

#if defined(PAIR_SECOND_COMPARE) || defined(PAIR_SECOND_EQUALS)
#define PAIR_EQUALS_SECOND_FNNAME PAIR_CAT(PAIR_TYPENAME, _equals_second)
#define PAIR_EQUALS_SECOND_SIGN \
    bool PAIR_EQUALS_SECOND_FNNAME(PAIR_TYPENAME *a, PAIR_TYPENAME *b)
#define PAIR_EQUALS_SECOND_DECL PAIR_EQUALS_SECOND_SIGN;
#else
#define PAIR_EQUALS_SECOND_FNNAME
#define PAIR_EQUALS_SECOND_SIGN
#define PAIR_EQUALS_SECOND_DECL
#endif

#if (defined(PAIR_FIRST_COMPARE) || defined(PAIR_FIRST_EQUALS)) && (defined(PAIR_SECOND_COMPARE) || defined(PAIR_SECOND_EQUALS))
#define PAIR_EQUALS_FNNAME PAIR_CAT(PAIR_TYPENAME, _equals)
#define PAIR_EQUALS_SIGN \
    bool PAIR_EQUALS_FNNAME(PAIR_TYPENAME *a, PAIR_TYPENAME *b)
#define PAIR_EQUALS_DECL PAIR_EQUALS_SIGN;
#else
#define PAIR_EQUALS_FNNAME
#define PAIR_EQUALS_SIGN
#define PAIR_EQUALS_DECL
#endif

#ifdef PAIR_HEADER

typedef struct PAIR_TYPENAME
{
    PAIR_FIRST_TYPENAME first;
    PAIR_SECOND_TYPENAME second;
} PAIR_TYPENAME;

PAIR_COPY_DECL
PAIR_MOVE_DECL
PAIR_CLEAR_DECL
PAIR_HASH_FIRST_DECL
PAIR_HASH_SECOND_DECL
PAIR_HASH_DECL
PAIR_COMPARE_FIRST_DECL
PAIR_COMPARE_SECOND_DECL
PAIR_COMPARE_DECL
PAIR_EQUALS_FIRST_DECL
PAIR_EQUALS_SECOND_DECL
PAIR_EQUALS_DECL

#endif

#ifdef PAIR_IMPL

#if defined(PAIR_FIRST_COPY) && defined(PAIR_SECOND_COPY)
PAIR_COPY_SIGN
{
    if(!PAIR_FIRST_COPY(&(dest->first), &(src->first))) return false;
    if(!PAIR_SECOND_COPY(&(dest->second), &(src->second)))
    {
        PAIR_FIRST_CLEAR(&(dest->first));
        return false;
    }
    return true;
}
#elif defined(PAIR_FIRST_COPY)
PAIR_COPY_SIGN
{
    if(!PAIR_FIRST_COPY(&(dest->first), &(src->first))) return false;
    dest->second = src->second;
    return true;
}
#elif defined(PAIR_SECOND_COPY)
PAIR_COPY_SIGN
{
    if(!PAIR_SECOND_COPY(&(dest->second), &(src->second))) return false;
    dest->first = src->first;
    return true;
}
#endif

#if defined(PAIR_FIRST_MOVE) && defined(PAIR_SECOND_MOVE)
PAIR_MOVE_SIGN
{
    PAIR_FIRST_MOVE(&(dest->first), &(src->first));
    PAIR_SECOND_MOVE(&(dest->second), &(src->second));
}
#elif defined(PAIR_FIRST_MOVE)
PAIR_MOVE_SIGN
{
    PAIR_FIRST_MOVE(&(dest->first), &(src->first));
    dest->second = src->second;
}
#elif defined(PAIR_SECOND_MOVE)
PAIR_MOVE_SIGN
{
    dest->first = src->first;
    PAIR_SECOND_MOVE(&(dest->second), &(src->second));
}
#endif

#if defined(PAIR_FIRST_CLEAR) && defined(PAIR_SECOND_CLEAR)
PAIR_CLEAR_SIGN
{
    PAIR_FIRST_CLEAR(&(to_clear->first));
    PAIR_SECOND_CLEAR(&(to_clear->second));
}
#elif defined(PAIR_FIRST_CLEAR)
PAIR_CLEAR_SIGN
{
    PAIR_FIRST_CLEAR(&(to_clear->first));
}
#elif defined(PAIR_SECOND_CLEAR)
PAIR_CLEAR_SIGN
{
    PAIR_SECOND_CLEAR(&(to_clear->second));
}
#endif

#ifdef PAIR_FIRST_HASH
PAIR_FIRST_HASH_SIGN
{
    return PAIR_FIRST_HASH(&(pair->first));
}
#endif

#ifdef PAIR_SECOND_HASH
PAIR_SECOND_HASH_SIGN
{
    return PAIR_SECOND_HASH(&(pair->second));
}
#endif

#if defined(PAIR_FIRST_HASH) && defined(PAIR_SECOND_HASH)
PAIR_HASH_SIGN
{
    return PAIR_FIRST_HASH(&(pair->first)) ^ PAIR_SECOND_HASH(&(pair->second));
}
#endif

#ifdef PAIR_FIRST_COMPARE
PAIR_COMPARE_FIRST_SIGN
{
    return PAIR_FIRST_COMPARE(&(a->first), &(b->first));
}
#endif

#ifdef PAIR_SECOND_COMPARE
PAIR_COMPARE_SECOND_SIGN
{
    return PAIR_SECOND_COMAPRE(&(a->second), &(b->second));
}
#endif

#if defined(PAIR_FIRST_COMPARE) && defined(PAIR_SECOND_COMPARE)
PAIR_COMPARE_SIGN
{
    int first_result = PAIR_FIRST_COMPARE(&(a->first), &(b->first));
    if(first_result != 0) return first_result;
    return PAIR_SECOND_COMPARE(&(a->second), &(b->second));
}
#endif

#if defined(PAIR_FIRST_EQUALS)
PAIR_EQUALS_FIRST_SIGN
{
    return PAIR_FIRST_EQUALS(&(a->first), &(b->first));
}
#elif defined(PAIR_FIRST_COMPARE)
PAIR_EQUALS_FIRST_SIGN
{
    return PAIR_FIRST_COMPARE(&(a->first), &(b->first)) == 0;
}
#endif

#if defined(PAIR_SECOND_EQUALS)
PAIR_EQUALS_SECOND_SIGN
{
    return PAIR_SECOND_EQUALS(&(a->second), &(b->second));
}
#elif defined(PAIR_SECOND_COMPARE)
PAIR_EQUALS_SECOND_SIGN
{
    return PAIR_SECOND_COMPARE(&(a->second), &(b->second)) == 0;
}
#endif

#if (defined(PAIR_FIRST_EQUALS) || defined(PAIR_FIRST_COMPARE)) && (defined(PAIR_SECOND_EQUALS) || defined(PAIR_SECOND_COMPARE))
PAIR_EQUALS_SIGN
{
    return PAIR_EQUALS_FIRST_FNNAME(a, b) && PAIR_EQUALS_SECOND_FNNAME(a, b);
}
#endif

#endif
