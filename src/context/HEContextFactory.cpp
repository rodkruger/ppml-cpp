//
// Created by rkruger on 23/10/24.
//

#include "context.h"

namespace hermesml {

    HEContext HEContextFactory::bgvHeContext() {

        auto multiplicativeDepth = 10; // 10-20

        auto parameters = CCParams<CryptoContextBGVRNS>();
        parameters.SetSecurityLevel(HEStd_128_classic);
        parameters.SetPlaintextModulus(537133057);
        parameters.SetMultiplicativeDepth(multiplicativeDepth);
        parameters.SetMaxRelinSkDeg(3);

        // Crypto Contexts
        auto cc = GenCryptoContext(parameters);
        cc->Enable(PKE);
        cc->Enable(KEYSWITCH);
        cc->Enable(LEVELEDSHE);
        cc->Enable(ADVANCEDSHE);
        cc->Enable(FHE);

        // Not supported by OpenFHE ----------------------------
        // cc->EvalBootstrapSetup(levelBudget);

        // Key Generation
        auto keys = cc->KeyGen();

        cc->EvalMultKeyGen(keys.secretKey);
        cc->EvalSumKeyGen(keys.secretKey);

        // Not supported by OpenFHE ----------------------------
        // auto numSlots = cc->GetRingDimension() / 2;
        // cc->EvalBootstrapKeyGen(keys.secretKey, numSlots);

        // Build Context
        auto ctx = HEContext();
        ctx.SetCc(cc);
        ctx.SetMultiplicativeDepth(multiplicativeDepth);
        ctx.SetNumSlots(cc->GetRingDimension());
        ctx.SetPublicKey(keys.publicKey);
        ctx.SetPrivateKey(keys.secretKey);

        return ctx;
    }

    HEContext HEContextFactory::ckksHeContext() {

        CCParams<CryptoContextCKKSRNS> parameters;

        /// - **Security Level**: Defines the desired level of cryptographic security (e.g., HEStd_128_classic).
        // This parameter determines the size of encryption parameters (such as key size and ciphertext modulus)
        // to ensure a given level of resistance to attacks. Higher security levels (e.g., 128, 192, 256 bits)
        // increase the computational cost but provide stronger encryption.
        parameters.SetSecurityLevel(HEStd_128_classic);

        // The scalingModSize parameter determines the bit-length of the scaling modulus in CKKS. It controls the
        // precision of the encrypted floating-point numbers. A higher scalingModSize value improves precision but
        // increases computational cost and ciphertext size. Setting an appropriate scalingModSize is important to
        // balance precision and performance in homomorphic operations.
        parameters.SetScalingModSize(59);

        // - **Multiplicative Depth**: Specifies the maximum number of sequential homomorphic multiplications can be
        // performed before the ciphertext becomes too noisy to decrypt. Higher depths allow more complex
        // computations but require larger ciphertext moduli, increasing computational cost and memory usage.
        // std::vector<uint32_t> levelBudget = {4, 4};
        // uint32_t levelsAvailableAfterBootstrap = 10;
        // auto multiplicativeDepth = levelsAvailableAfterBootstrap + FHECKKSRNS::GetBootstrapDepth(levelBudget, parameters.GetSecretKeyDist());
        auto multiplicativeDepth = 10;
        parameters.SetMultiplicativeDepth(multiplicativeDepth);
        parameters.SetBatchSize(16);

        auto cc = GenCryptoContext(parameters);

        // Enable Public Key Encryption (PKE) functionality in the crypto context. This allows the context to support
        // basic encryption, decryption, and key generation operations.
        cc->Enable(PKE);

        // Enable key switching functionality in the crypto context. Key switching allows ciphertexts encrypted under
        // one key to be transformed into ciphertexts encrypted under another key. It's essential for advanced
        // homomorphic operations like relinearization after multiplication or vector rotation.
        cc->Enable(KEYSWITCH);

        // Enable Leveled Homomorphic Encryption (LEVELEDSHE) functionality. This enables the crypto context to
        // support a fixed number of homomorphic operations (e.g., additions and multiplications) without requiring
        // bootstrapping. It's useful when you know the maximum depth of computations required in advance, optimizing
        // performance by avoiding the need for bootstrapping.
        cc->Enable(LEVELEDSHE);

        // Enable Advanced Somewhat Homomorphic Encryption (ADVANCEDSHE) functionality in the crypto context.
        // This allows for more advanced homomorphic operations, such as rotations and more complex multiplications.
        // These operations typically require additional keys (e.g., rotation keys) and enable more sophisticated
        // computations on encrypted data, like matrix-vector multiplications and convolutions.
        cc->Enable(ADVANCEDSHE);

        // Enable Fully Homomorphic Encryption (FHE) functionality in the crypto context. This allows advanced
        // operations such as bootstrapping, which refreshes ciphertexts and reduces accumulated noise, enabling
        // an unlimited number of operations on encrypted data. Bootstrapping is required for deep computations where
        // the noise level in ciphertexts needs to be reset after multiple homomorphic operations.
        // cc->Enable(FHE);

        // EvalBootstrapSetup: This function sets up the parameters required for bootstrapping in CKKS or similar FHE
        // schemes. Bootstrapping is used to refresh ciphertexts by reducing accumulated noise, enabling further
        // homomorphic operations without decryption. The setup involves configuring key parameters such as the number
        // of levels, scaling factors, and the number of slots (data elements) to be supported during the bootstrap.
        // Proper configuration is crucial to ensure that bootstrapping can handle the desired precision, data size,
        // and the number of operations, while maintaining efficiency. Misconfiguration may lead to ineffective noise
        // reduction or high computational costs during bootstrapping, so it should be carefully tuned based on the
        // application’s needs.
        // cc->EvalBootstrapSetup(levelBudget);

        // Key generation
        auto keys = cc->KeyGen();

        // EvalMultKeyGen generates the evaluation (relinearization) keys required for performing homomorphic
        // ciphertext-ciphertext multiplication. After multiplying two ciphertexts, the resulting ciphertext has a
        // higher degree, which increases computational complexity and noise. The generated evaluation keys allow
        // relinearization, reducing the degree of the ciphertext back to its original form, ensuring efficient and
        // manageable further homomorphic operations.
        cc->EvalMultKeyGen(keys.secretKey);

        // Generate the evaluation (summation) keys needed for homomorphic sum operations. These keys allow for the
        // efficient summation of elements in packed ciphertexts, enabling operations such as computing the sum of a
        // vector of encrypted values. The summation keys are necessary for performing homomorphic summation across
        // slots in SIMD (Single Instruction, Multiple Data) packed ciphertexts.
        // cc->EvalSumKeyGen(keys.secretKey);

        // EvalBootstrapKeyGen generates the bootstrapping keys required to perform the bootstrapping process, which
        // refreshes a ciphertext by reducing its noise. This allows for continued homomorphic operations on the
        // encrypted data without the ciphertext becoming too noisy to decrypt correctly. It is essential when
        // performing many operations on encrypted data to maintain accuracy.
        // auto numSlots = cc->GetRingDimension() / 2;     // This is the maximum number of slots that can be used for full packing.
        // cc->EvalBootstrapKeyGen(keys.secretKey, numSlots);

        // Build Context
        auto ctx = HEContext();

        ctx.SetCc(cc);
        ctx.SetMultiplicativeDepth(multiplicativeDepth);
        // ctx.SetNumSlots(numSlots);
        ctx.SetPublicKey(keys.publicKey);
        ctx.SetPrivateKey(keys.secretKey);

        return ctx;
    }

}
