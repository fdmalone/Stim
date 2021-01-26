#ifndef SIM_TABLEAU_H
#define SIM_TABLEAU_H

#include <cassert>
#include <functional>
#include <iostream>
#include <new>
#include <queue>
#include <random>
#include <sstream>

#include "../circuit.h"
#include "../stabilizers/tableau.h"
#include "../stabilizers/tableau_transposed_raii.h"
#include "vector_simulator.h"

struct TableauSimulator {
    Tableau inv_state;
    std::mt19937_64 &rng;
    int8_t sign_bias;
    std::queue<bool> recorded_measurement_results;

    /// Args:
    ///     num_qubits: The initial number of qubits in the simulator state.
    ///     rng: The random number generator to use for random operations.
    ///     sign_bias: 0 means collapse randomly, -1 means collapse towards True, +1 means collapse towards False.
    explicit TableauSimulator(size_t num_qubits, std::mt19937_64 &rng, int8_t sign_bias = 0);

    /// Samples the given circuit in a deterministic fashion.
    ///
    /// Discards all noisy operations, and biases all collapse events towards +Z instead of randomly +Z/-Z.
    static simd_bits reference_sample_circuit(const Circuit &circuit);
    static simd_bits sample_circuit(const Circuit &circuit, std::mt19937_64 &rng, int8_t sign_bias = 0);
    static void sample_stream(FILE *in, FILE *out, bool newline_after_measurements, std::mt19937_64 &rng);

    /// Expands the internal state of the simulator (if needed) to ensure the given qubit exists.
    ///
    /// Failing to ensure the state is large enough for a qubit, before that qubit is acted on for the first time,
    /// results in undefined behavior.
    void ensure_large_enough_for_qubit(size_t max_q);

    /// Finds a state vector satisfying the current stabilizer generators, and returns a vector simulator in that state.
    VectorSimulator to_vector_sim() const;

    /// Applies a custom Clifford operation (represented by a stabilizer tableau) to the given targets.
    ///
    /// The number of targets must be a multiple of the number of qubits the tableau acts on.
    /// The operation will be broadcast over the aligned targets. For example, CNOT 0 1 2 3 is
    /// equivalent to CNOT 0 1 then CNOT 2 3.
    void apply(const Tableau &operation, const OperationData &target_data);

    /// Collapses then records the Z signs of the target qubits. Supports flipping the result.
    ///
    /// Args:
    ///     target_data: The qubits to target, with flag data indicating whether to invert results.
    void measure(const OperationData &target_data);

    /// Collapses then clears the target qubits.
    ///
    /// Args:
    ///     target_data: The qubits to target, with flag data indicating whether to reset to 1 instead of 0.
    ///     sign_bias: 0 means collapse randomly, -1 means collapse towards True, +1 means collapse towards False.
    void reset(const OperationData &target_data);

    /// Applies a named operation to the given targets.
    ///
    /// See `SIM_TABLEAU_GATE_FUNC_DATA` and `gate_data.c` for a list of supported operations.
    ///
    /// The number of targets must be a multiple of the number of qubits the operation acts on.
    /// The operation will be broadcast over the aligned targets. For example, CNOT 0 1 2 3 is
    /// equivalent to CNOT 0 1 then CNOT 2 3.
    void apply(const std::string &name, const OperationData &target_data);

    /// Determines if a qubit's Z observable commutes (vs anti-commutes) with the current stabilizer generators.
    bool is_deterministic(size_t target) const;

    /// === SPECIALIZED VECTORIZED OPERATION IMPLEMENTATIONS ===
    void H_XZ(const OperationData &target_data);
    void H_YZ(const OperationData &target_data);
    void H_XY(const OperationData &target_data);
    void SQRT_X(const OperationData &target_data);
    void SQRT_Y(const OperationData &target_data);
    void SQRT_Z(const OperationData &target_data);
    void SQRT_X_DAG(const OperationData &target_data);
    void SQRT_Y_DAG(const OperationData &target_data);
    void SQRT_Z_DAG(const OperationData &target_data);
    void ZCX(const OperationData &target_data);
    void ZCY(const OperationData &target_data);
    void ZCZ(const OperationData &target_data);
    void SWAP(const OperationData &target_data);
    void X(const OperationData &target_data);
    void Y(const OperationData &target_data);
    void Z(const OperationData &target_data);
    void ISWAP(const OperationData &target_data);
    void ISWAP_DAG(const OperationData &target_data);
    void XCX(const OperationData &target_data);
    void XCY(const OperationData &target_data);
    void XCZ(const OperationData &target_data);
    void YCX(const OperationData &target_data);
    void YCY(const OperationData &target_data);
    void YCZ(const OperationData &target_data);

   private:
    /// Forces a qubit to have a collapsed Z observable.
    ///
    /// If the qubit already has a collapsed Z observable, this method has no effect.
    /// Other, the qubit's Z observable anticommutes with the current stabilizers and this method will apply state
    /// transformations that pick out a single stabilizer generator to destroy and replace with the measurement's
    /// stabilizer.
    ///
    /// Args:
    ///     target: The index of the qubit to collapse.
    ///     transposed_raii: A RAII value whose existence certifies the tableau data is currently transposed
    ///         (to make operations efficient).
    void collapse_qubit(size_t target, TableauTransposedRaii &transposed_raii);

    /// Collapses the given qubits.
    ///
    /// Args:
    ///     targets: The qubits to collapse.
    void collapse(const OperationData &target_data);
};

#endif