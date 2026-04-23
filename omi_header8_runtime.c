/*
 * omi_header8_runtime.c
 *
 * Concrete C witness for the omi-lisp pre-header runtime idea.
 *
 * Core idea:
 *   HEADER8 is the control-injection surface over runtime.
 *
 * Canonical control slots:
 *   [0] NULL / quiescent marker
 *   [1] ESC  / escape / mode shift
 *   [2] FS   / function-frame separator
 *   [3] GS   / group separator
 *   [4] RS   / rule / record separator
 *   [5] US   / unit separator
 *   [6] input byte / current surface token
 *   [7] current state / runtime witness
 *
 * The runtime below is intentionally small:
 *   - fixed-width state update
 *   - HEADER8 projection
 *   - reference/pointer partition
 *   - minimal mixed-radix address witness
 *   - replay + receipt
 *
 * This is not the whole ontology. It is the low-level witness.
 */

#define _DEFAULT_SOURCE

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/reboot.h>

#define OMI_FLAG_ESC       0x01u
#define OMI_FLAG_BOUNDARY  0x02u
#define OMI_FLAG_POINTER   0x04u
#define OMI_FLAG_REFERENCE 0x08u
#define OMI_FLAG_PRINTING  0x10u
#define OMI_FLAG_BRAILLE   0x20u
#define OMI_FLAG_AEGEAN    0x40u

typedef struct {
    uint64_t tick;
    uint8_t previous_state;
    uint8_t current_state;
    uint8_t input;
    uint8_t winner;
    uint8_t chirality;   /* 0 = LTR / forward, 1 = RTL / reverse */
    uint8_t lane;
    uint8_t channel;
    uint8_t slot;
    uint8_t flags;
    uint8_t header8[8];
    uint32_t step_digest;
    uint32_t artifact_hash;
    uint8_t list_depth;
    uint8_t dot_pending;
    uint8_t pair_left;
    uint8_t last_atom;
    uint8_t has_last_atom;
} omi_state_t;

typedef struct {
    uint64_t tick;
    uint8_t input;
    uint8_t state;
    uint8_t winner;
    uint8_t flags;
    uint8_t reserved;
    uint32_t step_digest;
} omi_step_receipt_t;

typedef struct {
    uint8_t plane;   /* 0=ASCII, 1=BRAILLE, 2=AEGEAN */
    uint8_t high;    /* high-order classification */
    uint8_t low;     /* low-order payload/offset */
    uint16_t raw;    /* packed virtual address */
} omi_virtual_address_t;

enum {
    OMI_PLANE_ASCII   = 0,
    OMI_PLANE_BRAILLE = 1,
    OMI_PLANE_AEGEAN  = 2
};

static inline uint8_t rotl8(uint8_t x, unsigned k) {
    k &= 7u;
    return (uint8_t)((x << k) | (x >> ((8u - k) & 7u)));
}

static inline uint8_t rotr8(uint8_t x, unsigned k) {
    k &= 7u;
    return (uint8_t)((x >> k) | (x << ((8u - k) & 7u)));
}

static inline uint8_t ones_complement8(uint8_t x) {
    return (uint8_t)~x;
}

static inline uint8_t twos_complement8(uint8_t x) {
    return (uint8_t)(~x + 1u);
}

static inline uint16_t sign_extend8_to16(uint8_t x) {
    return (x & 0x80u) ? (uint16_t)(0xFF00u | x) : (uint16_t)x;
}

static uint32_t omi_fnv1a32(const void *data, size_t length) {
    const uint8_t *p = (const uint8_t *)data;
    uint32_t h = UINT32_C(2166136261);
    for (size_t i = 0; i < length; ++i) {
        h ^= p[i];
        h *= UINT32_C(16777619);
    }
    return h;
}

/* Delta law witness:
 *   rotl(x,1) XOR rotl(x,3) XOR rotr(x,2) XOR C
 */
static uint8_t omi_delta(uint8_t x, uint8_t C) {
    return (uint8_t)(rotl8(x, 1u) ^ rotl8(x, 3u) ^ rotr8(x, 2u) ^ C);
}

/* Small winner law: deterministic but separate from delta witness. */
static uint8_t omi_pick_winner(uint8_t previous_state, uint8_t input, uint64_t tick) {
    uint8_t mixed = (uint8_t)((previous_state * 17u) ^ (input * 29u) ^ (uint8_t)tick);
    return (uint8_t)(mixed % 7u);
}

