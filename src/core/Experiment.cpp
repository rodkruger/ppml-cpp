#include <filesystem>
#include <utility>
#include "core.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace hermesml {
    Experiment::Experiment(std::string experimentId, Dataset &dataset) : experimentId(std::move(experimentId)),
                                                                         dataset(dataset) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            "../logs/" + this->experimentId + ".txt", true);

        this->logger = std::make_shared<spdlog::logger>("multi_sink", spdlog::sinks_init_list{console_sink, file_sink});
        this->logger->set_level(spdlog::level::info);
        this->logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");

        spdlog::set_default_logger(this->logger);
        spdlog::flush_on(spdlog::level::info);

        this->contentPath = std::filesystem::current_path().string() + "/Predictions/" + dataset.GetName() + "/" + this->experimentId + "/";

        if (!std::filesystem::exists(this->contentPath)) {
            std::filesystem::create_directory(this->contentPath);
        }
    }

    std::string Experiment::BuildFilePath(const std::string &fileName) const {
        return this->contentPath + fileName;
    }

    Dataset &Experiment::GetDataset() const {
        return this->dataset;
    }

    std::string Experiment::GetExperimentId() const {
        return this->experimentId;
    }

    std::string Experiment::GetContentPath() const {
        return this->contentPath;
    }

    void Experiment::Info(const std::string &message) const {
        this->logger->info(message);
    }

    void Experiment::Error(const std::string &message) const {
        this->logger->error(message);
    }

    void Experiment::Run() {
    }
}
