#include <print>
#include <tuple>
#include "./include/client.hpp"
#include "./include/server.hpp"
#include "include/context.hpp"

using namespace psi;

using key_type = size_t;
using label_type = size_t;
using labels_type = std::vector<label_type>;

auto main() -> int {
    /**
     * ------------------------------------------------------------
     * 1. Create client cryptographic context
     * ------------------------------------------------------------
     * - Generates encryption parameters and keys
     * - Owns the secret key
     */
    auto cli_ctx = client::Context::create(15, 16);

    /**
     * Serialize public context information (parameters + public keys)
     * to be sent to the server.
     */
    auto ctxbuf = cli_ctx.to_stream();

    /**
     * ------------------------------------------------------------
     * 2. Initialize server cryptographic context
     * ------------------------------------------------------------
     * - Reconstructs SEAL context from client-provided stream
     * - Contains no secret key material
     */
    auto srv_ctx = server::Context::from_stream(ctxbuf);

    /**
     * ------------------------------------------------------------
     * 3. Prepare input datasets
     * ------------------------------------------------------------
     * Server holds a key → labels mapping.
     * Client holds a set of keys.
     */
    std::map<key_type, labels_type> srv_data = {{1, {1}}, {3, {1, 2, 3}}};

    std::set<key_type> cli_data = {1, 2, 3};

    /**
     * ------------------------------------------------------------
     * 4. Client packs and encrypts its dataset
     * ------------------------------------------------------------
     * - Produces encrypted payload to send to server
     * - Produces a reveal table kept locally by the client
     */
    auto [payload, reveal_table] =
        client::pack_payload(cli_ctx, std::move(cli_data));

    /**
     * Serialize encrypted client payload for transmission.
     */
    auto bufa = payload.to_stream();

    /**
     * ------------------------------------------------------------
     * 5. Server receives and processes client payload
     * ------------------------------------------------------------
     * - Reconstruct encrypted data using server context
     * - Server never decrypts client data
     */
    auto srv_recv = CipherPayload::from_stream(bufa, srv_ctx);

    /**
     * Pack server-side plaintext database for matching.
     */
    auto labeled_payload = server::pack_for_matching(srv_ctx, srv_data);

    /**
     * ------------------------------------------------------------
     * 6. Perform PSI matching on the server
     * ------------------------------------------------------------
     * - Combines encrypted client data with server plaintext data
     * - Produces encrypted result payload
     */
    auto result = server::do_matching(
        srv_ctx, std::move(srv_recv), std::move(labeled_payload)
    );

    /**
     * Serialize encrypted result for transmission back to client.
     */
    auto bufb = result.to_stream();

    /**
     * ------------------------------------------------------------
     * 7. Client receives and reveals result
     * ------------------------------------------------------------
     * - Decrypts server result
     * - Uses reveal table to map results back to client keys
     */
    auto cli_recv = ResultPayload::from_stream(bufb, cli_ctx);

    auto res =
        client::reveal_result(cli_ctx, reveal_table, std::move(cli_recv));

    /**
     * ------------------------------------------------------------
     * 8. Output final PSI result
     * ------------------------------------------------------------
     * For each matched key, print associated labels.
     */
    for (const auto& [k, vs] : res) {
        std::print("find key {} with labels : ", k);
        for (auto v : vs) {
            std::print("{} ", v);
        }
        std::println("");
    }

    return 0;
}
