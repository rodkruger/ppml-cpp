#include "model.h"

namespace hermesml {
    CkksNeuralNetwork::CkksNeuralNetwork(const HEContext &ctx, const uint16_t n_features, const uint16_t epochs,
                                         const std::vector<size_t> &sizes, const uint32_t seed,
                                         const ActivationFn activation,
                                         const ApproximationFn approx): EncryptedObject(ctx), MlModel(seed),
                                                                        calculus(Calculus(ctx)),
                                                                        constants(Constants(ctx)),
                                                                        activation(activation),
                                                                        approximation(approx),
                                                                        layerSizes(sizes),
                                                                        n_features(n_features),
                                                                        epochs(epochs) {
        InitWeights();
    }

    BootstrapableCiphertext CkksNeuralNetwork::GetLearningRate() const {
        constexpr auto lr = 0.01;
        return this->EncryptCKKS(std::vector(this->n_features, lr));
    }

    BootstrapableCiphertext CkksNeuralNetwork::Activation(const BootstrapableCiphertext &x) const {
        switch (this->activation) {
            case SIGMOID: return this->calculus.Sigmoid(x, this->approximation);
            default: return this->calculus.Tanh(x, this->approximation);
        }
    }

    BootstrapableCiphertext CkksNeuralNetwork::ActivationDerivative(const BootstrapableCiphertext &x) const {
        switch (this->activation) {
            case SIGMOID: return this->calculus.SigmoidDerivative(x, this->approximation);
            default: return this->calculus.TanhDerivative(x, this->approximation);
        }
    }

