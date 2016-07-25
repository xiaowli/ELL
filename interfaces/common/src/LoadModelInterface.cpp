////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LoadModelInterface.cpp (interfaces)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadModelInterface.h"

// common
#include "ModelLoadArguments.h"
#include "LoadModel.h"
#include "LoadModelGraph.h"

// layers
#include "Model.h"

// model
#include "ModelGraph.h"

// interface
#include "ModelInterface.h"

namespace interfaces
{
    interfaces::Model LoadModel(const common::ModelLoadArguments& modelLoadArguments)
    {
        auto model = common::LoadModel(modelLoadArguments);
        return interfaces::Model(std::move(model));
    }

    interfaces::Model LoadModel(const std::string& filename)
    {
        common::ModelLoadArguments args;
        args.inputModelFile = filename;
        return interfaces::LoadModel(args);
    }
}
