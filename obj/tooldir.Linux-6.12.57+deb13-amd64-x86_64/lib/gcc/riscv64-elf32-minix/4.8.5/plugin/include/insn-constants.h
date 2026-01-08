/* Generated automatically by the program `genconstants'
   from the machine description file `md'.  */

#ifndef GCC_INSN_CONSTANTS_H
#define GCC_INSN_CONSTANTS_H

#define RETURN_ADDR_REGNUM 1

enum unspec {
  UNSPEC_LOAD_LOW = 0,
  UNSPEC_LOAD_HIGH = 1,
  UNSPEC_STORE_WORD = 2,
  UNSPEC_EH_RETURN = 3,
  UNSPEC_ADDRESS_FIRST = 4,
  UNSPEC_LOAD_GOT = 5,
  UNSPEC_TLS = 6,
  UNSPEC_TLS_LE = 7,
  UNSPEC_TLS_IE = 8,
  UNSPEC_TLS_GD = 9,
  UNSPEC_BLOCKAGE = 10,
  UNSPEC_FENCE = 11,
  UNSPEC_FENCE_I = 12,
  UNSPEC_COMPARE_AND_SWAP = 13,
  UNSPEC_SYNC_OLD_OP = 14,
  UNSPEC_SYNC_EXCHANGE = 15,
  UNSPEC_ATOMIC_STORE = 16,
  UNSPEC_MEMORY_BARRIER = 17
};
#define NUM_UNSPEC_VALUES 18
extern const char *const unspec_strings[];

#endif /* GCC_INSN_CONSTANTS_H */