/* Next state: bounded 6-bit state, keeping your original 0x3F style. */
static uint8_t omi_next_state(uint8_t previous_state, uint8_t input, uint8_t winner, uint8_t constant) {
    uint8_t d = omi_delta((uint8_t)(previous_state ^ input), constant);
    return (uint8_t)((previous_state + input + winner + d) & 0x3Fu);
}

static uint32_t omi_step_digest_local(uint64_t tick,
                                      uint8_t prev_state,
                                      uint8_t curr_state,
                                      uint8_t input,
                                      uint8_t winner) {
    uint8_t bytes[12];
    bytes[0] = (uint8_t)(tick >> 0);
    bytes[1] = (uint8_t)(tick >> 8);
    bytes[2] = (uint8_t)(tick >> 16);
    bytes[3] = (uint8_t)(tick >> 24);
    bytes[4] = (uint8_t)(tick >> 32);
    bytes[5] = (uint8_t)(tick >> 40);
    bytes[6] = (uint8_t)(tick >> 48);
    bytes[7] = (uint8_t)(tick >> 56);
    bytes[8] = prev_state;
    bytes[9] = curr_state;
    bytes[10] = input;
    bytes[11] = winner;
    return omi_fnv1a32(bytes, sizeof(bytes));
}

static uint8_t omi_is_reference(uint8_t b) {
    return (uint8_t)(b <= 0x0Fu);
}

static uint8_t omi_is_pointer(uint8_t b) {
    return (uint8_t)(b >= 0x10u && b <= 0x1Fu);
}

static uint8_t omi_is_printing(uint8_t b) {
    return (uint8_t)(b >= 0x40u);
}

static uint8_t omi_is_ascii_atom(uint8_t b) {
    if (b <= 0x20u) return 0u;
    if (b == 0x28u || b == 0x29u || b == 0x2Eu) return 0u;
    if (b >= 0x80u) return 0u;
    return 1u;
}

/* A very small plane classifier:
 *   ASCII   = default plane
 *   BRAILLE = inputs in 0x80..0xBF treated as dense payload dialect
 *   AEGEAN  = inputs in 0xC0..0xFF treated as header/governance dialect
 *
 * This is not Unicode itself. It is a compact plane witness over a byte stream.
 */
static uint8_t omi_classify_plane(uint8_t input) {
    if (input >= 0xC0u) return OMI_PLANE_AEGEAN;
    if (input >= 0x80u) return OMI_PLANE_BRAILLE;
    return OMI_PLANE_ASCII;
}

/* High/low virtual addressing witness:
 *   high = plane / major class
 *   low  = payload / offset
 */
static omi_virtual_address_t omi_make_address(uint8_t input, uint8_t plane) {
    omi_virtual_address_t a;
    a.plane = plane;

    switch (plane) {
        case OMI_PLANE_ASCII:
            a.high = (uint8_t)((input >> 4) & 0x0Fu);
            a.low  = (uint8_t)(input & 0x0Fu);
            a.raw  = (uint16_t)((a.high << 8) | a.low);
            break;

        case OMI_PLANE_BRAILLE:
            /* dense payload plane: preserve full byte in low, tag high */
            a.high = 0x28u; /* Braille block witness */
            a.low  = input;
            a.raw  = (uint16_t)((a.high << 8) | a.low);
            break;

        case OMI_PLANE_AEGEAN:
        default:
            /* header / governance plane */
            a.high = 0x10u; /* Aegean block witness */
            a.low  = input;
            a.raw  = (uint16_t)((a.high << 8) | a.low);
            break;
    }
    return a;
}

void omi_init(omi_state_t *state) {
    memset(state, 0, sizeof(*state));
    state->header8[0] = 0x00u; /* NULL */
    state->header8[1] = 0x1Bu; /* ESC */
    state->header8[2] = 0x1Cu; /* FS */
    state->header8[3] = 0x1Du; /* GS */
    state->header8[4] = 0x1Eu; /* RS */
    state->header8[5] = 0x1Fu; /* US */
    state->header8[6] = 0x00u; /* input witness */
    state->header8[7] = 0x00u; /* current state witness */
}

