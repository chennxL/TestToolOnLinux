#pragma once

#include <set>
#include "common.hpp"
#include "context.hpp"

namespace psi::client {

/**
 * @brief Pack a client set into an encrypted payload for PSI.
 *
 * This function takes a client-side set of elements, encodes and encrypts
 * them using the provided client context, and produces:
 *   1) a CipherPayload containing encrypted data to be sent to the server
 *   2) a RevealTable describing how results should be interpreted after
 *      server-side computation
 *
 * Ownership of the input set is transferred (rvalue reference) to avoid
 * unnecessary copies.
 *
 * @param ctx Client cryptographic context (owns secret key)
 * @param set Client input set (moved-from after call)
 *
 * @return A tuple consisting of:
 *   - CipherPayload: encrypted client data
 *   - RevealTable: auxiliary table used for result reconstruction
 */
auto pack_payload(const Context& ctx, std::set<size_t>&& set)
    -> std::tuple<CipherPayload, RevealTable>;

/**
 * @brief Reveal and decode the server's PSI computation result.
 *
 * This function takes the encrypted result returned by the server and:
 *   - decrypts ciphertexts using the client secret key
 *   - applies the reveal table to map decrypted values back to
 *     original client elements
 *
 * Ownership of the result payload is transferred (rvalue reference),
 * as ciphertexts are typically consumed during decryption.
 *
 * @param ctx Client cryptographic context (used for decryption)
 * @param tab Reveal table produced during pack_payload
 * @param result Encrypted result payload returned by the server
 *
 * @return A map from client element (size_t) to a vector of associated values,
 *         typically representing matched labels or intersection results.
 */
auto reveal_result(Context& ctx, const RevealTable& tab, ResultPayload&& result)
    -> std::map<size_t, std::vector<size_t>>;

}  // namespace psi::client
