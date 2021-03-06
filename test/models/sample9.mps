************************************************************************
*
*  The data in this file represents the following problem:
*
*  Minimize or maximize
*
*      Z = -40C1      - 50c2    - 50c3    - 40c4
*        + 1/2(80c1c1 + 80c1c4  + 100c2c3 + 100c2c2
*        + 100c3c3    + 100c3c2 + 80c4c1  + 80c4c4)
*
*
*  Subject to:
*
*                           c1 + c2           = 1
*                                     c3 + c4 = 1
*                           c1      + c3      = 1
*                                c2      + c4 = 1
*
*  where:                   0 <= c1,c2,c3,c4 <= 1
*                             c1,c2,c3,c4 integer
*
*
************************************************************************
NAME
ROWS
 N  OBJECTRW
 E  R0000001
 E  R0000002
 E  R0000003
 E  R0000004
COLUMNS
    C0000001  OBJECTRW  -0040.000000   R0000001      1.000000
    C0000001  R0000003      1.000000
    C0000002  OBJECTRW  -0050.000000   R0000001      1.000000
    C0000002  R0000004      1.000000
    C0000003  OBJECTRW  -0050.000000   R0000002      1.000000
    C0000003  R0000003      1.000000
    C0000004  OBJECTRW  -0040.000000   R0000002      1.000000
    C0000004  R0000004      1.000000
RHS
    RHS1      R0000001      1.000000   R0000002      1.000000
    RHS1      R0000003      1.000000   R0000004      1.000000
BOUNDS
 BV BOUND1    C0000001      1.000000
 BV BOUND1    C0000002      1.000000
 BV BOUND1    C0000003      1.000000
 BV BOUND1    C0000004      1.000000
ENDATA
