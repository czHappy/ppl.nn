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

#include "ppl/nn/engines/cuda/optimizer/ops/pmx/channel_shuffle_op.h"

#include "ppl/nn/common/logger.h"
#include "ppl/nn/engines/cuda/kernels/pmx/channel_shuffle_kernel.h"
#include <iostream>

using namespace std;
using namespace ppl::common;
using namespace ppl::nn::pmx;

#ifdef PPLNN_ENABLE_PMX_MODEL
#include "ppl/nn/models/pmx/utils.h"
#include "ppl/nn/models/pmx/oputils/pmx/channel_shuffle.h"
#endif

namespace ppl { namespace nn { namespace cuda {

RetCode ChannelShuffleOp::Init(const OptKernelOptions& options) {
    GenericLoadParam<ChannelShuffleParam>(options, &param_);

    return RC_SUCCESS;
}

ChannelShuffleOp::ChannelShuffleOp(const ir::Node* node) : CudaOptKernel(node) {
    infer_type_func_ = [](InputOutputInfo* info, std::vector<CudaTensorQuant>* quant, datatype_t type) -> RetCode {
        ppl::common::RetCode status;
        if (type == DATATYPE_UNKNOWN) {
            status = InferInheritedType(info);
        } else if (type == DATATYPE_INT8) {
            status = CopyQuantType(info, quant);
        } else {
            status = InferDefaultType(info, type);
        }
        for (uint32_t i = info->GetOutputCount(); i < info->GetInputCount(); ++i) { // Shape op's outputs
            auto shape = info->GetInput<TensorImpl>(i)->GetShape();
            shape->SetDataType(ppl::common::DATATYPE_INT64);
        }
        return status;
    };

    infer_dims_func_ = [](InputOutputInfo* info) -> RetCode {
        auto& in_shape0 = *info->GetInput<TensorImpl>(0)->GetShape();
        for (uint32_t i = 0; i < info->GetOutputCount(); ++i) {
            info->GetOutput<TensorImpl>(i)->GetShape()->Reshape(in_shape0.GetDims(), in_shape0.GetRealDimCount());
        }
        return RC_SUCCESS;
    };
}

RetCode ChannelShuffleOp::Finalize(const OptKernelOptions& options) {
    auto status = SetCommonParam(options);
    if (status != RC_SUCCESS) {
        LOG(ERROR) << "load common param failed: " << GetRetCodeStr(status);
        return status;
    }

    return RC_SUCCESS;
}

KernelImpl* ChannelShuffleOp::CreateKernelImpl() const {
    return CreateKernelImplWithParam<ChannelShuffleKernel>(&param_);
}

#ifdef PPLNN_ENABLE_PMX_MODEL
    ppl::common::RetCode ChannelShuffleOp::SerializeData(const pmx::SerializationContext&, utils::DataStream* ds) const {
        flatbuffers::FlatBufferBuilder builder;
        auto fb_param = pmx::pmx::SerializeChannelShuffleParam(param_, &builder);
        auto fb_op_param = pmx::pmx::CreateOpParam(builder, pmx::pmx::OpParamType_ChannelShuffleParam, fb_param.Union());
        pmx::pmx::FinishOpParamBuffer(builder, fb_op_param);
        return ds->Write(builder.GetBufferPointer(), builder.GetSize());
    }
    ppl::common::RetCode ChannelShuffleOp::DeserializeData(const pmx::DeserializationContext&, const void* base, uint64_t size) {
        auto fb_op_param = pmx::pmx::GetOpParam(base);
        auto fb_argmax_param = fb_op_param->value_as_ChannelShuffleParam();
        pmx::pmx::DeserializeChannelShuffleParam(*fb_argmax_param, &param_);
        return ppl::common::RC_SUCCESS;
    }
#endif

}}} // namespace ppl::nn::cuda
