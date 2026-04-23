/*
 * omi_riscv_vm.c
 *
 * OMI-LISP VM for RISC-V virt.
 *
 * Four points of control over the control plane:
 *   0. BOM      0xEF        frame sync / beginning of message
 *   1. ESC      0x1B        mode shift
 *   2. BOUNDARY 0x1C..0x1F FS / GS / RS / US structure separators
 *   3. ECC      0xE0..0xEE binary error-correction witness
 *
 * The runtime keeps direct byte-level control. Every input byte is first
 * represented as a raw binary lexer mask, then projected into HEADER8,
 * dot-notation structure, ECC status, and an emission witness.
 */

#define _DEFAULT_SOURCE

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/reboot.h>

#define OMI_BOM_BYTE 0xEFu

#define OMI_MASK_BOM       0x0001u
#define OMI_MASK_ESC       0x0002u
#define OMI_MASK_BOUNDARY  0x0004u
#define OMI_MASK_ECC       0x0008u
#define OMI_MASK_REF       0x0010u
#define OMI_MASK_PTR       0x0020u
#define OMI_MASK_PRINT     0x0040u
#define OMI_MASK_DOT       0x0080u
#define OMI_MASK_LPAREN    0x0100u
#define OMI_MASK_RPAREN    0x0200u
#define OMI_MASK_BRAILLE   0x0400u
#define OMI_MASK_AEGEAN    0x0800u

typedef enum {
    OMI_PLANE_ASCII = 0,
    OMI_PLANE_BRAILLE = 1,
    OMI_PLANE_AEGEAN = 2,
    OMI_PLANE_OMICRON = 3
} omi_plane_t;

typedef enum {
    OMI_CTL_DATA = 0,
    OMI_CTL_BOM = 1,
    OMI_CTL_ESC = 2,
    OMI_CTL_BOUNDARY = 3,
    OMI_CTL_ECC = 4
} omi_control_kind_t;

static omi_control_kind_t control_kind(uint8_t b);

typedef struct {
    uint8_t synced;
    uint8_t esc_active;
    uint8_t boundary;
    uint8_t ecc_status;
    uint8_t ecc_nibble;
} omi_control_plane_t;

typedef struct {
    uint8_t raw;
    uint8_t corrected;
    uint8_t data;
    uint8_t syndrome;
    uint8_t status;
} omi_ecc_witness_t;

typedef struct {
    uint8_t bytes[8];
    uint8_t ecc_low[8];
    uint8_t ecc_high[8];
} omi_header8_t;

typedef struct {
    const char *name;
    uint16_t radix;
    uint16_t digit;
} omi_radix_witness_t;

typedef struct {
    uint64_t tick;
    uint8_t previous_state;
    uint8_t current_state;
    uint8_t input;
    uint8_t winner;
    uint8_t header8_current;
    uint8_t list_depth;
    uint8_t dot_pending;
    uint8_t pair_left;
    uint8_t last_atom;
    uint8_t has_last_atom;
    uint32_t digest;
    omi_control_plane_t control;
    omi_header8_t header8;
} omi_state_t;

static uint8_t rotl8(uint8_t x, unsigned k) {
    k &= 7u;
    return (uint8_t)((x << k) | (x >> ((8u - k) & 7u)));
}

static uint8_t rotr8(uint8_t x, unsigned k) {
    k &= 7u;
    return (uint8_t)((x >> k) | (x << ((8u - k) & 7u)));
}

static uint32_t fnv1a32(const void *data, size_t length) {
    const uint8_t *p = (const uint8_t *)data;
    uint32_t h = UINT32_C(2166136261);
    for (size_t i = 0; i < length; ++i) {
        h ^= p[i];
        h *= UINT32_C(16777619);
    }
    return h;
}

static uint8_t hamming84_encode(uint8_t nibble) {
    uint8_t d0 = (uint8_t)((nibble >> 0) & 1u);
    uint8_t d1 = (uint8_t)((nibble >> 1) & 1u);
    uint8_t d2 = (uint8_t)((nibble >> 2) & 1u);
    uint8_t d3 = (uint8_t)((nibble >> 3) & 1u);
    uint8_t p1 = (uint8_t)(d0 ^ d1 ^ d3);
    uint8_t p2 = (uint8_t)(d0 ^ d2 ^ d3);
    uint8_t p4 = (uint8_t)(d1 ^ d2 ^ d3);
    uint8_t code = 0;

    code |= (uint8_t)(p1 << 0);
    code |= (uint8_t)(p2 << 1);
    code |= (uint8_t)(d0 << 2);
    code |= (uint8_t)(p4 << 3);
    code |= (uint8_t)(d1 << 4);
    code |= (uint8_t)(d2 << 5);
    code |= (uint8_t)(d3 << 6);
    code |= (uint8_t)((p1 ^ p2 ^ d0 ^ p4 ^ d1 ^ d2 ^ d3) << 7);
    return code;
}

