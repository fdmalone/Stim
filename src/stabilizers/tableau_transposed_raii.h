#ifndef TABLEAU_TRANSPOSED_RAII_H
#define TABLEAU_TRANSPOSED_RAII_H

#include <iostream>
#include <unordered_map>
#include <immintrin.h>
#include "../simd/simd_bit_table.h"
#include "../simd/simd_util.h"
#include "pauli_string.h"
#include "tableau.h"

struct TransposedPaulisRef {
    simd_bits_range_ref x;
    simd_bits_range_ref z;
    simd_bits_range_ref s;
};

struct TransposedTableauXZ {
    TransposedPaulisRef xz[2];
};

/// When this class is constructed, it transposes the tableau given to it.
/// The transpose is undone on deconstruction.
///
/// This is useful when appending operations to the tableau, since otherwise
/// the append would be working against the grain of memory.
struct TableauTransposedRaii {
    Tableau &tableau;

    explicit TableauTransposedRaii(Tableau &tableau);
    ~TableauTransposedRaii();

    TableauTransposedRaii() = delete;
    TableauTransposedRaii(const TableauTransposedRaii &) = delete;
    TableauTransposedRaii(TableauTransposedRaii &&) = delete;

    TransposedTableauXZ transposed_xz_ptr(size_t qubit) const;

    bool z_sign(size_t a) const;
    bool x_obs_z_bit(size_t input_qubit, size_t output_qubit) const;
    bool z_obs_x_bit(size_t input_qubit, size_t output_qubit) const;
    bool z_obs_z_bit(size_t input_qubit, size_t output_qubit) const;

    void append_H(size_t q);
    void append_H_XY(size_t q);
    void append_H_YZ(size_t q);
    void append_CX(size_t control, size_t target);
    void append_CY(size_t control, size_t target);
    void append_CZ(size_t control, size_t target);
    void append_X(size_t q);
    void append_SWAP(size_t q1, size_t q2);
};

#endif