% The .text contents is supposed to be linked --oformat binary with
% b-twoinsn.s and b-goodmain.s, and will provide a LOP_FIXO with invalid;
% (!= 1, != 2), YZ field.
 .text
 .byte 0x98,3,0,3
