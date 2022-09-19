// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#ifndef _ST_HPC_PPL_NN_PARAMS_ONNX_GRU_PARAM_H_
#define _ST_HPC_PPL_NN_PARAMS_ONNX_GRU_PARAM_H_

#include "ppl/nn/ir/attr.h"
#include <stdint.h>
#include <vector>
#include <string>

namespace ppl { namespace nn { namespace onnx {

struct GRUParam final : public ir::TypedAttr<GRUParam> {
    enum {
        ACT_RELU = 0,
        ACT_TANH,
        ACT_SIGMOID,
        ACT_AFFINE,
        ACT_LEAKY_RELU,
        ACT_THRESHOLDED_RELU,
        ACT_SCALED_TANH,
        ACT_HARD_SIGMOID,
        ACT_ELU,
        ACT_SOFTSIGN,
        ACT_SOFTPLUS
    };

    enum {
        DIR_FORWARD = 0,
        DIR_REVERSE,
        DIR_BIDIRECTIONAL,
    };

    std::vector<float> activation_alpha;
    std::vector<float> activation_beta;
    std::vector<int32_t> activations;
    float clip;
    int32_t direction;
    int32_t hidden_size;
    // When computing the output of the hidden gate,
    // apply the linear transformation before multiplying by the output of the reset gate.
    int32_t linear_before_reset;

    bool operator==(const GRUParam& p) const {
        const bool val_eq = (this->direction == p.direction && this->hidden_size == p.hidden_size &&
                             this->linear_before_reset == p.linear_before_reset && this->clip == p.clip);
        bool list_eq = (this->activation_alpha.size() == p.activation_alpha.size() &&
                        this->activation_beta.size() == p.activation_beta.size() &&
                        this->activations.size() == p.activations.size());
        if (list_eq) {
            for (size_t i = 0; i < this->activation_alpha.size(); ++i) {
                if (this->activation_alpha[i] != p.activation_alpha[i]) {
                    list_eq = false;
                    goto _LABEL_onnx_gru_param_exit_list_cmp;
                }
            }
            for (size_t i = 0; i < this->activation_beta.size(); ++i) {
                if (this->activation_beta[i] != p.activation_beta[i]) {
                    list_eq = false;
                    goto _LABEL_onnx_gru_param_exit_list_cmp;
                }
            }
            for (size_t i = 0; i < this->activations.size(); ++i) {
                if (this->activations[i] != p.activations[i]) {
                    list_eq = false;
                    goto _LABEL_onnx_gru_param_exit_list_cmp;
                }
            }
        }
    _LABEL_onnx_gru_param_exit_list_cmp:
        return list_eq && val_eq;
    }
};

}}} // namespace ppl::nn::onnx

#endif
