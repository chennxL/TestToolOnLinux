#pragma once

#include "common.hpp"
#include "context.hpp"

namespace psi::server {

/**
 * @brief Pack server-side key–value data into a labeled payload.
 *
 * This function prepares the server's plaintext database for PSI matching.
 * Each key corresponds to an element identifier, and each value vector
 * represents the labels associated with that key.
 *
 * The resulting LabeledPayload contains plaintext data only and is later
 * combined with encrypted client data during matching.
 *
 * @param ctx Server cryptographic context (public parameters only)
 * @param kvs Server key–value store:
 *            - key: element identifier
 *            - value: associated labels
 *
 * @return LabeledPayload containing encoded plaintext keys and labels
 */
auto pack_for_matching(
    const Context& ctx,
    std::map<size_t, std::vector<size_t>> kvs
) -> LabeledPayload;

/**
 * @brief Perform PSI matching between client and server datasets.
 *
 * This function executes the core homomorphic computation:
 *   - consumes encrypted client payload
 *   - combines it with server plaintext labeled data
 *   - produces encrypted results that can only be decrypted by the client
 *
 * Ownership of both input payloads is transferred, as ciphertexts
 * and plaintexts are typically consumed or transformed in-place.
 *
 * @param ctx Server cryptographic context
 * @param cdata Encrypted client payload (moved-from after call)
 * @param sdata Server labeled plaintext payload (moved-from after call)
 *
 * @return ResultPayload containing encrypted PSI matching results
 */
auto do_matching(
    const Context& ctx,
    CipherPayload&& cdata,
    LabeledPayload&& sdata
) -> ResultPayload;

}  // namespace psi::server
