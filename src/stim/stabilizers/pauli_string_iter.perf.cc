// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "stim/stabilizers/pauli_string_iter.h"

#include "stim/benchmark_util.perf.h"

using namespace stim;

BENCHMARK(PauliStringIter) {
    size_t num_qubits = 63;
    size_t min_weight = 3;
    size_t max_weight = 3;
    benchmark_go([&]() {
        PauliStringIterator<MAX_BITWORD_WIDTH> iter(num_qubits, min_weight, max_weight);
        size_t c = 0;
        while (iter.iter_next()) {
            c += iter.result.num_qubits;
        }
    })
        .goal_millis(350)
        .show_rate("PaulisPerSecond", 8.1883305e8);
}