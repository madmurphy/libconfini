/*  Conditionals  */
#define __PP_IIF__(CONDITION, ...) __PP_EVALUCAT__(__PP_UCAT__(__COND, CONDITION), _)(__VA_ARGS__, , )
#define __COND_0__(IF_TRUE, IF_FALSE, ...) IF_FALSE
#define __COND_1__(IF_TRUE, ...) IF_TRUE
