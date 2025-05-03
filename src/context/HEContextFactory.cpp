//
// Created by rkruger on 23/10/24.
//

#include "context.h"

namespace hermesml {
    uint32_t HEContextFactory::NextPowerOfTwo(const uint32_t n) {
        auto i = n;

        if (i == 0)
            return 1;

        i--;
        i |= i >> 1;
        i |= i >> 2;
        i |= i >> 4;
        i |= i >> 8;
        i |= i >> 16;
        i++;

        return i;
    }

    HEContext HEContextFactory::ckksHeContext(const uint32_t n_features) {
        const std::vector<uint32_t> levelBudget = {1, 1};
        const std::vector<uint32_t> bsgsDim = {0, 0};
        constexpr auto ringDimension = 2048;
        constexpr auto scalingModSize = 56;
        const auto numSlots = NextPowerOfTwo(n_features);

        auto parameters = CCParams<CryptoContextCKKSRNS>();
        parameters.SetSecurityLevel(HEStd_NotSet);
        parameters.SetRingDim(ringDimension);
        parameters.SetScalingModSize(scalingModSize);
        parameters.SetKeySwitchTechnique(HYBRID);
        parameters.SetScalingTechnique(FLEXIBLEAUTO);
        parameters.SetSecretKeyDist(UNIFORM_TERNARY);
        parameters.SetBatchSize(numSlots);

        constexpr uint32_t depth = 30;
        const uint32_t levelsAfterBootstrap = depth - FHECKKSRNS::GetBootstrapDepth(
                                                  levelBudget, parameters.GetSecretKeyDist());
        parameters.SetMultiplicativeDepth(depth);

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

        std::vector<int> rotationIndices;
        for (int i = 1; i < numSlots; i++) {
            rotationIndices.push_back(i);
            rotationIndices.push_back(-i);
        }
        cc->EvalRotateKeyGen(keys.secretKey, rotationIndices);

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