static omi_ecc_witness_t hamming84_decode(uint8_t code) {
    omi_ecc_witness_t w;
    uint8_t b1 = (uint8_t)((code >> 0) & 1u);
    uint8_t b2 = (uint8_t)((code >> 1) & 1u);
    uint8_t b3 = (uint8_t)((code >> 2) & 1u);
    uint8_t b4 = (uint8_t)((code >> 3) & 1u);
    uint8_t b5 = (uint8_t)((code >> 4) & 1u);
    uint8_t b6 = (uint8_t)((code >> 5) & 1u);
    uint8_t b7 = (uint8_t)((code >> 6) & 1u);
    uint8_t b8 = (uint8_t)((code >> 7) & 1u);
    uint8_t s1 = (uint8_t)(b1 ^ b3 ^ b5 ^ b7);
    uint8_t s2 = (uint8_t)(b2 ^ b3 ^ b6 ^ b7);
    uint8_t s4 = (uint8_t)(b4 ^ b5 ^ b6 ^ b7);
    uint8_t syndrome = (uint8_t)(s1 | (s2 << 1) | (s4 << 2));
    uint8_t parity = (uint8_t)(b1 ^ b2 ^ b3 ^ b4 ^ b5 ^ b6 ^ b7 ^ b8);
    uint8_t corrected = code;
    uint8_t status = 0;

    if (syndrome != 0u && parity != 0u) {
        corrected ^= (uint8_t)(1u << (syndrome - 1u));
        status = 1u;
    } else if (syndrome == 0u && parity != 0u) {
        corrected ^= 0x80u;
        status = 1u;
    } else if (syndrome != 0u && parity == 0u) {
        status = 2u;
    }

    w.raw = code;
    w.corrected = corrected;
    w.syndrome = syndrome;
    w.status = status;
    w.data = (uint8_t)((((corrected >> 6) & 1u) << 3) |
                       (((corrected >> 5) & 1u) << 2) |
                       (((corrected >> 4) & 1u) << 1) |
                       (((corrected >> 2) & 1u) << 0));
    return w;
}

static omi_plane_t plane_of_byte(uint8_t b) {
    if (control_kind(b) != OMI_CTL_DATA) return OMI_PLANE_OMICRON;
    if (b >= 0xC0u) return OMI_PLANE_AEGEAN;
    if (b >= 0x80u) return OMI_PLANE_BRAILLE;
    return OMI_PLANE_ASCII;
}

static const char *plane_name(omi_plane_t plane) {
    switch (plane) {
        case OMI_PLANE_ASCII: return "ASCII";
        case OMI_PLANE_BRAILLE: return "BRAILLE";
        case OMI_PLANE_AEGEAN: return "AEGEAN";
        case OMI_PLANE_OMICRON: return "OMICRON";
    }
    return "UNKNOWN";
}

static uint8_t omicron_digit(omi_control_kind_t kind) {
    switch (kind) {
        case OMI_CTL_BOM: return 0u;
        case OMI_CTL_ESC: return 1u;
        case OMI_CTL_BOUNDARY: return 2u;
        case OMI_CTL_ECC: return 3u;
        case OMI_CTL_DATA:
        default: return 0u;
    }
}

static omi_radix_witness_t notation_witness(uint8_t b) {
    omi_control_kind_t kind = control_kind(b);
    omi_plane_t plane = plane_of_byte(b);
    omi_radix_witness_t w;

    if (kind != OMI_CTL_DATA) {
        w.name = "OMICRON";
        w.radix = 4u;
        w.digit = omicron_digit(kind);
        return w;
    }

    if (plane == OMI_PLANE_BRAILLE) {
        w.name = "BRAILLE";
        w.radix = 256u;
        w.digit = b;
        return w;
    }

    if (plane == OMI_PLANE_AEGEAN) {
        w.name = "AEGEAN";
        w.radix = 64u;
        w.digit = (uint16_t)(b & 0x3Fu);
        return w;
    }

    w.name = "HEXADECIMAL";
    w.radix = 16u;
    w.digit = (uint16_t)(b & 0x0Fu);
    return w;
}

