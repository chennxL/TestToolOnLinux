#pragma once

#include <SEAL-4.1/seal/batchencoder.h>
#include <SEAL-4.1/seal/context.h>
#include <SEAL-4.1/seal/decryptor.h>
#include <SEAL-4.1/seal/encryptionparams.h>
#include <SEAL-4.1/seal/encryptor.h>
#include <SEAL-4.1/seal/keygenerator.h>
#include <SEAL-4.1/seal/modulus.h>
#include <SEAL-4.1/seal/publickey.h>
#include <SEAL-4.1/seal/relinkeys.h>
#include <SEAL-4.1/seal/secretkey.h>
#include <seal/seal.h>

namespace psi {

/**
 * @brief Base context shared by client and server.
 *
 * Encapsulates all homomorphic encryption parameters and the
 * SEALContext required to construct cryptographic primitives.
 *
 * This structure contains no secret material by itself and can
 * be safely shared once public parameters are fixed.
 */
struct ContextBase {
    /**
     * @brief Protocol-specific weight parameter.
     *
     * Typically controls the number of non-zero slots or
     * the Hamming weight of encoded vectors.
     */
    size_t weight;

    /**
     * @brief Effective security level (lambda).
     *
     * Used to select encryption parameters that achieve
     * the desired security margin.
     */
    size_t effective_lambda;

    /**
     * @brief log2 of the polynomial modulus degree.
     *
     * The actual degree is 2^{log_poly_mod}.
     */
    size_t log_poly_mod;

    /**
     * @brief Protocol-specific expansion or repetition factor.
     */
    size_t ell;

    /**
     * @brief SEAL encryption parameters.
     *
     * Includes polynomial modulus degree, coefficient moduli,
     * and plaintext modulus.
     */
    seal::EncryptionParameters params;

    /**
     * @brief SEAL context constructed from encryption parameters.
     *
     * This object validates parameters and is required for
     * constructing all SEAL primitives.
     */
    seal::SEALContext ctx;
};

namespace client {

/**
 * @brief Client-side cryptographic context.
 *
 * Owns all secret material and provides functionality for:
 *   - encryption
 *   - decryption
 *   - plaintext batching
 *
 * This context should never be shared with the server.
 */
struct Context : ContextBase {
    /**
     * @brief Key generator bound to the SEAL context.
     */
    seal::KeyGenerator keygen;

    /**
     * @brief Client secret key.
     *
     * Used exclusively for decryption.
     */
    seal::SecretKey seckey;

    /**
     * @brief Encryptor initialized with the public key.
     */
    seal::Encryptor encryptor;

    /**
     * @brief Batch encoder for packing vectors into plaintexts.
     */
    seal::BatchEncoder encoder;

    /**
     * @brief Decryptor initialized with the secret key.
     */
    seal::Decryptor decryptor;

    /**
     * @brief Create a fully initialized client context.
     *
     * This function:
     *   - chooses encryption parameters
     *   - constructs SEALContext
     *   - generates keys
     *
     * @param weight Protocol weight parameter
     * @param effective_lambda Target security level
     * @param log_poly_mod log2 of polynomial modulus degree (default: 14)
     * @return Initialized client Context
     */
    static auto
    create(size_t weight, size_t effective_lambda, size_t log_poly_mod = 14)
        -> Context;

    /**
     * @brief Serialize public components of the context.
     *
     * The resulting stream typically contains:
     *   - encryption parameters
     *   - public key
     *   - relinearization keys
     *
     * Secret key material is NOT included.
     *
     * @return Stream containing serialized public context
     */
    auto to_stream() -> std::stringstream;

   private:
    /**
     * @brief Construct a client context from a prepared base context.
     *
     * Private to enforce creation via Context::create.
     */
    explicit Context(ContextBase&& base);
};

}  // namespace client

namespace server {

/**
 * @brief Server-side cryptographic context.
 *
 * Contains only public cryptographic material and evaluation tools.
 * The server can perform homomorphic computations but cannot decrypt.
 */
struct Context : ContextBase {
    /**
     * @brief Batch encoder for interpreting plaintext slots.
     */
    seal::BatchEncoder encoder;

    /**
     * @brief Evaluator for homomorphic operations.
     */
    seal::Evaluator evaluator;

    /**
     * @brief Public key received from the client.
     */
    seal::PublicKey pubkey;

    /**
     * @brief Relinearization keys received from the client.
     */
    seal::RelinKeys rlkkey;

    /**
     * @brief Construct a server context from a serialized stream.
     *
     * The stream is typically produced by client::Context::to_stream.
     *
     * @param stream Input stream containing public context data
     * @return Initialized server Context
     */
    static auto from_stream(std::stringstream& stream) -> Context;

   private:
    /**
     * @brief Internal constructor enforcing controlled initialization.
     *
     * @param base Shared encryption parameters and SEAL context
     * @param pubkey Client public key
     * @param rlkkey Client relinearization keys
     */
    Context(ContextBase&& base, seal::PublicKey pubkey, seal::RelinKeys rlkkey);
};

}  // namespace server

}  // namespace psi
