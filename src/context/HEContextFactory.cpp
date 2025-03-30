//
// Created by rkruger on 23/10/24.
//

#include "context.h"

namespace hermesml {
    HEContext HEContextFactory::bgvHeContext() {
        constexpr auto multiplicativeDepth = 10; // 10-20

        auto parameters = CCParams<CryptoContextBGVRNS>();
        parameters.SetSecurityLevel(HEStd_128_classic);
        parameters.SetPlaintextModulus(537133057); // m = 1 / (q - 1) => Número primo
        parameters.SetMultiplicativeDepth(multiplicativeDepth);
        parameters.SetMaxRelinSkDeg(3);

        // Crypto Contexts
        const auto cc = GenCryptoContext(parameters);
        cc->Enable(PKE);
        cc->Enable(KEYSWITCH);
        cc->Enable(LEVELEDSHE);
        cc->Enable(ADVANCEDSHE);
        cc->Enable(FHE);

        // Bootstrapping is not yet implemented by openfhe for BGV scheme ---------------------------------------------
        // cc->EvalBootstrapSetup(levelBudget);

        // Key generation ---------------------------------------------------------------------------------------------
        const auto keys = cc->KeyGen();

        cc->EvalMultKeyGen(keys.secretKey);
        cc->EvalSumKeyGen(keys.secretKey);

        // Bootstrapping is not yet implemented by openfhe for BGV scheme ---------------------------------------------
        // auto numSlots = cc->GetRingDimension() / 2;
        // cc->EvalBootstrapKeyGen(keys.secretKey, numSlots);

        // Build context ----------------------------------------------------------------------------------------------
        auto ctx = HEContext();
        ctx.SetCc(cc);
        ctx.SetMultiplicativeDepth(multiplicativeDepth);
        ctx.SetNumSlots(cc->GetRingDimension());
        ctx.SetPublicKey(keys.publicKey);
        ctx.SetPrivateKey(keys.secretKey);

        return ctx;
    }

    HEContext HEContextFactory::ckksHeContext() {
        const std::vector<uint32_t> levelBudget = {1, 1};
        const std::vector<uint32_t> bsgsDim = {0, 0};
        constexpr int32_t ringDimension = 2048;
        constexpr int32_t scalingModSize = 56;
        constexpr int32_t depth = 30;
        // constexpr int32_t numSlots = ringDimension / 2;
        constexpr int32_t numSlots = 32; // Adjust according to the number of features

        auto parameters = CCParams<CryptoContextCKKSRNS>();
        parameters.SetSecurityLevel(HEStd_NotSet);
        parameters.SetRingDim(ringDimension);
        parameters.SetScalingModSize(scalingModSize);
        parameters.SetMultiplicativeDepth(depth);
        parameters.SetKeySwitchTechnique(HYBRID);
        parameters.SetScalingTechnique(FLEXIBLEAUTO);
        parameters.SetSecretKeyDist(UNIFORM_TERNARY);
        parameters.SetBatchSize(numSlots);

        /* https://github.com/malb/lattice-estimator
         *
         * Martin R. Albrecht, Rachel Player and Sam Scott. On the concrete hardness of Learning with Errors.
         * Journal of Mathematical Cryptology. Volume 9, Issue 3, Pages 169–203, ISSN (Online) 1862-2984,
         * ISSN (Print) 1862-2976 DOI: 10.1515/jmc-2015-0016, October 2015
         *
         * https://eprint.iacr.org/2015/046
         *
         * params = LWE.Parameters(n=4096, q=2^(59), Xs = ND.Uniform(-1,1,n), Xe=ND.DiscreteGaussian(3.2))
         * LWE.estimate.rough(params)
         *
         * usvp                 :: rop: ≈2^223.1, red: ≈2^223.1, δ: 1.002500, β: 764, d: 7929, tag: usvp
         * dual_hybrid          :: rop: ≈2^219.0, red: ≈2^219.0, guess: ≈2^209.6, β: 750, p: 3, ζ: 0, t: 120, β': 750, N: ≈2^155.3, m: ≈2^12.0
         */

        int32_t levelsAfterBootstrap =
                depth - static_cast<int32_t>(FHECKKSRNS::GetBootstrapDepth(levelBudget, parameters.GetSecretKeyDist()));

        if (levelsAfterBootstrap < 0) {
            levelsAfterBootstrap = 0;
        }

        const auto cc = GenCryptoContext(parameters);
        cc->Enable(PKE);
        cc->Enable(KEYSWITCH);
        cc->Enable(LEVELEDSHE);
        cc->Enable(ADVANCEDSHE);
        cc->Enable(FHE);

        cc->EvalBootstrapSetup(levelBudget, bsgsDim, numSlots);

        // Key generation ---------------------------------------------------------------------------------------------
        const auto keys = cc->KeyGen();

        cc->EvalMultKeyGen(keys.secretKey);
        cc->EvalSumKeyGen(keys.secretKey);
        cc->EvalBootstrapKeyGen(keys.secretKey, numSlots);

        // Build context ----------------------------------------------------------------------------------------------
        auto ctx = HEContext();
        ctx.SetCc(cc);
        ctx.SetScalingModSize(scalingModSize);
        ctx.SetMultiplicativeDepth(depth);
        ctx.SetLevelsAfterBootstrapping(levelsAfterBootstrap);
        ctx.SetNumSlots(numSlots);
        ctx.SetPublicKey(keys.publicKey);
        ctx.SetPrivateKey(keys.secretKey);

        return ctx;
    }
}
