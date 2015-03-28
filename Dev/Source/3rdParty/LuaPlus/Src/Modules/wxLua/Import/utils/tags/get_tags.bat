ctags -u --excmd=number --fields=afikms --c-types=cdefgmnpstuvx --c++-types=cdefgmnpstuvx -R %1\*.h

ctags -u --excmd=number --fields=afikms --c-types=cdefgmnpstuvx --c++-types=cdefgmnpstuvx -R -a %1\generic/*.h

ctags -u --excmd=number --fields=afikms --c-types=cdefgmnpstuvx --c++-types=cdefgmnpstuvx -R -a %1\protocol/*.h

ctags -u --excmd=number --fields=afikms --c-types=cdefgmnpstuvx --c++-types=cdefgmnpstuvx -R -a %1\html/*.h

ctags -u --excmd=number --fields=afikms --c-types=cdefgmnpstuvx --c++-types=cdefgmnpstuvx -R -a %1\gtk/*.h

rem --format=2 \

rem --c++-kinds=v \

rem  --fields
rem                a   Access (or export) of class members
rem                f   File-restricted scoping [enabled]
rem                i   Inheritance information
rem                k   Kind of tag as a single letter [enabled]
rem                K   Kind of tag as full name
rem                l   Language of source file containing tag
rem                m   Implementation information
rem                n   Line number of tag definition
rem                s   Scope of tag definition [enabled]
rem                S   Signature of routine (e.g. prototype or parameter list)
rem                z   Include the "kind:" key in kind field


rem C
rem     c  classes
rem     d  macro definitions
rem     e  enumerators (values inside an enumeration)
rem     f  function definitions
rem     g  enumeration names
rem     m  class, struct, and union members
rem     n  namespaces
rem     p  function prototypes [off]
rem     s  structure names
rem     t  typedefs
rem     u  union names
rem     v  variable definitions
rem     x  external variable declarations [off]
rem C++
rem     c  classes
rem     d  macro definitions
rem     e  enumerators (values inside an enumeration)
rem     f  function definitions
rem     g  enumeration names
rem     m  class, struct, and union members
rem     n  namespaces
rem     p  function prototypes [off]
rem     s  structure names
rem     t  typedefs
rem     u  union names
rem     v  variable definitions
rem     x  external variable declarations [off]