    void CkksNeuralNetwork::InitWeights() {
        std::mt19937 gen(this->GetSeed());
        std::normal_distribution dist(0.0, 0.1);

        std::vector<std::vector<std::vector<double> > > weights;
        std::vector<std::vector<double> > biases;

        for (size_t i = 0; i < this->layerSizes.size() - 1; ++i) {
            const size_t rows = this->layerSizes[i];
            const size_t cols = this->layerSizes[i + 1];

            std::vector weightMatrix(rows, std::vector<double>(cols));
            for (size_t r = 0; r < rows; ++r) {
                for (size_t c = 0; c < cols; ++c) {
                    weightMatrix[r][c] = dist(gen);
                }
            }

            std::vector biasVector(cols, 0.0);

            weights.push_back(weightMatrix);
            biases.push_back(biasVector);
        }

        /*
        const std::vector<std::vector<std::vector<double> > > weights = {
            {
                {
                    0.04967142, -0.01382643, 0.06476885, 0.15230299, -0.02341534,
                    -0.02341370, 0.15792128, 0.07674347, -0.04694744, 0.05425600,
                    -0.04634177, -0.04657298, 0.02419623, -0.19132802, -0.17249178,
                    -0.05622875, -0.10128311, 0.03142473, -0.09080241, -0.14123037
                },
                {
                    0.14656488, -0.02257763, 0.00675282, -0.14247482, -0.05443827,
                    0.01109226, -0.11509936, 0.03756980, -0.06006387, -0.02916937,
                    -0.06017066, 0.18522782, -0.00134972, -0.10577109, 0.08225449,
                    -0.12208436, 0.02088636, -0.19596701, -0.13281860, 0.01968612
                },
                {
                    0.07384666, 0.01713683, -0.01156483, -0.03011037, -0.14785220,
                    -0.07198442, -0.04606388, 0.10571222, 0.03436183, -0.17630402,
                    0.03240840, -0.03850823, -0.06769220, 0.06116763, 0.10309995,
                    0.09312801, -0.08392175, -0.03092124, 0.03312634, 0.09755451
                },
                {
                    -0.04791742, -0.01856590, -0.11063350, -0.11962066, 0.08125258,
                    0.13562400, -0.00720101, 0.10035329, 0.03616360, -0.06451198,
                    0.03613956, 0.15380366, -0.00358260, 0.15646437, -0.26197451,
                    0.08219025, 0.00870471, -0.02990074, 0.00917608, -0.19875689
                },
                {
                    -0.02196719, 0.03571126, 0.14778940, -0.05182702, -0.08084936,
                    -0.05017570, 0.09154021, 0.03287511, -0.05297602, 0.05132674,
                    0.00970775, 0.09686450, -0.07020531, -0.03276621, -0.03921082,
                    -0.14635149, 0.02961203, 0.02610553, 0.00051135, -0.02345871
                },
                {
                    -0.14153707, -0.04206453, -0.03427145, -0.08022773, -0.01612857,
                    0.04040509, 0.18861859, 0.01745778, 0.02575504, -0.00744459,
                    -0.19187712, -0.00265139, 0.00602302, 0.24632421, -0.01923610,
                    0.03015473, -0.00347118, -0.11686780, 0.11428228, 0.07519330
                },
                {
                    0.07910319, -0.09093875, 0.14027943, -0.14018511, 0.05868571,
                    0.21904556, -0.09905363, -0.05662977, 0.00996514, -0.05034757,
                    -0.15506634, 0.00685630, -0.10623037, 0.04735924, -0.09194242,
                    0.15499344, -0.07832533, -0.03220615, 0.08135172, -0.12308643
                },
                {
                    0.02274599, 0.13071428, -0.16074832, 0.01846339, 0.02598828,
                    0.07818229, -0.12369507, -0.13204566, 0.05219416, 0.02969847,
                    0.02504929, 0.03464482, -0.06800247, 0.02322537, 0.02930725,
                    -0.07143514, 0.18657745, 0.04738329, -0.11913035, 0.06565536
                },
                {
                    -0.09746817, 0.07870846, 0.11585956, -0.08206823, 0.09633761,
                    0.04127809, 0.08220602, 0.18967930, -0.02453881, -0.07537362,
                    -0.08895144, -0.08158103, -0.00771017, 0.03411520, 0.02766908,
                    0.08271832, 0.00130019, 0.14535341, -0.02646568, 0.27201692
                },
                {
                    0.06256673, -0.08571576, -0.10708925, 0.04824724, -0.02234628,
                    0.07140005, 0.04732376, -0.00728289, -0.08467937, -0.15148472,
                    -0.04465150, 0.08563988, 0.02140937, -0.12457388, 0.01731809,
                    0.03853174, -0.08838574, 0.01537251, 0.00582087, -0.11429703
                },
                {
                    0.03577874, 0.05607845, 0.10830512, 0.10538021, -0.13776694,
                    -0.09378250, 0.05150353, 0.05137860, 0.05150477, 0.38527315,
                    0.05708905, 0.11355656, 0.09540018, 0.06513913, -0.03152692,
                    0.07589692, -0.07728252, -0.02368186, -0.04853635, 0.00818741
                },
                {
                    0.23146586, -0.18672652, 0.06862602, -0.16127159, -0.04719319,
                    0.10889506, 0.00642800, -0.10777448, -0.07153037, 0.06795977,
                    -0.07303666, 0.02164586, 0.00455718, -0.06516003, 0.21439441,
                    0.06339190, -0.20251426, 0.01864543, -0.06617865, 0.08524333
                },
                {
                    -0.07925207, -0.01147364, 0.05049873, 0.08657552, -0.12002964,
                    -0.03345012, -0.04749453, -0.06533292, 0.17654542, 0.04049817,
                    -0.12608840, 0.09178619, 0.21221562, 0.10324653, -0.15193700,
                    -0.04842341, 0.12669111, -0.07076695, 0.04438194, 0.07746341
                },
                {
                    -0.09269305, -0.00595254, -0.32412673, -0.10243876, -0.02525682,
                    -0.12477832, 0.16324113, -0.14301414, -0.04400445, 0.01307406,
                    0.14412733, -0.14358622, 0.11631638, 0.00102331, -0.09815087,
                    0.04621035, 0.01990597, -0.06002169, 0.00698021, -0.03853136
                },
                {
                    0.01135173, 0.06621307, 0.15860168, -0.12378155, 0.21330334,
                    -0.19520878, -0.01517851, 0.05883172, 0.02809919, -0.06226995,
                    -0.02081223, -0.04930009, -0.05893648, 0.08496021, 0.03570155,
                    -0.06929096, 0.08995999, 0.03072995, 0.08128621, 0.06296288
                },
                {
                    -0.08289950, -0.05601810, 0.07472936, 0.06103703, -0.00209016,
                    0.01173274, 0.12776649, -0.05915714, 0.05470974, -0.02021927,
                    -0.02176812, 0.10987769, 0.08254163, 0.08135096, 0.13054788,
                    0.00210038, 0.06819530, -0.03102668, 0.03241664, -0.01301431
                },
                {
                    0.00969960, 0.05951570, -0.08182207, 0.20923873, -0.10060174,
                    -0.12141886, 0.11581109, 0.07916627, 0.06241198, 0.06283455,
                    -0.00122468, -0.08972544, 0.00758046, -0.06771617, 0.09751197,
                    -0.01470574, -0.08254972, -0.03213858, 0.04129315, -0.05637246
                },
                {
                    -0.08222204, 0.02436872, 0.02449666, -0.05069432, -0.04710383,
                    0.02320499, -0.14480843, -0.14074638, -0.07184442, -0.02134472,
                    0.03109076, 0.14753562, 0.08576596, -0.01599385, -0.00190162,
                    -0.10025294, -0.00185131, -0.02886586, 0.03227186, -0.08272309
                },
                {
                    0.05193465, 0.15327389, -0.01087601, 0.04017117, 0.06901440,
                    -0.04012205, 0.02240925, 0.00125924, 0.00976761, -0.07730098,
                    0.00245102, 0.04979983, 0.14511436, 0.09592708, 0.21531825,
                    -0.07673476, 0.08723206, 0.01833420, 0.21898029, -0.08082983
                },
                {
                    -0.08397218, -0.05993926, -0.21238957, -0.05257550, -0.07591327,
                    0.01503938, 0.03417560, 0.18761708, 0.09504238, -0.05769037,
                    -0.08984147, 0.04919192, -0.13202332, 0.18314588, 0.11794401,
                    -0.04691757, -0.17131345, 0.13538724, -0.01145398, 0.12378163
                },
                {
                    -0.15944277, -0.05993750, 0.00052437, 0.00469806, -0.04500655,
                    0.06228499, -0.10676204, -0.01423795, 0.01202956, 0.05144388,
                    0.07116149, -0.11246421, -0.15341142, 0.12776768, 0.03323140,
                    -0.07484865, 0.15511520, 0.01156746, 0.11792972, 0.00675185
                },
                {
                    0.20607479, 0.17553408, -0.02489641, 0.09715710, 0.06453759,
                    0.13686316, -0.09649235, 0.06860515, 0.10584245, -0.17587395,
                    -0.11832585, -0.20392322, -0.02694068, 0.07175423, 0.15023571,
                    0.00740948, 0.16286155, -0.13801015, -0.17033824, -0.00555477
                },
                {
                    0.03840654, -0.00326947, -0.20674421, -0.00891200, -0.13044695,
                    0.06696725, 0.03665982, -0.09398798, -0.05138669, -0.10592135,
                    -0.00626791, 0.09551423, -0.09857260, 0.05040465, -0.05302576,
                    -0.07928728, -0.01070304, -0.10352423, -0.05536493, -0.11978779
                },
                {
                    0.19647251, 0.00352636, -0.06997255, 0.02139799, -0.01123280,
                    -0.02209696, 0.06141667, 0.07575077, -0.05305011, -0.05758182,
                    -0.02750517, -0.23019212, -0.15151911, 0.13668743, 0.16449677,
                    -0.02490360, 0.05765570, 0.03112502, 0.30788808, 0.11195749
                },
                {
                    -0.01279176, -0.09555404, -0.16064463, 0.02034636, -0.07563507,
                    -0.14222537, -0.06465729, -0.10815480, 0.16871416, 0.08816398,
                    -0.00079726, 0.14799441, 0.00773683, -0.08612842, 0.15231241,
                    0.05389100, -0.10372462, -0.01903387, -0.08756183, -0.13827997
                },
                {
                    0.09261775, 0.19094166, -0.13985676, 0.05629692, -0.06506426,
                    -0.04871254, -0.05923939, -0.08639908, 0.00485216, -0.08309501,
                    0.02704568, -0.00502381, -0.02389480, -0.09075637, -0.05767713,
                    0.07553912, 0.05009172, -0.09775552, 0.00993323, 0.07513871
                },
                {
                    -0.16694053, 0.05433602, -0.06626238, 0.05705987, -0.07632592,
                    -0.18048821, -0.16275424, 0.00480849, 0.02597225, -0.09043166,
                    0.06385925, -0.16615201, -0.00660798, -0.12110162, -0.06518361,
                    0.00473987, -0.08604134, -0.03845555, 0.10062928, -0.05768919
                },
                {
                    0.08356921, -0.11297069, 0.05298042, 0.14415686, -0.24716445,
                    -0.07968953, 0.05770721, -0.02030454, 0.03711459, -0.06039852,
                    0.00865898, -0.01556772, 0.11677821, 0.02544208, 0.03376027,
                    -0.04118770, -0.04876062, -0.04325582, 0.03944521, -0.04209845
                },
                {
                    0.02897749, 0.20754008, 0.08711247, -0.03260235, 0.12012139,
                    -0.04080754, -0.20381245, -0.10080863, -0.18707919, -0.03515135,
                    0.00184184, 0.16764373, 0.03269274, -0.02191005, 0.08294056,
                    -0.22111353, 0.02356146, 0.07708652, -0.14785862, 0.11437540
                },
                {
                    0.03384964, -0.04152879, 0.06327819, 0.22706929, 0.01818663,
                    0.02482206, -0.04593609, -0.08498444, 0.08303358, -0.08560838,
                    0.00715662, -0.04776574, 0.04789798, 0.03336621, 0.10375399,
                    -0.05100164, -0.02698749, -0.09787637, -0.04442933, 0.03773005
                }
            },
            {
                {
                    0.07569886, -0.09221653, 0.08696059, 0.13556379, 0.04134349,
                    0.18767958, -0.07737892, -0.12446547, -0.17787202, 0.14960443
                },
                {
                    0.06543657, -0.00555847, 0.02799686, -0.11254890, 0.24457520,
                    0.01292212, 0.01093948, 0.07257666, 0.04810092, 0.02238840
                },
                {
                    -0.07904745, 0.04714684, 0.18820245, 0.13454200, 0.15931866,
                    -0.05112157, -0.09896048, -0.01257869, 0.00557249, 0.10941915
                },
                {
                    -0.16924646, 0.15295503, -0.01580079, -0.04268811, -0.10121044,
                    -0.16548567, 0.08231706, 0.00733180, -0.12899609, -0.12950788
                },
                {
                    -0.03357847, 0.16690215, -0.02595914, -0.15031430, -0.02457431,
                    -0.02727236, -0.26968866, -0.00542949, -0.02309345, 0.06962064
                },
                {
                    0.18489561, 0.11265650, -0.02688887, -0.11065259, 0.25733598,
                    0.00592184, 0.00139293, -0.00241251, 0.01980848, -0.01443604
                },
                {
                    -0.05736620, -0.05468589, -0.00327533, -0.05434248, -0.07128458,
                    0.01064302, -0.02549772, 0.15039930, -0.26509698, 0.10915069
                },
                {
                    0.12460852, -0.20733902, -0.03426876, -0.03714409, -0.14075117,
                    -0.07778167, -0.11105758, 0.17522704, 0.09356784, 0.12715551
                },
                {
                    0.07216721, -0.11290518, -0.05245203, 0.04893746, -0.12221278,
                    0.07129984, -0.02403254, -0.03748208, 0.07109600, 0.04442633
                },
                {
                    -0.03609662, 0.11593298, -0.10810633, 0.06159356, 0.05931013,
                    -0.03095464, 0.03261330, -0.12511136, 0.09240270, -0.01849021
                },
                {
                    -0.05227230, 0.10490092, -0.07043437, -0.14084613, -0.15566292,
                    0.06060100, -0.12804294, 0.17547942, -0.20819294, 0.16964564
                },
                {
                    0.02110175, -0.00967131, -0.05449191, 0.03991361, -0.00376347,
                    0.11033019, 0.01142276, 0.01503018, -0.03636122, -0.00569456
                },
                {
                    0.03078018, -0.17101684, -0.13481854, 0.07432641, 0.01708654,
                    -0.01839833, 0.00184339, 0.03475817, -0.05397597, -0.07783047
                },
                {
                    0.01958453, -0.09783728, 0.04082528, -0.17025836, 0.10291556,
                    0.04725975, 0.02560297, 0.09826910, 0.16654744, 0.10143701
                },
                {
                    -0.18408742, -0.12795770, -0.06248186, 0.00260911, 0.05176590,
                    -0.07257438, 0.01867668, -0.07553829, -0.06115178, -0.14066611
                },
                {
                    -0.09232332, -0.13516846, -0.09758733, 0.10536418, -0.09493989,
                    0.26323821, 0.04933179, 0.01848361, -0.08583578, 0.07003099
                },
                {
                    -0.05756378, 0.01220098, 0.25600845, -0.00960599, 0.11492733,
                    -0.07031764, -0.00349885, 0.17708006, -0.06269671, 0.18124486
                },
                {
                    0.07077519, -0.05624668, 0.06324077, 0.09725544, 0.06218100,
                    -0.15702247, -0.07271372, -0.02475186, -0.00744334, 0.06206721
                },
                {
                    0.01777010, -0.13353444, 0.03801979, 0.06105857, 0.05597904,
                    0.10807807, 0.08339222, 0.04591801, -0.00701657, -0.16609609
                },
                {
                    0.04296182, 0.02076877, 0.02715788, -0.12767486, -0.10810565,
                    0.10531529, -0.00395552, 0.06815007, 0.00283184, 0.00297561
                }
            },
            {
                {0.09382838},
                {-0.05160447},
                {0.00961208},
                {-0.04622753},
                {-0.04344962},
                {-0.03091721},
                {0.02221338},
                {-0.04787486},
                {0.12557561},
                {-0.08946073}
            }
        };

        const std::vector<std::vector<double> > biases = {
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
            },
            {
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
            },
            {
                {0}
            }
        };
        */

        // Transpose the weights, as CKKS does not support arithmetic under transposed matrixes
        for (const auto &w: weights) {
            std::vector<BootstrapableCiphertext> eW;

            const auto w_transposed = Calculus::Transpose(w);
            for (const auto &w_i: w_transposed) {
                eW.emplace_back(this->EncryptCKKS(w_i));
            }

            this->eWeights.push_back(eW);
        }

        for (const auto &b: biases) {
            std::vector<BootstrapableCiphertext> eB;
            for (const auto &b_i: b) {
                eB.emplace_back(this->EncryptCKKS(std::vector{b_i}));
            }
            this->eBias.emplace_back(eB);
        }

        /*
        const std::vector<std::vector<std::vector<double> > > weights =
        {
            {
                {0.5, -0.3, 0.8},
                {-0.2, 0.4, 0.1}
            },
            {
                {1.2, -0.7},
                {0.5, 0.9}
            },
            {
                {0.9, -1.1}
            }
        };

        for (const auto &w: weights) {
            std::vector<BootstrapableCiphertext> eW;
            for (const auto &w_i: w) {
                eW.emplace_back(this->EncryptCKKS(std::vector{w_i}));
            }
            this->eWeights.emplace_back(eW);
        }

        const std::vector<std::vector<double> > biases =
        {
            {0.1, -0.05},
            {0.2, -0.1},
            {-0.3}
        };

        for (const auto &b: biases) {
            std::vector<BootstrapableCiphertext> eB;
            for (const auto &b_i: b) {
                eB.emplace_back(this->EncryptCKKS(std::vector{b_i}));
            }
            this->eBias.emplace_back(eB);
        }
        */
    }