uint32_t omi_step(omi_state_t *state, uint8_t input, omi_step_receipt_t *receipt) {
    const uint8_t prev = state->current_state;
    const uint8_t winner = omi_pick_winner(prev, input, state->tick + 1u);
    const uint8_t curr = omi_next_state(prev, input, winner, 0x02u); /* STX-like constant */
    const uint8_t plane = omi_classify_plane(input);
    uint8_t flags = 0;

    state->tick += 1u;
    state->previous_state = prev;
    state->current_state = curr;
    state->input = input;
    state->winner = winner;
    state->chirality = (uint8_t)((state->tick ^ input ^ curr) & 0x01u);
    state->lane = (uint8_t)(winner & 0x07u);
    state->channel = (uint8_t)((curr >> 1) & 0x07u);
    state->slot = (uint8_t)((input ^ curr) & 0x0Fu);

    if (input == 0x1Bu) flags |= OMI_FLAG_ESC;
    if (input >= 0x1Cu && input <= 0x1Fu) flags |= OMI_FLAG_BOUNDARY;
    if (omi_is_reference(input)) flags |= OMI_FLAG_REFERENCE;
    if (omi_is_pointer(input)) flags |= OMI_FLAG_POINTER;
    if (omi_is_printing(input)) flags |= OMI_FLAG_PRINTING;
    if (plane == OMI_PLANE_BRAILLE) flags |= OMI_FLAG_BRAILLE;
    if (plane == OMI_PLANE_AEGEAN) flags |= OMI_FLAG_AEGEAN;

    state->flags = flags;

    /* HEADER8 as control injection over runtime */
    state->header8[0] = 0x00u; /* NULL */
    state->header8[1] = 0x1Bu; /* ESC */
    state->header8[2] = 0x1Cu; /* FS */
    state->header8[3] = 0x1Du; /* GS */
    state->header8[4] = 0x1Eu; /* RS */
    state->header8[5] = 0x1Fu; /* US */
    state->header8[6] = input;
    state->header8[7] = curr;

    state->step_digest = omi_step_digest_local(state->tick, prev, curr, input, winner);
    state->artifact_hash = omi_fnv1a32(state->header8, sizeof(state->header8));

    if (receipt != NULL) {
        receipt->tick = state->tick;
        receipt->input = input;
        receipt->state = curr;
        receipt->winner = winner;
        receipt->flags = flags;
        receipt->reserved = 0;
        receipt->step_digest = state->step_digest;
    }

    return 0u;
}

size_t omi_replay_bytes(omi_state_t *state,
                        const uint8_t *bytes,
                        size_t length,
                        omi_step_receipt_t *receipts,
                        size_t receipt_capacity) {
    size_t produced = 0;
    if (bytes == NULL && length != 0u) return 0u;

    for (size_t i = 0; i < length; ++i) {
        if (receipts != NULL && produced >= receipt_capacity) break;
        omi_step(state, bytes[i], receipts != NULL ? &receipts[produced] : NULL);
        produced += 1u;
    }
    return produced;
}

static void omi_print_flags(uint8_t flags) {
    printf("flags:");
    if (flags & OMI_FLAG_ESC)       printf(" ESC");
    if (flags & OMI_FLAG_BOUNDARY)  printf(" BOUNDARY");
    if (flags & OMI_FLAG_REFERENCE) printf(" REF");
    if (flags & OMI_FLAG_POINTER)   printf(" PTR");
    if (flags & OMI_FLAG_PRINTING)  printf(" PRINT");
    if (flags & OMI_FLAG_BRAILLE)   printf(" BRAILLE");
    if (flags & OMI_FLAG_AEGEAN)    printf(" AEGEAN");
    if (flags == 0)                 printf(" NONE");
}

static void omi_print_header8(const uint8_t h[8]) {
    printf("header8: [");
    for (size_t i = 0; i < 8; ++i) {
        printf("%s0x%02X", i ? " " : "", h[i]);
    }
    printf("]");
}

static void omi_print_address(uint8_t input) {
    uint8_t plane = omi_classify_plane(input);
    omi_virtual_address_t a = omi_make_address(input, plane);

    printf(" address{plane=");
    switch (plane) {
        case OMI_PLANE_ASCII:   printf("ASCII"); break;
        case OMI_PLANE_BRAILLE: printf("BRAILLE"); break;
        case OMI_PLANE_AEGEAN:  printf("AEGEAN"); break;
        default:                printf("UNKNOWN"); break;
    }
    printf(",high=0x%02X,low=0x%02X,raw=0x%04X}", a.high, a.low, a.raw);
}

static void omi_demo_complements(uint8_t x) {
    printf(" complement{ones=0x%02X,twos=0x%02X,signext=0x%04X}",
           ones_complement8(x),
           twos_complement8(x),
           sign_extend8_to16(x));
}

