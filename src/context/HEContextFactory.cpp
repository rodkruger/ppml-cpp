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
        const std::vector<uint32_t> levelBudget = {3, 3};
        const std::vector<uint32_t> bsgsDim = {0, 0};
        constexpr int32_t numSlots = 32;
        auto parameters = CCParams<CryptoContextCKKSRNS>();

        // parameters.SetSecurityLevel(HEStd_128_classic);
        parameters.SetSecurityLevel(HEStd_NotSet);
        parameters.SetRingDim(32768);
        parameters.SetKeySwitchTechnique(HYBRID);
        parameters.SetScalingModSize(59);
        parameters.SetScalingTechnique(FLEXIBLEAUTO);
        parameters.SetBatchSize(numSlots);
        parameters.SetSecretKeyDist(UNIFORM_TERNARY);

        auto depth = 30;
        auto levelsAfterBootstrap = depth - FHECKKSRNS::GetBootstrapDepth(levelBudget, parameters.GetSecretKeyDist());
        parameters.SetMultiplicativeDepth(depth);

        auto cc = GenCryptoContext(parameters);
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
        ctx.SetMultiplicativeDepth(depth);
        ctx.SetLevelsAfterBootstrapping(levelsAfterBootstrap);
        ctx.SetNumSlots(numSlots);
        ctx.SetPublicKey(keys.publicKey);
        ctx.SetPrivateKey(keys.secretKey);

        return ctx;
    }
}
