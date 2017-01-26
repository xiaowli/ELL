////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Evaluator.tcc (evaluators)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BinaryErrorAggregator.h"

// utilities
#include "FunctionUtils.h"

namespace ell
{
namespace evaluators
{
    template <typename PredictorType, typename... AggregatorTypes>
    Evaluator<PredictorType, AggregatorTypes...>::Evaluator(const data::AnyDataset& anyDataset, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators)
        : _dataset(anyDataset), _evaluatorParameters(evaluatorParameters), _aggregatorTuple(std::make_tuple(aggregators...))
    {
        static_assert(sizeof...(AggregatorTypes) > 0, "Evaluator must contains at least one aggregator");

        if (_evaluatorParameters.addZeroEvaluation)
        {
            EvaluateZero();
        }
    }

    template <typename PredictorType, typename... AggregatorTypes>
    void Evaluator<PredictorType, AggregatorTypes...>::Evaluate(const PredictorType& predictor)
    {
        ++_evaluateCounter;
        if (_evaluateCounter % _evaluatorParameters.evaluationFrequency != 0)
        {
            return;
        }

        auto iterator = _dataset.GetExampleReferenceIterator();

        while (iterator.IsValid())
        {
            const auto& example = iterator.Get();

            double weight = example.GetMetadata().weight;
            double label = example.GetMetadata().label;
            double prediction = predictor.Predict(example.GetDataVector());

            DispatchUpdate(prediction, label, weight, std::make_index_sequence<sizeof...(AggregatorTypes)>());
            iterator.Next();
        }
        Aggregate(std::make_index_sequence<sizeof...(AggregatorTypes)>());
    }

    template <typename PredictorType, typename... AggregatorTypes>
    double Evaluator<PredictorType, AggregatorTypes...>::GetGoodness() const
    {
        if (_values.size() == 0)
        {
            return 0.0;
        }
        return _values.back()[0][0];
    }

    template <typename T>
    void PrintVector(std::ostream& os, const std::vector<T>& v)
    {
        if (v.size() == 0) return;

        os << v[0];
        for (size_t j = 1; j < v.size(); ++j)
        {
            os << '\t' << v[j];
        }
    }

    template <typename T>
    std::vector<T> FlattenJaggedVector(const std::vector<std::vector<T>>& v)
    {
        std::vector<T> concat;
        auto iter = v.cbegin();
        auto end = v.end();
        while (iter != end)
        {
            concat.insert(concat.end(), iter->cbegin(), iter->cend());
            ++iter;
        }
        return concat;
    }

    template <typename PredictorType, typename... AggregatorTypes>
    void Evaluator<PredictorType, AggregatorTypes...>::Print(std::ostream& os) const
    {
        auto originalPrecision = os.precision(6);
        auto originalFlags = os.setf(std::ios::fixed);

        PrintVector(os, FlattenJaggedVector(GetValueNames()));

        for (const auto& values : _values)
        {
            os << std::endl;
            PrintVector(os, FlattenJaggedVector(values));
        }

        os.setf(originalFlags);
        os.precision(originalPrecision);
    }

    template <typename PredictorType, typename... AggregatorTypes>
    void Evaluator<PredictorType, AggregatorTypes...>::EvaluateZero()
    {
        auto iterator = _dataset.GetExampleIterator();

        while (iterator.IsValid())
        {
            const auto& example = iterator.Get();

            double weight = example.GetMetadata().weight;
            double label = example.GetMetadata().label;

            DispatchUpdate(0.0, label, weight, std::make_index_sequence<sizeof...(AggregatorTypes)>());
            iterator.Next();
        }
        Aggregate(std::make_index_sequence<sizeof...(AggregatorTypes)>());
    }

    template <typename PredictorType, typename... AggregatorTypes>
    template <std::size_t... Sequence>
    void Evaluator<PredictorType, AggregatorTypes...>::DispatchUpdate(double prediction, double label, double weight, std::index_sequence<Sequence...>)
    {
        // Call (X.Update(), 0) for each X in _aggregatorTuple
        utilities::InOrderFunctionEvaluator(std::bind(std::mem_fn(&std::tuple_element<Sequence, decltype(_aggregatorTuple)>::type::Update), std::get<Sequence>(_aggregatorTuple), prediction, label, weight)...);
        // utilities::InOrderFunctionEvaluator([this, &prediction, &label, &weight](){std::get<Sequence>(_aggregatorTuple).Update(prediction, label, weight);}...); // GCC bug prevents compilation
    }

    template <typename PredictorType, typename... AggregatorTypes>
    template <std::size_t... Sequence>
    void Evaluator<PredictorType, AggregatorTypes...>::Aggregate(std::index_sequence<Sequence...>)
    {
        // Call X.GetResult() for each X in _aggregatorTuple
        _values.push_back({ std::get<Sequence>(_aggregatorTuple).GetResult()... });

        // Call X.Reset() for each X in _aggregatorTuple
        utilities::InOrderFunctionEvaluator(std::bind(std::mem_fn(&std::tuple_element<Sequence, decltype(_aggregatorTuple)>::type::Reset), std::get<Sequence>(_aggregatorTuple))...);
        // utilities::InOrderFunctionEvaluator([this](){std::get<Sequence>(_aggregatorTuple).Reset();}...); // GCC bug prevents compilation
    }

    template <typename PredictorType, typename... AggregatorTypes>
    std::vector<std::vector<std::string>> Evaluator<PredictorType, AggregatorTypes...>::GetValueNames() const
    {
        return DispatchGetValueNames(std::make_index_sequence<sizeof...(AggregatorTypes)>());
    }

    template <typename PredictorType, typename... AggregatorTypes>
    template <std::size_t... Sequence>
    std::vector<std::vector<std::string>> Evaluator<PredictorType, AggregatorTypes...>::DispatchGetValueNames(std::index_sequence<Sequence...>) const
    {
        return { std::get<Sequence>(_aggregatorTuple).GetValueNames()... };
    }

    template <typename PredictorType, typename... AggregatorTypes>
    std::shared_ptr<IEvaluator<PredictorType>> MakeEvaluator(const data::AnyDataset& anyDataset, const EvaluatorParameters& evaluatorParameters, AggregatorTypes... aggregators)
    {
        return std::make_unique<Evaluator<PredictorType, AggregatorTypes...>>(anyDataset, evaluatorParameters, aggregators...);
    }
}
}