    void CkksNeuralNetwork::Fit(const std::vector<BootstrapableCiphertext> &x,
                                const std::vector<BootstrapableCiphertext> &y) {
        if (x.size() != y.size()) {
            throw std::runtime_error(
                "The size of x must be equal to the size of y. (" + std::to_string(x.size()) + " vs " +
                std::to_string(y.size()) + ")");
        }

        const auto eLearningRate = this->GetLearningRate();

        for (int epoch = 0; epoch < this->epochs; epoch++) {
            for (size_t i = 0; i < x.size(); i++) {
                const auto &eInput = x[i];
                const auto &eTrue = y[i];

                // Forward --------------------------------------------------------------------------------------------
                const auto ePred = this->Predict(eInput);
                // -------------------------------------------------------------------------------------------- Forward

                // Backward -------------------------------------------------------------------------------------------
                std::vector<std::vector<BootstrapableCiphertext> > eScaledGradWeights;
                std::vector<std::vector<BootstrapableCiphertext> > eScaledGradBias;
                std::vector<std::vector<BootstrapableCiphertext> > eLosses;
                std::vector<std::vector<BootstrapableCiphertext> > eDeltaLs;

                auto ePreAct = this->ePreActivations.back();
                auto eZL = this->ActivationDerivative(ePreAct);
                auto eLoss = this->EvalFlatten(this->EvalSub(ePred, eTrue)); // Only 1 Neuron supported
                auto eDeltaL = this->EvalFlatten(this->EvalMult(eLoss, eZL)); // Only 1 Neuron supported

                eLosses.emplace_back(std::vector({eLoss}));
                eDeltaLs.emplace_back(std::vector({eDeltaL}));

                // Update the weights of the last layer ---------------------------------------------------------------
                ePreAct = this->eActivations[this->eActivations.size() - 2];
                auto eGradWK = this->EvalMult(eDeltaL, ePreAct);
                auto eScaledGradWK = this->EvalMult(eGradWK, eLearningRate);
                eScaledGradWeights.emplace_back(std::vector({eScaledGradWK}));

                auto eScaledGradBiasK = this->EvalMult(eDeltaL, eLearningRate);
                eScaledGradBias.emplace_back(std::vector({eScaledGradBiasK}));

                // Update the weights of the remaining layers ---------------------------------------------------------
                for (auto k = static_cast<int32_t>(this->eWeights.size() - 2); k >= 0; k--) {
                    const auto &eLayerWeights = this->eWeights[k + 1];
                    const auto &ePreDeltaL = eDeltaLs.back();

                    auto eLocalLoss = this->constants.Zero();
                    for (auto l = 0; l < eLayerWeights.size(); l++) {
                        // ePreDeltaL[1] * eLayerWeights[1] + ePreDeltaL[2] * eLayerWeights[2] ... ePreDeltaL[l] * eLayerWeights[l]
                        const auto z = this->EvalMult(eLayerWeights[l], ePreDeltaL[l]);
                        eLocalLoss = this->EvalAdd(eLocalLoss, z);

                        /* Use only for debugging purpose
                        this->Snoop(eLayerWeights[l]);
                        this->Snoop(ePreDeltaL[l]);
                        this->Snoop(z);
                        /* */
                    }

                    auto eAct = this->eActivations[k];
                    auto eLocalZl = this->EvalSub(this->constants.One(), this->EvalMult(eAct, eAct));

                    std::vector<BootstrapableCiphertext> eLocalScaledGradWeights;
                    std::vector<BootstrapableCiphertext> eLocalScaledGradBias;
                    std::vector<BootstrapableCiphertext> eLocalLosses;
                    std::vector<BootstrapableCiphertext> eLocalDeltaLs;

                    if (k > 0) {
                        ePreAct = this->eActivations[k - 1];
                    } else {
                        ePreAct = eInput;
                    }

                    for (auto n = 0; n < this->eWeights[k].size(); n++) {
                        if (n > 0) {
                            eLocalLoss = this->EvalRotate(eLocalLoss, 1);
                        }
                        const auto eLocalLossFlat = this->EvalFlatten(eLocalLoss);

                        if (n > 0) {
                            eLocalZl = this->EvalRotate(eLocalZl, 1);
                        }
                        const auto eLocalZlFlat = this->EvalFlatten(eLocalZl);

                        eDeltaL = this->EvalMult(eLocalLossFlat, eLocalZlFlat);
                        eDeltaL = this->EvalFlatten(eDeltaL);
                        eLocalDeltaLs.emplace_back(eDeltaL);

                        eGradWK = this->EvalMult(eDeltaL, ePreAct);
                        eScaledGradWK = this->EvalMult(eGradWK, eLearningRate);
                        eLocalScaledGradWeights.emplace_back(eScaledGradWK);

                        eScaledGradBiasK = this->EvalMult(eDeltaL, eLearningRate);
                        eLocalScaledGradBias.emplace_back(eScaledGradBiasK);

                        /*
                        std::cout << "===========================================" << std::endl;
                        this->Snoop(eAct);
                        this->Snoop(eLoss);
                        this->Snoop(eZL);
                        this->Snoop(ePreAct);
                        this->Snoop(eGradWK);
                        this->Snoop(eScaledGradWK);
                        this->Snoop(eScaledGradBiasK);
                        /* */
                    }

                    eScaledGradWeights.emplace_back(eLocalScaledGradWeights);
                    eScaledGradBias.emplace_back(eLocalScaledGradBias);
                    eDeltaLs.emplace_back(eLocalDeltaLs);
                }

                std::reverse(eScaledGradWeights.begin(), eScaledGradWeights.end());
                std::reverse(eScaledGradBias.begin(), eScaledGradBias.end());
                // ------------------------------------------------------------------------------------------- Backward

                // Gradient Descent -----------------------------------------------------------------------------------
                for (auto k = 0; k < this->eWeights.size(); k++) {
                    const auto &eLayerUnits = this->eWeights[k];

                    for (auto j = 0; j < eLayerUnits.size(); j++) {
                        this->eWeights[k][j] = this->EvalSub(this->eWeights[k][j], eScaledGradWeights[k][j]);
                        this->eBias[k][j] = this->EvalSub(this->eBias[k][j], eScaledGradBias[k][j]);
                    }
                }
                // ----------------------------------------------------------------------------------- Gradient Descent
            }
        }
    }

