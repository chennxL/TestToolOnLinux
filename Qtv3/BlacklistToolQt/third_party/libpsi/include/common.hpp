#pragma once

#include <SEAL-4.1/seal/ciphertext.h>
#include <cstddef>
#include <sstream>
#include "context.hpp"

namespace psi {

/**
 * @brief Encrypted payload used in PSI protocol.
 *
 * This structure represents a batch of ciphertexts together with
 * protocol-related metadata. It supports serialization to and
 * deserialization from a stream.
 *
 * Typical usage:
 *   - Sender serializes CipherPayload into a stream and sends it over network
 *   - Receiver reconstructs CipherPayload using the shared SEAL context
 */
struct CipherPayload {
    /**
     * @brief Protocol parameter (e.g., polynomial degree, batching factor).
     *
     * Semantics of gamma depend on the PSI protocol variant,
     * but it is assumed to be agreed upon by both parties.
     */
    size_t gamma;

    /**
     * @brief Encrypted data.
     *
     * Each element is a SEAL ciphertext encrypted under the same
     * encryption parameters contained in ContextBase.
     */
    std::vector<seal::Ciphertext> data;

    /**
     * @brief Serialize this payload into a stream.
     *
     * The stream contains:
     *   - gamma
     *   - number of ciphertexts
     *   - serialized form of each ciphertext
     *
     * @return std::stringstream containing the serialized payload
     */
    auto to_stream() const -> std::stringstream;

    /**
     * @brief Deserialize a CipherPayload from a stream.
     *
     * The caller must provide a valid SEAL context via ContextBase,
     * which is required to correctly load ciphertext objects.
     *
     * @param buf Input stream containing serialized CipherPayload
     * @param ctx SEAL context wrapper used for deserialization
     * @return Reconstructed CipherPayload
     */
    static auto from_stream(std::stringstream& buf, ContextBase& ctx)
        -> CipherPayload;
};

/**
 * @brief Table describing revealed indices or positions.
 *
 * Each inner vector typically represents a row of indices revealed
 * during the PSI protocol (e.g., matching positions).
 */
using RevealTable = std::vector<std::vector<size_t>>;

/**
 * @brief Payload containing plaintext labels and associated keys.
 *
 * This structure is typically used after partial or full PSI execution,
 * where certain labels are revealed in plaintext.
 */
struct LabeledPayload {
    /**
     * @brief Number of elements or protocol-specific multiplicity.
     */
    size_t mu;

    /**
     * @brief Plaintext keys.
     *
     * These are SEAL plaintexts, typically representing identifiers
     * or hashed keys corresponding to PSI elements.
     */
    std::vector<seal::Plaintext> keys;

    /**
     * @brief Associated plaintext labels.
     *
     * labels[i] contains all labels associated with keys[i].
     */
    std::vector<std::vector<seal::Plaintext>> labels;
};

/**
 * @brief Result payload produced after PSI computation.
 *
 * Contains encrypted results, possibly grouped by label or category,
 * and supports serialization.
 */
struct ResultPayload {
    /**
     * @brief Number of labels associated with each result entry.
     */
    size_t nr_labels;

    /**
     * @brief Encrypted result data.
     *
     * data[i][j] is the j-th ciphertext corresponding to the i-th result
     * (e.g., label bucket or matched element).
     */
    std::vector<std::vector<seal::Ciphertext>> data;

    /**
     * @brief Serialize this result payload into a stream.
     *
     * @return std::stringstream containing serialized ResultPayload
     */
    auto to_stream() const -> std::stringstream;

    /**
     * @brief Deserialize a ResultPayload from a stream.
     *
     * Requires a valid SEAL context to correctly load ciphertexts.
     *
     * @param buf Input stream containing serialized ResultPayload
     * @param ctx SEAL context wrapper used for deserialization
     * @return Reconstructed ResultPayload
     */
    static auto from_stream(std::stringstream& buf, ContextBase& ctx)
        -> ResultPayload;
};

}  // namespace psi
