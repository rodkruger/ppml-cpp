#ifndef CORE_H
#define CORE_H

#define QUANTIZE_SCALE_FACTOR 1e4 // 10^4

#pragma once

#include "context.h"
#include "spdlog/spdlog.h"

namespace hermesml {
    //-----------------------------------------------------------------------------------------------------------------

    class BootstrapableCiphertext {
        Ciphertext<DCRTPoly> ciphertext;
        uint8_t remainingLevels = 0;

    public:
        explicit BootstrapableCiphertext();

        explicit BootstrapableCiphertext(const Ciphertext<DCRTPoly> &ciphertext, uint8_t remainingLevels);

        [[nodiscard]] Ciphertext<DCRTPoly> GetCiphertext() const;

        [[nodiscard]] uint8_t GetRemainingLevels() const;

        void SetRemainingLevels(uint8_t pRemainingLevels);
    };

    //-----------------------------------------------------------------------------------------------------------------

    class EncryptedObject {
        HEContext ctx;
        CryptoContext<DCRTPoly> cc;

        static uint8_t ComputeRemainingLevels(const BootstrapableCiphertext &ciphertext1,
                                              const BootstrapableCiphertext &ciphertext2);

        //-----------------------------------------------------------------------------------------------------------------

    public:
        explicit EncryptedObject(const HEContext &ctx);

        [[nodiscard]] HEContext GetCtx() const;

        [[nodiscard]] CryptoContext<DCRTPoly> GetCc() const;

        [[nodiscard]] BootstrapableCiphertext Encrypt(const std::vector<int64_t> &plaintext) const;

        [[nodiscard]] BootstrapableCiphertext EncryptCKKS(const std::vector<double> &plaintext) const;

        [[nodiscard]] static std::vector<double> UnpackValues(const Plaintext &plaintext, uint16_t n_features);

        [[nodiscard]] BootstrapableCiphertext EvalAdd(const BootstrapableCiphertext &ciphertext1,
                                                      const BootstrapableCiphertext &ciphertext2) const;

        [[nodiscard]] BootstrapableCiphertext EvalSum(const BootstrapableCiphertext &ciphertext1) const;

        [[nodiscard]] BootstrapableCiphertext EvalSub(const BootstrapableCiphertext &ciphertext1,
                                                      const BootstrapableCiphertext &ciphertext2) const;

        [[nodiscard]] BootstrapableCiphertext EvalMult(const BootstrapableCiphertext &ciphertext1,
                                                       const BootstrapableCiphertext &ciphertext2) const;

        [[nodiscard]] BootstrapableCiphertext EvalBootstrap(const BootstrapableCiphertext &ciphertext) const;

        void Snoop(const BootstrapableCiphertext &ciphertext, uint16_t n_features) const;

        [[nodiscard]] static int16_t GetScalingFactor();
    };

    //-----------------------------------------------------------------------------------------------------------------

    class MinMaxScaler {
    public:
        static void Scale(std::vector<std::vector<double> > &data);
    };

    //-----------------------------------------------------------------------------------------------------------------

    class Quantizer {
    public:
        static std::vector<std::vector<int64_t> > Quantize(const std::vector<std::vector<double> > &data);

        static std::vector<int64_t> Quantize(const std::vector<double> &data);
    };

    //-----------------------------------------------------------------------------------------------------------------

    class Experiment {
        std::string experimentId;

    protected:
        std::shared_ptr<spdlog::logger> logger;

    public:
        virtual ~Experiment() = default;

        explicit Experiment(std::string experimentId);

        virtual void run();
    };
}

#endif //CORE_H