static const char *omi_emit_name(uint8_t input, uint8_t flags) {
    if (flags & OMI_FLAG_ESC) return "MODE-SHIFT";
    if (flags & OMI_FLAG_BOUNDARY) return "PARTITION";
    if (input == 0x28u) return "STRUCT.LIST-OPEN";
    if (input == 0x29u) return "STRUCT.LIST-CLOSE";
    if (input == 0x2Eu) return "STRUCT.DOT";
    if (flags & OMI_FLAG_REFERENCE) return "TOKEN.REFERENCE";
    if (flags & OMI_FLAG_POINTER) return "TOKEN.POINTER";
    if (flags & OMI_FLAG_AEGEAN) return "TOKEN.AEGEAN";
    if (flags & OMI_FLAG_BRAILLE) return "TOKEN.BRAILLE";
    if (omi_classify_plane(input) == OMI_PLANE_ASCII) return "TOKEN.ASCII";
    return "TOKEN.UNKNOWN";
}

static void omi_print_dot_interpolation(omi_state_t *st, uint8_t input) {
    if (input == 0x28u) {
        if (st->list_depth != UINT8_MAX) st->list_depth += 1u;
        st->dot_pending = 0u;
        printf(" dot=STRUCT.LIST(depth=%u)", st->list_depth);
        return;
    }

    if (input == 0x29u) {
        printf(" dot=STRUCT.CLOSE(depth=%u)", st->list_depth);
        if (st->list_depth != 0u) st->list_depth -= 1u;
        st->dot_pending = 0u;
        return;
    }

    if (input == 0x2Eu) {
        if (st->has_last_atom) {
            st->dot_pending = 1u;
            st->pair_left = st->last_atom;
            printf(" dot=STRUCT.DOT(left=0x%02X)", st->pair_left);
        } else {
            st->dot_pending = 0u;
            printf(" dot=STRUCT.DOT-DANGLING");
        }
        return;
    }

    if (input == 0x20u || input == 0x09u || input == 0x0Au || input == 0x0Du) {
        st->dot_pending = 0u;
        printf(" dot=STRUCT.SEPARATOR");
        return;
    }

    if (omi_is_ascii_atom(input)) {
        if (st->dot_pending) {
            printf(" dot=STRUCT.PAIR(left=0x%02X,right=0x%02X)", st->pair_left, input);
            st->dot_pending = 0u;
        } else {
            printf(" dot=STRUCT.ATOM(value=0x%02X)", input);
        }
        st->last_atom = input;
        st->has_last_atom = 1u;
        return;
    }

    printf(" dot=STRUCT.NONE");
}

static void omi_print_step(omi_state_t *st, uint8_t input) {
    omi_step_receipt_t r;
    omi_step(st, input, &r);

    printf("tick=%llu input=0x%02X state=0x%02X winner=%u chirality=%u lane=%u channel=%u slot=%u ",
           (unsigned long long)r.tick,
           r.input,
           r.state,
           r.winner,
           st->chirality,
           st->lane,
           st->channel,
           st->slot);

    omi_print_flags(r.flags);
    printf(" ");
    omi_print_header8(st->header8);
    omi_print_address(r.input);
    omi_demo_complements(r.input);
    printf(" emit=%s", omi_emit_name(r.input, r.flags));
    omi_print_dot_interpolation(st, r.input);
    printf(" digest=0x%08X hash=0x%08X\n", r.step_digest, st->artifact_hash);
}

static void omi_run_stream(const uint8_t *stream, size_t length) {
    omi_state_t st;
    omi_init(&st);

    for (size_t i = 0; i < length; ++i) {
        omi_print_step(&st, stream[i]);
    }
}

static void omi_run_stdin(void) {
    omi_state_t st;
    int c;

    omi_init(&st);
    while ((c = getchar()) != EOF) {
        omi_print_step(&st, (uint8_t)c);
    }
}

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "-") == 0) {
        printf("omi_header8_runtime stdin\n");
        printf("=========================\n");
        omi_run_stdin();
        return 0;
    }

    {
        /* Demo stream:
         * NULL ESC FS GS RS US '.' '0' '@' 0x80 0xC0 '(A.B)'
         */
        const uint8_t stream[] = {
            0x00u, 0x1Bu, 0x1Cu, 0x1Du, 0x1Eu, 0x1Fu,
            0x2Eu, 0x30u, 0x40u, 0x80u, 0xC0u,
            0x28u, 0x41u, 0x2Eu, 0x42u, 0x29u
        };

        printf("omi_header8_runtime demo\n");
        printf("========================\n");
        omi_run_stream(stream, sizeof(stream));
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