static uint16_t lexer_mask(uint8_t b) {
    uint16_t m = 0;
    if (b == OMI_BOM_BYTE) m |= OMI_MASK_BOM;
    if (b == 0x1Bu) m |= OMI_MASK_ESC;
    if (b >= 0x1Cu && b <= 0x1Fu) m |= OMI_MASK_BOUNDARY;
    if (b >= 0xE0u && b <= 0xEEu) m |= OMI_MASK_ECC;
    if (b <= 0x0Fu) m |= OMI_MASK_REF;
    if (b >= 0x10u && b <= 0x1Fu) m |= OMI_MASK_PTR;
    if (b >= 0x40u) m |= OMI_MASK_PRINT;
    if (b == 0x2Eu) m |= OMI_MASK_DOT;
    if (b == 0x28u) m |= OMI_MASK_LPAREN;
    if (b == 0x29u) m |= OMI_MASK_RPAREN;
    if (b >= 0x80u && b < 0xC0u) m |= OMI_MASK_BRAILLE;
    if (b >= 0xC0u) m |= OMI_MASK_AEGEAN;
    return m;
}

static omi_control_kind_t control_kind(uint8_t b) {
    if (b == OMI_BOM_BYTE) return OMI_CTL_BOM;
    if (b == 0x1Bu) return OMI_CTL_ESC;
    if (b >= 0x1Cu && b <= 0x1Fu) return OMI_CTL_BOUNDARY;
    if (b >= 0xE0u && b <= 0xEEu) return OMI_CTL_ECC;
    return OMI_CTL_DATA;
}

static uint8_t omi_delta(uint8_t x, uint8_t ecc) {
    return (uint8_t)(rotl8(x, 1u) ^ rotl8(x, 3u) ^ rotr8(x, 2u) ^ ecc);
}

static uint8_t pick_winner(uint8_t prev, uint8_t input, uint64_t tick, uint8_t ecc) {
    uint8_t mixed = (uint8_t)((prev * 17u) ^ (input * 29u) ^ (uint8_t)tick ^ ecc);
    return (uint8_t)(mixed % 7u);
}

static uint8_t next_state(uint8_t prev, uint8_t input, uint8_t winner, uint8_t ecc) {
    uint8_t d = omi_delta((uint8_t)(prev ^ input), ecc);
    return (uint8_t)((prev + input + winner + d) & 0x3Fu);
}

static void header8_set(omi_state_t *st, uint8_t input, uint8_t curr) {
    uint8_t h[8] = {0x00u, 0x1Bu, 0x1Cu, 0x1Du, 0x1Eu, 0x1Fu, input, curr};
    memcpy(st->header8.bytes, h, sizeof(h));
    for (size_t i = 0; i < sizeof(h); ++i) {
        st->header8.ecc_low[i] = hamming84_encode((uint8_t)(h[i] & 0x0Fu));
        st->header8.ecc_high[i] = hamming84_encode((uint8_t)((h[i] >> 4) & 0x0Fu));
    }
}

static uint8_t is_ascii_atom(uint8_t b) {
    if (b <= 0x20u) return 0u;
    if (b == 0x28u || b == 0x29u || b == 0x2Eu) return 0u;
    if (b >= 0x80u) return 0u;
    return 1u;
}

static const char *emit_name(uint8_t b, uint16_t mask) {
    if (mask & OMI_MASK_BOM) return "CONTROL.BOM";
    if (mask & OMI_MASK_ESC) return "CONTROL.ESC";
    if (mask & OMI_MASK_BOUNDARY) return "CONTROL.BOUNDARY";
    if (mask & OMI_MASK_ECC) return "CONTROL.ECC";
    if (mask & OMI_MASK_LPAREN) return "STRUCT.LIST-OPEN";
    if (mask & OMI_MASK_RPAREN) return "STRUCT.LIST-CLOSE";
    if (mask & OMI_MASK_DOT) return "STRUCT.DOT";
    if (mask & OMI_MASK_REF) return "TOKEN.REFERENCE";
    if (mask & OMI_MASK_PTR) return "TOKEN.POINTER";
    if (mask & OMI_MASK_AEGEAN) return "TOKEN.AEGEAN";
    if (mask & OMI_MASK_BRAILLE) return "TOKEN.BRAILLE";
    if (plane_of_byte(b) == OMI_PLANE_ASCII) return "TOKEN.ASCII";
    return "TOKEN.UNKNOWN";
}

static void print_header8(const omi_state_t *st) {
    printf(" header8=[");
    for (size_t i = 0; i < 8u; ++i) {
        printf("%s0x%02X", i == 0u ? "" : " ", st->header8.bytes[i]);
    }
    printf("]");
}