    void CkksNeuralNetwork::Fit(const std::string &eTrainingFeaturesFilePath,
                                const std::string &eTrainingLabelsFilePath) {
    }

    BootstrapableCiphertext CkksNeuralNetwork::Predict(const BootstrapableCiphertext &x) {
        BootstrapableCiphertext bLayerInput = x;

        this->ePreActivations.clear();
        this->eActivations.clear();

        // Forward ----------------------------------------------------------------------------------------------------
        for (auto k = 0; k < this->eWeights.size(); k++) {
            const auto &eLayerUnits = this->eWeights[k];
            const auto &eLayerBiases = this->eBias[k];

            std::vector<BootstrapableCiphertext> preActivationLayer;
            std::vector<BootstrapableCiphertext> activationLayer;

            for (auto j = 0; j < eLayerUnits.size(); j++) {
                const auto &eWeights = eLayerUnits[j];
                const auto eBias = eLayerBiases[j];
                const auto a = this->WeightedSum(eWeights, bLayerInput, eBias);
                const auto z = this->Activation(a);
                preActivationLayer.emplace_back(a);
                activationLayer.emplace_back(z);

                /* Use for debugging only
                std::cout << "a = " << std::flush;
                this->Snoop(a, n_features);
                std::cout << "z = " << std::flush;
                this->Snoop(z, n_features);
                /* */
            }

            this->ePreActivations.emplace_back(this->EvalMerge(preActivationLayer));
            this->eActivations.emplace_back(this->EvalMerge(activationLayer));
            bLayerInput = this->EvalMerge(activationLayer);
        } // -------------------------------------------------------------------------------------------------- Forward

        /* Use for debugging only
        std::cout << "Pre-Activations" << std::endl;
        for (auto z = 0; z < this->ePreActivations.size(); z++) {
            std::cout << "Layer " << z << std::endl;
            this->Snoop(this->ePreActivations[z], n_features);
        }

        std::cout << "Activations" << std::endl;
        for (auto z = 0; z < this->eActivations.size(); z++) {
            std::cout << "Layer " << z << std::endl;
            this->Snoop(this->eActivations[z], this->n_features);
        }
        /* */

        auto ePred = this->eActivations.back();

        /* Use for debugging only */
        // this->Snoop(ePred, n_features);
        /* */

        return ePred;
    }

    std::vector<BootstrapableCiphertext> CkksNeuralNetwork::PredictAll(
        const std::vector<BootstrapableCiphertext> &x) {
        std::vector<BootstrapableCiphertext> predictions(x.size());

        for (size_t i = 0; i < x.size(); ++i) {
            predictions[i] = this->Predict(x[i]);
        }

        return predictions;
    }

    std::vector<BootstrapableCiphertext>
    CkksNeuralNetwork::PredictAll(const std::string &eTestingFeaturesFilePath) {
        std::vector<BootstrapableCiphertext> predictions{};

        std::ifstream eFeaturesStream(eTestingFeaturesFilePath, std::ios::binary);

        while (eFeaturesStream.peek() != EOF) {
            Ciphertext<DCRTPoly> eFeatures;
            Serial::Deserialize(eFeatures, eFeaturesStream, SerType::BINARY);
            predictions.emplace_back(
                this->Predict(BootstrapableCiphertext(eFeatures, this->GetCtx().GetMultiplicativeDepth())));
        }

        eFeaturesStream.close();

        return predictions;
    }
}
