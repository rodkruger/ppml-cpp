#ifndef CORE_H
#define CORE_H

#define QUANTIZE_SCALE_FACTOR 1e4 // 10^4

#pragma once

#include "context.h"
#include "datasets.h"
#include "spdlog/spdlog.h"

namespace hermesml {
    //-----------------------------------------------------------------------------------------------------------------

    class BootstrapableCiphertext {
        Ciphertext<DCRTPoly> ciphertext;
        int32_t remainingLevels = 0;
        int32_t additionsExecuted = 0;

    public:
        explicit BootstrapableCiphertext();

        explicit BootstrapableCiphertext(const Ciphertext<DCRTPoly> &ciphertext, int32_t remainingLevels,
                                         int32_t additionsExecuted = 0);

        [[nodiscard]] const Ciphertext<DCRTPoly> &GetCiphertext() const;

        [[nodiscard]] int32_t GetRemainingLevels() const;

        void SetRemainingLevels(int32_t pRemainingLevels);

        [[nodiscard]] int32_t GetAdditionsExecuted() const;
    };

    //-----------------------------------------------------------------------------------------------------------------

    class EncryptedObject {
        HEContext ctx;
        CryptoContext<DCRTPoly> cc;

    protected:
        static int32_t ComputeRemainingLevels(const BootstrapableCiphertext &ciphertext1,
                                              const BootstrapableCiphertext &ciphertext2);

        [[nodiscard]] Ciphertext<DCRTPoly> SafeRescaling(const Ciphertext<DCRTPoly> &ciphertext) const;

        //-----------------------------------------------------------------------------------------------------------------

    public:
        explicit EncryptedObject(const HEContext &ctx);

        [[nodiscard]] HEContext GetCtx() const;

        [[nodiscard]] CryptoContext<DCRTPoly> GetCc() const;

        [[nodiscard]] BootstrapableCiphertext Encrypt(const std::vector<int64_t> &plaintext) const;

        [[nodiscard]] BootstrapableCiphertext EncryptCKKS(const std::vector<double> &plaintext) const;

        [[nodiscard]] std::vector<BootstrapableCiphertext> EncryptCKKS(
            const std::vector<std::vector<double> > &plaintext) const;

        [[nodiscard]] std::vector<double> UnpackValues(const Plaintext &plaintext) const;

        [[nodiscard]] BootstrapableCiphertext EvalAdd(const BootstrapableCiphertext &ciphertext1,
                                                      const BootstrapableCiphertext &ciphertext2) const;

        [[nodiscard]] BootstrapableCiphertext EvalSum(const BootstrapableCiphertext &ciphertext1) const;

        [[nodiscard]] BootstrapableCiphertext EvalSub(const BootstrapableCiphertext &ciphertext1,
                                                      const BootstrapableCiphertext &ciphertext2) const;

        [[nodiscard]] BootstrapableCiphertext EvalMult(const BootstrapableCiphertext &ciphertext1,
                                                       const BootstrapableCiphertext &ciphertext2) const;

        [[nodiscard]] BootstrapableCiphertext EvalBootstrap(const BootstrapableCiphertext &ciphertext) const;

        void Snoop(const BootstrapableCiphertext &ciphertext) const;

        [[nodiscard]] static int16_t GetScalingFactor();

        [[nodiscard]] BootstrapableCiphertext WeightedSum(
            const BootstrapableCiphertext &weights,
            const BootstrapableCiphertext &features,
            const BootstrapableCiphertext &bias) const;

        [[nodiscard]] BootstrapableCiphertext EvalMerge(const std::vector<BootstrapableCiphertext> &ciphertexts) const;

        [[nodiscard]] BootstrapableCiphertext EvalFlatten(const BootstrapableCiphertext &ciphertext) const;

        [[nodiscard]] BootstrapableCiphertext
        EvalRotate(const BootstrapableCiphertext &ciphertext, int32_t index) const;
    };

    //-----------------------------------------------------------------------------------------------------------------

    class MinMaxScaler {
        int8_t alpha{0};
        int8_t beta{1};

    public:
        explicit MinMaxScaler(int8_t alpha = 0, int8_t beta = 1);

        void Scale(std::vector<std::vector<double> > &data) const;
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
        std::string contentPath;
        Dataset &dataset;
        std::shared_ptr<spdlog::logger> logger;

    protected:
        [[nodiscard]] std::string BuildFilePath(const std::string &fileName);

    public:
        virtual ~Experiment() = default;

        explicit Experiment(std::string experimentId, Dataset &dataset);

        void SetExperimentId(const std::string &experimentId);

        [[nodiscard]] std::string GetExperimentId() const;

        [[nodiscard]] std::string GetContentPath() const;

        [[nodiscard]] Dataset &GetDataset() const;

        void Info(const std::string &message) const;

        void Error(const std::string &message) const;

        virtual void Run();
    };
}

#endif //CORE_H