static void print_dot(omi_state_t *st, uint8_t b) {
    if (b == 0x28u) {
        if (st->list_depth != UINT8_MAX) st->list_depth += 1u;
        st->dot_pending = 0u;
        printf(" dot=STRUCT.LIST(depth=%u)", st->list_depth);
        return;
    }
    if (b == 0x29u) {
        printf(" dot=STRUCT.CLOSE(depth=%u)", st->list_depth);
        if (st->list_depth != 0u) st->list_depth -= 1u;
        st->dot_pending = 0u;
        return;
    }
    if (b == 0x2Eu) {
        if (st->has_last_atom) {
            st->pair_left = st->last_atom;
            st->dot_pending = 1u;
            printf(" dot=STRUCT.DOT(left=0x%02X)", st->pair_left);
        } else {
            st->dot_pending = 0u;
            printf(" dot=STRUCT.DOT-DANGLING");
        }
        return;
    }
    if (is_ascii_atom(b)) {
        if (st->dot_pending) {
            printf(" dot=STRUCT.PAIR(left=0x%02X,right=0x%02X)", st->pair_left, b);
            st->dot_pending = 0u;
        } else {
            printf(" dot=STRUCT.ATOM(value=0x%02X)", b);
        }
        st->last_atom = b;
        st->has_last_atom = 1u;
        return;
    }
    printf(" dot=STRUCT.NONE");
}

static void process_byte(omi_state_t *st, uint8_t b) {
    uint16_t mask = lexer_mask(b);
    omi_control_kind_t kind = control_kind(b);
    omi_radix_witness_t notation = notation_witness(b);
    omi_ecc_witness_t ew = hamming84_decode(hamming84_encode((uint8_t)(b & 0x0Fu)));
    uint8_t prev;
    uint8_t curr;
    uint8_t winner;

    if (kind == OMI_CTL_BOM) {
        memset(&st->control, 0, sizeof(st->control));
        st->control.synced = 1u;
    } else if (kind == OMI_CTL_ESC) {
        st->control.esc_active ^= 1u;
    } else if (kind == OMI_CTL_BOUNDARY) {
        st->control.boundary = (uint8_t)(b - 0x1Bu);
    } else if (kind == OMI_CTL_ECC) {
        ew = hamming84_decode(b);
        st->control.ecc_status = ew.status;
        st->control.ecc_nibble = ew.data;
    }

    prev = st->current_state;
    winner = pick_winner(prev, b, st->tick + 1u, st->control.ecc_nibble);
    curr = next_state(prev, b, winner, st->control.ecc_nibble);
    st->tick += 1u;
    st->previous_state = prev;
    st->current_state = curr;
    st->input = b;
    st->winner = winner;
    header8_set(st, b, curr);
    st->digest = fnv1a32(st->header8.bytes, sizeof(st->header8.bytes));

    printf("tick=%llu raw=0b",
           (unsigned long long)st->tick);
    for (int bit = 7; bit >= 0; --bit) {
        printf("%u", (unsigned)((b >> bit) & 1u));
    }
    printf(" input=0x%02X mask=0x%04X cp={bom=%u esc=%u boundary=%u ecc=%u:%X}",
           b,
           mask,
           st->control.synced,
           st->control.esc_active,
           st->control.boundary,
           st->control.ecc_status,
           st->control.ecc_nibble);
    printf(" plane=%s state=0x%02X winner=%u emit=%s",
           plane_name(plane_of_byte(b)),
           curr,
           winner,
           emit_name(b, mask));
    printf(" radix={binary=2 hex=16 notation=%s:%u digit=%u}",
           notation.name,
           notation.radix,
           notation.digit);
    print_header8(st);
    printf(" ecc={low=0x%02X high=0x%02X syndrome=%u status=%u}",
           st->header8.ecc_low[6],
           st->header8.ecc_high[6],
           ew.syndrome,
           ew.status);
    print_dot(st, b);
    printf(" digest=0x%08X\n", st->digest);
}

static void run_stream(const uint8_t *bytes, size_t length) {
    omi_state_t st;
    memset(&st, 0, sizeof(st));
    for (size_t i = 0; i < length; ++i) {
        process_byte(&st, bytes[i]);
    }
}

static void run_stdin(void) {
    omi_state_t st;
    int c;
    memset(&st, 0, sizeof(st));
    while ((c = getchar()) != EOF) {
        process_byte(&st, (uint8_t)c);
    }
}

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "-") == 0) {
        printf("omi_riscv_vm stdin\n");
        printf("==================\n");
        run_stdin();
        return 0;
    }

    {
        const uint8_t boot_stream[] = {
            OMI_BOM_BYTE,
            0x1Bu,
            0x1Cu,
            0x1Du,
            0x1Eu,
            0x1Fu,
            0xE1u,
            0x28u,
            0x41u,
            0x2Eu,
            0x42u,
            0x29u,
            0x80u,
            0xC0u
        };
        printf("omi_riscv_vm boot\n");
        printf("=================\n");
        run_stream(boot_stream, sizeof(boot_stream));
    }

    fflush(stdout);
    if (getpid() == 1) {
        sync();
        reboot(RB_POWER_OFF);
        for (;;) {
            pause();
        }
    }
    return 0;
}
