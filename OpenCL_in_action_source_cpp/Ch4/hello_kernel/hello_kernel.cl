__kernel void hello_kernel_16(__global char16 *msg) {
  *msg = (char16)('O', 'p', 'e', 'n', 'C', 'L', ' ', 'L', 'e', 'a', 'r', 'n',
                  'i', 'n', 'g', '\0');
}

__kernel void hello_kernel_hi(__global char8 *msg) {
  *msg = (char16)('O', 'p', 'e', 'n', 'C', 'L', ' ', 'L', 'e', 'a', 'r', 'n',
                  'i', 'n', 'g', '\0')
             .hi;
}

__kernel void hello_kernel_lo(__global char8 *msg) {
  *msg = (char16)('O', 'p', 'e', 'n', 'C', 'L', ' ', 'L', 'e', 'a', 'r', 'n',
                  'i', 'n', 'g', '\0')
             .lo;
}

__kernel void hello_kernel_odd(__global char8 *msg) {
  *msg = (char16)('O', 'p', 'e', 'n', 'C', 'L', ' ', 'L', 'e', 'a', 'r', 'n',
                  'i', 'n', 'g', '\0')
             .odd;
}

__kernel void hello_kernel_even(__global char8 *msg) {
  *msg = (char16)('O', 'p', 'e', 'n', 'C', 'L', ' ', 'L', 'e', 'a', 'r', 'n',
                  'i', 'n', 'g', '\0')
             .even;
}

__kernel void hello_kernel_s045e(__global char4 *msg) {
  *msg = (char16)('O', 'p', 'e', 'n', 'C', 'L', ' ', 'L', 'e', 'a', 'r', 'n',
                  'i', 'n', 'g', '\0')
             .s045e;
}
